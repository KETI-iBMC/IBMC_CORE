#include "ipmi/chassis.hpp"
#include <gpiod.h>
#include <gpiod.hpp>
#include <ipmi/lightning_sensor.hpp>
#include <ipmi/sel.hpp>
#include <util/dbus_system.hpp>
bool get_chassis_status_string() {
  FILE *fp = popen("gpioget 0 169", "r");
  char temp[4];

  if (fp != NULL) {
    while (fgets(temp, sizeof(temp), fp) != NULL) {
      temp[strcspn(temp, "\n")] = '\0'; // 개행 문자 제거
      if (strcmp(temp, "1") == 0) {
        pclose(fp);
        return true;
      }
    }
  }

  if (fp != NULL)
    pclose(fp);

  return false;
}
extern int redfish_power_ctrl;
extern int bPowerGD;
/**
 * @brief chassis power 상태 정보 전달
 * @bug 현재 redfish power control action이 off로 오면 무조건 0리턴 되는 문제
 * @return uint8_t
 * @bug
 */
uint8_t Ipmichassis::get_power_status(void) {
  try {
    int32_t ps = 0;
    BOOST_LOG_SEV(g_logger, info) << "get_power_status ";
    // DBus::BusDispatcher dispatcher;
    // DBus::default_dispatcher = &dispatcher;
    // DBus::Connection conn_n = DBus::Connection::SystemBus();
    // DBus_Sensor chassis_dbus_sensor =
    //     DBus_Sensor(conn_n, SERVER_PATH_1, SERVER_NAME_1);
    // ps = chassis_dbus_sensor.lightning_sensor_read(FRU_PEB,
    //                                                PEB_SENSOR_ADC_P12V_PSU1);
    // dispatcher.leave();
    // cout << "get_power_status ps=" <<std::hex<<ps << endl;
    if (get_chassis_status_string())
      return 1;
    else
      return 0;
    // return 1;
    // if (ps > (int32_t)150)
    // {
    //   redfish_power_ctrl = 1;
    //   return 1;
    // }
    // else
    // {
    //   redfish_power_ctrl = 0;
    //   return 0;
    // }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 0;
  }
  return bPowerGD;
}
void Ipmichassis::chassis_restart_cause(ipmi_req_t *request,
                                        ipmi_res_t *response,
                                        uint8_t *res_len) {
  uint8_t *data = &response->data[0];

  response->cc = CC_SUCCESS;

  *data++ = this->restart_cause;
  *data++ = 1;

  *res_len = data - &response->data[0];
}

void Ipmichassis::chassis_get_status(ipmi_req_t *request, ipmi_res_t *response,
                                     uint8_t *res_len) {
  uint8_t *data = &response->data[0];
  uint8_t chassis_status = 0;
  uint8_t chassis_policy = 0;

  response->cc = CC_SUCCESS;

  if (this->restart_policy & 0x100)
    chassis_policy = 2;
  else if (this->restart_policy & 0x10)
    chassis_policy = 1;
  else if (this->restart_policy & 0x1)
    chassis_policy = 0;

  // pwr_status = this->get_power_status();
  // pwr_gpio
  if (get_chassis_status_string()) {
    chassis_status = 1;
    cout << "chassis_get_status 11111111" << endl;
  } else {
    chassis_status = 0;
    cout << "chassis_get_status 0000" << endl;
  }
  *data++ = (0x1 & chassis_status) | (chassis_policy << 5);
  *data++ = 0x0;
  *data++ = 0x40;
  *data++ = 0x0;

  *res_len = data - &response->data[0];
}

void Ipmichassis::chassis_control(ipmi_req_t *request, ipmi_res_t *response,
                                  uint8_t *res_len) {
  ::gpiod::chip chip("gpiochip0");
  ::gpiod::line_request config;
  config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
  config.consumer = "keti";

  // uint8_t *data = &response->data[0];
  uint8_t param = request->data[0];
  response->cc = CC_SUCCESS;
  printf("params= %d \n", param);
  switch (param) {
  case PARAM_POWER_OFF: {
    printf("poweroff\n");
    /**
     * @brief 23.07.31 hangyeol
     *
     */
    auto lines_c = chip.get_lines({122, 123});
    lines_c.request(config);
    sleep(5);
    lines_c.set_values({0, 0});
    lines_c.release();
    this->restart_cause = 1;
    break;
  }
  case PARAM_POWER_ON: {
    /**
     * @brief hangyeol
     *
     */
    this->restart_cause = 3;

    auto lines_c = chip.get_lines({122, 123});
    lines_c.request(config);
    sleep(1);
    lines_c.set_values({0, 0});
    lines_c.release();
    break;
  }
  case PARAM_POWER_CYCLE: {
    auto lines_c = chip.get_lines({120, 121});
    lines_c.request(config);
    sleep(1);
    lines_c.set_values({0, 0});
    lines_c.release();
    this->restart_cause = 1;
    break;
  }
  case PARAM_HARD_RESET: {
    auto lines_h = chip.get_lines({120, 121});
    lines_h.request(config);
    sleep(1);
    lines_h.set_values({0, 0});
    lines_h.release();

    this->restart_cause = 2;
    break;
  }

  case PARAM_POWER_OFF_ORDERLY: {
    printf("poweroff\n");
    /**
     * @brief 23.07.31 hangyeol
     *
     */
    auto lines_c = chip.get_lines({122, 123});
    lines_c.request(config);
    sleep(5);
    lines_c.set_values({0, 0});
    lines_c.release();
    this->restart_cause = 0xA;
    plat_sel_power_off_add_entry();
    break;
  }
  default:
    response->cc = CC_PARAM_OUT_OF_RANGE;
    break;
  }
}

void Ipmichassis::chassis_set_policy(ipmi_req_t *request, ipmi_res_t *response,
                                     uint8_t *res_len) {
  uint8_t policy = request->data[0];
  uint8_t *data = &response->data[0];

  response->cc = CC_SUCCESS;

  switch (policy) {
  case 0x0: // Always stay powered off
    this->restart_policy = 0x1;
    *data++ = this->restart_policy;
    break;
  case 0x1:
    this->restart_policy = 0x2;
    this->restart_cause = 0x2;
    *data++ = this->restart_policy;
    break;
  case 0x2:
    this->restart_policy = 0x4;
    this->restart_cause = 0x6;
    *data++ = this->restart_policy;
    break;
  case 0x3:
    *data++ = this->restart_policy;
    break;
  default:
    response->cc = CC_PARAM_OUT_OF_RANGE;
    break;
  }

  if (response->cc == CC_SUCCESS) {
    *res_len = data - &response->data[0];
  }
}
void Ipmichassis::chassis_identify(ipmi_req_t *request, ipmi_res_t *response,
                                   uint8_t *res_len) {
  uint8_t interval = request->data[0];
  uint8_t force_on = request->data[1] & 0x1;

  if (force_on == 1) {
    printf("[Chassis] Force on Identify LED\n");
    response->cc = CC_SUCCESS;
  } else {
    if (interval == 3 || interval == 15)
      this->id_interval = 15;
    else
      this->id_interval = interval;

    response->cc = CC_SUCCESS;
  }
}

void Ipmichassis::chassis_get_poh(ipmi_req_t *request, ipmi_res_t *response,
                                  uint8_t *res_len) {
  uint8_t *data = &response->data[0];
  uint8_t mins_per_count = 60;
  long count = 180;
  struct sysinfo info;

  sysinfo(&info);
  count = info.uptime / mins_per_count;

  *data++ = 1;
  *data++ = count & 0xff;
  *data++ = (count & 0xff00) >> 8;
  *data++ = (count & 0xff0000) >> 16;
  *data++ = (count & 0xff000000) >> 24;

  response->cc = CC_SUCCESS;

  *res_len = data - &response->data[0];
}

void Ipmichassis::chassis_get_boot_options(ipmi_req_t *request,
                                           ipmi_res_t *response,
                                           uint8_t *res_len) {
  uint8_t param = request->data[0];
  uint8_t *data = &response->data[0];

  response->cc = CC_SUCCESS;

  *data++ = 0x01; // Parameter Version
  *data++ = request->data[0];

  switch (param) {
  case PARAM_SET_IN_PROG:
    *data++ = this->g_chassis_bootp.set_in_prog;
    break;
  case PARAM_SVC_PART_SELECT:
    *data++ = this->g_chassis_bootp.svc_part_select;
    break;
  case PARAM_SVC_PART_SCAN:
    *data++ = this->g_chassis_bootp.svc_part_scan;
    break;
  case PARAM_BOOT_FLAG_CLR:
    *data++ = this->g_chassis_bootp.boot_flag_clr;
    break;
  case PARAM_BOOT_INFO_ACK:
    std::copy(this->g_chassis_bootp.boot_info_ack.begin(),
              this->g_chassis_bootp.boot_info_ack.end(), data);
    data += BOOT_INFO_ACK_SIZE; // ACK Size
    break;
  case PARAM_BOOT_FLAGS:
    std::copy(this->g_chassis_bootp.boot_flags.begin(),
              this->g_chassis_bootp.boot_flags.end(), data);
    data += BOOT_FLAGS_SIZE; // Boot Flag Size
    break;
  case PARAM_BOOT_INIT_INFO:
    std::copy(this->g_chassis_bootp.boot_init_info.begin(),
              this->g_chassis_bootp.boot_flags.end(), data);
    data += BOOT_INIT_INFO_SIZE; // Boot init Size
    break;
  default:
    response->cc = CC_PARAM_OUT_OF_RANGE;
    break;
  }

  if (response->cc == CC_SUCCESS)
    *res_len = data - &response->data[0];
}

void Ipmichassis::chassis_set_boot_options(ipmi_req_t *request,
                                           ipmi_res_t *response,
                                           uint8_t *res_len) {
  uint8_t param = request->data[0];
  uint8_t *data = &response->data[0];

  response->cc = CC_SUCCESS;

  switch (param) {
  case PARAM_SET_IN_PROG:
    this->g_chassis_bootp.set_in_prog = request->data[1];
    break;
  case PARAM_SVC_PART_SELECT:
    this->g_chassis_bootp.svc_part_select = request->data[1];
    break;
  case PARAM_SVC_PART_SCAN:
    this->g_chassis_bootp.svc_part_scan = request->data[1];
    break;
  case PARAM_BOOT_FLAG_CLR:
    this->g_chassis_bootp.boot_flag_clr = request->data[1];
    break;
  case PARAM_BOOT_INFO_ACK:
    this->g_chassis_bootp.boot_info_ack.assign(
        request->data + 1, request->data + 1 + BOOT_INFO_ACK_SIZE);
    break;
  case PARAM_BOOT_FLAGS:
    this->g_chassis_bootp.boot_flags.assign(
        request->data + 1, request->data + 1 + BOOT_FLAGS_SIZE);
    break;
  case PARAM_BOOT_INIT_INFO:
    this->g_chassis_bootp.boot_init_info.assign(
        request->data + 1, request->data + 1 + BOOT_INIT_INFO_SIZE);
    break;
  default:
    response->cc = CC_PARAM_OUT_OF_RANGE;
    break;
  }

  if (response->cc == CC_SUCCESS)
    *res_len = data - &response->data[0];
}

Ipmichassis::Ipmichassis() {}