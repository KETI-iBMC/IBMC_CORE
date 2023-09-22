#include "ipmi/sel.hpp"
#include "ipmi/user.hpp"
#include <bitset>
#include <cstdlib>
#include <ipmi/apps.hpp>
#include <ipmi/lightning_sensor.hpp>
#include <ipmi/sensor_define.hpp>
#include <ipmi/sol.hpp>
#include <math.h>
#include <redfish/eventservice.hpp>
#include <redfish/resource.hpp>
#include <sqlite3.h>
#include <string>

#include <system_error>
#include <util/dbus_system.hpp>
#define __ARM_PCS_VFP 1
#define __ARM_PCS_VFP 1
extern unordered_map<string, Resource *> g_record;
#pragma once
extern sensor_thresh_t peb_sensors[PEB_SENSOR_COUNT];
// extern sensor_thresh_t nva_sensors[NVA_SENSOR_COUNT];
extern sensor_thresh_t pdpb_sensors[PDPB_SENSOR_COUNT];

/**
 *
 * @namespace tos32
 */

/**
 * @namespace json사용하기 위한 web
 */

#define DEBUG

using namespace std;
#define SIZE_IPMI_RES_HDR 3

static pthread_mutex_t m_app;
static pthread_mutex_t m_storage;
static pthread_mutex_t m_transport;
static pthread_mutex_t m_oem;
static pthread_mutex_t m_chassis;

/**
 * @brief Firmware build time 문자열
 *
 */
char g_firmware_build_time[50];

extern std::map<uint8_t, std::map<uint8_t, Ipmisdr>> sdr_rec;
extern std::map<uint8_t, std::map<uint8_t, Ipmifru>> fru_rec;
#define ETH_COUNT 4
std::thread t_lanMonitor[ETH_COUNT];
Ipmichassis ipmiChassis = Ipmichassis();
Ipminetwork ipmiNetwork[ETH_COUNT] = {Ipminetwork(1), Ipminetwork(6),
                                      Ipminetwork(7), Ipminetwork(8)};
extern Ipmiuser ipmiUser[MAX_USER];
Ipmichannel ipmiChannel[9] = {
    Ipmichannel(MEDIUM_IPMB, PROTOCOL_IPMB_LAN, 0, CHANNEL_SESSIONLESS),
    Ipmichannel(MEDIUM_802_3_LAN, PROTOCOL_IPMB_LAN, 1, CHANNEL_MULTI_SESSION),
    Ipmichannel(MEDIUM_SI, PROTOCOL_KCS, 2, CHANNEL_SESSIONLESS),
    Ipmichannel(3),
    Ipmichannel(4),
    Ipmichannel(5),
    Ipmichannel(MEDIUM_802_3_LAN, PROTOCOL_IPMB_LAN, 6, CHANNEL_MULTI_SESSION),
    Ipmichannel(MEDIUM_802_3_LAN, PROTOCOL_IPMB_LAN, 7, CHANNEL_MULTI_SESSION),
    Ipmichannel(MEDIUM_802_3_LAN, PROTOCOL_IPMB_LAN, 6, CHANNEL_MULTI_SESSION)};
Ipmiapplication ipmiApplication = Ipmiapplication();
extern Ipmisession ipmiSession[5];
extern char uuid_hex[16];

extern std::mutex redfish_m_mutex[5];
extern std::condition_variable redfish_m_cond[5];

/**
 * watchdog 설정
 *
 */
static bmc_watchdog_param_t g_watchdog_config;
int IPMI_Interval = 10, IPMI_Timeout = 600, IPMI_Pretimeout = 10,
    IPMI_PretimeoutInterrupt = 0, IPMI_Action = 1;
char *IPMI_Daemon = NULL, *IPMI_Pidfile = NULL, *IPMI_debug = NULL;
Ipmiapplication::Ipmiapplication() {
  cout << "Ipmiapplication" << endl;
  g_watchdog_config.interval = &IPMI_Interval;
  ReadConfigurationFile(ConfigurationFileDir);
  cout << "read WDT " << endl;
  g_watchdog_config.Islogging = true;
  g_watchdog_config.timer_actions = IPMI_Action;
  g_watchdog_config.pre_timeout = IPMI_Pretimeout;
  //*g_watchdog_config.interval = 19;
  // WriteConfigurationFile(ConfigurationFileDir);
  this->g_mc_device.mc_device_id = 0x20;
  this->g_mc_device.mc_device_rev = 0x01;
  this->g_mc_device.mc_fw_version[0] = 0x01;
  this->g_mc_device.mc_fw_version[1] = 0x12;
  this->g_mc_device.mc_ipmi_version = 0x02;
  this->g_mc_device.mc_additional_dev = 0xBF;
  this->g_mc_device.mc_mnfr_id[0] = 0x83;
  this->g_mc_device.mc_mnfr_id[1] = 0x23;
  this->g_mc_device.mc_mnfr_id[2] = 0x00;
  this->g_mc_device.mc_prod_id[0] = 0x51;
  this->g_mc_device.mc_prod_id[1] = 0x16;
  this->g_mc_device.mc_aux_fw_version[0] = 0x00;
  this->g_mc_device.mc_aux_fw_version[1] = 0x00;
  this->g_mc_device.mc_aux_fw_version[2] = 0x00;
  this->g_mc_device.mc_aux_fw_version[3] = 0x00;

  for (int i = 0; i < 4; i++) {
    switch (i) {
    case 0:
      this->g_authrsp[i].channel_number = 1;
      break;
    case 1:
      this->g_authrsp[i].channel_number = 6;
      break;
    case 2:
      this->g_authrsp[i].channel_number = 7;
      break;
    case 3:
      this->g_authrsp[i].channel_number = 8;
      break;
    }
    this->g_authrsp[i].enabled_auth_types = 0;
    this->g_authrsp[i].v20_data_available = 0b1; // IPMI 2.0 Support
    this->g_authrsp[i].kg_status = 0b0;
    this->g_authrsp[i].per_message_auth = 0;
    this->g_authrsp[i].user_level_auth = 0b1;
    this->g_authrsp[i].anon_login_enabled = 0b0;
    this->g_authrsp[i].non_null_usernames = 0b0;
    this->g_authrsp[i].non_null_usernames = 0b0;
    this->g_authrsp[i].ipmiv15_support = 0b1;
    this->g_authrsp[i].ipmiv20_support = 0b1;
  }

  this->g_sysinfo.set_in_prog = 0;
}

void mutex_init(void) {
  pthread_mutex_init(&m_app, NULL);
  pthread_mutex_init(&m_storage, NULL);
  pthread_mutex_init(&m_transport, NULL);
  pthread_mutex_init(&m_oem, NULL);
  pthread_mutex_init(&m_chassis, NULL);
}

void mutex_destroy(void) {
  pthread_mutex_destroy(&m_app);
  pthread_mutex_destroy(&m_storage);
  pthread_mutex_destroy(&m_transport);
  pthread_mutex_destroy(&m_oem);
  pthread_mutex_destroy(&m_chassis);

  t_lanMonitor[0].join();
}

void Ipmiapplication::app_get_sysinfo_param(uint8_t *request, uint8_t *response,
                                            uint8_t *res_len) {
  cout << "app_get_sysinfo_param" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  printf("apps:://app_get_sysinfo_param request ");
  uint8_t *data = &res->data[0];
  uint8_t rev = req->data[0] & 0x80 ? 1 : 0;
  uint8_t param = req->data[1];
  *data++ = 1;

  if (rev == 0) {
    if (param != SYS_INFO_PARAM_SET_IN_PROG)
      *data++ = 0; // ASCII + Latin 1

    switch (param) {
    case SYS_INFO_PARAM_SET_IN_PROG:
      *data++ = this->g_sysinfo.set_in_prog;
      break;
    case SYS_INFO_PARAM_SYSFW_VER:
      *data++ = this->g_sysinfo.sysfw_ver.length();
      std::copy(this->g_sysinfo.sysfw_ver.begin(),
                this->g_sysinfo.sysfw_ver.end(), data);
      break;

    case SYS_INFO_PARAM_SYS_NAME:
      *data++ = this->g_sysinfo.sys_name.length();
      std::copy(this->g_sysinfo.sys_name.begin(),
                this->g_sysinfo.sys_name.end(), data);
      break;

    case SYS_INFO_PARAM_PRI_OS_NAME:
      *data++ = this->g_sysinfo.pri_os_name.length();
      std::copy(this->g_sysinfo.pri_os_name.begin(),
                this->g_sysinfo.pri_os_name.end(), data);
      break;

    case SYS_INFO_PARAM_PRESENT_OS_NAME:
      *data++ = this->g_sysinfo.present_os_name.length();
      std::copy(this->g_sysinfo.present_os_name.begin(),
                this->g_sysinfo.present_os_name.end(), data);
      break;

    case SYS_INFO_PARAM_PRESENT_OS_VER:
      *data++ = this->g_sysinfo.present_os_ver.length();
      std::copy(this->g_sysinfo.present_os_ver.begin(),
                this->g_sysinfo.present_os_ver.end(), data);
      break;
    }
  }
}

void Ipmiapplication::app_set_sysinfo_param(uint8_t *request, uint8_t *response,
                                            uint8_t *res_len) {
  cout << "app_set_sysinfo_param" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  uint8_t *data = &res->data[0];
  uint8_t param = req->data[0];
  uint8_t str_len = req->data[3];

  res->cc = CC_SUCCESS;

  switch (param) {
  case SYS_INFO_PARAM_SET_IN_PROG:
    this->g_sysinfo.set_in_prog = req->data[1];
    break;
  case SYS_INFO_PARAM_SYSFW_VER:
    this->g_sysinfo.sysfw_ver.assign(req->data + 4, req->data + 4 + str_len);
    break;
  case SYS_INFO_PARAM_SYS_NAME:
    this->g_sysinfo.sys_name.assign(req->data + 4, req->data + 4 + str_len);
    break;
  case SYS_INFO_PARAM_PRI_OS_NAME:
    this->g_sysinfo.pri_os_name.assign(req->data + 4, req->data + 4 + str_len);
    break;
  case SYS_INFO_PARAM_PRESENT_OS_NAME:
    this->g_sysinfo.present_os_name.assign(req->data + 4,
                                           req->data + 4 + str_len);
    break;
  case SYS_INFO_PARAM_PRESENT_OS_VER:
    this->g_sysinfo.present_os_ver.assign(req->data + 4,
                                          req->data + 4 + str_len);
    break;
  case SYS_INFO_PARAM_BMC_URL:
    this->g_sysinfo.bmc_url.assign(req->data + 4, req->data + 4 + str_len);
    break;
  case SYS_INFO_PARAM_OS_HV_URL:
    this->g_sysinfo.os_hv_url.assign(req->data + 4, req->data + 4 + str_len);
    break;
  default:
    res->cc = CC_INVALID_PARAM;
    break;
  }
}

void Ipmiapplication::app_set_useraccess(uint8_t *request, uint8_t *response,
                                         uint8_t *res_len) {
  cout << "app_set_useraccess" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  uint8_t *data = &res->data[0];

  uint8_t id = req->data[1] & 0x3F;

  ipmiUser[id - 1].setUserAccess(req);

  res->cc = CC_SUCCESS;

  *res_len = 0;
}

void Ipmiapplication::app_set_user_passwd(uint8_t *request, uint8_t *response,
                                          uint8_t *res_len) {
  cout << "app_set_user_passwd" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  res->cc = CC_SUCCESS;
  std::string temp_pass;
  uint8_t *data = &res->data[0];

  uint8_t size_pwd = (req->data[0] & 0x80) >> 7;
  uint8_t user_id = (req->data[0] & 0x1F);
  uint8_t operation = (req->data[1] & 0x03);

  if (operation == 2 || operation == 3) {
    if (size_pwd == 0)
      temp_pass.assign(req->data + 2, req->data + 16);
    else if (size_pwd == 1)
      temp_pass.assign(req->data + 2, req->data + 22);
  }
  printf("set_user operation = %d\n", operation);
  printf("set_user temp_pass = %d\n", temp_pass);
  printf("set_user user_id = %d\n", user_id);
  switch (operation) {
  case 0: // disable user
    ipmiUser[user_id].setUserAccess(req);
    break;
  case 1: // enable user
    req->data[0] |= 0x10;
    req->data[0] |= 0x20;
    req->data[0] |= 0x40;
    req->data[0] |= 0x80;
    req->data[2] = (req->data[2] & 0xF0) | 0x04;
    ipmiUser[user_id].setUserAccess(req);
    break;
  case 2: // set password
    ipmiUser[user_id].setUserPasswd(temp_pass);
    break;
  case 3: // test password
  {
    std::string user_pwd = ipmiUser[user_id].getUserpassword();
    if (user_pwd != temp_pass)
      res->cc = 0x81;
    else if (user_pwd.size() == temp_pass.size()) {
      if (user_pwd != temp_pass)
        res->cc = CC_INVALID_PARAM;
      else
        res->cc = CC_SUCCESS;
    }
  } break;
  }
}
bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
         c == '\v';
}
void Ipmiapplication::app_set_username(uint8_t *request, uint8_t *response,
                                       uint8_t *res_len) {
  cout << "app_set_username" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  std::string username(req->data + 1, req->data + 16);
  for (int i = 0; i < 16; i++)
    printf("%c ", username[i]);
  printf("\n");
  printf("size =%d \n", username.size());
  uint8_t id = req->data[0];
  printf("set_username id =%d\n", id);
  size_t firstSpaceIndex = username.find('0');
  std::string temp(username.c_str());
  printf("temp =%s \n", temp.c_str());

  uint8_t flag = 0;
  for (int i = 0; i < MAX_ID; i++) {
    if (ipmiUser[i].getUsername() == temp) {
      flag++;
    }
  }

  if (flag == 0) {
    res->cc = CC_SUCCESS;
    ipmiUser[id].setUserName(temp);
  }
  *res_len = 0;
}

void Ipmiapplication::app_get_username(uint8_t *request, uint8_t *response,
                                       uint8_t *res_len) {
  cout << "app_get_username" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];
  uint8_t id = req->data[0];
  if (id == 0 | id >= 11) {
    printf("app_get_username 3 id 버그 발생 이유 확인중= %d\n", id);
    id = 10;
  }
  cout << "ipmiUser[id - 1].getUsername();" << ipmiUser[id - 1].getUsername()
       << endl;

  std::string name = ipmiUser[id].getUsername();

  res->cc = CC_SUCCESS;
  cout << "end app_get_username" << endl;
  std::copy(name.begin(), name.end(), data);
  *res_len = 16;
}

void Ipmiapplication::app_get_useraccess(uint8_t *request, uint8_t *response,
                                         uint8_t *res_len) {
  cout << "app_get_useraccess" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  uint8_t *data = &res->data[0];
  uint8_t id = req->data[1];
  uint8_t entry, flag, en_users = 0;
  std::string temp_name;

  if (id == 1) {
    flag = 2;
    res->cc = CC_SUCCESS;
    for (int i = 0; i < 10; i++) {
      if (ipmiUser[i].getUserenable() == 1)
        en_users++;
    }
    ipmiUser[id - 1].getUserAccess(flag, en_users, res, res_len);
  }

  else {
    flag = 1;
    for (int i = 0; i < 10; i++) {
      temp_name = ipmiUser[i].getUsername();
      if (temp_name.length() != 0)
        entry++;
    }
    res->cc = CC_SUCCESS;
    ipmiUser[id - 1].getUserAccess(flag, entry, res, res_len);
  }
}

void Ipmiapplication::app_get_session_info(uint8_t *request, uint8_t *response,
                                           uint8_t *res_len) {
  cout << "app_get_session_info" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  uint8_t *data = &res->data[0];
  uint8_t type = req->data[0];
  std::vector<uint8_t> console_ip, console_mac;
  uint16_t console_port;
  int idx = -1;

  switch (type) {
  case 0x0:
    for (int i = 0; i < 5; i++) {
      if (ipmiSession[i].getSessionHandle() != 0) {
        idx = i;
        break;
      }
    }
    break;
  case 0xfe: {
    uint8_t handle = req->data[1];

    for (int i = 0; i < 5; i++) {
      if (handle == ipmiSession[i].getSessionHandle()) {
        idx = i;
        break;
      }
    }
    break;
  }
  case 0xff: {
    uint8_t user_id = req->data[1] & 0xff;

    for (int i = 0; i < 5; i++) {
      if (user_id == ipmiSession[i].getSessionUserId()) {
        idx = i;
        break;
      }
    }
    break;
  }
  }

  if (idx != -1) {
    uint8_t count = 0;
    for (int i = 0; i < 5; i++) {
      if (ipmiSession[i].getSessionHandle() != 0) {
        count++;
      }
    }
    for (int i = 0; i < 5; i++) {
      ipmiSession[i].setActiveSessionCount(count);
    }
    res->cc = CC_SUCCESS;
    *data++ = ipmiSession[idx].getSessionHandle();
    *data++ = ipmiSession[idx].getSessionSlotCount();
    *data++ = ipmiSession[idx].getActiveSessCount();
    *data++ = ipmiSession[idx].getSessionUserId();
    *data++ = ipmiSession[idx].getSessionPriv();
    *data++ = ipmiSession[idx].getSessionChanData();

    console_ip = ipmiSession[idx].getSessionIpAddr();
    console_mac = ipmiSession[idx].getSessionMacAddr();
    console_port = ipmiSession[idx].getSessionPort();

    memcpy(data, console_ip.data(), sizeof(uint8_t) * console_ip.size());
    data += sizeof(uint8_t) * console_ip.size();
    memcpy(data, console_mac.data(), sizeof(uint8_t) * console_mac.size());
    data += sizeof(uint8_t) * console_mac.size();
    *data++ = console_port & 0xff;
    *data++ = (console_port >> 8) & 0xff;
  } else
    res->cc = CC_UNSPECIFIED_ERROR;
  *res_len = data - &res->data[0];
}

void Ipmiapplication::app_bmc_selftest(uint8_t *response, uint8_t *res_len) {
  cout << "app_bmc_selftest" << endl;

  ipmi_res_t *res = (ipmi_res_t *)response;
  unsigned char *data = &res->data[0];
  res->cc = CC_SUCCESS;

  *data++ = 0x55; // self-test result
  *data++ = 0x00; // Extra error info in case of failure
  *res_len = data - &res->data[0];
}

void Ipmiapplication::app_clear_message_flag(uint8_t *request,
                                             uint8_t *response,
                                             uint8_t *res_len) {
  cout << "app_clear_message_flag" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t param = req->data[0];
  uint8_t oem[3] = {((param & 0x20) >> 5), ((param & 0x40) >> 6),
                    ((param & 0x80) >> 7)};
  uint8_t f_watchdog = (param & 0x08) >> 3;
  uint8_t f_evtmsg = (param & 0x02) >> 1;
  uint8_t f_recvmsg = param & 0x1;

  res->cc = CC_SUCCESS;
  *res_len = 0;
}

void Ipmiapplication::app_cold_reset(uint8_t *response, uint8_t *res_len) {
  cout << "app_cold_reset" << endl;
  ipmi_res_t *res = (ipmi_res_t *)response;
  res->cc = CC_SUCCESS;
  system("reboot");
  *res_len = 0;
}

void Ipmiapplication::app_warm_reset(uint8_t *response, uint8_t *res_len) {
  cout << "app_warm_reset" << endl;
  system("systemctl restart KETI-Edge");
  ipmi_res_t *res = (ipmi_res_t *)response;
  res->cc = CC_SUCCESS;
  *res_len = 0;
}

void Ipmiapplication::app_set_global_enables(uint8_t *request,
                                             uint8_t *response,
                                             uint8_t *res_len) {
  cout << "app_set_global_enables" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];
  KETI_define::global_enabler = static_cast<int>(req->data[0]);
  // 저장
  plat_globalenable_save();
  res->cc = CC_SUCCESS;
  *res_len = 0;
}

void Ipmiapplication::app_get_global_enables(uint8_t *response,
                                             uint8_t *res_len) {
  cout << "app_get_global_enables" << endl;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];

  res->cc = CC_SUCCESS;

  *data++ = KETI_define::global_enabler;
  ;
  *res_len = data - &res->data[0];
}

void Ipmiapplication::app_get_device_guid(uint8_t *response, uint8_t *res_len) {
  cout << "app_get_device_guid" << endl;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];

  res->cc = CC_SUCCESS;

  for (int i = 3; i >= 0; i--)
    *data++ = uuid_hex[i];
  *data++ = uuid_hex[5];
  *data++ = uuid_hex[4];
  *data++ = uuid_hex[7];
  *data++ = uuid_hex[6];

  for (int i = 8; i < 16; i++)
    *data++ = uuid_hex[i];

  *res_len = data - &res->data[0];
}

void Ipmiapplication::app_close_session(uint8_t *request, uint8_t *response,
                                        uint8_t *res_len) {
  cout << "app_close_session" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];

  std::vector<uint8_t> reqSessionId(req->data, req->data + 4);
  std::vector<uint8_t> currentSessionId;
  int index = 0;

  res->cc = CC_UNSPECIFIED_ERROR;

  for (int i = 4; i >= 0; i--) {
    currentSessionId = ipmiSession[i].getSessionMgntId();

    if (currentSessionId == reqSessionId) {
      index = i;
      res->cc = CC_SUCCESS;
      break;
    }
  }

  ipmiSession[index].closeSession();

  *res_len = 16;
}

void Ipmiapplication::app_set_session_priv(uint8_t *request, uint8_t *response,
                                           uint8_t *res_len) {
  cout << "app_set_session_priv" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];
  uint8_t req_priv = req->data[0];
  int ret = 0;
  for (int i = 4; i >= 0; i--) {
    uint8_t handle = ipmiSession[i].getSessionHandle();
    if (handle >= 32) {
      ret = ipmiSession[i].setSessionPriv(req_priv);

      if (ret == 0) {
        res->cc = CC_SUCCESS;
        *data++ = req_priv;
      } else if (ret == -1)
        res->cc = 0x80;
      else {
        res->cc = 0x81;
      }
      break;
    }
  }
  *res_len = data - &res->data[0];
}

void Ipmiapplication::app_set_channel_access(uint8_t *request,
                                             uint8_t *response,
                                             uint8_t *res_len) {
  cout << "app_set_channel_access" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  uint8_t ch_num = req->data[0] & 0x0f;

  ipmiChannel[ch_num].setChannelAccess(req);

  res->cc = CC_SUCCESS;
  *res_len = 0;
}

void Ipmiapplication::app_get_channel_access(uint8_t *request,
                                             uint8_t *response,
                                             uint8_t *res_len) {
  cout << "app_get_channel_access" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  uint8_t ch_num = req->data[0] & 0x0f;

  ipmiChannel[ch_num].getChannelAccess(res, res_len);
}
/**
 *
 * @author KICHEOL
 * @bug event 를 위한 channel은 14인데 현재는 0~8까지의 channel만보유 따라서
 * 임시로 ipmichannel[0]으로 수정 오류 발생예정
 */
void Ipmiapplication::app_get_channel_info(uint8_t *request, uint8_t *response,
                                           uint8_t *res_len) {
  cout << "app_get_channel_info" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];
  uint8_t param = req->data[0];
  uint8_t ch_num = param & 0x0f;
  printf("chnum : %d\n", ch_num);
  if (ch_num <= 8) {
    ipmiChannel[ch_num].getChannelInfo(res, res_len, ch_num);
    res->cc = CC_SUCCESS;
  } else {
    ipmiChannel[0].getChannelInfo(res, res_len, ch_num);
    // res->cc = CC_UNKNOWN;
  }
}

void Ipmiapplication::app_get_channel_auth_cap(uint8_t *request,
                                               uint8_t *response,
                                               uint8_t *res_len) {
  cout << "app_get_channel_auth_cap" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  // //기철 수정
  // unsigned char *data = &res->data[0];
  // res->cc = CC_SUCCESS;
  // //

  uint8_t chan = (req->data[0] % 0x10);
  uint8_t priv = (int)(req->data[1] % 0x10);
  uint8_t lan_chan = 0;
  uint8_t flag_not_null_username = 0;
  uint8_t flag_null_username = 0;
  uint8_t flag_anon_login = 0;

  // 기철 수정
  cout << "app_get_channel_auth_cap channel count =" << (int)chan << endl;
  switch (chan) {
  case 1:
    lan_chan = 0;
    break;
  case 6:
    lan_chan = 1;
    break;
  case 7:
    lan_chan = 2;
    break;
  case 8:
    lan_chan = 3;
    break;
  default:
    lan_chan = 0;
  }
  cout << "lan_chan = " << lan_chan << endl;
  for (int i = 0; i < 10; i++) {
    if (ipmiUser[i].getUserenable() == 1) {
      if (ipmiUser[i].getUsername() != "") {
        flag_not_null_username = 1;
        break;
      }
    }
  }
  cout << "getUserenable " << endl;
  if (ipmiUser[0].getUserenable() == 1) {
    if (ipmiUser[0].getUsername() == "") {
      if (ipmiUser[0].getUserpassword() == "") {
        flag_null_username = 1;
      } else {
        flag_anon_login = 1;
      }
    }
  }
  cout << "username " << endl;
  if (flag_null_username == 0)
    this->g_authrsp[lan_chan].null_usernames = 0b0;
  else
    this->g_authrsp[lan_chan].null_usernames = 0b1;

  if (flag_not_null_username == 0)
    this->g_authrsp[lan_chan].non_null_usernames = 0b0;
  else
    this->g_authrsp[lan_chan].non_null_usernames = 0b1;

  if (flag_anon_login == 0)
    this->g_authrsp[lan_chan].anon_login_enabled = 0b0;
  else
    this->g_authrsp[lan_chan].anon_login_enabled = 0b1;

  res->cc = CC_SUCCESS;
  memcpy(res->data, &this->g_authrsp[lan_chan], sizeof(channel_auth_cap_t));

  *res_len = sizeof(channel_auth_cap_t);
  cout << "close app_get_channel_auth_cap" << endl;
  cout << "close app_get_channel_auth_cap" << endl;
}

void Ipmiapplication::app_get_device_id(uint8_t *response, uint8_t *res_len) {
  cout << "app_get_device_id" << endl;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t *data = &res->data[0];

  res->cc = CC_SUCCESS;

  *data++ = this->g_mc_device.mc_device_id;
  *data++ = this->g_mc_device.mc_device_rev;
  *data++ = this->g_mc_device.mc_fw_version[0];
  *data++ = this->g_mc_device.mc_fw_version[1];
  *data++ = this->g_mc_device.mc_ipmi_version;
  *data++ = this->g_mc_device.mc_additional_dev;
  *data++ = this->g_mc_device.mc_mnfr_id[0];
  *data++ = this->g_mc_device.mc_mnfr_id[1];
  *data++ = this->g_mc_device.mc_mnfr_id[2];
  *data++ = this->g_mc_device.mc_prod_id[0];
  *data++ = this->g_mc_device.mc_prod_id[1];
  *data++ = this->g_mc_device.mc_aux_fw_version[0];
  *data++ = this->g_mc_device.mc_aux_fw_version[1];
  *data++ = this->g_mc_device.mc_aux_fw_version[2];
  *data++ = this->g_mc_device.mc_aux_fw_version[3];

  cout << "Get Device Info" << endl;
  printf("mc_device_id : 0x%02x\n", this->g_mc_device.mc_device_id);
  printf("mc_device_rev : 0x%02x\n", this->g_mc_device.mc_device_rev);
  printf("mc_fw_version[0] : 0x%02x \n", this->g_mc_device.mc_fw_version[0]);
  printf("mc_fw_version[1] : 0x%02x\n", this->g_mc_device.mc_fw_version[1]);
  printf("mc_ipmi_version : 0x%02x \n", this->g_mc_device.mc_ipmi_version);
  printf("mc_additional_dev : 0x%02x \n", this->g_mc_device.mc_additional_dev);
  printf("mc_mnfr_id : 0x%02x \n", this->g_mc_device.mc_mnfr_id[0]);
  printf("mc_mnfr_id : 0x%02x \n", this->g_mc_device.mc_mnfr_id[1]);
  printf("mc_mnfr_id : 0x%02x \n", this->g_mc_device.mc_mnfr_id[2]);
  printf("mc_prod_id : 0x%02x \n", this->g_mc_device.mc_prod_id[0]);
  printf("mc_prod_id : 0x%02x \n", this->g_mc_device.mc_prod_id[1]);
  printf("mc_aux_fw_version : 0x%02x \n",
         this->g_mc_device.mc_aux_fw_version[0]);
  printf("mc_aux_fw_version : 0x%02x \n",
         this->g_mc_device.mc_aux_fw_version[1]);
  printf("mc_aux_fw_version : 0x%02x \n",
         this->g_mc_device.mc_aux_fw_version[2]);
  printf("mc_aux_fw_version : 0x%02x \n",
         this->g_mc_device.mc_aux_fw_version[3]);

  *res_len = data - &res->data[0];
}

void Ipmiapplication::ipmi_handle_app(uint8_t *request, uint8_t *response,
                                      uint8_t *res_len) {
  cout << "ipmi_handle_app" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;

  uint8_t cmd = req->cmd;
  pthread_mutex_lock(&m_app);
  switch (cmd) {
  case CMD_APP_SET_WDT:
    app_set_watchdog_timer_params(request, response, res_len);
    WriteConfigurationFile(ConfigurationFileDir);
    break;
  case CMD_APP_GET_WDT:
    app_get_watchdog_timer_params(request, response, res_len);
    break;
  case CMD_APP_GET_DEVICE_ID:
    app_get_device_id(response, res_len);
    break;
  case CMD_APP_GET_CHANNEL_AUTH_CAP:
    app_get_channel_auth_cap(request, response, res_len);
    cout << "CMD_APP_GET_CHANNEL_AUTH_CAP" << endl;
    cout << "CMD_APP_GET_CHANNEL_AUTH_CAP" << endl;
    break;
  case CMD_APP_SET_SESSION_PRIV:
    app_set_session_priv(request, response, res_len);
    break;
  case CMD_APP_CLOSE_SESSION:
    app_close_session(request, response, res_len);
    break;
  case CMD_APP_GET_SYSTEM_GUID:
    app_get_device_guid(response, res_len);
    break;
  case CMD_APP_SET_GLOBAL_ENABLES:
    app_set_global_enables(request, response, res_len);
    break;
  case CMD_APP_GET_GLOBAL_ENABLES:
    app_get_global_enables(response, res_len);
    break;
  case CMD_APP_COLD_RESET:
    app_cold_reset(response, res_len);
    break;
  case CMD_APP_WARM_RESET:
    app_warm_reset(response, res_len);
    break;
  case CMD_APP_CLEAR_MESSAGE_FLAGS:
    app_clear_message_flag(request, response, res_len);
    break;
  case CMD_APP_GET_SELFTEST_RESULTS:
    app_bmc_selftest(response, res_len);
    break;
  case CMD_APP_GET_SESSION_INFO:
    app_get_session_info(request, response, res_len);
    break;
  case CMD_APP_GET_CHANNEL_INFO:
    app_get_channel_info(request, response, res_len);
    break;
  case CMD_APP_GET_CHANNEL_ACCESS:
    app_get_channel_access(request, response, res_len);
    break;
  case CMD_APP_GET_USER_NAME:
    app_get_username(request, response, res_len);
    break;
  case CMD_APP_GET_USER_ACCESS:
    app_get_useraccess(request, response, res_len);
    break;
  case CMD_APP_SET_USER_NAME:
    app_set_username(request, response, res_len);
    break;
  case CMD_APP_SET_USER_ACCESS:
    app_set_useraccess(request, response, res_len);
    break;
  case CMD_APP_SET_USER_PASSWD:
    app_set_user_passwd(request, response, res_len);
    break;
  case CMD_APP_SET_SYS_INFO_PARAMS:
    break;
  case CMD_APP_GET_SYS_INFO_PARAMS:
    break;
  case CMD_APP_SOL_ACTIVE_PAYLOAD:
    log(info) << "start CMD_APP_SOL_ACTIVE_PAYLOAD";
    (SerialOverLan::Instance().app_active_payload(req, res, res_len));
    log(info) << "end CMD_APP_SOL_ACTIVE_PAYLOAD";
    break;
  case CMD_APP_SOL_DEACTIVATE_PAYLOAD:
    log(info) << "start CMD_APP_SOL_DEACTIVATE_PAYLOAD";
    (SerialOverLan::Instance().app_deactive_payload(req, res, res_len));
    log(info) << "end CMD_APP_SOL_DEACTIVATE_PAYLOAD";
    break;
  default:
    res->cc = 0xfe;
    break;
  }
  pthread_mutex_unlock(&m_app);
}
#define IPMI_EVENT_LOG_VIEW 1
static void ipmi_handle_sensor(ipmi_req_t *request, ipmi_res_t *response,
                               uint8_t *res_len) {
  cout << "ipmi_handle_sensor" << endl;
  uint8_t cmd = request->cmd;
  log(info) << "cmd =" << std::hex << cmd;
  response->cc = CC_SUCCESS;

  switch (cmd) {
  case CMD_SENSOR_GET_SENSOR_READING:
    sensor_get_reading(request, response, res_len);
    break;
  case CMD_SENSOR_GET_SENSOR_THRESHOLD:
    sensor_get_threshod(request, response, res_len);
    break;
  case CMD_SENSOR_SET_SENSOR_THRESHOLD:
    sensor_set_threshold(request, response, res_len);
    break;
  case CMD_SENSOR_GET_DEVICE_SDR_INFO:
    storage_get_sdr_info(response, res_len);
    break;
  case CMD_SENSOR_RES_DEVICE_SDR_REPOSITORY:
    storage_rsv_sdr(response, res_len);
    break;
#if IPMI_EVENT_LOG_VIEW
  case CMD_SENSOR_PLATFORM_EVENT: // !!rjs : for "event [1|2|3]"
    (sensor_event_get_num(request, response, res_len));
    // IPMI_Handle_Event::ipmi_handle_sel(request, response, res_len);
    response->cc = CC_SUCCESS;
    // storage_add_sel(request, response, res_len);
    // 정상동작확인
    break;
  case CMD_GET_PEF_CAPABILITIES:
    cout << "CMD_GET_PEF_CAPABILITIES" << endl;
    (pef_capabilities_info(response, res_len));
    response->cc = CC_SUCCESS;
    break;
  case CMD_SET_PEF_CONFIG_PARMS:
    cout << "CMD_SET_PEF_CONFIG_PARMS" << endl;
    (pef_set_config_param(request, response, res_len));
    response->cc = CC_SUCCESS;
    break;
  case CMD_GET_PEF_CONFIG_PARMS:
    cout << "CMD_GET_PEF_CONFIG_PARMS" << endl;
    (pef_get_config_param(request, response, res_len));
    break;

  case CMD_GET_LAST_PROCESSED_EVT_ID:
    cout << "CMD_GET_LAST_PROCESSED_EVT_ID" << endl;
    (pef_get_status(response, res_len));
    response->cc = CC_SUCCESS;
    break;
#endif
  default:
    response->cc = CC_INVALID_CMD;
    break;
  }
}

static void ipmi_handle_storage(ipmi_req_t *request, ipmi_res_t *response,
                                uint8_t *res_len) {
  cout << "ipmi_handle_storage" << endl;
  uint8_t cmd = request->cmd;
  int s_ret = 0;
  switch (cmd) {
  case CMD_STORAGE_CLR_SDR_REPO:
    cout << "CMD_STORAGE_CLR_SDR_REPO" << endl;
    storage_clear_sdr_repository(response, res_len);
    break;
  case CMD_STORAGE_GET_SDR_INFO:
    cout << "CMD_STORAGE_GET_SDR_INFO" << endl;
    storage_get_sdr_info(response, res_len);
    break;
  case CMD_STORAGE_RSV_SDR:
    cout << "CMD_STORAGE_RSV_SDR" << endl;
    storage_rsv_sdr(response, res_len);
    break;
  case CMD_STORAGE_GET_SDR:
    cout << "CMD_STORAGE_GET_SDR" << endl;
    storage_get_sdr(request, response, res_len);
    break;
  // add fru fru 핸들러 추가
  case CMD_STORAGE_GET_FRUID_INFO:
    cout << "CMD_STORAGE_GET_FRUID_INFO" << endl;
    storage_get_fru_info(request, response, res_len);
    break;
  case CMD_STORAGE_READ_FRUID_DATA:
    cout << "CMD_STORAGE_READ_FRUID_DATA" << endl;
    storage_get_fru(request, response, res_len);

    break;
  case CMD_STORAGE_WRITE_FRUID_DATA:
    cout << "CMD_STORAGE_WRITE_FRUID_DATA" << endl;
    break;
  case CMD_STORAGE_GET_SEL_INFO:
    cout << "CMD_STORAGE_GET_SEL_INFO DATA" << endl;
    storage_get_sel_info(response, res_len);
    break;
    // sel 파트
  case CMD_STORAGE_RSV_SEL:
    if ((KETI_define::global_enabler & 0x8) != 0x8) {
      response->cc = CC_DISABLED;
      break;
    }
    storage_rsv_sel(response, res_len);
    break;
  case CMD_STORAGE_ADD_SEL:
    cout << "CMD_STORAGE_ADD_SEL" << endl;
    if ((KETI_define::global_enabler & 0x8) != 0x8) {
      response->cc = CC_DISABLED;
      break;
    }
    cout << "BF storage_add_sel" << endl;
    storage_add_sel(request, response, res_len);
    cout << "AF storage_add_sel" << endl;
    break;
  case CMD_STORAGE_GET_SEL:
    if ((KETI_define::global_enabler & 0x8) != 0x8) {
      response->cc = CC_DISABLED;
      break;
    }
    storage_get_sel(request, response, res_len);
    break;
  case CMD_STORAGE_DEL_SEL_ENTRY:
    if ((KETI_define::global_enabler & 0x8) != 0x8) {
      response->cc = CC_DISABLED;
      break;
    }
    storage_del_sel_entry(request, response, res_len);
    break;
  case CMD_STORAGE_CLR_SEL:
    if ((KETI_define::global_enabler & 0x8) != 0x8) {
      response->cc = CC_DISABLED;
      break;
    }
    storage_clr_sel(request, response, res_len);
    break;
    /////////////////////
  }
}

// 	CMD_STORAGE_DEL_SEL_ENTRY = 0x46,
// 	CMD_STORAGE_CLR_SEL = 0x47,
// 	CMD_STORAGE_GET_SEL_TIME = 0x48,
// 	CMD_STORAGE_SET_SEL_TIME = 0x49,
// 	CMD_STORAGE_GET_SEL_UTC = 0x5C,
static void ipmi_handle_transport(ipmi_req_t *request, ipmi_res_t *response,
                                  uint8_t *res_len) {
  cout << "ipmi_handle_transport start" << endl;
  uint8_t cmd = request->cmd;
  uint8_t chan = request->data[0] & 0xf;

  uint8_t r_chan = 0;
  printf("chan = %d\n", chan);
  switch (chan) {
  case 1:
    r_chan = 0;
    break;
  case 6:
    r_chan = 1;
    break;
  case 7:
    r_chan = 2;
    break;
  case 8:
    r_chan = 3;
    break;
  }
  printf("r_chan = %d\n", r_chan);
  switch (cmd) {
  case CMD_TRANSPORT_SET_LAN_CONFIG:
    ipmiNetwork[r_chan].set_lan_config(request, response, res_len);
    break;
  case CMD_TRANSPORT_GET_LAN_CONFIG:
    ipmiNetwork[r_chan].get_lan_config(request, response, res_len);
    break;
  case CMD_TRANSPORT_GET_SOL_CONFIG_PARAMETERS: {
    log(info) << "=======CMD_TRANSPORT_GET_SOL_CONFIG_PARAMETERS ========";
    SerialOverLan::Instance().transport_get_sol_config_params(request, response,
                                                              res_len);
    log(info) << "=======end ========";
    break;
  case CMD_TRANSPORT_SET_SOL_CONFIG_PARAMETERS: {
    log(info) << "=======CMD_TRANSPORT_GET_SOL_CONFIG_PARAMETERS ========";
    SerialOverLan::Instance().transport_set_sol_config_params(request, response,
                                                              res_len);
    log(info) << "=======end ========";
    break;
  }
  }
  default:
    response->cc = CC_INVALID_CMD;
    break;
  }
  cout << "Leave apps.cpp:ipmi_handle_transport \n" << endl;
}

static void ipmi_handle_chassis(ipmi_req_t *request, ipmi_res_t *response,
                                uint8_t *res_len) {
  uint8_t cmd = request->cmd;
  switch (cmd) {
  case CMD_CHASSIS_POH:

    ipmiChassis.chassis_get_poh(request, response, res_len);
    break;
  case CMD_CHASSIS_GET_STATUS:
    ipmiChassis.chassis_get_status(request, response, res_len);
    break;
  case CMD_CHASSIS_CONTROL:
    ipmiChassis.chassis_control(request, response, res_len);
    redfish_m_cond[MU_CHASSIS].notify_one();
    break;
  case CMD_CHASSIS_IDENTIFY:
    ipmiChassis.chassis_identify(request, response, res_len);
    break;
  case CMD_CHASSIS_POWER_POLICY:
    ipmiChassis.chassis_set_policy(request, response, res_len);
    break;
  case CMD_CHASSIS_RESTART_CAUSE:
    ipmiChassis.chassis_restart_cause(request, response, res_len);
    break;
  case CMD_CHASSIS_SET_BOOT_PARAM:
    ipmiChassis.chassis_set_boot_options(request, response, res_len);
    break;
  case CMD_CHASSIS_GET_BOOT_OPTIONS:
    ipmiChassis.chassis_get_boot_options(request, response, res_len);
    break;
  case 99:
    cout << "error" << endl;
    break;
  default:
    response->cc = 0xfe;
    break;
  }
}

uint8_t ipmi_handle(uint8_t bRMCP, uint8_t *request, uint8_t req_len,
                    uint8_t *response, uint8_t *res_len) {
  // cout << "=========ipmi_handle==========" << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  uint8_t netfn;
  std::vector<uint8_t> data(req->data, req->data + req_len);

  if (bRMCP)
    netfn = req->netfn_lun;
  else
    netfn = req->netfn_lun >> 2;

  res->cmd = req->cmd;
  res->cc = 0xFF;
  *res_len = 0;
  cout << "ipmi_handle:netfn = " << std::hex << (int)netfn << endl;
  cout << "ipmi_handle:req cmd = " << std::hex << (int)req->cmd << endl;
  // cout << "ipmi_handle:res cmd = " << std::hex << (int)res->cmd << endl;
  cout << "ipmi_handle:req_t data = ";
  // for(int i=0; i<req->netfn_lun;i++)
  // 	cout<<(char)req->data[i];
  switch (netfn) {
  case NETFN_CHASSIS_REQ:
    res->netfn_lun = NETFN_CHASSIS_RES << 2;
    ipmi_handle_chassis(req, res, res_len);
    break;
  case NETFN_STORAGE_REQ:
    res->netfn_lun = NETFN_STORAGE_RES << 2;
    ipmi_handle_storage(req, res, res_len);
    break;
  case NETFN_SENSOR_REQ:
    res->netfn_lun = NETFN_SENSOR_RES << 2;
    ipmi_handle_sensor(req, res, res_len);
    break;
  case NETFN_TRANSPORT_REQ:
    res->netfn_lun = NETFN_TRANSPORT_RES << 2;
    ipmi_handle_transport(req, res, res_len);
    break;
  case NETFN_APP_REQ:
    res->netfn_lun = NETFN_APP_RES << 2;
    ipmiApplication.ipmi_handle_app(request, response, res_len);
    break;
  case NETFN_DCMI_REQ:
    res->netfn_lun = NETFN_DCMI_RES << 2;
    ipmi_handle_dcmi(request, response, res_len);
    break;
    // case 0x05: //KCS
    //   // res->netfn_lun = 0x0b <<2;
    //   IPMI_Event_handle::ipmi_event_handle(req, res, res_len);
    //   break;
  }
  *res_len += SIZE_IPMI_RES_HDR;
  return response[2];
}
int app_getUserPriv(std::string _username, uint8_t index) {
  cout << "app_getUserPriv" << endl;
  if (index < 10) {
    if (ipmiUser[index].getUsername() == _username)
      return ipmiUser[index].getUserPriv();
  } else
    return 0;
}

std::string app_getUserpassword(uint8_t index) {
  cout << "app_getUserpassword" << endl;
  return ipmiUser[index].getUserpassword();
}

int app_getUserindex(std::string _username) {
  cout << "app_getUserindex" << endl;
  for (int i = 0; i < 10; i++) {
    if (ipmiUser[i].getUsername() == _username) {
      if (ipmiUser[i].getUserenable() == 1) {
        return i;
      } else
        return 0;
    }
  }
  return 0;
}
/**
 *
 * @brief main of project
 * @details ipmi sensor들 초기화 NVA PDPB PEB 센서
 * @bug PDPB는 아직 핀맵이 정의되지 않아 제거됨
 */
void plat_ipmi_init(void) {
  log(info) << "plat_ipmi_init start";
  char g_host_name[20];
  char g_kernel_version[20];

  FILE *pp = fopen("/proc/sys/kernel/osrelease", "r");

  fgets(g_kernel_version, sizeof(char) * 20, pp);
  fclose(pp);
  log(info) << "plat_ipmi_init 1";
  uint8_t chan = 0;
  unsigned char buff[100] = {
      0,
  };
  FILE *fp = fopen("/etc/hostname", "r");
  fgets(buff, 100, fp);
  fclose(fp);
  strcpy(g_host_name, buff);
  log(info) << "plat_ipmi_init 2";
  g_host_name[strlen(g_host_name) - 1] = '\0';

  g_kernel_version[strlen(g_kernel_version) - 1] = '\0';
  ipmiApplication.g_host_name = (string)g_host_name;
  ipmiApplication.g_kernel_version = (string)g_kernel_version;
  log(info) << "plat_ipmi_init 3";
  get_build_time(g_firmware_build_time);
  log(info) << "plat_ipmi_init 3-1";
  for (int i = 0; i < 10; i++)
    ipmiUser[i] = Ipmiuser();
  log(info) << "plat_ipmi_init 3-2";
  log(info) << "current user num : " << user_loading();

  KETI_define::global_enabler = 8;
  // sel dcmi init !

  plat_sel_init();
  log(info) << "plat_ipmi_init 4";
  plat_dcmi_init();
  log(info) << "plat_ipmi_init 5";
  // alert_init
  plat_lan_alert_init();
  log(info) << "plat_ipmi_init 6";

  // global enable loading

  t_lanMonitor[0] = std::thread([&]() { ipmiNetwork[0].lanMonitorHandler(); });
  t_lanMonitor[1] = std::thread([&]() { ipmiNetwork[1].lanMonitorHandler(); });
  t_lanMonitor[2] = std::thread([&]() { ipmiNetwork[2].lanMonitorHandler(); });
  t_lanMonitor[3] = std::thread([&]() { ipmiNetwork[3].lanMonitorHandler(); });
  log(info) << "plat_ipmi_init 7";
  uint8_t rec_id = 0;
  std::map<uint8_t, Ipmisdr> inner;

  for (int i = 0; i < PEB_SENSOR_COUNT; i++) {
    inner.insert(std::make_pair(
        rec_id, Ipmisdr(rec_id, peb_sensors[i].sensor_num, peb_sensors[i])));
    sdr_rec.insert(std::make_pair(rec_id, inner));
    rec_id++;
  }
  // AST2600a3 포팅과정 LPC 초기화 이후 IPMB를통한 데이터 init필요
  for (int i = 0; i < PDPB_SENSOR_COUNT; i++) {
    inner.insert(std::make_pair(
        rec_id, Ipmisdr(rec_id, pdpb_sensors[i].sensor_num, pdpb_sensors[i])));
    sdr_rec.insert(std::make_pair(rec_id, inner));
    rec_id++;
  }
  // sysfan part so .. not include N520
  //  for (int i = 0; i < NVA_SENSOR_COUNT; i++) {
  //    inner.insert(std::make_pair(
  //        rec_id, Ipmisdr(rec_id, nva_sensors[i].sensor_num,
  //        nva_sensors[i])));
  //    sdr_rec.insert(std::make_pair(rec_id, inner));

  //   rec_id++;
  // }
}
/**
 * @brief 로그인을 인증시 패스워드, 유저이름이 있는지 확인
 * @param _username RESTful로 받은 json에서 USERNAME
 * @param _password RESTful로 받은 json에서 PASSWORD
 */
int authenticate_ipmi(std::string _username, std::string _password) {
  for (int i = 0; i < 10; i++) {
    // ipmiUser[i].printUserInfo();
    if (ipmiUser[i].getUsername() == _username) {

      int result =
          strcmp(ipmiUser[i].getUserpassword().c_str(), _password.c_str());
      // if (ipmiUser[i].getUserpassword() == _password) {
      if (result == 0) {
        if (ipmiUser[i].getUserenable() == 1)
          return ipmiUser->getUserPriv();
        cout << "\t return true authenticate_ipmi end" << endl;
        return 0;
      }
    }
  }
  log(warning) << "[...]cannot find user";
  return 0;
}
/**
 * @brief LDAP 클라이언트 인증 연동 설정
 * @param _username RESTful로 받은 json에서 USERNAME
 * @param _password RESTful로 받은 json에서 PASSWORD
 * @return 2 or 0
 * @todo 어떤 작업을 수행하는지 파악중
 */
int authenticate_ldap(std::string _username, std::string _password) {
  puts("auth ldap");
  char cmd[64];
  sprintf(cmd, "echo %s | auth %s ldap", _password.c_str(), _username.c_str());
  if (system(cmd) == 0) {
    return 2;
  }
  return 0;
}
/**
 * @brief auth active dir
 * @param _username RESTful로 받은 json에서 USERNAME
 * @param _password RESTful로 받은 json에서 PASSWORD
 * @return 2 or 0
 * @todo 어떤 작업을 수행하는지 파악중
 */
int authenticate_ad(std::string _username, std::string _password) {
  puts("auth active dir");
  char cmd[64];
  sprintf(cmd, "echo %s | auth %s ldap", _password.c_str(), _username.c_str());
  if (system(cmd) == 0) {
    return 2;
  }
  return 0;
}

void get_temp_cpu0(json::value &JCPU) {
  unsigned char status = 1;
  status = ipmiChassis.get_power_status();
  if (status == 1) {
    unsigned char data = 0;
    data = sdr_sensor_read(PDPB_SENSOR_TEMP_CPU0);
    JCPU["VALUE"] = json::value::string(to_string(data));
  }

  return;
}
/**
 * @brief Get
 *
 * @param JCPU
 */
void get_temp_cpu1(json::value &JCPU) {
  unsigned char status = 1;
  status = ipmiChassis.get_power_status();
  if (status == 1) {
    unsigned char data = 0;
    // data = sdr_sensor_read(PDPB_SENSOR_TEMP_CPU1);
    // cout << ("data =") << data << endl;
    JCPU["VALUE"] = json::value::string(to_string(data));
  }

  return;
}
/**
 * @brief get_voltage_fan_power
 * @bug	 POWER FAN이 없음
 * @param JPOWER
 * @bug 기철 버그
 * @return * void
 */
void get_voltage_fan_power(json::value &JPOWER) {
  int pow1_vol = 300;
  int pow2_vol = 100;
  int pow1_fan = 0;
  int pow2_fan = 0;

  int psu1data = sdr_sensor_read(PEB_SENSOR_ADC_P12V_PSU1);
  int psu2data = sdr_sensor_read(PEB_SENSOR_ADC_P12V_PSU2);
  JPOWER["POWER1_VOLTAGE"] = json::value::string(to_string(psu1data * 60));
  JPOWER["POWER2_VOLTAGE"] = json::value::string(to_string(psu2data * 60));
  JPOWER["POWER1_FAN"] = json::value::string(to_string(pow1_fan));
  JPOWER["POWER2_FAN"] = json::value::string(to_string(pow2_fan));

  return;
}
/**fan삭제 요청으로인한 임시제거 PKC*/
void get_fans(json::value &JFANS) {
  int fan[5] = {
      0,
  };

  // fan[0] = sdr_sensor_read(NVA_SYSTEM_FAN1) * 100;
  // fan[1] = sdr_sensor_read(NVA_SYSTEM_FAN2) * 100;
  // fan[2] = sdr_sensor_read(NVA_SYSTEM_FAN3) * 100;
  // fan[3] = sdr_sensor_read(NVA_SYSTEM_FAN4) * 100;
  // fan[4] = sdr_sensor_read(NVA_SYSTEM_FAN5) * 100;
  for (int i = 1; i <= 5; i++) {
    char fanstr[5] = {
        0,
    };
    sprintf(fanstr, "FAN%d", i);
    JFANS[fanstr] = json::value::string(to_string(fan[i - 1]));
  }

  return;
}
/**
 * @brief 보드 온도 측정
 * @details PDPB_SENSOR_TEMP_LEFT_REAR에서 PDPB_SENSOR_TEMP_REAR_RIGHT로 변경
 * @author 기철
 **/
void get_temp_board(json::value &JBOARD_TEMP) {
  double temp;
  DBus::BusDispatcher dispatcher;
  DBus::default_dispatcher = &dispatcher;
  DBus::Connection conn_n = DBus::Connection::SystemBus();
  DBus_Sensor apps_dbus_sensor =
      DBus_Sensor(conn_n, SERVER_PATH_1, SERVER_NAME_1);
  temp = apps_dbus_sensor.read_lm75_value(1);

  JBOARD_TEMP["VALUE"] = json::value::string(to_string(temp));
  dispatcher.leave();

  return;
}
/**
 * @brief REST  system정보 LAN정보
 * @bug  net_priority= 8로 고정되어있는 문제 특별한 문제 없으면 사용가능 kernel
 * name 에러
 *
 */
void get_sys_info(json::value &JSYS_INFO) {

  string temp;
  string _uri;
  json::value subobj = json::value::object();
  _uri = ODATA_ETHERNET_INTERFACE_ID + string("/NIC") + to_string(1);

  char ip_str[150], mac_str[150];
  sprintf(mac_str, "%x.%x.%x.%x.%x.%x", ipmiNetwork[0].mac_addr[0],
          ipmiNetwork[0].mac_addr[1], ipmiNetwork[0].mac_addr[2],
          ipmiNetwork[0].mac_addr[3], ipmiNetwork[0].mac_addr[4],
          ipmiNetwork[0].mac_addr[5]);
  sprintf(ip_str, "%u.%u.%u.%u", ipmiNetwork[0].ip_addr[0],
          ipmiNetwork[0].ip_addr[1], ipmiNetwork[0].ip_addr[2],
          ipmiNetwork[0].ip_addr[3]);
  subobj["IPV4_ADDRESS"] = json::value::string(U(ip_str));

  // subobj["IPV4_NETWORK_MODE"] = json::value::string("DHCP");

  subobj["IPV4_NETWORK_MODE"] = json::value::string("Static");
  subobj["MAC_ADDRESS"] = json::value::string(mac_str);
  subobj["IPV6_NETWORK_MODE"] = json::value::string(U("Static"));
  // subobj["IPV6_ADDRESS"] = json::value::string(U(ether->v_ipv6[0].address));

  JSYS_INFO["LAN"] = subobj;

  subobj = json::value::object();
  subobj["IPMIFW_VERSION"] = json::value::string(U(IPMI_VERSION));
  subobj["IPMIFW_BLDTIME"] =
      json::value::string(U(std::string(g_firmware_build_time)));
  subobj["KERNAL_VERSION"] =
      json::value::string(U(std::string(ipmiApplication.g_kernel_version)));
  JSYS_INFO["MC"] = subobj;
  // JSYS_INFO["LAN"] = jv_network;

  return;
}
#define MAX_CPU 2
#define MAX_PSU 2
#define MAX_CPU0_MEM 12
#define MAX_CPU1_MEM 12
#define MAX_FAN 9
/**
 * @brief Get the health summary object
 * @bug 2020-04-08 psu 포팅문제로 인해 주석처리
 * @param jhealth_summary
 */
void get_health_summary(json::value &jhealth_summary) {
  std::vector<json::value> JVCPU, JCPU0_MEMORY, JCPU1_MEMORY, JPOWER, JFAN;
  char buf[64];
  int snr_id, idx;
  int index = 0;
  sensor_thresh_t *p_sdr, *t_sdr;
  int sensor_index = -1, namelen = -1;
  sensor_thresh_t temp;
  string sensorname;

  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH0_DIMM0);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM1_CH_B"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM1_CH_B"] = json::value::number(0);

  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH0_DIMM1);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM1_CH_A"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM1_CH_A"] = json::value::number(0);

  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH0_DIMM2);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM1_CH_C"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM1_CH_C"] = json::value::number(0);
  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH1_DIMM0);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM1_CH_D"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM1_CH_D"] = json::value::number(0);

  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH1_DIMM1);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM2_CH_A"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM2_CH_A"] = json::value::number(0);

  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH1_DIMM2);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM2_CH_B"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM2_CH_B"] = json::value::number(0);

  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH2_DIMM0);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM2_CH_C"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM2_CH_C"] = json::value::number(0);

  sensor_index = plat_find_sdr_index(PDPB_SENSOR_TEMP_CPU0_CH2_DIMM1);
  temp = sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
  if (temp.nominal > 0)
    jhealth_summary["DIMM2_CH_D"] = json::value::number(temp.nominal);
  else
    jhealth_summary["DIMM2_CH_D"] = json::value::number(0);

  vector<json::value> TEMP_ARRAY;
  for (snr_id = PDPB_SENSOR0_TEMP_LM75; snr_id <= PDPB_SENSOR4_TEMP_LM75;
       snr_id++) {
    sensor_index = plat_find_sdr_index(snr_id);
    temp =
        sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
    if (temp.nominal > 0)
      TEMP_ARRAY.push_back(json::value::number(temp.nominal));
    else
      TEMP_ARRAY.push_back(json::value::number(0));
  }

  jhealth_summary["Temperature"] = json::value::array(TEMP_ARRAY);

  return;
}
/**
 * @brief sNum에 해당하는 sensor_name 찾아줌
 * @return 1 성공
 * @return 0 실패
 * @param sType 센서종류
 * @param sNum 센서 인덱스
 * @param msg string msg 레퍼런스 센서이름을 반환함
 * @todo sdr sensor만 정상작동 SENSOR_TYPE_TEMPERATURE만 수행되는중 나머지부분
 */
int find_sensor_name(uint8_t sType, uint8_t sNum, string &msg) {
  int sensor_index = -1, namelen = -1;
  sensor_thresh_t temp;
  string sensorname;
  switch (sType) {
  case SENSOR_TYPE_TEMPERATURE: // temperature
    sensor_index = plat_find_sdr_index(sNum);
    temp =
        sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
    if (sensor_index < 0) {
      cout << "get_pef_alert_msg: can't find sensor." << endl;
      return 0;
    }
    sensorname = std::string(temp.str);
    break;
  case SENSOR_TYPE_VOLTAGE: // temperature
    sensor_index = plat_find_sdr_index(sNum);
    temp =
        sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
    if (sensor_index < 0) {
      cout << "get_pef_alert_msg: can't find sensor." << endl;
      return 0;
    }
    sensorname = std::string(temp.str);
    break;
  case SENSOR_TYPE_CURRENT: // temperature
    sensor_index = plat_find_sdr_index(sNum);
    temp =
        sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
    if (sensor_index < 0) {
      cout << "get_pef_alert_msg: can't find sensor." << endl;
      return 0;
    }
    sensorname = std::string(temp.str);
    break;
  case SENSOR_TYPE_FAN: // temperature
    sensor_index = plat_find_sdr_index(sNum);
    temp =
        sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
    if (sensor_index < 0) {
      cout << "get_pef_alert_msg: can't find sensor." << endl;
      return 0;
    }
    sensorname = std::string(temp.str);
    break;
  default:
    // Sensor Type 9= P12PSU
    // cout << "warning: find_sensor_name sensor s_Type"
    // <<static_cast<int>(sType)<< endl; sensor_index =
    // plat_find_sdr_index(sNum); temp =
    // sdr_rec[sensor_index].find(sensor_index)->second.get_sensor_thresh_t();
    // if (sensor_index < 0)
    // {
    // 	cout << "sensor s_Type: can't find sensor. sensor name =" <<temp.str<<
    // endl; 	return 0;
    // }
    // cout << "sensor name = " <<temp.str<< endl;
    break;
  }
  msg = sensorname;
  return 1;
}

void get_build_time(unsigned char *build_time) {

  char buildtime[50] = {
      0,
  };
  char unamev[100] = {
      0,
  };
  char cmds[20] = {
      0,
  };
  FILE *p = NULL;
  log(info) << "get_build_time 1";
  p = fopen("/proc/sys/kernel/version", "r");
  log(info) << "get_build_time 2";
  if (p)
    while (fgets(unamev, sizeof(unamev), p) != NULL)
      ;
  fclose(p);
  log(info) << "get_build_time 3";
  char *p_1 = NULL;
  p_1 = index(unamev, ' ');
  char *p_2 = NULL;
  p_2 = index(p_1 + 1, ' ');
  char *p_3 = NULL;
  p_3 = rindex(p_2, '\n');
  log(info) << "get_build_time 4";
  strncpy(g_firmware_build_time, p_2, strlen(p_2) - strlen(p_3));
  log(info) << "get_build_time 5";
}
/**
 * @brief redfish 및 rest를 통한 유저 추가
 */
int new_ipmi_user(string user_name, string password, bool enabled, int priv,
                  int index) {
  int count = index - 1;

  if (count > 10)
    return 0;

  Ipmiuser *user = &ipmiUser[count];
  cout << "new ipmi user " << count << endl;
  user->setUserEnable((uint8_t)enabled);
  user->setUserPasswd(password);
  user->setUserName(user_name);
  user->priv = priv;
  cout << "user add" << endl;
  plat_user_save();
  return 1;
}

/**
 * @brief get chassis power status, return json format string & size
 * @date 21.05.21
 * @author doyoung
 */
int rest_get_power_status(unsigned char *res) {
  json::value obj = json::value::object();
  json::value power = json::value::object();
  unsigned char status = 0;

  status = ipmiChassis.get_power_status();

  if (status == 1)
    power["STATUS"] = json::value::string(U("1"));
  else if (status == 0)
    power["STATUS"] = json::value::string(U("0"));
  else
    power["STATUS"] = json::value::string(U("Unknown")); // error

  obj["POWER"] = power;
  strncpy(res, obj.serialize().c_str(), obj.serialize().length());

  return obj.serialize().length();
}

void get_ddns_host_name(unsigned char *host_name) {
  unsigned char ddns_buf[100];

  FILE *fp;

  fp = popen("hostname", "r");
  if (fp == NULL) {
    perror("popen() failed");
    return;
  }

  while (fgets(ddns_buf, 100, fp))

    strncpy(host_name, ddns_buf, strlen(ddns_buf));
  pclose(fp);
}

void get_ddns_domain_name(unsigned char *domain_name) {
  FILE *ddns_fp;
  unsigned char ddns_buf[512];

  if (access(DOMAINNAME_FILE, F_OK) == 0) {
    ddns_fp = popen("hostname -f", "r");

    if (ddns_fp == NULL) {
      perror("popen() failed!\n");
    }
    while (fgets(ddns_buf, 100, ddns_fp))
      ddns_buf[strlen(ddns_buf) - 1] = '\0';
    strncpy(domain_name, ddns_buf, strlen(ddns_buf));
    pclose(ddns_fp);
  } else
    strcpy(domain_name, "-");
}

void get_ddns_nameserver(int flag, unsigned char *nameserver) {
  FILE *ddns_fp;
  unsigned char buffer[1024] = {0};
  unsigned char ddns_buf[100];
  int line_count = 0;
  int f_count = 0;

  if (access(DNSSERVER_FILE, F_OK) == 0) {
    ddns_fp = fopen(DNSSERVER_FILE, "r");
    while (feof(ddns_fp) == 0) {
      fread(buffer, sizeof(buffer), 1, ddns_fp);
    }
    if (strstr((char *)buffer, "nameserver") != NULL) {
      int i, dns_count = 0;

      while (i < strlen(buffer)) {
        if (buffer[i] == '\n')
          dns_count = dns_count + 1;
        i++;
      }

      char *n_server[dns_count];
      char *ptr = strtok(buffer, "\n");
      i = 0;

      while (ptr != NULL && i < dns_count) {
        n_server[i] = ptr;
        i++;
        ptr = strtok(NULL, "\n");
      }
      if (dns_count > 1) {
        char *ptrs = strtok(n_server[flag - 1], " ");
        ptrs = strtok(NULL, " ");
        strcpy(nameserver, ptrs);
      }

      else if (dns_count == 1) {
        if (flag == 1) {
          char *ptrs = strtok(n_server[flag - 1], " ");
          ptrs = strtok(NULL, " ");
          strcpy(nameserver, ptrs);
        } else {
          strcpy(nameserver, "-");
        }
      } else {
        strcpy(nameserver, "-");
      }

      fclose(ddns_fp);
    } else {
      strcpy(nameserver, "-");
    }
  }
}

int set_ddns_host_name(unsigned char *host_name) {
  unsigned char ddns_cmd[100] = {0};
  int rets = 0;
  set_ddns_domain_name(1, host_name);
  sprintf(ddns_cmd, "hostname %s\n", host_name);
  rets = system(ddns_cmd);

  if (rets == 0)
    return 0;
  else
    return 0;
}

int set_ddns_domain_name(int flag, unsigned char *domain_name) {
  unsigned char buffer[4096], t_buffer[100], t_cmd[100] = {0};
  FILE *ddns_fp;
  if (access(INTERFACE_FILE, F_OK) == 0) {
    ddns_fp = fopen(DOMAINNAME_FILE, "r");
    if (ddns_fp == NULL) {
      perror("fopen error!\n");
      return 0;
    }

    while (feof(ddns_fp) == 0) {
      fread(buffer, sizeof(buffer), 1, ddns_fp);
    }
    fclose(ddns_fp);

    int dns_count = 0;
    int i = 0;
    while (i < strlen(buffer)) {
      if (buffer[i] == '\n')
        dns_count += 1;
      i++;
    }
    if (dns_count != 0) {
      char hosts_line[dns_count][200];
      char *ptr = strtok(buffer, "\n");
      i = 0;

      while (ptr != NULL && i < dns_count) {
        strcpy(hosts_line[i], ptr);
        i++;
        ptr = strtok(NULL, "\n");
      }
      unsigned char host_name[50], dns_addr[50] = {0};

      if (flag == 1) {
        get_ddns_nameserver(1, dns_addr);
        FILE *dn_fp = popen("hostname -f", "r");
        if (dn_fp == NULL)
          perror("popen failed\n");

        while (fgets(host_name, 50, dn_fp))
          host_name[strlen(host_name) - 1] = '\0';
        pclose(dn_fp);
        sprintf(t_buffer, "%s\t%s\t%s\n", dns_addr, host_name, domain_name);
      }
      if (flag == 2) {
        FILE *p_fp = popen("hostname", "r");
        if (p_fp == NULL) {
          perror("popen failed");
          return 0;
        }
        while (fgets(host_name, 50, p_fp))
          host_name[strlen(host_name) - 1] = '\0';

        get_ddns_nameserver(1, dns_addr);
        sprintf(t_buffer, "%s\t%s\t%s\n", dns_addr, domain_name, host_name);
        pclose(p_fp);
      }
      memset(hosts_line[1], 0, sizeof(hosts_line[1]));
      strcpy(hosts_line[1], t_buffer);
      ddns_fp = fopen(DOMAINNAME_FILE, "w");
      for (i = 0; i < dns_count - 1; i++) {
        strcat(hosts_line[i], "\n");
        fwrite(hosts_line[i], strlen(hosts_line[i]), 1, ddns_fp);
      }
      fclose(ddns_fp);
      return 0;
    } else
      return 0;
  }
}

int set_ddns_nameserver(int flag, unsigned char *nameserver) {

  unsigned char dns_count = 0;
  unsigned char buffer[1024];
  FILE *ddns_fp;

  if (access(DNSSERVER_FILE, F_OK) == 0) {

    ddns_fp = fopen(DNSSERVER_FILE, "r");

    if (ddns_fp == NULL) {
      perror("popen() failed!\n");
      return 0;
    }

    while (feof(ddns_fp) == 0) {
      fread(buffer, sizeof(buffer), 1, ddns_fp);
    }
    fclose(ddns_fp);
    if (strstr((char *)buffer, "nameserver") != NULL) {
      int i = 0;
      while (i < strlen(buffer)) {
        if (buffer[i] == '\n')
          dns_count = dns_count + 1;
        i++;
      }

      if (dns_count != 0 && (strcmp(nameserver, "-") != 0)) {
        ddns_fp = fopen(DNSSERVER_FILE, "w");

        char *n_server[dns_count];
        char *ptr = strtok(buffer, "\n");
        i = 0;

        while ((ptr != NULL) && (i < dns_count)) {
          n_server[i] = ptr;
          i++;
          ptr = strtok(NULL, "\n");
        }
        switch (dns_count) {
        case 1:
          if (flag == 1)
            fprintf(ddns_fp, "nameserver %s\n", nameserver);
          fclose(ddns_fp);
          return 0;
          break;
        case 2:
          if (flag == 1) {
            fprintf(ddns_fp, "nameserver %s\n", nameserver);
            fprintf(ddns_fp, "%s\n", n_server[1]);
            fclose(ddns_fp);
            return 0;
          } else if (flag == 2) {
            fprintf(ddns_fp, "%s\n", n_server[0]);
            fprintf(ddns_fp, "nameserver %s\n", nameserver);
            fclose(ddns_fp);
            return 0;
          }
          break;
        case 3:
          if (flag == 1) {
            fprintf(ddns_fp, "nameserver %s\n", nameserver);
            fprintf(ddns_fp, "%s\n", n_server[1]);
            fprintf(ddns_fp, "%s\n", n_server[2]);
            fclose(ddns_fp);
            return 0;
          } else if (flag == 2) {
            fprintf(ddns_fp, "%s\n", n_server[0]);
            fprintf(ddns_fp, "nameserver %s\n", nameserver);
            fprintf(ddns_fp, "%s\n", n_server[2]);
            fclose(ddns_fp);
            return 0;
          }
          break;
        }
      } else
        return 0;
    }
  }
}

#define MAX_NIC 2

static void app_get_watchdog_timer_params(unsigned char *request,
                                          unsigned char *response,
                                          unsigned char *res_len) {
  cout << "Get watchdog timer parameter" << endl;
  ;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  unsigned char *data = &res->data[0];

  res->cc = CC_SUCCESS;

  *data++ = g_watchdog_config.timer_use;
  *data++ = g_watchdog_config.timer_actions;
  *data++ = g_watchdog_config.pre_timeout;
  *data++ = g_watchdog_config.timer_use_exp;
  *data++ = g_watchdog_config.initial_countdown_lsb;
  *data++ = g_watchdog_config.initial_countdown_msb;
  *data++ = g_watchdog_config.present_countdown_lsb;
  *data++ = g_watchdog_config.present_countdown_msb;

  *data++ = 0x00; // Extra error info in case of failure
  if (res->cc == CC_SUCCESS) {
    res_len = data - &res->data[0];
  }

  return;
}
/*
 * Editor : KICHEOL PARK
 * Description : set watchdog timer functions
 */
static void app_set_watchdog_timer_params(unsigned char *request,
                                          unsigned char *response,
                                          unsigned char *res_len) {
  cout << ("Get watchdog timer parameter") << endl;
  ipmi_req_t *req = (ipmi_req_t *)request;
  ipmi_res_t *res = (ipmi_res_t *)response;
  cout << ("Get watchdog timer 11") << endl;

  bool Ischanged = false;
  for (int i = 0; i < 6; i++) {
    cout << "bitset data[" << i << "] =" << uint32_t(req->data[i]) << endl;
    bitset<16> bit = bitset<16>(req->data[i]);
    cout << bit << endl;
    if (bitset<16>(req->data[i]).any()) {

      switch (i) {
      case 0:

        if (bitset<16>(req->data[i]).test(7)) {
          g_watchdog_config.Islogging = false;
          Ischanged = true;
        }
        break;
      case 1:

        if (bit.test(4))
          g_watchdog_config.pretimeoutInterrupt = 0x01; // SMI
        if (bit.test(5))
          g_watchdog_config.pretimeoutInterrupt = 0x02; // NVI
        break;
        if (bit.test(6))
          g_watchdog_config.pretimeoutInterrupt = 0x03; // MSI
        if ((bit.test(6) || bit.test(5) || bit.test(4)) == 0)
          g_watchdog_config.pretimeoutInterrupt = 0x00;

        g_watchdog_config.timer_actions = 0x00;
        if (bit.test(0))
          g_watchdog_config.timer_actions = 0x01; // HardReset
        if (bit.test(1))
          g_watchdog_config.timer_actions = 0x02; // PowerDown
        if (bit.test(2))
          g_watchdog_config.timer_actions = 0x03; // PowerDown
        if ((bit.test(0) || bit.test(1) || bit.test(2)) == 0)
          g_watchdog_config.pretimeoutInterrupt = 0x00;

        break;
      case 2:
        g_watchdog_config.pre_timeout = uint32_t(req->data[i]);
        Ischanged = true;
        break;
      case 3:
        break;
      case 4:
        g_watchdog_config.initial_countdown_lsb = uint32_t(req->data[i]);
        Ischanged = true;
        break;
      case 5:
        g_watchdog_config.initial_countdown_msb = uint32_t(req->data[i]);
        Ischanged = true;
        break;
      }
    }
  }

  res->cc = (Ischanged) ? CC_SUCCESS : CC_UNSPECIFIED_ERROR;
  *res_len = 0;

  return;
}
/* used by ReadConfigurationFile, check the line if it's valuable*/
/* This file refer to the watchdog version 5.5*/
static int spool(char *line, int *i, int offset) {
  for ((*i) += offset; line[*i] == ' ' || line[*i] == '\t'; (*i)++)
    ;
  if (line[*i] == '=')
    (*i)++;
  for (; line[*i] == ' ' || line[*i] == '\t'; (*i)++)
    ;
  if (line[*i] == '\0')
    return (1);
  else
    return (0);
}

/*Function used to read the configuration from the conf file in firectory
 * defined by 'filename' */
/* This file refer to the watchdog version 5.5*/
static int ReadConfigurationFile(char *file) {
  FILE *ReadConfigurationFile;

  /* Open the configuration file with readonly parameter*/
  printf("Trying the configuration file %s \n", ConfigurationFileDir);
  if ((ReadConfigurationFile = fopen(ConfigurationFileDir, "r")) == NULL) {
    printf("There is no configuration file, use default values for IPMI "
           "watchdog \n");
    return (1);
  }

  /* Check to see the configuration has data or not*/
  while (!feof(ReadConfigurationFile)) {
    char Configurationline[CONFIG_LINE_LEN];

    /* Read the line from configuration file */
    if (fgets(Configurationline, CONFIG_LINE_LEN, ReadConfigurationFile) ==
        NULL) {
      if (!ferror(ReadConfigurationFile)) {
        break;
      } else {
        return (1);
      }
    } else {
      int i, j;

      /* scan the actual line for an option , first remove the leading blanks*/
      for (i = 0; Configurationline[i] == ' ' || Configurationline[i] == '\t';
           i++)
        ;

      /* if the next sign is a '#' we have a comment , so we ignore the
       * configuration line */
      if (Configurationline[i] == '#') {
        continue;
      }

      /* also remove the trailing blanks and the \n */
      for (j = strlen(Configurationline) - 1;
           Configurationline[j] == ' ' || Configurationline[j] == '\t' ||
           Configurationline[j] == '\n';
           j--)
        ;

      Configurationline[j + 1] = '\0';

      /* if the line is empty now, we don't have to parse it */
      if (strlen(Configurationline + i) == 0) {
        continue;
      }

      /* now check for an option , interval first */

      /*Interval */
      if (strncmp(Configurationline + i, IPMI_INTERVAL,
                  strlen(IPMI_INTERVAL)) == 0) {
        if (spool(Configurationline, &i, strlen(IPMI_INTERVAL))) {
          fprintf(stderr, "Ignoring invalid line in config file:\n%s\n",
                  Configurationline);
        } else {
          IPMI_Interval = atol(Configurationline + i);

          { printf(" IPMI_Interval = %d \n", IPMI_Interval); }
        }
      }

      /*Timeout */
      else if (strncmp(Configurationline + i, IPMI_TIMEOUT,
                       strlen(IPMI_TIMEOUT)) == 0) {
        if (spool(Configurationline, &i, strlen(IPMI_TIMEOUT))) {
          fprintf(stderr, "Ignoring invalid line in config file:\n%s\n",
                  Configurationline);
        } else {
          IPMI_Timeout = atol(Configurationline + i);
          g_watchdog_config.initial_countdown_lsb = IPMI_Timeout & 0xFF;
          g_watchdog_config.initial_countdown_msb = IPMI_Timeout >> 8;
          printf(" IPMI_Timeout = %d \n", IPMI_Timeout);
          printf(" initial_countdown_lsb = %d \n",
                 g_watchdog_config.initial_countdown_msb);
          printf(" initial_countdown_msb = %d \n",
                 g_watchdog_config.initial_countdown_msb);
        }
      }

      /*Pretimeout */
      else if (strncmp(Configurationline + i, IPMI_PRETIMEOUT,
                       strlen(IPMI_PRETIMEOUT)) == 0) {
        if (spool(Configurationline, &i, strlen(IPMI_PRETIMEOUT))) {
          fprintf(stderr, "Ignoring invalid line in config file:\n%s\n",
                  Configurationline);
        } else {
          IPMI_Pretimeout = atol(Configurationline + i);

          g_watchdog_config.pre_timeout = IPMI_Pretimeout;
          printf(" IPMI_Pretimeout = %d \n", IPMI_Pretimeout);
        }
      }

      /*Daemon */
      else if (strncmp(Configurationline + i, IPMI_DAEMON,
                       strlen(IPMI_DAEMON)) == 0) {
        if (spool(Configurationline, &i, strlen(IPMI_DAEMON))) {
          IPMI_Daemon = NULL;
        } else {
          IPMI_Daemon = strdup(Configurationline + i);

          printf(" IPMI_Daemon = %s \n", IPMI_Daemon);
        }
      }

      /*PretimeoutInterrupt */
      else if (strncmp(Configurationline + i, IPMI_PRETIMEOUTINTERRUPT,
                       strlen(IPMI_PRETIMEOUTINTERRUPT)) == 0) {
        if (spool(Configurationline, &i, strlen(IPMI_PRETIMEOUTINTERRUPT))) {
          fprintf(stderr, "Ignoring invalid line in config file:\n%s\n",
                  Configurationline);
        } else {
          g_watchdog_config.pretimeoutInterrupt = atol(Configurationline + i);
          printf(" IPMI_PretimeoutInterrupt = %d \n",
                 g_watchdog_config.pretimeoutInterrupt);
        }
      }

      /*Action */
      else if (strncmp(Configurationline + i, IPMI_ACTION,
                       strlen(IPMI_ACTION)) == 0) {
        if (spool(Configurationline, &i, strlen(IPMI_ACTION))) {
          fprintf(stderr, "Ignoring invalid line in config file:\n%s\n",
                  Configurationline);
        } else {
          IPMI_Action = atol(Configurationline + i);
          g_watchdog_config.timer_actions = IPMI_Action;
          printf(" IPMI_Action = %d \n", IPMI_Action);
        }
      }

      /*Pidfile */
      else if (strncmp(Configurationline + i, IPMI_PIDFILE,
                       strlen(IPMI_PIDFILE)) == 0) {
        if (spool(Configurationline, &i, strlen(IPMI_PIDFILE))) {
          IPMI_Pidfile = NULL;
        } else {
          IPMI_Pidfile = strdup(Configurationline + i);

          printf(" IPMI_Pidfile = %s \n", IPMI_Pidfile);
        }
      }

      else {
        fprintf(stderr, "Ignoring config Configurationline: %s\n",
                Configurationline);
      }
    }
  }

  /* Close the configuration file */
  if (fclose(ReadConfigurationFile) != 0) {
    return (1);
  }
}

static int WriteConfigurationFile(char *file) {

  vector<string> lines;
  ifstream readFromFile(ConfigurationFileDir);
  if (readFromFile.is_open()) {
    lines.clear();
    while (!readFromFile.eof()) {
      string tmp;
      getline(readFromFile, tmp);
      cout << tmp << endl;
      lines.push_back(tmp);
    }
    readFromFile.close();
  } else {
    printf("%s not exist", ConfigurationFileDir);
    return 1;
  }

  std::ofstream writeFile(ConfigurationFileDir);
  if (writeFile.is_open()) {
    for (int i = 0; i < lines.size(); i++) {
      string tmp = lines[i];
      cout << tmp << endl;
      if (tmp.find(IPMI_INTERVAL) != string::npos) {
        tmp = "Interval = " + to_string(*g_watchdog_config.interval);
      } else if (tmp.find(IPMI_TIMEOUT) != string::npos) {
        tmp = "Timeout = " + to_string(IPMI_Timeout);
      } else if (tmp.find(IPMI_PRETIMEOUT) != string::npos) {
        tmp = "Pretimeout = " + to_string((int)g_watchdog_config.pre_timeout);
      } else if (tmp.find(IPMI_DAEMON) != string::npos) {
        if (IPMI_Daemon != NULL) {
          tmp = "Daemon = " + string(IPMI_Daemon);
        }
      } else if (tmp.find(IPMI_PRETIMEOUTINTERRUPT) != string::npos) {
        tmp = "Daemon5 = " +
              to_string((int)g_watchdog_config.pretimeoutInterrupt);
      } else if (tmp.find(IPMI_ACTION) != string::npos) {
        tmp = "Action = " + to_string((int)g_watchdog_config.timer_actions);
      } else if (tmp.find(IPMI_PIDFILE) != string::npos) {
        if (IPMI_Pidfile != NULL) {
          tmp = "Pidfile = " + string(IPMI_Pidfile);
        }
      }

      if (i != lines.size() - 1) {
        tmp += "\n";
      }
      cout << "change =" << tmp << endl;
      writeFile.write(tmp.c_str(), tmp.size());
    }

  } else {
    printf("file not exist");
  }
  writeFile.close();

  return 1;
}
/**
 * @brief LAN Alert Configuration Descriptor
 *
 */
typedef struct {
  unsigned char no_of_dest;
  unsigned char dest_type[SIZE_NUM_OF_DEST][SIZE_DEST_TYPE];
  unsigned char dest_addr[SIZE_NUM_OF_DEST][SIZE_DEST_ADDR];
} lan_alert_config_t;

void plat_lan_alert_init() {

  cout << ("Initializing LAN Alert information...") << endl;
  FILE *l_a;

  int i = 0;
  static lan_alert_config_t g_lan_alert[4];

  if (access(IPMI_LAN_ALERT_PATH, F_OK) != 0) {
    l_a = fopen(IPMI_LAN_ALERT_PATH, "w");
  } else {
    l_a = fopen(IPMI_LAN_ALERT_PATH, "r");
  }

  fread(g_lan_alert, sizeof(g_lan_alert), 4, l_a);

  for (i = 0; i < ETH_COUNT; i++) {
    memcpy(ipmiNetwork[i].dest_type, g_lan_alert[i].dest_type,
           sizeof(g_lan_alert[i].dest_type));
    memcpy(ipmiNetwork[i].dest_addr, g_lan_alert[i].dest_addr,
           sizeof(g_lan_alert[i].dest_addr));
  }
  fclose(l_a);
}

//** radius 관련

int set_radius_disable() {
  if (access(RADFILE, F_OK) == 0)
    system("rm /etc/raddb/server");
  return 0;
}

int set_radius_config(char *ip, char *port, char *secret) {
  FILE *fp_bin = fopen(RAD_BIN, "w");
  rad_config_t rad_config;
  rad_config.enable = 1;

  fclose(fp_bin);

  if (access(RADFILE, F_OK) != 0) {
    system("mkdir /etc/raddb");
  }

  FILE *fp = fopen(RADFILE, "w");
  fprintf(fp, "%s:%s\t%s\t5\n", ip, port, secret);
  fclose(fp);
}

//