#include <stdio.h>      // snprintf, vsnprintf
#include <stdlib.h>     // malloc, free
#include <stdarg.h>     // va_start, va_end, va_list
#include <string.h>     // memset, memcpy, strlen, strcpy, strcmp, strncasecmp, strerror
#include <ctype.h>      // isprint, isspace
#include <errno.h>      // errno
#include <unistd.h>     // close
#include <sys/time.h>   // gettimeofday
#include <sys/ioctl.h>  // ioctl, FIONBIO
#include <net/if.h>     // struct ifconf, struct ifreq
#include <fcntl.h>      // fcntl, F_GETFD, F_SETFD, FD_CLOEXEC
#include <sys/socket.h> // struct sockaddr, AF_INET, SOL_SOCKET, socklen_t, setsockopt, socket, bind, sendto, recvfrom
#include <netinet/in.h> // struct sockaddr_in, struct ip_mreq, INADDR_ANY, IPPROTO_IP, also include <sys/socket.h>
#include <arpa/inet.h>  // inet_aton, inet_ntop, inet_addr, also include <netinet/in.h>
#include "lssdp.hpp"

#ifndef _SIZEOF_ADDR_IFREQ
#define _SIZEOF_ADDR_IFREQ sizeof
#endif

/** Definition **/
#define LSSDP_BUFFER_LEN    2048
#define lssdp_debug(fmt, agrs...) lssdp_log(LSSDP_LOG_DEBUG, __LINE__, __func__, fmt, ##agrs)
#define lssdp_info(fmt, agrs...)  lssdp_log(LSSDP_LOG_INFO,  __LINE__, __func__, fmt, ##agrs)
#define lssdp_warn(fmt, agrs...)  lssdp_log(LSSDP_LOG_WARN,  __LINE__, __func__, fmt, ##agrs)
#define lssdp_error(fmt, agrs...) lssdp_log(LSSDP_LOG_ERROR, __LINE__, __func__, fmt, ##agrs)


/** Struct: lssdp_packet **/
typedef struct lssdp_packet {
    char            method      [LSSDP_FIELD_LEN];      // M-SEARCH, NOTIFY, RESPONSE
    char            st          [LSSDP_FIELD_LEN];      // Search Target
    char            usn         [LSSDP_FIELD_LEN];      // Unique Service Name
    char            location    [LSSDP_LOCATION_LEN];   // Location

    /* Additional SSDP Header Fields */
    char            sm_id       [LSSDP_FIELD_LEN];
    char            device_type [LSSDP_FIELD_LEN];
    long long       update_time;
} lssdp_packet;


/** Internal Function **/
static int send_multicast_data(const char * data, const lssdp_interface interface, unsigned short ssdp_port);
static int lssdp_send_response(lssdp_ctx * lssdp, struct sockaddr_in address);
static int lssdp_packet_parser(const char * data, size_t data_len, lssdp_packet * packet);
static int parse_field_line(const char * data, size_t start, size_t end, lssdp_packet * packet);
static int get_colon_index(const char * string, size_t start, size_t end);
static int trim_spaces(const char * string, size_t * start, size_t * end);
static long long get_current_time();
static int lssdp_log(int level, int line, const char * func, const char * format, ...);
static int neighbor_list_add(lssdp_ctx * lssdp, const lssdp_packet packet);
static int lssdp_neighbor_remove_all(lssdp_ctx * lssdp);
static void neighbor_list_free(lssdp_nbr * list);
static lssdp_interface * find_interface_in_LAN(lssdp_ctx * lssdp, uint32_t address);


/** Global Variable **/
static struct {
    const char * MSEARCH;
    const char * NOTIFY;
    const char * RESPONSE;

    const char * HEADER_MSEARCH;
    const char * HEADER_NOTIFY;
    const char * HEADER_RESPONSE;

    const char * ADDR_LOCALHOST;
    const char * ADDR_MULTICAST;

    void (* log_callback)(const char * file, const char * tag, int level, int line, const char * func, const char * message);

} Global = {
    // SSDP Method
    .MSEARCH  = "M-SEARCH",
    .NOTIFY   = "NOTIFY",
    .RESPONSE = "RESPONSE",

    // SSDP Header
    .HEADER_MSEARCH  = "M-SEARCH * HTTP/1.1\r\n",
    .HEADER_NOTIFY   = "NOTIFY * HTTP/1.1\r\n",
    .HEADER_RESPONSE = "HTTP/1.1 200 OK\r\n",

    // IP Address
    .ADDR_LOCALHOST = "10.0.6.107",
    .ADDR_MULTICAST = "239.255.255.250",

    // Log Callback
    .log_callback = NULL
};


// 01. lssdp_network_interface_update
int lssdp_network_interface_update(lssdp_ctx * lssdp) {
    if (lssdp == NULL) {
        lssdp_error("lssdp should not be NULL\n");
        return -1;
    }
    const size_t SIZE_OF_INTERFACE_LIST = sizeof(lssdp_interface) * LSSDP_INTERFACE_LIST_SIZE;

    // 1. copy orginal interface
    lssdp_interface original_interface[LSSDP_INTERFACE_LIST_SIZE] = {};
    memcpy(original_interface, lssdp->interface, SIZE_OF_INTERFACE_LIST);

    // 2. reset lssdp->interface
    lssdp->interface_num = 0;
    memset(lssdp->interface, 0, SIZE_OF_INTERFACE_LIST);

    int result = -1;

    /* Reference to this article:
    * http://stackoverflow.com/a/8007079
    */

    // 3. create UDP socket
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    do {
       if (fd < 0) {
            lssdp_error("create socket failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // 4. get ifconfig
        char buffer[LSSDP_BUFFER_LEN] = {};
        ifconf ifc;
        ifc.ifc_len = sizeof(buffer);
        ifc.ifc_buf = (caddr_t) buffer;

        if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
            lssdp_error("ioctl SIOCGIFCONF failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // 5. setup lssdp->interface
        size_t i;
        struct ifreq * ifr;
        for (i = 0; i < ifc.ifc_len; i += _SIZEOF_ADDR_IFREQ(*ifr)) {
            ifr = (struct ifreq *)(buffer + i);
            if (ifr->ifr_addr.sa_family != AF_INET) {
                // only support IPv4
                continue;
            }

            // 5-1. get interface ip string
            char ip[LSSDP_IP_LEN] = {};
            struct sockaddr_in * addr = (struct sockaddr_in *) &ifr->ifr_addr;
            if (inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip)) == NULL) {
                lssdp_error("inet_ntop failed, errno = %s (%d)\n", strerror(errno), errno);
                continue;
            }

            // 5-2. get network mask
            struct ifreq netmask = {};
            strcpy(netmask.ifr_name, ifr->ifr_name);
            if (ioctl(fd, SIOCGIFNETMASK, &netmask) != 0) {
                lssdp_error("ioctl SIOCGIFNETMASK failed, errno = %s (%d)\n", strerror(errno), errno);
                continue;
            }

            // 5-3. check network interface number
            if (lssdp->interface_num >= LSSDP_INTERFACE_LIST_SIZE) {
                lssdp_warn("interface number is over than MAX SIZE (%d)     %s %s\n", LSSDP_INTERFACE_LIST_SIZE, ifr->ifr_name, ip);
                continue;
            }

            // 5-4. set interface
            size_t n = lssdp->interface_num;
            snprintf(lssdp->interface[n].name, LSSDP_INTERFACE_NAME_LEN, "%s", ifr->ifr_name); // name
            snprintf(lssdp->interface[n].ip,   LSSDP_IP_LEN,             "%s", ip);            // ip string
            lssdp->interface[n].addr = addr->sin_addr.s_addr;                                  // address in network byte order

        
            // set network mask
            addr = (struct sockaddr_in *) &netmask.ifr_addr;
            lssdp->interface[n].netmask = addr->sin_addr.s_addr;                               // mask in network byte order

            // increase interface number
            lssdp->interface_num++;
        }

        result = 0;

    } while (false);

    // close socket
    if (fd >= 0 && close(fd) != 0) {
        lssdp_error("close fd %d failed, errno = %s (%d)\n", strerror(errno), errno);
    }

    // compare with original interface
    if (memcmp(original_interface, lssdp->interface, SIZE_OF_INTERFACE_LIST) == 0) {
        // interface is not changed
        return result;
    }

    /* Network Interface is changed */

    // 1. force clean up neighbor_list
    lssdp_neighbor_remove_all(lssdp);

    // 2. invoke network interface changed callback
    if (lssdp->network_interface_changed_callback != NULL) {
        lssdp->network_interface_changed_callback(lssdp);
    }

    return result;
}

// 02. lssdp_socket_create
int lssdp_socket_create(lssdp_ctx * lssdp) {
    if (lssdp == NULL) {
        lssdp_error("lssdp should not be NULL\n");
        return -1;
    }

    if (lssdp->port == 0) {
        lssdp_error("SSDP port (%d) has not been setup.\n", lssdp->port);
        return -1;
    }

    // close original SSDP socket
    lssdp_socket_close(lssdp);

    // create UDP socket
    lssdp->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (lssdp->sock < 0) {
        lssdp_error("create socket failed, errno = %s (%d)\n", strerror(errno), errno);
        return -1;
    }

    int result = -1;

    // set non-blocking
    do {
        int opt = 1;
        if (ioctl(lssdp->sock, FIONBIO, &opt) != 0) {
            lssdp_error("ioctl FIONBIO failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // set reuse address
        if (setsockopt(lssdp->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
            lssdp_error("setsockopt SO_REUSEADDR failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // set FD_CLOEXEC (http://kaivy2001.pixnet.net/blog/post/32726732)
        int sock_opt = fcntl(lssdp->sock, F_GETFD);
        if (sock_opt == -1) {
            lssdp_error("fcntl F_GETFD failed, errno = %s (%d)\n", strerror(errno), errno);
        } else {
            // F_SETFD
            if (fcntl(lssdp->sock, F_SETFD, sock_opt | FD_CLOEXEC) == -1) {
                lssdp_error("fcntl F_SETFD FD_CLOEXEC failed, errno = %s (%d)\n", strerror(errno), errno);
            }
        }

        // bind socket
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(lssdp->port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(lssdp->sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            lssdp_error("bind failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // set IP_ADD_MEMBERSHIP
        ip_mreq imr;
        imr.imr_multiaddr.s_addr = inet_addr(Global.ADDR_MULTICAST);
        imr.imr_interface.s_addr = htonl(INADDR_ANY);
        if (setsockopt(lssdp->sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(struct ip_mreq)) != 0) {
            lssdp_error("setsockopt IP_ADD_MEMBERSHIP failed: %s (%d)\n", strerror(errno), errno);
            break;
        }

        lssdp_info("create SSDP socket %d\n", lssdp->sock);
        result = 0;

    } while (false);

    if (result == -1) {
        lssdp_socket_close(lssdp);
    }
    return result;
}

// 03. lssdp_socket_close
int lssdp_socket_close(lssdp_ctx * lssdp) {
    if (lssdp == NULL) {
        lssdp_error("lssdp should not be NULL\n");
        return -1;
    }

    // check lssdp->sock
    do {
        if (lssdp->sock <= 0) {
            lssdp_warn("SSDP socket is %d, ignore socket_close request.\n", lssdp->sock);
            break;
        }

        // close socket
        if (close(lssdp->sock) != 0) {
            lssdp_error("close socket %d failed, errno = %s (%d)\n", lssdp->sock, strerror(errno), errno);
            return -1;
        };

        // close socket success
        lssdp_info("close SSDP socket %d\n", lssdp->sock);
    } while (false);
    lssdp->sock = -1;
    lssdp_neighbor_remove_all(lssdp);  // force clean up neighbor_list
    return 0;
}

// 04. lssdp_socket_read
int lssdp_socket_read(lssdp_ctx * lssdp) {
    if (lssdp == NULL) {
        lssdp_error("lssdp should not be NULL\n");
        return -1;
    }

    // check socket and port
    if (lssdp->sock <= 0) {
        lssdp_error("SSDP socket (%d) has not been setup.\n", lssdp->sock);
        return -1;
    }

    if (lssdp->port == 0) {
        lssdp_error("SSDP port (%d) has not been setup.\n", lssdp->port);
        return -1;
    }

    char buffer[LSSDP_BUFFER_LEN] = {};
    struct sockaddr_in address = {};
    socklen_t address_len = sizeof(struct sockaddr_in);

    ssize_t recv_len = recvfrom(lssdp->sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&address, &address_len);
    if (recv_len == -1) {
        lssdp_error("recvfrom fd %d failed, errno = %s (%d)\n", lssdp->sock, strerror(errno), errno);
        return -1;
    }

    // ignore the SSDP packet received from self
    do {
        size_t i;
        for (i = 0; i < lssdp->interface_num; i++) {
            if (lssdp->interface[i].addr == address.sin_addr.s_addr) {
                break;
            }
        }

        // parse SSDP packet to struct
        lssdp_packet packet = {};
        if (lssdp_packet_parser(buffer, recv_len, &packet) != 0) {
            break;
        }

        // check search target
        if (strcmp(packet.st, lssdp->header.search_target) != 0) {
            // search target is not match
            if (lssdp->debug) {
                lssdp_info("RECV <- %-8s   not match with %-14s %s\n", packet.method, lssdp->header.search_target, packet.location);
            }
            break;
        }

        // M-SEARCH: send RESPONSE back
        if (strcmp(packet.method, Global.MSEARCH) == 0) {
            lssdp_send_response(lssdp, address);
            break;
        }

        // RESPONSE, NOTIFY: add to neighbor_list
        neighbor_list_add(lssdp, packet);

        if (lssdp->debug) {
            lssdp_info("RECV <- %-8s   %-28s  %s\n", packet.method, packet.location, packet.sm_id);
        }
    } while (false);
    
    // invoke packet received callback
    if (lssdp->packet_received_callback != NULL) {
        lssdp->packet_received_callback(lssdp, buffer, recv_len);
    }

    return 0;
}

// 05. lssdp_send_msearch
int lssdp_send_msearch(lssdp_ctx * lssdp) {
    if (lssdp == NULL) {
        lssdp_error("lssdp should not be NULL\n");
        return -1;
    }

    if (lssdp->port == 0) {
        lssdp_error("SSDP port (%d) has not been setup.\n", lssdp->port);
        return -1;
    }

    // check network inerface number
    if (lssdp->interface_num == 0) {
        lssdp_warn("Network Interface is empty, no destination to send %s\n", Global.MSEARCH);
        return -1;
    }

    // 1. set M-SEARCH packet
    char msearch[LSSDP_BUFFER_LEN] = {};
    snprintf(msearch, sizeof(msearch),
        "%s"
        "HOST:%s:%d\r\n"
        "MAN:\"ssdp:discover\"\r\n"
        "MX:1\r\n"
        "ST:%s\r\n"
        "USER-AGENT:OS/version product/version\r\n"
        "\r\n",
        Global.HEADER_MSEARCH,              // HEADER
        Global.ADDR_MULTICAST, lssdp->port, // HOST
        lssdp->header.search_target         // ST (Search Target)
    );

    // 2. send M-SEARCH to each interface
    size_t i;
    for (i = 0; i < lssdp->interface_num; i++) {
        lssdp_interface * interface = &lssdp->interface[i];

        // avoid sending multicast to localhost
        if (interface->addr == inet_addr(Global.ADDR_LOCALHOST)) {
            continue;
        }

        // send M-SEARCH
        int ret = send_multicast_data(msearch, *interface, lssdp->port);
        if (ret == 0 && lssdp->debug) {
            lssdp_info("SEND => %-8s   %s => MULTICAST\n", Global.MSEARCH, interface->ip);
        }
    }

    return 0;
}

// 06. lssdp_send_notify
int lssdp_send_notify(lssdp_ctx * lssdp) {
    if (lssdp == NULL) {
        lssdp_error("lssdp should not be NULL\n");
        return -1;
    }

    if (lssdp->port == 0) {
        lssdp_error("SSDP port (%d) has not been setup.\n", lssdp->port);
        return -1;
    }

    // check network inerface number
    if (lssdp->interface_num == 0) {
        lssdp_warn("Network Interface is empty, no destination to send %s\n", Global.NOTIFY);
        return -1;
    }

    size_t i;
    for (i = 0; i < lssdp->interface_num; i++) {
        lssdp_interface * interface = &lssdp->interface[i];

        // avoid sending multicast to localhost
        if (interface->addr == inet_addr(Global.ADDR_LOCALHOST)) {
            continue;
        }

        // set notify packet
        char notify[LSSDP_BUFFER_LEN] = {};
        char * domain = lssdp->header.location.domain;
        snprintf(notify, sizeof(notify),
            "%s"
            "HOST:%s:%d\r\n"
            "CACHE-CONTROL:max-age=120\r\n"
            "LOCATION:%s%s%s\r\n"
            "SERVER:OS/version product/version\r\n"
            "NT:%s\r\n"
            "NTS:ssdp:alive\r\n"
            "USN:%s\r\n"
            "SM_ID:%s\r\n"
            "DEV_TYPE:%s\r\n"
            "\r\n",
            Global.HEADER_NOTIFY,                       // HEADER
            Global.ADDR_MULTICAST, lssdp->port,         // HOST
            lssdp->header.location.prefix,              // LOCATION
            strlen(domain) > 0 ? domain : interface->ip,
            lssdp->header.location.suffix,
            lssdp->header.search_target,                // NT (Notify Type)
            lssdp->header.unique_service_name,          // USN
            lssdp->header.sm_id,                        // SM_ID    (addtional field)
            lssdp->header.device_type                   // DEV_TYPE (addtional field)
        );

        // send NOTIFY
        // log(info) << "SEND NOTIFY PACKET >>>";
        // log(info) << notify;
        int ret = send_multicast_data(notify, *interface, lssdp->port);
        if (ret == 0 && lssdp->debug) {
            lssdp_info("SEND => %-8s   %s => MULTICAST\n", Global.NOTIFY, interface->ip);
        }
    }

    // network inerface is empty
    if (i == 0) lssdp_warn("Network Interface is empty, no destination to send %s\n", Global.NOTIFY);

    return 0;
}

// 07. lssdp_neighbor_check_timeout
int lssdp_neighbor_check_timeout(lssdp_ctx * lssdp) {
    if (lssdp == NULL) {
        lssdp_error("lssdp should not be NULL\n");
        return -1;
    }

    // check neighbor_timeout
    if (lssdp->neighbor_timeout <= 0) {
        lssdp_warn("lssdp->neighbor_timeout (%ld) is invalid, ignore check_timeout request.\n", lssdp->neighbor_timeout);
        return 0;
    }

    long long current_time = get_current_time();
    if (current_time < 0) {
        lssdp_error("got invalid timestamp %lld\n", current_time);
        return -1;
    }

    bool is_changed = false;
    lssdp_nbr * prev = NULL;
    lssdp_nbr * nbr  = lssdp->neighbor_list;
    while (nbr != NULL) {
        long pass_time = current_time - nbr->update_time;
        if (pass_time < lssdp->neighbor_timeout) {
            prev = nbr;
            nbr  = nbr->next;
            continue;
        }

        is_changed = true;
        lssdp_warn("remove timeout SSDP neighbor: %s (%s) (%ldms)\n", nbr->sm_id, nbr->location, pass_time);

        if (prev == NULL) {
            // it's first neighbor in list
            lssdp->neighbor_list = nbr->next;
            free(nbr);
            nbr = lssdp->neighbor_list;
        } else {
            prev->next = nbr->next;
            free(nbr);
            nbr = prev->next;
        }
    }

    // invoke neighbor list changed callback
    if (is_changed == true && lssdp->neighbor_list_changed_callback != NULL) {
        lssdp->neighbor_list_changed_callback(lssdp);
    }
    return 0;
}

// 08. lssdp_set_log_callback
void lssdp_set_log_callback(void (* callback)(const char * file, const char * tag, int level, int line, const char * func, const char * message)) {
    Global.log_callback = callback;
}


/** Internal Function **/

static int send_multicast_data(const char * data, const lssdp_interface interface, unsigned short ssdp_port) {
    if (data == NULL) {
        lssdp_error("data should not be NULL\n");
        return -1;
    }

    size_t data_len = strlen(data);
    if (data_len == 0) {
        lssdp_error("data length should not be empty\n");
        return -1;
    }

    if (strlen(interface.name) == 0) {
        lssdp_error("interface.name should not be empty\n");
        return -1;
    }

    int result = -1;

    // 1. create UDP socket
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    do {
        if (fd < 0) {
            lssdp_error("create socket failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // 2. bind socket
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = interface.addr;
        
        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            lssdp_error("bind failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // 3. disable IP_MULTICAST_LOOP
        char opt = 0;
        if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &opt, sizeof(opt)) < 0) {
            lssdp_error("setsockopt IP_MULTICAST_LOOP failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // 4. set destination address
        struct sockaddr_in dest_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(ssdp_port)
        };
        if (inet_aton(Global.ADDR_MULTICAST, &dest_addr.sin_addr) == 0) {
            lssdp_error("inet_aton failed, errno = %s (%d)\n", strerror(errno), errno);
            break;
        }

        // 5. send data
        if (sendto(fd, data, data_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
            lssdp_error("sendto %s (%s) failed, errno = %s (%d)\n", interface.name, interface.ip, strerror(errno), errno);
            break;
        }

        result = 0;
    } while (false);
    if (fd >= 0 && close(fd) != 0) {
        lssdp_error("close fd %d failed, errno = %s (%d)\n", strerror(errno), errno);
    }
    return result;
}

static int lssdp_send_response(lssdp_ctx * lssdp, struct sockaddr_in address) {
    // get M-SEARCH IP
    char msearch_ip[LSSDP_IP_LEN] = {};
    if (inet_ntop(AF_INET, &address.sin_addr, msearch_ip, sizeof(msearch_ip)) == NULL) {
        lssdp_error("inet_ntop failed, errno = %s (%d)\n", strerror(errno), errno);
        return -1;
    }

    // 1. find the interface which is in LAN
    lssdp_interface * interface = find_interface_in_LAN(lssdp, address.sin_addr.s_addr);
    if (interface == NULL) {
        if (lssdp->debug) {
            lssdp_info("RECV <- %-8s   Interface is not found        %s\n", Global.MSEARCH, msearch_ip);
        }

        if (lssdp->interface_num == 0) {
            lssdp_warn("Network Interface is empty, no destination to send %s\n", Global.RESPONSE);
        }
        return -1;
    }

    // 2. set response packet
    char response[LSSDP_BUFFER_LEN] = {};
    char * domain = lssdp->header.location.domain;
    int response_len = snprintf(response, sizeof(response),
        "%s"
        "CACHE-CONTROL:max-age=120\r\n"
        "DATE:\r\n"
        "EXT:\r\n"
        "LOCATION:%s%s%s\r\n"
        "SERVER:OS/version product/version\r\n"
        "ST:%s\r\n"
        "USN:%s\r\n"
        "SM_ID:%s\r\n"
        "DEV_TYPE:%s\r\n"
        "\r\n",
        Global.HEADER_RESPONSE,                     // HEADER
        lssdp->header.location.prefix,              // LOCATION
        strlen(domain) > 0 ? domain : interface->ip,
        lssdp->header.location.suffix,
        lssdp->header.search_target,                // ST (Search Target)
        lssdp->header.unique_service_name,          // USN
        lssdp->header.sm_id,                        // SM_ID    (addtional field)
        lssdp->header.device_type                   // DEV_TYPE (addtional field)
    );

    // 3. set port to address
    address.sin_port = htons(lssdp->port);

    if (lssdp->debug) {
        lssdp_info("RECV <- %-8s   %s <- %s\n", Global.MSEARCH, interface->ip, msearch_ip);
    }

    // 4. send data
    if (sendto(lssdp->sock, response, response_len, 0, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1) {
        lssdp_error("send RESPONSE to %s failed, errno = %s (%d)\n", msearch_ip, strerror(errno), errno);
        return -1;
    }

    if (lssdp->debug) {
        lssdp_info("SEND => %-8s   %s => %s\n", Global.RESPONSE, interface->ip, msearch_ip);
    }

    return 0;
}

static int lssdp_packet_parser(const char * data, size_t data_len, lssdp_packet * packet) {
    if (data == NULL) {
        lssdp_error("data should not be NULL\n");
        return -1;
    }

    if (data_len != strlen(data)) {
        lssdp_error("data_len (%zu) is not match to the data length (%zu)\n", data_len, strlen(data));
        return -1;
    }

    if (packet == NULL) {
        lssdp_error("packet should not be NULL\n");
        return -1;
    }

    // 1. compare SSDP Method Header: M-SEARCH, NOTIFY, RESPONSE
    size_t i;
    if ((i = strlen(Global.HEADER_MSEARCH)) < data_len && memcmp(data, Global.HEADER_MSEARCH, i) == 0) {
        strcpy(packet->method, Global.MSEARCH);
    } else if ((i = strlen(Global.HEADER_NOTIFY)) < data_len && memcmp(data, Global.HEADER_NOTIFY, i) == 0) {
        strcpy(packet->method, Global.NOTIFY);
    } else if ((i = strlen(Global.HEADER_RESPONSE)) < data_len && memcmp(data, Global.HEADER_RESPONSE, i) == 0) {
        strcpy(packet->method, Global.RESPONSE);
    } else {
        lssdp_warn("received unknown SSDP packet\n");
        lssdp_debug("%s\n", data);
        return -1;
    }

    // 2. parse each field line
    size_t start = i;
    for (i = start; i < data_len; i++) {
        if (data[i] == '\n' && i - 1 > start && data[i - 1] == '\r') {
            parse_field_line(data, start, i - 2, packet);
            start = i + 1;
        }
    }

    // 3. set update_time
    long long current_time = get_current_time();
    if (current_time < 0) {
        lssdp_error("got invalid timestamp %lld\n", current_time);
        return -1;
    }
    packet->update_time = current_time;
    return 0;
}

static int parse_field_line(const char * data, size_t start, size_t end, lssdp_packet * packet) {
    // 1. find the colon
    if (data[start] == ':') {
        lssdp_warn("the first character of line should not be colon\n");
        lssdp_debug("%s\n", data);
        return -1;
    }

    int colon = get_colon_index(data, start + 1, end);
    if (colon == -1) {
        lssdp_warn("there is no colon in line\n");
        lssdp_debug("%s\n", data);
        return -1;
    }

    if (colon == end) {
        // value is empty
        return -1;
    }


    // 2. get field, field_len
    size_t i = start;
    size_t j = colon - 1;
    if (trim_spaces(data, &i, &j) == -1) {
        return -1;
    }
    const char * field = &data[i];
    size_t field_len = j - i + 1;


    // 3. get value, value_len
    i = colon + 1;
    j = end;
    if (trim_spaces(data, &i, &j) == -1) {
        return -1;
    };
    const char * value = &data[i];
    size_t value_len = j - i + 1;


    // 4. set each field's value to packet
    if (field_len == strlen("st") && strncasecmp(field, "st", field_len) == 0) {
        memcpy(packet->st, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
        return 0;
    }

    if (field_len == strlen("nt") && strncasecmp(field, "nt", field_len) == 0) {
        memcpy(packet->st, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
        return 0;
    }

    if (field_len == strlen("usn") && strncasecmp(field, "usn", field_len) == 0) {
        memcpy(packet->usn, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
        return 0;
    }

    if (field_len == strlen("location") && strncasecmp(field, "location", field_len) == 0) {
        memcpy(packet->location, value, value_len < LSSDP_LOCATION_LEN ? value_len : LSSDP_LOCATION_LEN - 1);
        return 0;
    }

    if (field_len == strlen("sm_id") && strncasecmp(field, "sm_id", field_len) == 0) {
        memcpy(packet->sm_id, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
        return 0;
    }

    if (field_len == strlen("dev_type") && strncasecmp(field, "dev_type", field_len) == 0) {
        memcpy(packet->device_type, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
        return 0;
    }

    // the field is not in the struct packet
    return 0;
}

static int get_colon_index(const char * string, size_t start, size_t end) {
    size_t i;
    for (i = start; i <= end; i++) {
        if (string[i] == ':') {
            return i;
        }
    }
    return -1;
}

static int trim_spaces(const char * string, size_t * start, size_t * end) {
    int i = *start;
    int j = *end;

    while (i <= *end   && (!isprint(string[i]) || isspace(string[i]))) i++;
    while (j >= *start && (!isprint(string[j]) || isspace(string[j]))) j--;

    if (i > j) {
        return -1;
    }

    *start = i;
    *end   = j;
    return 0;
}

static long long get_current_time() {
    struct timeval time = {};
    if (gettimeofday(&time, NULL) == -1) {
        lssdp_error("gettimeofday failed, errno = %s (%d)\n", strerror(errno), errno);
        return -1;
    }
    return (long long) time.tv_sec * 1000 + (long long) time.tv_usec / 1000;
}

static int lssdp_log(int level, int line, const char * func, const char * format, ...) {
    if (Global.log_callback == NULL) {
        return -1;
    }

    char message[LSSDP_BUFFER_LEN] = {};

    // create message by va_list
    va_list args;
    va_start(args, format);
    vsnprintf(message, LSSDP_BUFFER_LEN, format, args);
    va_end(args);

    // invoke log callback function
    Global.log_callback(__FILE__, "SSDP", level, line, func, message);
    return 0;
}

static int neighbor_list_add(lssdp_ctx * lssdp, const lssdp_packet packet) {
    lssdp_nbr * last_nbr = lssdp->neighbor_list;

    bool is_changed = false;
    lssdp_nbr * nbr;
    for (nbr = lssdp->neighbor_list; nbr != NULL; last_nbr = nbr, nbr = nbr->next) {
        if (strcmp(nbr->location, packet.location) != 0) {
            // location is not match
            continue;
        }

        /* location is not found in SSDP list: update neighbor */

        // usn
        if (strcmp(nbr->usn, packet.usn) != 0) {
            lssdp_debug("neighbor usn is changed. (%s -> %s)\n", nbr->usn, packet.usn);
            memcpy(nbr->usn, packet.usn, LSSDP_FIELD_LEN);
            is_changed = true;
        }

        // sm_id
        if (strcmp(nbr->sm_id, packet.sm_id) != 0) {
            lssdp_debug("neighbor sm_id is changed. (%s -> %s)\n", nbr->sm_id, packet.sm_id);
            memcpy(nbr->sm_id, packet.sm_id, LSSDP_FIELD_LEN);
            is_changed = true;
        }

        // device type
        if (strcmp(nbr->device_type, packet.device_type) != 0) {
            lssdp_debug("neighbor device_type is changed. (%s -> %s)\n", nbr->device_type, packet.device_type);
            memcpy(nbr->device_type, packet.device_type, LSSDP_FIELD_LEN);
            is_changed = true;
        }

        // update_time
        nbr->update_time = packet.update_time;
        
        // invoke neighbor list changed callback
        if (lssdp->neighbor_list_changed_callback != NULL && is_changed == true) {
            lssdp->neighbor_list_changed_callback(lssdp);
        }

        return 0;
    }


    /* location is not found in SSDP list: add to list */

    // 1. memory allocate lssdp_nbr
    nbr = (lssdp_nbr *) malloc(sizeof(lssdp_nbr));
    if (nbr == NULL) {
        lssdp_error("malloc failed, errno = %s (%d)\n", strerror(errno), errno);
        return -1;
    }

    // 2. setup neighbor
    memcpy(nbr->usn,         packet.usn,         LSSDP_FIELD_LEN);
    memcpy(nbr->sm_id,       packet.sm_id,       LSSDP_FIELD_LEN);
    memcpy(nbr->device_type, packet.device_type, LSSDP_FIELD_LEN);
    memcpy(nbr->location,    packet.location,    LSSDP_LOCATION_LEN);
    nbr->update_time = packet.update_time;
    nbr->next = NULL;

    // 3. add neighbor to the end of list
    if (last_nbr == NULL) {
        // it's the first neighbor
        lssdp->neighbor_list = nbr;
    } else {
        last_nbr->next = nbr;
    }

    is_changed = true;

    // invoke neighbor list changed callback
    if (lssdp->neighbor_list_changed_callback != NULL && is_changed == true) {
        lssdp->neighbor_list_changed_callback(lssdp);
    }

    return 0;
}

static int lssdp_neighbor_remove_all(lssdp_ctx * lssdp) {
    if (lssdp->neighbor_list == NULL) {
        return 0;
    }

    // free neighbor_list
    neighbor_list_free(lssdp->neighbor_list);
    lssdp->neighbor_list = NULL;

    lssdp_info("neighbor list has been force clean up.\n");

    // invoke neighbor list changed callback
    if (lssdp->neighbor_list_changed_callback != NULL) {
        lssdp->neighbor_list_changed_callback(lssdp);
    }
    return 0;
}

static void neighbor_list_free(lssdp_nbr * list) {
    if (list != NULL) {
        neighbor_list_free(list->next);
        free(list);
    }
}

static lssdp_interface * find_interface_in_LAN(lssdp_ctx * lssdp, uint32_t address) {
    lssdp_interface * ifc;
    size_t i;
    for (i = 0; i < lssdp->interface_num; i++) {
        ifc = &lssdp->interface[i];

        // mask address to check whether the interface is under the same Local Network Area or not
        if ((ifc->addr & ifc->netmask) == (address & ifc->netmask)) {
            return ifc;
        }
    }
    return NULL;
}