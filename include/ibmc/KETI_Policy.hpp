#ifndef POLICY_ADAPTOR_H 
#define POLICY_ADAPTOR_H
#include <ibmc/Policy.hpp>
#include <dbus/policy_adaptor.h>

static const char *POLICY_SERVER_NAME = "org.freedesktop.keti.bmc.policy";
static const char *POLICY_SERVER_PATH = "/org/freedesktop/keti/bmc/policy";
class Policy_Adaptor : public org::freedesktop::keti::bmc::policy_adaptor,
                       public DBus::IntrospectableAdaptor,
                       public DBus::ObjectAdaptor
{
  public:

    Policy_Adaptor(DBus::Connection &connection) : DBus::ObjectAdaptor(connection, POLICY_SERVER_PATH){}
    //return값이 DBus struct인걸로 바꿔줘야함 
    /* policyName에 해당하는 값 반환
    string policyName,
    string description,
    string algorithm,
    int desiredTemperature,
    string temperatureSource
    */
    ::DBus::Struct< std::string, int32_t, int32_t, int32_t, int32_t, std::string > getFan(const std::string& FanName) override;
    ::DBus::Struct< int32_t, std::string > getFanTargetPolicy(const std::string& FanPolicyName) override;
    int32_t setFan(const std::string& FanName, const std::string& FanName_string) override;
    ::DBus::Struct< std::string, std::string, std::string, int32_t, std::string, std::string > getFanPolicy(const std::string& policyName) override;
    //업데이트 하는 함수 저장 된 DB값 바꿔줌
    int32_t setFanPolicyString(const std::string& policyName, const std::string& attribute, const std::string& attributeName) override;
    int32_t setFanPolicyInt(const std::string& policyName, const std::string& attribute, const int32_t& attributeValue) override;
    int32_t createFanPolicy(const std::string& description, const std::string& policyName, const std::string& algorithm, const std::string& temperatureSource, const int32_t& desiredTemperature) override;
    int32_t deleteFanPolicy(const std::string& policyName) override;
    ::DBus::Struct< std::string, std::string, std::string, int32_t, int32_t > getCPUPolicy(const std::string& policyName) override;
    int32_t setCPUPolicyString(const std::string& policyName, const std::string& attribute, const std::string& attributeName) override;
    int32_t setCPUPolicyInt(const std::string& policyName, const std::string& attribute, const int32_t& attributeValue) override;
    ::DBus::Struct< std::string, bool, bool, int32_t, bool, int32_t, bool, int32_t > getFeedbackPolicy(const int32_t& policyID) override;
    int32_t setFeedbackPolicyString(const std::string& policyName, const std::string& attribute, const std::string& attributeName) override;
    int32_t setFeedbackPolicyInt(const std::string& policyName, const std::string& attribute, const int32_t& attributeValue) override;
    int32_t createFeedbackPolicy(const std::string& policyName, const int32_t& yellowTemperature, const int32_t& orangeTemperature, const int32_t& redTemperature) override;
    int32_t deleteFeedbackPolicy(const std::string& policyName) override;
    int32_t PolicyInitialize() override;
  
};
#endif //POLICY_ADAPTOR_H
#ifndef F511DF33_036E_452C_8A68_37279B70A447
#define F511DF33_036E_452C_8A68_37279B70A447
#pragma once
#include <new>
#include <ibmc/Policy.hpp>
#include <unordered_map>

static int FanNameStructCallback(void *data, int argc, char **argv,
                                char **colNames); 
static int FanStructCallback(void *data, int argc, char **argv,
                                char **colNames); 
static int FanPolicyStructCallback(void *data, int argc, char **argv,
                                char **colNames);
static int CPUPolicyStructCallback(void *data, int argc, char **argv,
                                char **colNames);
static int FeedbackPolicyStructCallback(void *data, int argc, char **argv,
                                char **colNames);                                                                

// CREATE TABLE 쿼리문 - PolicyField
static const char *createPolicyFieldQuery =
    "CREATE TABLE PolicyField ("
    "    PolicyFieldID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    Description TEXT,"
    "    Target TEXT,"
    "    IsActive BOOLEAN,"
    "    DATETIME TEXT"
    ");";
// CREATE TABLE 쿼리문 - CPUPolicy
static const char *createCPUPolicyQuery =
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
    "    PolicyField(PolicyFieldID) "
    "    ON DELETE CASCADE "
    ");";   
// CREATE TABLE 쿼리문 - FanPolicy
static const char *createFanPolicyQuery =
    "CREATE TABLE FanPolicy ("
    "    FanPolicyID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "    PolicyName TEXT UNIQUE, "
    "    Description TEXT, "
    "    Algorithm TEXT, "
    "    DesiredTemperature INT, "
    "    TemperatureSource TEXT, "
    "    DATETIME TEXT, "
    "    PolicyFieldID INTEGER, "
    "    FOREIGN KEY (PolicyFieldID) REFERENCES "
    "    PolicyField(PolicyFieldID) "
    ");";
// CREATE TABLE 쿼리문 - Fan
static const char *createFanTableQuery =
    "CREATE TABLE Fan ("
    "    FanID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "    FanName TEXT, "
    "    PWM INT, "
    "    RPM INT, "
    "    slotNumber INT, "
    "    columnNumber INT, "
    "    chassisName TEXT, "
    "    Manufacturer TEXT, "
    "    Model TEXT, "
    "    MaxPWM INT, "
    "    MaxRPM INT, "
    "    FanController TEXT, "
    "    FanPolicyName TEXT, "
    "    FOREIGN KEY (FanPolicyName) REFERENCES FanPolicy(PolicyName) "
    ");";
         
// CREATE TABLE 쿼리문 - Feedback
static const char *createFeedbackPolicyQuery =
    "CREATE TABLE FeedbackPolicy ("
    "    FeedbackID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "    PolicyName TEXT, "
    "    GreenIsActive BOOLEAN, "
    "    YellowIsActive BOOLEAN, "
    "    YellowTemperature INT, "
    "    OrangeIsActive BOOLEAN, "    
    "    OrangeTemperature INT, "
    "    RedIsActive BOOLEAN, "
    "    RedTemperature INT, "
    "    PolicyFieldID INTEGER,"
    "    FOREIGN KEY (PolicyFieldID) REFERENCES "
    "PolicyField(PolicyFieldID) "
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
  bool Insert_Fans(DBFan fan);
  bool Update_Fan(DBFan fan, std::string fanName, string fanPolicyName);

public:
  static bool only_one;
  bool Insert_Policy(int policyFieldID, Policy *pol);

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
  //db에 접근 할 수 있는 함수 
  bool Del_Energy_Saving_Disable_Policy();
  //sqlite3 getDB(){return db;}
  void Factory_Info() { cout << "Only One Policy Factory\n" << endl; }
  void Db_print();
};

#endif /* F511DF33_036E_452C_8A68_37279B70A447 */
