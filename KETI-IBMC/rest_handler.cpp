#include "rest_handler.hpp"
#include<iostream>

#include<ipmi/apps.hpp>
#define BAUDRATE B115200
#define DEVNAME "/dev/ttyS1"
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1
#define FALSE 0
#define TRUE 1
#define FT_LED_DEBUG 0
#define ERROR(fmt, ...)           \
    do                            \
    {                             \
        printf(fmt, __VA_ARGS__); \
        return -1;                \
    } while (0)
#define TIMER_100MS_ID 10
#define TIMER_100MS_SECONDS 0
#define TIMER_250MS_SECONDS 0
#define TIMER_250MS_ID 25
#define TIMER_250MS_SECONDS 0
#define TIMER_500MS_ID 50
#define TIMER_500MS_SECONDS 0
#define TIMER_1SEC_ID 1
#define TIMER_1SEC_SECONDS 1
#define TIMER_60SEC_ID 60
#define TIMER_60SEC_SECONDS 60

#define TIMER_3SEC_ID 3
#define TIMER_3SEC_SECONDS 3
#define TIMER_5SEC_ID 5
#define TIMER_5SEC_SECONDS 5

#define MAX_IPMI_MSG_SIZE 100
#define MAX_DVNAME_LEN 128

#define POST_FINISHED PIN_GPIOAA7

#define HOST_POWER_BTN PIN_GPIOD3
#define HOST_RESET_BTN PIN_GPIOD4
#define POWER_BTN_INPUT PIN_GPIOD5
#define CPLD_PWR_BTN PIN_GPIOZ2
#define FRONT_ID_BTN PIN_GPIOE4

#define BACK_ID_LED PIN_GPIOH1
using namespace std;

// pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;
uint8_t POWER_BTN_EN = 1;

static int fan_count = 0;
static int lc_flag=0;
static int l_interrupt=0;
static char* password_padded;


timer_t timer_60second, timer_3second, timer_250msecond, timer_500msecond, timer_5second, timer_1second;

int timer_stop_flag = 0;
unsigned char lan_check = 0;
unsigned char lb_flag = 0;
unsigned char l_running = 0;

int m_min, h_min, d_min, m_max, h_max, d_max, m_count, d_count, h_count = 0;
int global_kcs_fd = 0;

int crnl_mapping = 0;
int initial_speed = 115200;
int bytes_recved;
int bytes_sent;
int sol_write_fd = 0;

unsigned char mesg[1000];
protocol_data *packet_in;
protocol_data *packet_out;

volatile int STOP = FALSE;

int rmcp_sockfd = 0;
int global_sockfd = 0;

pthread_mutex_t m_sensor;
pthread_t sel_device;
pthread_t tid, tid2, tipmb, rest_tid, lan_tid, redfish_tid;
static int g_bus_id = 0; // store the i2c bus ID for debug print
static int bic_up_flag = 0;

/*---------------------------------------------REST_Handler-------------------------------------*/
#define QSIZE 35000 // 한번에 왔다갔다하는 메시지의 '데이터' 크기
// typedef struct
// {
//     long type;
//     int index;
//     rest_req_t ipmi_msg;
// } ;

// typedef struct
// {
//     long type;
//     int next;
//     int res_len;
//     unsigned char data[QSIZE];
// } msq_rsp_t;

#define SAFE_FREE(a) \
    if (a)           \
    {                \
        free(a);     \
        a = NULL;    \
    }
#define MAX_THREADS 5
int active_threads = 0;
pthread_t threads[MAX_THREADS] = {
    0,
};
pthread_mutex_t a_mutex = PTHREAD_MUTEX_INITIALIZER;

void *run_ipmi_handle_rest(void *void_msq_req)
{
    msq_req_t *msq_req = (msq_req_t *)void_msq_req;
    unsigned char res_buf[QSIZE];
    int req_len = 0;
    int res_len = 0;
    int *count_p = &(l_interrupt);
    msq_rsp_t msq_rsp;
    memset(res_buf, 0, sizeof(res_buf));
    
    ipmi_handle_rest(&msq_req->ipmi_msg, res_buf, &res_len);
    int msqid_rsp;
    if (-1 == (msqid_rsp = msgget((key_t)5678, IPC_CREAT | 0666)))
    { // req에서 생성한 응답 메시지 큐를 리턴함
        fprintf(stderr, "msgget in handle_rest\n");
        exit(1);
    }
    
    msq_rsp.type = msq_req->type; // 요청을 보낸 req로 응답을 보내도록 하는 식별자
    msq_rsp.res_len = res_len;
    strncpy(msq_rsp.data, res_buf, res_len);

    if (-1 == msgsnd(msqid_rsp, &msq_rsp, sizeof(msq_rsp_t) - sizeof(long), 0))
    {
        fprintf(stderr, "msgsnd() when res_len > QSIZE 실패\n");
        exit(1);
    }
    
    SAFE_FREE(void_msq_req);
    pthread_exit(NULL);
}

void *rest_handler(void *data)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    printf("Starting REST Handler\n");
    int msqid_req;
    if (-1 == (msqid_req = msgget((key_t)1234, IPC_CREAT | 0666)))// 요청 큐 전체의 id를 받아옴(대문). 한번만 함
    { 
        fprintf(stderr, "msgget() msqid_req in udp-server failed\n");
        exit(1);
    }
    int index;
    while (1)
    {
        msq_req_t *msq_req = NULL;
        msq_req = (msq_req_t *)malloc(sizeof(msq_req_t));
        if (msq_req == NULL)
        {
            puts("[ERROR] malloc failed.");
            sleep(5);
            continue;
        }
        //printf("\t Enter msgrcv  \n");
        if (-1 == msgrcv(msqid_req, msq_req, sizeof(msq_req_t) - sizeof(long), 0, 0))
        {
            fprintf(stderr, "msgrcv in udp-server at first failed\n");
            exit(1);
        }
        //printf("\t leave msgrcv  \n");
        msq_req->index = index;
        pthread_t a_thread;

        if (pthread_create(&a_thread, &attr, run_ipmi_handle_rest, (void *)msq_req) != 0)
        {
            perror("pthread_create handle_rest failed");
            SAFE_FREE(msq_req);
            continue;
        }
       
        if (msq_req->ipmi_msg.cmd == CMD_SET_LAN_IPV4_DHCP)
        {
            printf("rest_Handler.cpp : CMD_SET_LAN_IPV4_DHCP\n");
        }
        printf("\t While end REST Handled  \n");
    }
    // if (lc_flag > lb_flag)
    // {
    //     if (l_running == 1)
    //     {
    //         printf("rest_Handler.cpp : l_running == 1\n");
    //         // l_interrupt += 1;
    //         // if (pthread_create(&lan_tid, NULL, lan_change_handler, NULL) < 0)
    //         // {
    //         //     fprintf(stderr, "pthread crreate lan tid 2\n");
    //         //     exit(0);
    //         // }
    //     }
    //     else
    //     {
    //         printf("rest_Handler.cpp : l_running != 1\n");
    //         // l_running += 1;
    //         // l_interrupt += 1;
    //         // if (pthread_create(&lan_tid, NULL, lan_change_handler, NULL) < 0)
    //         // {
    //         //     fprintf(stderr, "pthread crreate lan tid 2\n");
    //         //     exit(0);
    //         // }
    //     }
    // }
    //lb_flag = lc_flag;
    pthread_exit(NULL);
}
