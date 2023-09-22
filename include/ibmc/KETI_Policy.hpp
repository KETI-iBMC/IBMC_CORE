#ifndef EE01B252_3645_44AD_819B_61886416A850
#define EE01B252_3645_44AD_819B_61886416A850

#pragma once
#include <new>
#include <policy/Policy.hpp>
#include <policy_adaptor.h>
#include <unordered_map>
DBus::BusDispatcher dispatcher;

static const char *POLICY_SERVER_NAME = "org.freedesktop.keti.bmc.policy";
static const char *POLICY_SERVER_PATH = "/org/freedesktop/keti/bmc/policy";

static int FanPolicyStructCallback(void *data, int argc, char **argv,
                                   char **colNames);
static int CPUPolicyStructCallback(void *data, int argc, char **argv,
                                   char **colNames);
static int FeedbackPolicyStructCallback(void *data, int argc, char **argv,
                                        char **colNames);
class Policy_Adaptor : public org::freedesktop::keti::bmc::policy_adaptor,
                       public DBus::IntrospectableAdaptor,
                       public DBus::ObjectAdaptor {
public:
  Policy_Adaptor(DBus::Connection &connection)
      : DBus::ObjectAdaptor(connection, POLICY_SERVER_PATH) {}
  // return값이 DBus struct인걸로 바꿔줘야함
  /* policyName에 해당하는 값 반환
  string policyName,
  string description,
  string algorithm,
  int desiredTemperature,
  string temperatureSource
  */
  ::DBus::Struct<std::string, std::string, std::string, int32_t, std::string>
  getFanPolicy(const std::string &policyName) override;
  // 업데이트 하는 함수 저장 된 DB값 바꿔줌
  int32_t setFanPolicyString(const std::string &policyName,
                             const std::string &attribute,
                             const std::string &attributeName) override;
  int32_t setFanPolicyInt(const std::string &policyName,
                          const std::string &attribute,
                          const int32_t &attributeValue) override;

  ::DBus::Struct<std::string, std::string, std::string, int32_t, int32_t>
  getCPUPolicy(const std::string &policyName) override;
  int32_t setCPUPolicyString(const std::string &policyName,
                             const std::string &attribute,
                             const std::string &attributeName) override;
  int32_t setCPUPolicyInt(const std::string &policyName,
                          const std::string &attribute,
                          const int32_t &attributeValue) override;
  // 정책명, 활성화 여부, 온도 범위, 목적지
  ::DBus::Struct<std::string, bool, int32_t, int32_t, std::string>
  getFeedbackPolicyGreen(const std::string &tableName,
                         const std::string &policyName) override;
  ::DBus::Struct<std::string, bool, int32_t, int32_t, int32_t, std::string>
  getFeedbackPolicyYellow(const std::string &tableName,
                          const std::string &policyName) override;
  ::DBus::Struct<std::string, bool, int32_t, int32_t, int32_t, std::string>
  getFeedbackPolicyOrange(const std::string &tableName,
                          const std::string &policyName) override;
  ::DBus::Struct<std::string, bool, int32_t, int32_t, int32_t, std::string,
                 bool, bool>
  getFeedbackPolicyRed(const std::string &tableName,
                       const std::string &policyName) override;
  int32_t setFeedbackPolicyString(const std::string &tableName,
                                  const std::string &policyName,
                                  const std::string &attribute,
                                  const std::string &attributeName) override;
  int32_t setFeedbackPolicyInt(const std::string &tableName,
                               const std::string &policyName,
                               const std::string &attribute,
                               const int32_t &attributeValue) override;
  int32_t PolicyInitialize() override;
};

// CREATE TABLE 쿼리문 - PolicyField
const char *createPolicyFieldQuery =
    "CREATE TABLE PolicyField ("
    "    PolicyFieldID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    Description TEXT,"
    "    Target TEXT,"
    "    IsActive BOOLEAN,"
    "    DATETIME TEXT"
    ");";

// CREATE TABLE 쿼리문 - FanPolicy
const char *createFanPolicyQuery =
    "CREATE TABLE FanPolicy ("
    "    FanPolicyID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    PolicyName TEXT,"
    "    Description TEXT,"
    "    Algorithm TEXT,"
    "    DesiredTemperature INT,"
    "    TemperatureSource TEXT,"
    "    DATETIME TEXT,"
    "    PolicyFieldID INTEGER,"
    "    "
    "    FOREIGN KEY (PolicyFieldID) REFERENCES "
    "PolicyField(PolicyFieldID)"
    ");";

// CREATE TABLE 쿼리문 - CPUPolicy
const char *createCPUPolicyQuery =
    "CREATE TABLE CPUPolicy ("
    "    CPUPolicyID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    PolicyName TEXT,"
    "    Description TEXT,"
    "    Mode TEXT,"
    "    BasicSpeedMHz INT,"
    "    MaxSpeedMHz INT,"
    "    DATETIME TEXT,"
    "    PolicyFieldID INTEGER,"
    "    FOREIGN KEY (PolicyFieldID) REFERENCES "
    "PolicyField(PolicyFieldID)"
    ");";
// CREATE TABLE 쿼리문 - Fan
const char *createFanTableQuery =
    "CREATE TABLE Fan ("
    "    FanID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    FanName TEXT,"
    "    PWM INT,"
    "    RPM INT,"
    "    slotNumber INT,"
    "    columnNumber INT,"
    "    chassisName TEXT,"
    "    Manufacturer TEXT,"
    "    Model TEXT,"
    "    MaxPWM INT,"
    "    MaxRPM INT,"
    "    FanController TEXT,"
    "    FanPolicyID INTEGER,"
    "    FOREIGN KEY (FanPolicyID) REFERENCES FanPolicy(FanPolicyID)"
    ");";
const char *createGreenQuery = "CREATE TABLE Green ("
                               "    GreenID INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "    Name TEXT,"
                               "    IsActive BOOLEAN, "
                               "    LowerThresholdUser INT, "
                               "    UpperThresholdUser INT, "
                               "    Destination TEXT,"
                               "    FeedbackID INTEGER,"
                               "    FOREIGN KEY (FeedbackID) REFERENCES "
                               "Feedback(FeedbackID)"
                               ");";
const char *createYellowQuery =
    "CREATE TABLE Yellow ("
    "    YellowID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "    Name TEXT,"
    "    IsActive BOOLEAN, "
    "    TargetTemperatureEstimatedTimeRefreshCycleSeconds INT, "
    "    LowerThresholdUser INT, "
    "    UpperThresholdUser INT, "
    "    Destination TEXT,"
    "    FeedbackID INTEGER,"
    "    FOREIGN KEY (FeedbackID) REFERENCES "
    "Feedback(FeedbackID)"
    ");";
const char *createOrangeQuery =
    "CREATE TABLE Orange ("
    "    OrangeID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "    Name TEXT,"
    "    IsActive BOOLEAN, "
    "    TargetTemperatureEstimatedTimeRefreshCycleSeconds INT, "
    "    LowerThresholdUser INT, "
    "    UpperThresholdUser INT, "
    "    Destination TEXT,"
    "    FeedbackID INTEGER,"
    "    FOREIGN KEY (FeedbackID) REFERENCES "
    "Feedback(FeedbackID)"
    ");";
const char *createRedQuery =
    "CREATE TABLE Red ("
    "    RedID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "    Name TEXT,"
    "    IsActive BOOLEAN, "
    "    TargetTemperatureEstimatedTimeRefreshCycleSeconds INT, "
    "    LowerThresholdUser INT, "
    "    UpperThresholdUser INT, "
    "    Destination TEXT, "
    "    CompulsoryControl BOOLEAN, "
    "    CauseAnalysis BOOLEAN, "
    "    FeedbackID INTEGER,"
    "    FOREIGN KEY (FeedbackID) REFERENCES "
    "Feedback(FeedbackID)"
    ");";
// CREATE TABLE 쿼리문 - Feedback
const char *createFeedbackPolicyQuery =
    "CREATE TABLE FeedbackPolicy ("
    "    FeedbackID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "    PolicyName TEXT, "
    "    Description TEXT, "
    "    GreenName TEXT, "
    "    YellowName TEXT, "
    "    OrangeName TEXT, "
    "    RedName TEXT, "
    "    DATETIME TEXT,"
    "    PolicyFieldID INTEGER,"
    "    FOREIGN KEY (PolicyFieldID) REFERENCES "
    "PolicyField(PolicyFieldID), "
    "    FOREIGN KEY (GreenName) REFERENCES "
    "Green(GreenName) "
    ");";
class KETI_Policy {
private:
  KETI_Policy();
  KETI_Policy(KETI_Policy &other);
  ~KETI_Policy() { only_one = false; };
  static KETI_Policy *phoenixInstance;

  static void Create() {
    static KETI_Policy m_instance;
    phoenixInstance = &m_instance;
  }
  static void Distory_KETI() { phoenixInstance->~KETI_Policy(); }
  bool Insert_Policy_Field(string des, POLICY_TYPE type, bool isActive);
  bool Insert_Policy(int policyFieldID, Policy *pol);
  bool Insert_Fans(Fan fan);
  bool Insert_Green(Green green);
  bool Insert_Yellow(Yellow yellow);
  bool Insert_Orange(Orange orange);
  bool Insert_Red(Red red);

public:
  static bool only_one;

  sqlite3 *db;
  void DB_print();
  Policy *Get_Policy(POLICY_TYPE type, int policyID);
  static KETI_Policy &Get_Instance() {
    if (!only_one) {
      new (phoenixInstance) KETI_Policy;
      atexit(Distory_KETI);
      only_one = true;
    } else {
      Create();
    }
    return *phoenixInstance;
  }
  // db에 접근 할 수 있는 함수
  bool Del_Energy_Saving_Disable_Policy();
  // sqlite3 getDB(){return db;}
  void Factory_Info() { cout << "Only One Policy Factory\n" << endl; }
  void Db_print();
};

#endif /* EE01B252_3645_44AD_819B_61886416A850 */
