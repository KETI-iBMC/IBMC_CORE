#include <cstdint>
#include <ibmc/KETI_Policy.hpp>

KETI_Policy *KETI_Policy::phoenixInstance = NULL;
bool KETI_Policy::only_one = true;
const std::string DB_currentDateTime() {
  time_t now = time(0); // 현재 시간을 time_t 타입으로 저장
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  // strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M",&tstruct); //
  // YYYY-MM-DD.HH:mm:ss 형태의 스트링
  strftime(buf, sizeof(buf), "%Y-%m-%d %X",
           &tstruct); // YYYY-MM-DD HH:mm:ss 형태의 스트링

  return buf;
}
int check_table_db_callback(void *NotUsed, int argc, char **argv,
                            char **azColName) {
  if (argv[0] != NULL) {
    *((int *)NotUsed) = atoi(argv[0]);
  }

  return 0;
}
// 콜백 함수
static int print_callback(void *data, int argc, char **argv, char **azColName) {
  for (int i = 0; i < argc; i++) {
    std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL")
              << std::endl;
  }
  return 0;
}
DBFan fan1(1, "Fan1", 50, 100, 2000, 1800, "ModelA", "ManufacturerA", "pid",
          1, 2, "KETI_CHASSIS", "FanPolicy1");
DBFan fan2(2, "Fan2", 50, 100, 2000, 1800, "ModelA", "ManufacturerA", "pid",
          1, 2, "KETI_CHASSIS", "FanPolicy1");
DBFan fan3(1, "Fan3", 50, 100, 2000, 1800, "ModelA", "ManufacturerA", "pid",
          1, 2, "KETI_CHASSIS", "FanPolicy2");
DBFan fan4(2, "Fan4", 50, 100, 2000, 1800, "ModelA", "ManufacturerA", "pid",
          1, 2, "KETI_CHASSIS", "FanPolicy2");       
KETI_Policy::KETI_Policy() {
  cout << "KETI_Policy Init " << endl;
  char *err_msg = 0;
  char query[500] = {0};
  int callback_value = 0;

  int rc = sqlite3_open(LOG_DB, &db);
  sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);

  /*
  * Energy_Saving_Manager Table 초기화
  */  
  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"PolicyField\";");

  rc = sqlite3_exec(db, query, check_table_db_callback, &callback_value,
                    &err_msg);
  sqlite3_exec(db, query, 0, 0, &err_msg);

  if (callback_value == 0) {
    sprintf(query, createPolicyFieldQuery);
    if(rc == SQLITE_OK){
      cout << "[PolicyField] Create" << endl;  
    }
    else{
      cout << "[PolicyField] Doesn't create" << err_msg << endl;
    }    
    
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    cout << "Insert policy field Data" << endl;

    Insert_Policy_Field("Insert policy field - fan", POLICY_TYPE::TYPE_FAN, true);
    Insert_Policy_Field("Insert policy field - cpu", POLICY_TYPE::TYPE_CPU, true);
    Insert_Policy_Field("Insert policy field - feedback", POLICY_TYPE::TYPE_FEEDBACK, true);    
  }
    
//---------------------------------------PolicyField---------------------------------------

  callback_value = 0;
  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"FanPolicy\";");
  rc = sqlite3_exec(db, query, check_table_db_callback, &callback_value,
                    &err_msg);
  sqlite3_exec(db, query, 0, 0, &err_msg);
  if (callback_value == 0) {

    sprintf(query, createFanPolicyQuery);
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
                   
    if (rc != SQLITE_OK) {
      std::cerr << "Create FanPolicy SQL error: " << err_msg << std::endl;
      sqlite3_free(err_msg);
    }
    else {
      cout << "[FanPolicy] Create" << endl;
    }
    cout << "Insert FanPolicy Data" << endl;
    //객체 생성해서 DB에 삽입 -> LFC를 Sensor1을 기준으로 80도로 조절하겠다~
    Fan_Policy* fanPolicy1 = new Fan_Policy("FanPolicy1 will use Algorithm as LFC until the temperature reaches 80 based on Sensor1", "FanPolicy1", "LFC",
                          "Sensor1", 80);
    Fan_Policy* fanPolicy2 = new Fan_Policy("FanPolicy2 will use Algorithm as BFC until the temperature reaches 80 based on Sensor2", "FanPolicy2", "BFC",
                          "Sensor2", 80);
    
                                                                 
    Insert_Policy(1, fanPolicy1);
    Insert_Policy(1, fanPolicy2);
    delete fanPolicy1;
    delete fanPolicy2;

  }
//---------------------------------------FanPolicy---------------------------------------  

//   callback_value = 0;
//   sprintf(query,
//           "SELECT COUNT(*) FROM sqlite_master where name=\"CPUPolicy\";");
//   rc = sqlite3_exec(db, query, check_table_db_callback, &callback_value,
//                     &err_msg);
//   sqlite3_exec(db, query, 0, 0, &err_msg);
//   if (callback_value == 0) {

//     sprintf(query, createCPUPolicyQuery);
//     rc = sqlite3_exec(db, query, 0, 0, &err_msg);
                   
//     if (rc != SQLITE_OK) {
//       std::cerr << "Create CPUPolicy SQL error: " << err_msg << std::endl;
//       sqlite3_free(err_msg);
//     }
//     else {
//       cout << "[CPUPolicy] Create" << endl;
//     }
//     cout << "Insert CPUPolicy Data" << endl;

//     CPU_Policy* cpuPolicy1 = new CPU_Policy("Cpu policy1.","CPUPolicy1","BasicClock", 1200, 1400);
//     CPU_Policy* cpuPolicy2 = new CPU_Policy("Cpu policy2.","CPUPolicy2","UnderClock", 1000, 1200);
//     CPU_Policy* cpuPolicy3 = new CPU_Policy("Cpu policy3.","CPUPolicy3","OverClock", 1400, 1600);
//     Insert_Policy(2, cpuPolicy1);
//     Insert_Policy(2, cpuPolicy2);
//     Insert_Policy(2, cpuPolicy3);  
//     delete cpuPolicy1;
//     delete cpuPolicy2;
//     delete cpuPolicy3;
//   }
// //---------------------------------------CPUPolicy---------------------------------------  


  callback_value = 0;
  sprintf(query, "SELECT COUNT(*) FROM sqlite_master where name=\"Fan\";");
  rc = sqlite3_exec(db, query, check_table_db_callback, &callback_value,
                    &err_msg);
  sqlite3_exec(db, query, 0, 0, &err_msg);
  if (callback_value == 0) {
    sprintf(query, createFanTableQuery);
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
      std::cerr << "Create Fan SQL error: " << err_msg << std::endl;
      sqlite3_free(err_msg);
    }
    else{
      cout << "[Fan] Create" << endl;
    }
                   
    Insert_Fans(fan1);
    Insert_Fans(fan2);
    Insert_Fans(fan3);
    Insert_Fans(fan4);

  }
//---------------------------------------Fan---------------------------------------  


  callback_value = 0;
  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"FeedbackPolicy\";");
  rc = sqlite3_exec(db, query, check_table_db_callback, &callback_value,
                    &err_msg);
  sqlite3_exec(db, query, 0, 0, &err_msg);
  if (callback_value == 0) {

    sprintf(query, createFeedbackPolicyQuery);
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
                   
    if (rc != SQLITE_OK) {
      std::cerr << "Create Feedback SQL error: " << err_msg << std::endl;
      sqlite3_free(err_msg);
    }
    else{
      cout << "[FeedbackPolicy] Create" << endl;
    }
    
    Feedback_Policy* feedback1 = new Feedback_Policy("CPUTemperature", true , true, 70, true , 80, true ,90);
    Insert_Policy(3, feedback1);
    Feedback_Policy* feedback2 = new Feedback_Policy("DIMMTemperature", true , true, 73, true , 83, true ,93);
    Insert_Policy(3, feedback2);
    Feedback_Policy* feedback3 = new Feedback_Policy("CabinetTemperature", true , true, 75, true , 85, true ,95);
    Insert_Policy(3, feedback3);
    Feedback_Policy* feedback4 = new Feedback_Policy("LM75Temperature", true , true, 77, true , 87, true ,97);
    Insert_Policy(3, feedback4);

    delete feedback1;
    delete feedback2;
    delete feedback3;
    delete feedback4;
  }
//---------------------------------------FeedbackPolicy---------------------------------------

  Update_Fan(fan1, "Fan1", "FanPolicy2");
  //Fan1을 FanPolicy2 정책으로 Update
  DB_print();

}
void KETI_Policy::DB_print() {
  char *err_msg = 0;
  char query[500] = {0};
  int callback_value = 0;
  int rc = 0;
  cout << "---------------------KETI Policy---------------------" << endl;
  cout << "---------------------Policy Field---------------------" << endl;
  sprintf(query, "SELECT * FROM PolicyField;");
  rc = sqlite3_exec(db, query, print_callback, &print_callback,
                    &err_msg);
  cout << "---------------------Fan Policy---------------------" << endl;                  
  sprintf(query, "SELECT * FROM FanPolicy;");
  rc = sqlite3_exec(db, query, print_callback, &print_callback,
                    &err_msg);
  sprintf(query, "SELECT * FROM Fan;");
  cout << "---------------------Fan---------------------" << endl;
  rc = sqlite3_exec(db, query, print_callback, &print_callback,
                    &err_msg);                    
  cout << "---------------------Feedback Policy---------------------" << endl;                  
  sprintf(query, "SELECT * FROM FeedbackPolicy;");
  rc = sqlite3_exec(db, query, print_callback, &print_callback,
                    &err_msg);

}
/**
 * @brief Insert_Policy_Field  테이블생성 현재는 단 한개의 테이블만 이
 * 존재해야함으로 외부생성금지
 *
 * @param des
 * @param type
 * @param isActive
 * @return true
 * @return false
 */
bool KETI_Policy::Insert_Policy_Field(string des, POLICY_TYPE type,
                                              bool isActive) {
  char query[500];
  char *errMsg = 0;
  int rc;
  string currentDateTime = DB_currentDateTime();
  // const char *deactivateQuery =
  //     "UPDATE EnergyManager SET IsActive = 0, Status = 'disable';";

  // rc = sqlite3_exec(db, deactivateQuery, 0, 0, &errMsg);

  // if (rc != SQLITE_OK) {
  //   std::cerr << "SQL error: " << errMsg << std::endl;
  //   sqlite3_free(errMsg);
  //   return false;
  // }

  sprintf(query,
          "INSERT INTO PolicyField (Description, DATETIME, "
          "Target, IsActive) "
          "VALUES('%s', '%s', '%s', %d);",
          des.c_str(), currentDateTime.c_str(), 
          PolicyoString(type).c_str(), isActive);
  rc = sqlite3_exec(db, query, 0, 0, &errMsg);

  if (rc != SQLITE_OK) {
    std::cerr << "Insert PolicyField SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return false;
  } else {
    std::cout << "[PolicyField] Insert" << std::endl;
    return true;
  }
}
bool KETI_Policy::Del_Energy_Saving_Disable_Policy() {
  char *errMsg = 0;
  int rc;

  // IsActive가 0인 데이터 삭제 쿼리 실행
  const char *deleteQuery = "DELETE FROM EnergyManager WHERE IsActive = 0;";

  rc = sqlite3_exec(db, deleteQuery, 0, 0, &errMsg);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return false;
  } else {
    std::cout << "Disabled policies deleted successfully" << std::endl;
    return true;
  }
}
bool KETI_Policy::Insert_Policy(int policyFieldID, Policy *pol) {
  char query[500];
  int rc;
  char *errMsg = 0;
  // type fan일경우
  if (pol->policy_type == POLICY_TYPE::TYPE_FAN) {
    Fan_Policy *fanPol = dynamic_cast<Fan_Policy *>(pol);
    if (fanPol) {
      fanPol->Insert_Policy(db, policyFieldID);
    } else {
      cout << "Fan_Policy Not polymorphic";
    }
  }
  else if (pol->policy_type == POLICY_TYPE::TYPE_CPU) {
    CPU_Policy *cpuPol = dynamic_cast<CPU_Policy *>(pol);
    if (cpuPol) {
      cpuPol->Insert_Policy(db, policyFieldID);

    } else {
      cout << "CPU_Policy Not polymorphic";
    }
  }  
  else if(pol->policy_type == POLICY_TYPE::TYPE_FEEDBACK){
    Feedback_Policy *feedback_Policy = dynamic_cast<Feedback_Policy *>(pol);
    if(feedback_Policy){
      feedback_Policy->Insert_Policy(db, policyFieldID);
    }
    else{
      cout << "Feedback_Policy Not polymorphic";
    }
  }
}

/**
 * @brief Policy는 new이니 사용후 delete 할껏. 선언한 정책을 가져오는 함수
 *
 * @param type
 * @param policyID
 * @return * Policy*
 */
Policy *KETI_Policy::Get_Policy(POLICY_TYPE type, int policyID) {

  Policy *return_pol = nullptr;
  if (POLICY_TYPE::TYPE_FAN == type) {
    return_pol = new Fan_Policy(db, policyID);
    return return_pol;
  }

  else {
    return nullptr;
  }
}
bool KETI_Policy::Insert_Fans(DBFan fan) { fan.Insert_Fan(db); }
//feedback Level 이거 외래키라서 고정시키면 안됨..

bool KETI_Policy::Update_Fan(DBFan fan, std::string fanName, std::string fanPolicyName){ 
  fan.fanPolicyName = fanPolicyName;
  fan.Update_Fan(db, fanName, fanPolicyName); 
}

/*

dbus FanPolicy

*/
/*
Fan 정보 return
getFan("Fan1") 
Fan1의 정보 return 

*/ 
::DBus::Struct< std::string, int32_t, int32_t, int32_t, int32_t, std::string > Policy_Adaptor::getFan(const std::string& FanName) {
//FanName, PWM, RPM, MaxPWM, MaxRPM, FanPolicyName 
  ::DBus::Struct< std::string, int32_t, int32_t, int32_t, int32_t, std::string > fan_struct;
  //db연결해서 policyName있나 확인 
  char query[500];
  char *errMsg = nullptr;
  int rc;
  sprintf(query,
        "SELECT * "
        "FROM Fan "
        "WHERE FanName = '%s';", FanName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, FanStructCallback, &fan_struct, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Get Fan SQL error: " << errMsg << std::endl;
    if (errMsg != nullptr) {
        sqlite3_free(errMsg);
    }    
    return ::DBus::Struct< std::string, int32_t, int32_t, int32_t, int32_t, std::string >();
  }
  cout << "fan구조체 return" << endl;
  return fan_struct; 
}

/*
FanPolicy1을 따르고 있는 Fan return 
getFanTargetPolicy("FanPolicy1");
return 2, Fan1, Fan2.. 이런식으로 
*/

::DBus::Struct< int32_t, std::string > Policy_Adaptor::getFanTargetPolicy(const std::string& FanPolicyName){
  ::DBus::Struct< int32_t, std::string > fanName_struct;
  //count 세는 sql문 필요 

  //Fan DB 들어가서 FanPolicyId가 처음엔 크기, 2번째는 이름들
  char query[500];
  char *errMsg = nullptr;
  int rc;
  int count;
  std::string fanNames; // 결과를 저장할 문자열
  sqlite3_stmt *stmt;
  const char *tail;
  sprintf(query,
        "SELECT COUNT(*) "
        "FROM Fan "
        "WHERE FanPolicyName = '%s';", FanPolicyName.c_str());  
  if (sqlite3_prepare_v2(KETI_Policy::Get_Instance().db, query, -1, &stmt, &tail) != SQLITE_OK) {
      std::cout << "count error" << std::endl;
  }

  // 쿼리를 실행하고 결과를 가져옵니다.
  if (sqlite3_step(stmt) == SQLITE_ROW) {
      // 결과가 존재하는 경우
      count = sqlite3_column_int(stmt, 0); // 첫 번째 열의 값을 정수로 가져옴
  }
  fanName_struct._1 = count;
  sprintf(query,
        "SELECT FanName "
        "FROM Fan "
        "WHERE FanPolicyName = '%s';", FanPolicyName.c_str());

  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, FanNameStructCallback, &fanNames, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Get FanTargerPolicy SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }
  fanName_struct._2 = fanNames;

  cout << "fanName구조체 return" << endl;

  return fanName_struct;  
}
static int FanNameStructCallback(void *data, int argc, char **argv, char **colNames) {
    std::string *result = static_cast<std::string *>(data);

    // 결과가 존재하는 경우
    if (argc > 0 && argv[0]) {
        if (!result->empty()) {
            *result += ", "; // 이미 결과가 있다면 쉼표로 구분
        }
        *result += argv[0]; // FanName 값을 덧붙임
    }

    return 0;

}
static int FanStructCallback(void *data, int argc, char **argv,
                            char **colNames){
  if(!data){
    std::cerr << "FanPolicyStruct : Invalid data pointer" << std::endl;
    return -1;
  }
  if(argc == 13){
  //*fan_policy_struct가 직접적으로 생기는거지 
  ::DBus::Struct< std::string, int32_t, int32_t, int32_t, int32_t, std::string > *fan_struct =
  static_cast<::DBus::Struct< std::string, int32_t, int32_t, int32_t, int32_t, std::string > *> (data);
    fan_struct->_1 = argv[1];  //FanName
    fan_struct->_2 = std::stoi(argv[2]); //PWM
    fan_struct->_3 = std::stoi(argv[3]); //RPM
    fan_struct->_4 = std::stoi(argv[9]); //MaxPWM
    fan_struct->_5 = std::stoi(argv[10]); //MaxRPM
    fan_struct->_6 = argv[12]; //FanPolicyName
    return 0;
  }
  else{
    std::cerr << "FanPolicyStruct : Invalid number of colums" << std::endl;
    return -1;
  }

}
/*
Fan 정책 바꾸는 용도
*/
int32_t Policy_Adaptor::setFan(const std::string& FanName, const std::string& FanName_string) {
  char query[500];
  char *errMsg = nullptr;
  int rc;
  //여기에 attribute 확인해서 그에 맞게 값을 넣어야함
  sprintf(query,
        "UPDATE Fan "
        "SET FanPolicyName = '%s' "
        "WHERE FanName = '%s';", FanName_string.c_str(), FanName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Set Fan SQL error: " << errMsg << std::endl;
    if (errMsg != nullptr) {
      sqlite3_free(errMsg);
    }
    return -1;
  }
  else{
    std:: cout << "Fan updated succesfully" << std::endl;
    return 0;
  }    
} 
 
::DBus::Struct< std::string, std::string, std::string, int32_t, std::string, std::string > Policy_Adaptor::getFanPolicy(const std::string& policyName) {
  ::DBus::Struct<std::string, std::string, std::string, int32_t, std::string, std::string> fan_policy_struct;
  //db연결해서 policyName있나 확인 
  char query[500];
  char *errMsg = nullptr;
  int rc;
  std::string fans;
  sprintf(query,
        "SELECT * "
        "FROM FanPolicy "
        "WHERE PolicyName = '%s';", policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, FanPolicyStructCallback, &fan_policy_struct, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Get FanPolicy SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return ::DBus::Struct<std::string, std::string, std::string, int32_t, std::string, std::string>();
  }
  sprintf(query,
        "SELECT FanName "
        "FROM Fan "
        "WHERE FanPolicyName = '%s';", policyName.c_str());

  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, FanNameStructCallback, &fans, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Get FanTargerPolicy SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }  
  cout << "Fan Policy구조체 return" << endl;
  fan_policy_struct._6 = fans;
  return fan_policy_struct;      
}

static int FanPolicyStructCallback(void *data, int argc, char **argv,
                            char **colNames){
  if(!data){
    std::cerr << "FanPolicyStruct : Invalid data pointer" << std::endl;
    return -1;
  }
  if(argc != 8){
    std::cerr << "FanPolicyStruct : Invalid number of colums" << std::endl;
    return -1;
  }
  //*fan_policy_struct가 직접적으로 생기는거지 
  ::DBus::Struct<std::string, std::string, std::string, int32_t, std::string> *fan_policy_struct =
  static_cast<::DBus::Struct<std::string, std::string, std::string, int32_t, std::string> *> (data);
  fan_policy_struct->_1 = argv[1];  //policyName
  fan_policy_struct->_2 = argv[2]; //description
  fan_policy_struct->_3 = argv[3]; //algorithm
  fan_policy_struct->_4 = std::stoi(argv[4]); //desiredTmperature
  fan_policy_struct->_5 = argv[5]; //temperatureSource
  return 0;
}
/* 입력 Fan1, Algorithm , LFC 
  FanPolicy 테이블에서 PolicyName이'Fan1'인 'Algorithm' 값을 'LFC'로 업데이트  
*/
//업데이트 하는 함수 저장 된 DB값 바꿔줌
int32_t Policy_Adaptor::setFanPolicyString(const std::string& policyName, const std::string& attribute, const std::string& attributeName){
  char query[500];
  char *errMsg = nullptr;
  int rc;
  //여기에 attribute 확인해서 그에 맞게 값을 넣어야함
  sprintf(query,
        "UPDATE FanPolicy "
        "SET '%s' = '%s' "
        "WHERE PolicyName = '%s';", attribute.c_str(), attributeName.c_str(), policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Set FanPolicySting SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return -1;
  }
  else{
    std:: cout << "Fan_Policy updated succesfully" << std::endl;
    return 0;
  }    
}
/* 입력 Fan1, DesiredTemperature , 80 
  FanPolicy 테이블에서 PolicyName이 'Fan1'인 'DesiredTemperature' 값을 '80'로 update 
*/
int32_t Policy_Adaptor::setFanPolicyInt(const std::string& policyName, const std::string& attribute, const int32_t& attributeValue){
  char query[500];
  char *errMsg = nullptr;
  int rc;
  sprintf(query,
        "UPDATE FanPolicy "
        "SET '%s' = '%d' "
        "WHERE PolicyName = '%s';", attribute.c_str() ,attributeValue, policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Set FanPolicyInt SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return -1;
  }
  else{
    std:: cout << "Fan_Policy updated succesfully" << std::endl;
    return 0;
  }  
}
int32_t Policy_Adaptor::createFanPolicy(const std::string& description, const std::string& policyName, const std::string& algorithm,
 const std::string& temperatureSource, const int32_t& desiredTemperature) {
  int result;
  Fan_Policy* newFanPolicy = new Fan_Policy(description, policyName, algorithm, temperatureSource, desiredTemperature);
  result = KETI_Policy::Get_Instance().Insert_Policy(1, newFanPolicy);
  delete newFanPolicy;
  std::cout << "[FanPolicy] Create" << std::endl;
  return result;
}
int32_t Policy_Adaptor::deleteFanPolicy(const std::string& policyName) {
  char query[500];
  int rc;
  char *errMsg = 0;
  int result = 0;
  sprintf(query,
          "DELETE FROM FanPolicy "
          "WHERE PolicyName = '%s'", policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);

  if (rc != SQLITE_OK) {
      std::cerr << "DELETE error: " << sqlite3_errmsg(KETI_Policy::Get_Instance().db) << std::endl;
      sqlite3_exec(KETI_Policy::Get_Instance().db, "ROLLBACK;", 0, 0, 0); // 롤백
      result = 0;
      return result;
  } else {
      sqlite3_exec(KETI_Policy::Get_Instance().db, "COMMIT;", 0, 0, 0); // 커밋
      std::cout << "[FanPolicy] Delete" << std::endl;
      result = 1;
      return result;
  }
}

/*

dbus CPUPolicy

*/
::DBus::Struct< std::string, std::string, std::string, int32_t, int32_t> Policy_Adaptor::getCPUPolicy(const std::string& policyName){
  ::DBus::Struct<std::string, std::string, std::string, int32_t, int32_t> cpu_policy_struct;
  //db연결해서 policyName있나 확인 
  char query[500];
  char *errMsg = nullptr;
  int rc;
  sprintf(query,
        "SELECT *"
        "FROM CPUPolicy "
        "WHERE PolicyName = '%s';", policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, CPUPolicyStructCallback, &cpu_policy_struct, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }
  cout << "feedback에서 요청해서 cpu_policy구조체 return" << endl;
  return cpu_policy_struct;      
}
static int CPUPolicyStructCallback(void *data, int argc, char **argv,
                            char **colNames){
  if(!data){
    std::cerr << "CPUPolicyStruct : Invalid data pointer" << std::endl;
    return -1;
  }
  if(argc != 8){
    std::cerr << "CPUPolicyStruct : Invalid number of colums" << std::endl;
    return -1;
  }//*cpu_policy_struct가 직접적으로 생기는거지 
  ::DBus::Struct<std::string, std::string, std::string, int32_t, std::string> *cpu_policy_struct =
  static_cast<::DBus::Struct<std::string, std::string, std::string, int32_t, std::string> *> (data);
  cpu_policy_struct->_1 = argv[1];  //policyName
  cpu_policy_struct->_2 = argv[2]; //description
  cpu_policy_struct->_3 = argv[3]; //mode
  cpu_policy_struct->_4 = std::stoi(argv[4]); //basicSpeed
  cpu_policy_struct->_5 = std::stoi(argv[5]); //maxSpeed
  return 0;
}
int32_t Policy_Adaptor::setCPUPolicyString(const std::string& policyName, const std::string& attribute, const std::string& attributeName) {
  char query[500];
  char *errMsg = nullptr;
  int rc;
  //여기에 attribute 확인해서 그에 맞게 값을 넣어야함
  sprintf(query,
        "UPDATE CPUPolicy "
        "SET '%s' = '%s' "
        "WHERE PolicyName = '%s';", attribute.c_str(), attributeName.c_str(), policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return -1;
  }
  else{
    std:: cout << "CPU_Policy updated succesfully" << std::endl;
    return 0;
  }      
}
int32_t Policy_Adaptor::setCPUPolicyInt(const std::string& policyName, const std::string& attribute, const int32_t& attributeValue) {
  char query[500];
  char *errMsg = nullptr;
  int rc;
  //여기에 attribute 확인해서 그에 맞게 값을 넣어야함
  sprintf(query,
        "UPDATE CPUPolicy "
        "SET '%s' = '%s' "
        "WHERE PolicyName = '%s';", attribute.c_str(), attributeValue, policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return -1;
  }
  else{
    std:: cout << "CPU_Policy updated succesfully" << std::endl;
    return 0;
  }      
}
/*

dbus FeedbackPolicy

*/

::DBus::Struct< std::string, bool, bool, int32_t, bool, int32_t, bool, int32_t > Policy_Adaptor::getFeedbackPolicy(const int32_t& policyID) {
  ::DBus::Struct< std::string, bool, bool, int32_t, bool, int32_t, bool, int32_t > feedback_policy_struct;
  //db연결해서 policyName있나 확인 
  char query[500];
  char *errMsg = nullptr;
  int rc;  
  sprintf(query,
        "SELECT *"
        "FROM FeedbackPolicy "
        "WHERE FeedbackID = '%d';", policyID);
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, FeedbackPolicyStructCallback, &feedback_policy_struct, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }
  cout << "Get Feedback Policy" << endl;

  return feedback_policy_struct;     
}
static int FeedbackPolicyStructCallback(void *data, int argc, char **argv,
                                char **colNames){
  if(!data){
    std::cerr << "FeedbackPolicyStruct : Invalid data pointer" << std::endl;
    return -1;
  }
  if(argc != 10){
    std::cerr << "FeedbackPolicyStruct : Invalid number of colums" << std::endl;
    return -1;
  }//*Feedback_policy_struct가 직접적으로 생기는거지 
  ::DBus::Struct< std::string, bool, bool, int32_t, bool, int32_t, bool, int32_t > *feedback_policy_struct =
  static_cast<::DBus::Struct< std::string, bool, bool, int32_t, bool, int32_t, bool, int32_t > *> (data);
  feedback_policy_struct->_1 = argv[1];  //policyName
  feedback_policy_struct->_2 = argv[2]; //green active
  feedback_policy_struct->_3 = argv[3]; //yellow active
  feedback_policy_struct->_4 = std::stoi(argv[4]); //yellow temperature int
  feedback_policy_struct->_5 = argv[5]; //orange active
  feedback_policy_struct->_6 = std::stoi(argv[6]);//orange temperature
  feedback_policy_struct->_7 = argv[7]; //red active
  feedback_policy_struct->_8 = std::stoi(argv[8]);//red temperature  

  return 0;

}    
int32_t Policy_Adaptor::setFeedbackPolicyString(const std::string& policyName, const std::string& attribute, const std::string& attributeName) {
  char query[500];
  char *errMsg = nullptr;
  int rc;
  //여기에 attribute 확인해서 그에 맞게 값을 넣어야함
  sprintf(query,
        "UPDATE FeedbackPolicy "
        "SET '%s' = '%s' "
        "WHERE PolicyName = '%s';", attribute.c_str(), attributeName.c_str(), policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Set FeedbackPolicySting SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return -1;
  }
  else{
    std:: cout << "Feedback Policy updated succesfully" << std::endl;
    return 0;
  }      
}
int32_t Policy_Adaptor::setFeedbackPolicyInt(const std::string& policyName, const std::string& attribute, const int32_t& attributeValue) {
  char query[500];
  char *errMsg = nullptr;
  int rc;
  sprintf(query,
        "UPDATE FeedbackPolicy "
        "SET '%s' = '%d' "
        "WHERE PolicyName = '%s';", attribute.c_str() ,attributeValue, policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Set FeedbackPolicyInt SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return -1;
  }
  else{
    std:: cout << "Feedback Policy updated succesfully" << std::endl;
    return 0;
  }    
}
int32_t Policy_Adaptor::createFeedbackPolicy(const std::string& policyName, const int32_t& yellowTemperature, const int32_t& orangeTemperature, const int32_t& redTemperature) {
  int result;
  Feedback_Policy* newFeedbackPolicy = new Feedback_Policy(policyName, true, true, yellowTemperature, true, orangeTemperature,true, redTemperature);
  result = KETI_Policy::Get_Instance().Insert_Policy(3, newFeedbackPolicy);
  delete newFeedbackPolicy;
  std::cout << "[FeedbackPolicy] Create" << std::endl;
  return result;
}
int32_t Policy_Adaptor::deleteFeedbackPolicy(const std::string& policyName) {
  char query[500];
  int rc;
  char *errMsg = 0;
  int result = 0;
  sprintf(query,
          "DELETE FROM FeedbackPolicy "
          "WHERE PolicyName = '%s'", policyName.c_str());
  rc = sqlite3_exec(KETI_Policy::Get_Instance().db, query, 0, 0, &errMsg);

  if (rc != SQLITE_OK) {
      std::cerr << "DELETE error: " << sqlite3_errmsg(KETI_Policy::Get_Instance().db) << std::endl;
      sqlite3_exec(KETI_Policy::Get_Instance().db, "ROLLBACK;", 0, 0, 0); // 롤백
      result = 0;
      return result;
  } else {
      sqlite3_exec(KETI_Policy::Get_Instance().db, "COMMIT;", 0, 0, 0); // 커밋
      std::cout << "[FeedbackPolicy] Delete" << std::endl;
      result = 1;
      return result;
  }  
}
int32_t Policy_Adaptor::PolicyInitialize() {
  KETI_Policy::Get_Instance().only_one = false;

  if (std::remove(LOG_DB) != 0) {
      std::perror("Error deleting file");
  } else {
      std::cout << "File deleted successfully." << std::endl;
  }
  //LOG_DB삭제 후 KETI_Policy();수행
  KETI_Policy::Get_Instance();
  

}

// int main() {
//   KETI_Policy::Get_Instance().Factory_Info();
  
//   std::cout << "  Server Start " << std::endl;

//   DBus::default_dispatcher = &dispatcher;

//   DBus::Connection conn = DBus::Connection::SystemBus();
//   conn.request_name(POLICY_SERVER_NAME);

//   Policy_Adaptor server(conn);

//   dispatcher.enter();
//   return 0;
// }
