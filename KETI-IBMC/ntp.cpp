#include <redfish/ntp.hpp>

/**
 * @brief Get the current full time info
 *
 * @param _date 날짜 정보 (YYYY-MM-DD 형식)
 * @param _time 시간 정보 (HH:MM:SS 형식)
 * @param _utc UTC Timezone 정보 (ex> +09:00)
 */
void get_current_fulltime_info(string &_date, string &_time, string &_utc) {
  string full_str;
  string cmd = "date \"+%F %T %:z\"";
  full_str = get_popen_string(cmd);

  vector<string> v = string_split(full_str, ' ');

  if (v.size() != 3) {
    cout << "Why DATE size not 3?" << endl;
    return;
  }

  log(info) << "[!@#$][FULL DATE INFO !!!] >>>>>>>>>>>> ";
  log(info) << v[0] << " / " << v[1] << " / " << v[2];
  // cout << "[FULL DATE INFO !!!] >>>>>>>>>>>> " << endl;
  // cout << v[0] << " / " << v[1] << " / " << v[2] << endl;

  _date = v[0];
  _time = v[1];
  _utc = v[2];

  return;
}

/**
 * @brief Get the current date time info
 *
 * @param _date 날짜 정보
 */
void get_current_date_info(string &_date) {
  string date_str;
  string cmd = "date \"+%F\"";
  date_str = get_popen_string(cmd);

  log(info) << "[!@#$][DATE INFO !!!] >>>>>>>>>>>> ";
  log(info) << date_str;
  // cout << "[DATE INFO !!!] >>>>>>>>>>>> " << endl;
  // cout << date_str << endl;

  _date = date_str;

  return;
}

/**
 * @brief Get the current time info
 *
 * @param _time 시간 정보
 */
void get_current_time_info(string &_time) {
  string time_str;
  string cmd = "date \"+%T\"";
  time_str = get_popen_string(cmd);

  log(info) << "[!@#$][TIME INFO !!!] >>>>>>>>>>>> ";
  log(info) << time_str;
  // cout << "[TIME INFO !!!] >>>>>>>>>>>> " << endl;
  // cout << time_str << endl;

  _time = time_str;

  return;
}

/**
 * @brief Get the current utc info
 *
 * @param _utc Timezone 정보
 */
void get_current_utc_info(string &_utc) {
  string utc_str;
  string cmd = "date \"+%:z\"";
  utc_str = get_popen_string(cmd);

  log(info) << "[!@#$][UTC INFO !!!] >>>>>>>>>>>> ";
  log(info) << utc_str;
  // cout << "[UTC INFO !!!] >>>>>>>>>>>> " << endl;
  // cout << utc_str << endl;

  _utc = utc_str;

  return;
}

/**
 * @brief Set the time by userDate
 *
 * @param _date 날짜정보
 * @param _time 시간정보
 * param date,time 둘중에 하나는 반드시 validate를 통과함, 하나는 ""공백일 수
 * 있음
 */
void set_time_by_userDate(string _date, string _time) {
  string cmd_disable_ntp = "timedatectl set-ntp 0";
  system(cmd_disable_ntp.c_str());
  string cmd = "date -s \"";

  if (_date == "")
    cmd += _time + "\"";
  else if (_time == "")
    cmd += _date + "\"";
  else
    cmd += _date + " " + _time + "\"";

  system(cmd.c_str());
  return;
}

/**
 * @brief Set the time by userTimezone
 *
 * @param _origin_tz 기존 timezone , 디폴트 +09:00
 * @param _new_tz 바뀔 timezone
 */
// void set_time_by_userTimezone(string _origin_tz="+09:00", string _new_tz)
void set_time_by_userTimezone(string _new_tz, string _origin_tz = "+09:00") {
  // timezone이 +09:00 형태로 들어올거임

  // CMM 적용코드
  // 계산해서 주는 함수
  // 계산을 해서 시간단위로 +인지 -인지랑 해서만 구하면됨 date -s "x hours
  // (ago)" 로 변경가능함 _origin_tz 에 빈거 들어오면 디폴트로 +09:00로 처리할게
  // string operation, diff_hour;
  // calculate_diff_time(_origin_tz, _new_tz, operation, diff_hour);
  // calculate_diff_time(_tz, operation, diff_hour);

  // string cmd = "date -s \"" + diff_hour + " hours";
  // if(operation == "+")
  //     cmd += "\"";
  // else if(operation == "-")
  //     cmd += " ago\"";

  // // cout << "Origin / New / op / diff" << endl;
  // // cout << _origin_tz << " / " << _new_tz << " / " << operation << " / " <<
  // diff_hour << endl;
  // // cout << "CMD : " << cmd << endl;

  // system(cmd.c_str());

  // BMC 적용코드
  // BMC 104번 보드에서 date명령어의 해당 사용이 안됨
  // 대신 BMC는 localtime 링크로 timezone 변경이 돼서 이 함수로 수행
  // 이 localtime 링크를 변경하여 적용하는 것은 현재 시간에서 timezone만 변경됨
  // 기억하셈 utc현재 시각 기준으로 timezone 변경해서 적용되는 것이 아님
  set_localtime_by_userTimezone(_new_tz);

  return;
}

/**
 * @brief timezone을 가지고 현재 시간에서 몇 시간 차이나는지 반환함
 * @details 현재 시간에서 _op + _hours 만큼 더해주는 방식으로 사용
 * Ex> _op = +, _hours = 6 이면 현재 시간에서 6시간을 더해줌
 * @param _origin_tz origin timezone string (manager-networkprotocol-ntp의
 * timezone값을 이용할거임)
 * @param _new_tz new timezone string
 * @param _op + or - operation
 * @param _hours time difference
 */
void calculate_diff_time(string _origin_tz, string _new_tz, string &_op,
                         string &_hours) {
  // string cmd = "date \"+%:z\"";
  // string origin_tz_str = get_popen_string(cmd);
  string origin_tz = _origin_tz.substr(0, 3);
  string new_tz = _new_tz.substr(0, 3);
  int o_time, n_time;
  o_time = stoi(origin_tz);
  n_time = stoi(new_tz);

  int result_time;
  result_time = -(o_time) + n_time;

  if (result_time < 0) {
    _op = "-";
    result_time = -(result_time);
  } else
    _op = "+";

  string result = to_string(result_time);

  // cout << "[CAL CAL INFO] >>>>>>>>>>>> " << endl;
  // cout << "Origin : " << _origin_tz << " / " << origin_tz << " / " << o_time
  // << endl; cout << "New : " << _new_tz << " / " << new_tz << " / " << n_time
  // << endl; cout << "Result : " << _op << " / " << result << endl;

  _hours = result;
}

/**
 * @brief Set the localtime by userTimezone
 *
 * @param _time timezone
 */
void set_localtime_by_userTimezone(string _time) {
  string cmd;
  if (_time == "-12:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+12 /etc/localtime";
  else if (_time == "-11:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+11 /etc/localtime";
  else if (_time == "-10:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+10 /etc/localtime";
  else if (_time == "-09:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+9 /etc/localtime";
  else if (_time == "-08:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+8 /etc/localtime";
  else if (_time == "-07:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+7 /etc/localtime";
  else if (_time == "-06:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+6 /etc/localtime";
  else if (_time == "-05:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+5 /etc/localtime";
  else if (_time == "-04:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+4 /etc/localtime";
  else if (_time == "-03:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+3 /etc/localtime";
  else if (_time == "-02:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+2 /etc/localtime";
  else if (_time == "-01:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT+1 /etc/localtime";
  else if (_time == "+00:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/UTC /etc/localtime";
  else if (_time == "+01:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-1 /etc/localtime";
  else if (_time == "+02:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-2 /etc/localtime";
  else if (_time == "+03:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-3 /etc/localtime";
  else if (_time == "+04:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-4 /etc/localtime";
  else if (_time == "+05:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-5 /etc/localtime";
  else if (_time == "+06:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-6 /etc/localtime";
  else if (_time == "+07:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-7 /etc/localtime";
  else if (_time == "+08:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-8 /etc/localtime";
  else if (_time == "+09:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-9 /etc/localtime";
  else if (_time == "+10:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-10 /etc/localtime";
  else if (_time == "+11:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-11 /etc/localtime";
  else if (_time == "+12:00")
    cmd = "ln -sf /usr/share/zoneinfo/Etc/GMT-12 /etc/localtime";

  log(info) << "[!@#$][LOCALTIME CMD] : " << cmd;
  // cout << "[LOCALTIME CMD] : " << cmd << endl;
  try {
    /* code */
    system(cmd.c_str());
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    log(error) << "Set LocalTime FAIL";
  }

  return;
}

/**
 * @brief Set the time by ntp server
 *
 * @param _server ntp server
 * @return system function return value
 */
int set_time_by_ntp_server(string _server) {

  string cmd_enable_ntp = "timedatectl set-ntp 1";
  system(cmd_enable_ntp.c_str());
  int ret_system;
  string cmd = "rdate -s " + _server;
  ret_system = system(cmd.c_str());

  log(info) << "[!@#$] RDATE SYSTEM RETURN : " << ret_system;
  // cout << "RDATE SYSTEM RETURN : " << ret_system << endl;

  return ret_system;
}
void initNtp() {
  std::unordered_map<std::string, std::string> time_info =
      read_time_info_from_file("/etc/timeInfo.conf");
  std::cout << "Read time_info from file:" << std::endl;
  for (const auto &entry : time_info) {
    std::cout << entry.first << "=" << entry.second << std::endl;
  }

  if (time_info["sync"] == "true") {
    set_time_by_ntp_server(time_info["syncServer"]);
  }

  else if (time_info["timezone"] != "") {
    set_localtime_by_userTimezone(time_info["timezone"]);
  } else {
    std::cout << "Set time by User time" << std::endl;
  }
}

std::unordered_map<std::string, std::string>
read_time_info_from_file(const std::string &filename) {
  std::unordered_map<std::string, std::string> time_info;
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      size_t pos = line.find('=');
      if (pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        time_info[key] = value;
      }
    }
    file.close();
  } else {
    std::cerr << "File " << filename
              << " not found. Creating new file with default values."
              << std::endl;

    // 파일이 없을 경우 기본 값을 설정하고 파일 생성
    time_info["sync"] = "false";
    time_info["timezone"] = "+09:00";
    time_info["date"] = "0000-00-00";
    time_info["time"] = "00:00:00";
    time_info["syncServer"] = "time.bora.net";
    write_time_info_to_file(filename, time_info);
  }
  return time_info;
}

void write_time_info_to_file(
    const std::string &filename,
    const std::unordered_map<std::string, std::string> &time_info) {
  std::ofstream file(filename);
  if (file.is_open()) {
    for (const auto &entry : time_info) {
      file << entry.first << "=" << entry.second << "\n";
    }
    file.close();
  } else {
    std::cerr << "Failed to open " << filename << std::endl;
  }
}

void changeConfig(const std::string &config, const std::string &value) {
  std::unordered_map<std::string, std::string> time_info =
      read_time_info_from_file("/etc/timeInfo.conf");
  time_info[config] = value;
  write_time_info_to_file("/etc/timeInfo.conf", time_info);
}
