#pragma once
#include <iostream>
#include <ipmi/apps.hpp>
#include <ipmi/fru.hpp>
#include <ipmi/sdr.hpp>
#include <ipmi/user.hpp>
#include <redfish/resource.hpp>
#include <string>

using namespace std;
#define QSIZE 35000

class Ipmiweb_PUT {
public:
  static void Set_Fru(json::value request_json);
  static void Set_Sensor(json::value request_json);
  static void Set_Ldap(json::value request_json);

  static void Set_Setting(json::value request_json);
  static void mq_key_send(json::value request_json);
  static void Set_fofl(json::value request_json);
  // static void rest_set_fru_board(int id,char f_mfg_date[4], string f_mfg,
  // string f_product, string f_serial, string f_part_num);
};
