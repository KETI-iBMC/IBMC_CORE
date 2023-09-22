#ifndef A7230D0F_8B20_43C4_AC1B_DC2E34F2EB35
#define A7230D0F_8B20_43C4_AC1B_DC2E34F2EB35
#pragma once
#include <common/keti_std.hpp>
#include <sqlite3.h>
#include <unordered_map>
#define LOG_DB "IBMC_log.db"
static int FanPolicyCallback(void *data, int argc, char **argv,
                             char **colNames);
static int CPUPolicyCallback(void *data, int argc, char **argv,
                             char **colNames);
static int FeedbackPolicyCallback(void *data, int argc, char **argv,
                                  char **colNames);
enum class POLICY_TYPE { TYPE_FAN, TYPE_CPU, TYPE_FEEDBACK, ALL };
std::unordered_map<std::string, POLICY_TYPE> colorMap = {
    {"FAN", POLICY_TYPE::TYPE_FAN},
    {"CPU", POLICY_TYPE::TYPE_CPU},
    {"FEEDBACK", POLICY_TYPE::TYPE_FEEDBACK},
    {"ALL", POLICY_TYPE::ALL}};

std::string PolicyoString(POLICY_TYPE policy) {
  for (const auto &pair : colorMap) {
    if (pair.second == policy) {
      return pair.first;
    }
  }
  return "unknown";
}

class Fan {
private:
  int fanID;
  string fan_name;
  int PWM;
  int max_PWM;
  int max_RPM;
  int RPM;
  string model;
  string manufacture;
  string FanController;
  int policyID;

  struct Position {
    int slotNumber; // raw
    int columnNumber;
    std::string chassisName;
  } pos;
  Fan(){};

public:
  Fan(int _fanID, const std::string &_fan_name, int _PWM, int _max_PWM,
      int _max_RPM, int _RPM, const std::string &_model,
      const std::string &_manufacture, const std::string &_FanController,
      int _slotNumber, int _columnNumber, const std::string &_chassisName,
      int policyID)
      : fanID(_fanID), fan_name(_fan_name), PWM(_PWM), max_PWM(_max_PWM),
        max_RPM(_max_RPM), RPM(_RPM), model(_model), manufacture(_manufacture),
        FanController(_FanController) {
    this->policyID = policyID;
    // pos의 멤버 변수 초기화
    pos.slotNumber = _slotNumber;
    pos.columnNumber = _columnNumber;
    pos.chassisName = _chassisName;
  }
  bool Insert_Fan(sqlite3 *db, int EnergyManagerID) {
    char query[500];
    sprintf(
        query,
        "INSERT INTO Fan (FanName, PWM, RPM, chassisName, slotNumber, "
        "columnNumber, "
        "Manufacturer, Model, MaxPWM, MaxRPM, FanController, FanPolicyID) "
        "VALUES ('%s', %d, %d, '%s', %d, %d, '%s', '%s', %d, %d, '%s', %d);",
        fan_name.c_str(), PWM, RPM, pos.chassisName.c_str(), pos.slotNumber,
        pos.columnNumber, manufacture.c_str(), model.c_str(), max_PWM, max_RPM,
        FanController.c_str(), policyID);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert Fan SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[Fan] Insert" << std::endl;
    }
    return true;
  }
  bool Update_Fan(sqlite3 *db, int fanID, const std::string &fan_name, int PWM,
                  int RPM, int slotNumber, int columnNumber,
                  const std::string &chassisName,
                  const std::string &manufacture, const std::string &model,
                  int max_PWM, int max_RPM, const std::string &FanController,
                  int fan_policyID) {
    char query[500];
    sprintf(
        query,
        "UPDATE Fan "
        "SET FanName = '%s', PWM = %d, RPM = %d, slotNumber = %d, columnNumber "
        "= %d, "
        "chassisName = '%s', Manufacturer = '%s', Model = '%s', MaxPWM = %d, "
        "MaxRPM = %d, FanController = '%s', FanPolicyID = %d "
        "WHERE FanID = %d;",
        fan_name.c_str(), PWM, RPM, slotNumber, columnNumber,
        chassisName.c_str(), manufacture.c_str(), model.c_str(), max_PWM,
        max_RPM, FanController.c_str(), fan_policyID, fanID);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Fan SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[Fan] Update" << std::endl;
    }
    return true;
  }
  ~Fan(){};
};
class Policy {
private:
public:
  sqlite3 *db;
  POLICY_TYPE policy_type;
  string description;
  string policyName;
  string last_date_time;
  string algorithm;
  bool status;
  int policyFieldID;
  Policy() {}
  virtual bool Enable_Policy(bool status) = 0;
  // virtual bool Delete_Policy() = 0;
  // virtual void *Policy_Monitor(string query) = 0;
  void Policy_Info() {
    log(info) << "policyName :" << policyName << "status" << status
              << "last modify time :" << last_date_time << endl;
  }
  virtual bool Insert_Policy(sqlite3 *db, int policyFieldID) = 0;
  ~Policy(){};
};
class Fan_Policy : public Policy {
private:
public:
  unordered_map<int, Fan *> Target_Fans;
  string temperatureSource;
  int desiredTemperature;
  Fan_Policy(sqlite3 *db, int policyID) {
    char query[500];
    sprintf(query,
            "SELECT FanPolicyID, PolicyName, Description, Algorithm, "
            "DesiredTemperature, TemperatureSource, DATETIME, PolicyFieldID "
            "FROM FanPolicy WHERE FanPolicyID = %d;",
            policyID);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, FanPolicyCallback, this, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Select FanPolicy SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }
  }
  Fan_Policy(const std::string &_description, const std::string &_policyName,
             const std::string &_algorithm, // bool _status,
             const std::string &_TemperatureSource, int _DesiredTemperature) {
    this->policy_type = (POLICY_TYPE::TYPE_FAN);
    this->description = (_description);
    this->policyName = (_policyName);
    // this->last_date_time = DB_currentDateTime();
    this->algorithm = (_algorithm);
    // this->status = (_status);
    this->desiredTemperature = (_DesiredTemperature);
    this->temperatureSource = (_TemperatureSource);
  }
  bool Enable_Policy(bool status) override { this->status = true; }
  bool Insert_Policy(sqlite3 *db, int policyFieldID) override {
    char query[500];
    int rc;
    char *errMsg = 0;
    sprintf(query,
            "INSERT INTO FanPolicy (PolicyFieldID, PolicyName, Description, "
            "Algorithm, DesiredTemperature, TemperatureSource, DATETIME) "
            "VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s');",
            policyFieldID, policyName.c_str(), description.c_str(),
            algorithm.c_str(), getDesiredTemperature(),
            getTemperatureSource().c_str(), DB_currentDateTime().c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert FanPolicy SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[FanPolicy] Insert " << std::endl;
      return true;
    }
  }
  // Fan *getFanByID(const std::string &key) {
  //   if (Target_Fans.find(key) != Target_Fans.end()) {
  //     return Target_Fans[key];
  //   }
  //   return nullptr;
  // }
  // TemperatureSource Getter
  // 외부 모듈에서 getTmperatureSource 호출시 반환
  std::string getTemperatureSource() const { return temperatureSource; }

  // TemperatureSource Setter
  // 외부 모듈에서 setTemperatureSource 호출시 반환
  void setTemperatureSource(const std::string &_temperatureSource) {
    temperatureSource = (_temperatureSource);
    // 저장함수만들어야함
  }

  // DesiredTemperature Getter
  // 외부 모듈에서 getDesiredTemperature 호출시 반환
  int getDesiredTemperature() const { return desiredTemperature; }

  // DesiredTemperature Setter
  // 외부 모듈에서 setDesiredTemperature 호출시 반환
  void setDesiredTemperature(int temperature) {
    desiredTemperature = temperature;
    // 저장함수만들어야함
  }

  ~Fan_Policy(){};
};

static int FanPolicyCallback(void *data, int argc, char **argv,
                             char **colNames) {
  if (!data) {
    std::cerr << "FanPolicyCallback: Invalid data pointer" << std::endl;
    return -1; // Return non-zero value to indicate an error
  }

  if (argc != 8) {
    std::cerr << "FanPolicyCallback: Invalid number of columns" << std::endl;
    return -1; // Return non-zero value to indicate an error
  }

  Fan_Policy *fanPolicy = static_cast<Fan_Policy *>(data);

  // fanPolicy->policy_id = std::stoi(argv[0]);
  fanPolicy->policyName = argv[1];
  fanPolicy->description = argv[2];
  fanPolicy->algorithm = argv[3];
  fanPolicy->desiredTemperature = std::stoi(argv[4]);
  fanPolicy->temperatureSource = argv[5];
  // fanPolicy->last_date_time = argv[6];
  fanPolicy->policyFieldID = std::stoi(argv[7]);

  return 0;
}
class CPU_Policy : public Policy {
private:
public:
  std::string mode;
  int basic_speed_mhz;
  int max_speed_mhz;

  CPU_Policy(sqlite3 *db, int policyID) {
    char query[500];
    sprintf(query,
            "SELECT CPUPolicyID, PolicyName, Description, Mode, "
            "BasicSpeedMHz, MaxSpeedMHz, DATETIME, EnergyManagerID "
            "FROM CPUPolicy WHERE CPUPolicyID = %d;",
            policyID);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, CPUPolicyCallback, this, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Select CPUPolicy SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }
  }
  CPU_Policy(const std::string &_description, const std::string &_policyName,
             const std::string &_mode, // bool _status,
             int _basic_speed_mhz, int _max_speed_mhz) {
    this->policy_type = (POLICY_TYPE::TYPE_CPU);
    this->description = (_description);
    this->policyName = (_policyName);
    // this->last_date_time = DB_currentDateTime();
    this->mode = (_mode);
    // this->status = (_status);
    this->basic_speed_mhz = (_basic_speed_mhz);
    this->max_speed_mhz = (_max_speed_mhz);
  }
  bool Enable_Policy(bool status) override { this->status = true; }
  bool Insert_Policy(sqlite3 *db, int policyFieldID) override {
    char query[500];
    int rc;
    char *errMsg = 0;
    sprintf(query,
            "INSERT INTO CPUPolicy (PolicyFieldID, PolicyName, Description, "
            "Mode, BasicSpeedMHz, MaxSpeedMHz, DATETIME) "
            "VALUES (%d, '%s', '%s', '%s', %d, '%d', '%s');",
            policyFieldID, policyName.c_str(), description.c_str(),
            getMode().c_str(), getBasicSpeedMHz(), getMaxSpeedMHz(),
            DB_currentDateTime().c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert CPUPolicy SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[CPUPolicy] Insert" << std::endl;
      return true;
    }
  }
  std::string getMode() const { return mode; }

  void setMode(const std::string &_mode) { mode = (_mode); }

  int getBasicSpeedMHz() const { return basic_speed_mhz; }

  void setBasicSpeedMHz(int _basic_speed) { basic_speed_mhz = _basic_speed; }

  int getMaxSpeedMHz() const { return max_speed_mhz; }

  void setMaxSpeedMHz(int _max_speed) { max_speed_mhz = _max_speed; }

  ~CPU_Policy(){};
};
static int CPUPolicyCallback(void *data, int argc, char **argv,
                             char **colNames) {
  if (!data) {
    std::cerr << "CPUPolicyCallback: Invalid data pointer" << std::endl;
    return -1; // Return non-zero value to indicate an error
  }

  if (argc != 8) {
    std::cerr << "CPUPolicyCallback: Invalid number of columns" << std::endl;
    return -1; // Return non-zero value to indicate an error
  }

  CPU_Policy *cpuPolicy = static_cast<CPU_Policy *>(data);

  // cpuPolicy->policy_id = std::stoi(argv[0]);
  cpuPolicy->policyName = argv[1];
  cpuPolicy->description = argv[2];
  cpuPolicy->mode = argv[3];
  cpuPolicy->basic_speed_mhz = std::stoi(argv[4]);
  cpuPolicy->max_speed_mhz = std::stoi(argv[5]);
  // cpuPolicy->last_date_time = argv[6];
  cpuPolicy->policyFieldID = std::stoi(argv[7]);

  return 0;
}
class Green {
private:
  bool isActive;
  int lowerThresholdUser;
  int upperThresholdUser;
  std::string destination;
  int feedbackid;

public:
  std::string greenName;

  Green(std::string _greenName, bool _isActive, int _lowerThresholdUser,
        int _upperThresholdUser, std::string _destination, int _feedbackid)
      : greenName(_greenName), isActive(_isActive),
        lowerThresholdUser(_lowerThresholdUser),
        upperThresholdUser(_upperThresholdUser), destination(_destination),
        feedbackid(_feedbackid) {}

  // feedbackid가 외래키라 매개변수로 받음
  bool Insert_Green_Level(sqlite3 *db) {
    char query[500];
    sprintf(query,
            "INSERT INTO Green (Name, IsActive, LowerThresholdUser, "
            "UpperThresholdUser, Destination, FeedbackID) "
            "VALUES ('%s', '%d', %d, %d, '%s', '%d');",
            greenName.c_str(), isActive, lowerThresholdUser, upperThresholdUser,
            destination.c_str(), feedbackid);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert Green SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[Green] Insert" << std::endl;
    }
    return true;
  }
  bool Update_Green_Level_Int(sqlite3 *db, std::string greenName,
                              std::string attribute_name, int attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Green "
            "SET '%s' = '%d' "
            "WHERE Name = '%s';",
            attribute_name.c_str(), attribute_value, greenName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Green SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Green] Update" << std::endl;
      return 0;
    }
  }

  bool Update_Green_Level_String(sqlite3 *db, std::string greenName,
                                 std::string attribute_name,
                                 std::string attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Green "
            "SET '%s' = '%s' "
            "WHERE Name = '%s';",
            attribute_name.c_str(), attribute_value.c_str(), greenName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Green SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Green] Update" << std::endl;
      return 0;
    }
  }
  ~Green() {}
};
class Yellow {
private:
  bool isActive;
  int targetTemperatureEstimatedTimeRefreshCycleSeconds;
  int lowerThresholdUser;
  int upperThresholdUser;
  std::string destination;
  int feedbackid;

public:
  std::string yellowName;
  Yellow(std::string _yellowName, bool _isActive,
         int _targetTemperatureEstimatedTimeRefreshCycleSeconds,
         int _lowerThresholdUser, int _upperThresholdUser,
         std::string _destination, int _feedbackid)
      : yellowName(_yellowName), isActive(_isActive),
        targetTemperatureEstimatedTimeRefreshCycleSeconds(
            _targetTemperatureEstimatedTimeRefreshCycleSeconds),
        lowerThresholdUser(_lowerThresholdUser),
        upperThresholdUser(_upperThresholdUser), destination(_destination),
        feedbackid(_feedbackid) {}
  bool Insert_Yellow_Level(sqlite3 *db) {
    char query[500];
    sprintf(query,
            "INSERT INTO Yellow (Name, IsActive, "
            "TargetTemperatureEstimatedTimeRefreshCycleSeconds,"
            "LowerThresholdUser, UpperThresholdUser, Destination, FeedbackID) "
            "VALUES ('%s', '%d', '%d', '%d', '%d', '%s', '%d');",
            yellowName.c_str(), isActive,
            targetTemperatureEstimatedTimeRefreshCycleSeconds,
            lowerThresholdUser, upperThresholdUser, destination.c_str(),
            feedbackid);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert Yellow SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[Yellow] Insert" << std::endl;
    }
    return true;
  }
  bool Update_Yellow_Level_Int(sqlite3 *db, std::string yellowName,
                               std::string attribute_name,
                               int attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Yellow "
            "SET '%s' = '%d' "
            "WHERE Name = '%s';",
            attribute_name.c_str(), attribute_value, yellowName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Yellow SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Yellow] Update" << std::endl;
      return 0;
    }
  }

  bool Update_Yellow_Level_String(sqlite3 *db, std::string yellowName,
                                  std::string attribute_name,
                                  std::string attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Yellow "
            "SET '%s' = '%s' "
            "WHERE Name = '%s';",
            attribute_name.c_str(), attribute_value.c_str(),
            yellowName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Yellow SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Yellow] Update" << std::endl;
      return 0;
    }
  }
  ~Yellow() {}
};

class Orange {
private:
  bool isActive;
  int targetTemperatureEstimatedTimeRefreshCycleSeconds;
  int lowerThresholdUser;
  int upperThresholdUser;
  std::string destination;
  int feedbackid;

public:
  std::string orangeName;
  Orange(std::string _orangeName, bool _isActive,
         int _targetTemperatureEstimatedTimeRefreshCycleSeconds,
         int _lowerThresholdUser, int _upperThresholdUser,
         std::string _destination, int _feedbackid)
      : orangeName(_orangeName), isActive(_isActive),
        targetTemperatureEstimatedTimeRefreshCycleSeconds(
            _targetTemperatureEstimatedTimeRefreshCycleSeconds),
        lowerThresholdUser(_lowerThresholdUser),
        upperThresholdUser(_upperThresholdUser), destination(_destination),
        feedbackid(_feedbackid) {}
  bool Insert_Orange_Level(sqlite3 *db) {
    char query[500];
    sprintf(query,
            "INSERT INTO Orange (Name, IsActive, "
            "TargetTemperatureEstimatedTimeRefreshCycleSeconds, "
            "LowerThresholdUser, UpperThresholdUser, Destination, FeedbackID) "
            "VALUES ('%s', '%d', '%d', '%d', '%d', '%s', '%d');",
            orangeName.c_str(), isActive,
            targetTemperatureEstimatedTimeRefreshCycleSeconds,
            lowerThresholdUser, upperThresholdUser, destination.c_str(),
            feedbackid);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert Orange SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[Orange] Insert" << std::endl;
    }
    return true;
  }
  bool Update_Orange_Level_Int(sqlite3 *db, std::string orangeName,
                               std::string attribute_name,
                               int attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Orange "
            "SET '%s' = '%d' "
            "WHERE Name = '%s';",
            attribute_name.c_str(), attribute_value, orangeName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Orange SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Orange] Update" << std::endl;
      return 0;
    }
  }

  bool Update_Orange_Level_String(sqlite3 *db, std::string orangeName,
                                  std::string attribute_name,
                                  std::string attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Orange "
            "SET '%s' = '%s' "
            "WHERE Name = '%s';",
            attribute_name.c_str(), attribute_value.c_str(),
            orangeName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Orange SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Orange] Update" << std::endl;
      return 0;
    }
  }
  ~Orange() {}
};
class Red {
private:
  bool isActive;
  int targetTemperatureEstimatedTimeRefreshCycleSeconds;
  int lowerThresholdUser;
  int upperThresholdUser;
  std::string destination;
  bool compulsoryControl;
  bool causeAnalysis;
  int feedbackid;

public:
  std::string redName;
  Red(std::string _redName, bool _isActive,
      int _targetTemperatureEstimatedTimeRefreshCycleSeconds,
      int _lowerThresholdUser, int _upperThresholdUser,
      std::string _destination, bool _compulsoryControl, bool _causeAnalysis,
      int _feedbackid)
      : redName(_redName), isActive(_isActive),
        targetTemperatureEstimatedTimeRefreshCycleSeconds(
            _targetTemperatureEstimatedTimeRefreshCycleSeconds),
        lowerThresholdUser(_lowerThresholdUser),
        upperThresholdUser(_upperThresholdUser), destination(_destination),
        compulsoryControl(_compulsoryControl), causeAnalysis(_causeAnalysis),
        feedbackid(_feedbackid) {}
  bool Insert_Red_Level(sqlite3 *db) {
    char query[500];
    sprintf(query,
            "INSERT INTO Red (Name, IsActive, "
            "TargetTemperatureEstimatedTimeRefreshCycleSeconds, "
            "LowerThresholdUser, UpperThresholdUser,"
            "Destination, CompulsoryControl, CauseAnalysis, FeedbackID) "
            "VALUES ('%s','%d', '%d', '%d', '%d', '%s', '%d', '%d', '%d');",
            redName.c_str(), isActive,
            targetTemperatureEstimatedTimeRefreshCycleSeconds,
            lowerThresholdUser, upperThresholdUser, destination.c_str(),
            compulsoryControl, causeAnalysis, feedbackid);
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert RedSQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[Red] Insert" << std::endl;
    }
    return true;
  }
  bool Update_Red_Level_Int(sqlite3 *db, std::string redName,
                            std::string attribute_name, int attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Red "
            "SET '%s' = '%d' "
            "WHERE Name = '%s';",
            attribute_name.c_str(), attribute_value, redName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Red SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Red] Update" << std::endl;
      return 0;
    }
  }

  bool Update_Red_Level_String(sqlite3 *db, std::string redName,
                               std::string attribute_name,
                               std::string attribute_value) {
    char query[500];
    char *errMsg = nullptr;
    int rc;
    // 여기에 attribute 확인해서 그에 맞게 값을 넣어야함
    sprintf(query,
            "UPDATE Red "
            "SET '%s' = '%s' "
            "WHERE RedName = '%s';",
            attribute_name.c_str(), attribute_value.c_str(), redName.c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Update Red SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return -1;
    } else {
      std::cout << "[Red] Update" << std::endl;
      return 0;
    }
  }
};
class Feedback_Policy : public Policy {
private:
public:
  Green *g;
  Yellow *y;
  Orange *o;
  Red *r;

  Feedback_Policy(const std::string &_description,
                  const std::string &_policyName, const Green *_green,
                  const Yellow *_yellow, const Orange *_orange, const Red *_red)
      : Policy(), g(_green), y(_yellow), o(_orange), r(_red) {
    this->policy_type = (POLICY_TYPE::TYPE_FEEDBACK);
    policyFieldID = 0; // Initialize policyFieldID to some default value
    description = _description;
    policyName = _policyName;
  }

  Green getGreen() { return *g; }
  Yellow getYellow() { return *y; }
  Orange getOrange() { return *o; }
  Red getRed() { return *r; }

  bool Enable_Policy(bool status) override { this->status = true; }

  bool Insert_Policy(sqlite3 *db, int policyFieldID) override {
    char query[500];
    int rc;
    char *errMsg = 0;
    sprintf(
        query,
        "INSERT INTO FeedbackPolicy (PolicyFieldID, PolicyName, Description, "
        "GreenName, YellowName, OrangeName, RedName, DATETIME) "
        "VALUES ('%d', '%s', '%s', "
        "'%s', '%s', '%s' , '%s' ,'%s');",
        policyFieldID, policyName.c_str(), description.c_str(),
        getGreen().greenName.c_str(), getYellow().yellowName.c_str(),
        getOrange().orangeName.c_str(), getRed().redName.c_str(),
        DB_currentDateTime().c_str());
    rc = sqlite3_exec(db, query, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      std::cerr << "Insert FeedbackPolicy SQL error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return false;
    } else {
      std::cout << "[FeedbackPolicy] Insert" << std::endl;
      return true;
    }
  }
};

#endif /* A7230D0F_8B20_43C4_AC1B_DC2E34F2EB35 */
