
#include "ipmi/user.hpp"
#include <redfish/resource.hpp>
#include <sqlite3.h>
#define LOG_DB "/conf/log.db"
// user_serial 구조체 정의

int check_reading_db_callback1(void *NotUsed, int argc, char **argv,
                               char **azColName) {
  if (argv[0] != NULL) {
    *((int *)NotUsed) = atoi(argv[0]);
  }

  return 0;
}
// Database
void make_user_db() {
  sqlite3 *db;
  char *err_msg = 0;
  char query[500] = {0};
  int callback_value = 0;
  int rc = sqlite3_open(LOG_DB, &db);
  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"Usertable\";");
  rc = sqlite3_exec(db, query, check_reading_db_callback1, &callback_value,
                    &err_msg);

  if (callback_value == 0) {
    cout << "callback!!!!!!!!!!!!" << endl;
    sprintf(query, "CREATE TABLE Usertable("
                   "ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT NOT NULL, "
                   "password TEXT NOT NULL, "
                   "enable INT NOT NULL, "
                   "ipmi INT NOT NULL, "
                   "priv INT NOT NULL, "
                   "link_auth INT NOT NULL, "
                   "callin INT NOT NULL);");
    sqlite3_exec(db, query, 0, 0, &err_msg);
    for (int i = 0; i < MAX_USER; i++) {
      sprintf(query,
              "INSERT INTO Usertable (name, password, enable, ipmi, priv, "
              "link_auth, callin) "
              "VALUES(\"%s\", \"%s\", %d, %d, %d, %d, %d);",
              ipmiUser[i].name.c_str(), ipmiUser[i].password.c_str(),
              ipmiUser[i].enable, ipmiUser[i].ipmi, ipmiUser[i].priv,
              ipmiUser[i].link_auth, ipmiUser[i].callin);
      sqlite3_exec(db, query, 0, 0, &err_msg);
    }
  } else {
    for (int i = 0; i < MAX_USER; i++) {
      sprintf(query,
              "UPDATE Usertable SET name=\"%s\", password=\"%s\", "
              "enable=%d, ipmi=%d, priv=%d, link_auth=%d, callin=%d "
              "WHERE id=\"%d\";",
              ipmiUser[i].name.c_str(), ipmiUser[i].password.c_str(),
              ipmiUser[i].enable, ipmiUser[i].ipmi, ipmiUser[i].priv,
              ipmiUser[i].link_auth, ipmiUser[i].callin, i + 1);
      sqlite3_exec(db, query, 0, 0, &err_msg);
    }
  }

  sqlite3_close(db);
  sqlite3_free(err_msg);

  cout << " FIN!! " << endl;
}

Ipmiuser ipmiUser[MAX_USER];
/**
 *@brief ipmi일경우에만 사용  아닐경우 직접 접근해야함
 */
void Ipmiuser::setUserAccess(ipmi_req_t *request) {
  uint8_t en = request->data[0] & 0x80 ? 1 : 0;
  printf("setUserAccess = 0x0%x,\n", request->data[0]);
  printf("en = %d \n", en);
  if (en) {
    this->ipmi = request->data[0] & 0x10 ? 1 : 0;
    this->link_auth = request->data[0] & 0x20 ? 1 : 0;
    this->callin = request->data[0] & 0x40 ? 1 : 0;
    this->priv = request->data[2] & 0x0F;
    this->enable = 1;
    printf("this->ipmi =%d \n", this->ipmi);
    printf("this->link_auth =%d \n", this->link_auth);
    printf("this->callin =%d \n", this->callin);
    printf("this->priv =%d \n", this->priv);
    printf("this->enable =%d \n", this->enable);
  } else {
    this->priv = request->data[2] & 0x0F;
  }
  plat_user_save();
}
/**
 *@brief ipmi일경우에만 사용  아닐경우 직접 접근해야함
 */
void Ipmiuser::setUserPasswd(std::string _passwd) {
  this->password = _passwd;
  char buf[500] = {
      0,
  };

  sprintf(buf, "User Set User Password User ID : %s ", this->name);
  // ipmiLogEventHandler.Event_Registration(IpmiLogEvent(buf, "User",
  // "User"));
  plat_user_save();
}

/**
 *@brief ipmi일경우에만 사용  아닐경우 직접 접근해야함
 */
void Ipmiuser::setUserEnable(uint8_t _en) {
  this->enable = _en;
  char buf[500] = {
      0,
  };
  sprintf(buf, "User set User Enable User ID : %s ", this->name);
  // ipmiLogEventHandler.Event_Registration(IpmiLogEvent(buf, "User",
  // "User"));
  plat_user_save();
}
/**
 *@brief ipmi일경우에만 사용  아닐경우 직접 접근해야함
 */
void Ipmiuser::setUserName(std::string _name) {

  this->name = _name;
  char buf[500] = {
      0,
  };
  sprintf(buf, "User set User Name User ID : %s ", this->name);
  // ipmiLogEventHandler.Event_Registration(IpmiLogEvent(buf, "User",
  // "User"));
  plat_user_save();
}

void Ipmiuser::getUserAccess(uint8_t _flag, uint8_t _val, ipmi_res_t *response,
                             uint8_t *res_len) {
  uint8_t *data = &response->data[0];

  if (_flag == 1) {

    *data++ = MAX_ID;
    *data++ = this->enable;
    *data++ = _val;
    *data++ = (this->callin << 6) | (this->link_auth << 5) | (this->ipmi << 4) |
              (this->priv & 0xf);
  } else if (_flag == 2) {
    *data++ = MAX_ID;
    *data++ = (this->enable << 6) | (_val & 0x3f);
    *data++ = FIXED_ID & 0x3f;
    *data++ = (this->callin << 6) | (this->link_auth << 5) | (this->ipmi << 4) |
              (this->priv & 0xf);
  }
  *res_len = data - &response->data[0];
}

bool Ipmiuser::plat_user_init(uint8_t index) {

  int i, acc_user = 0;
  if (index == 0) {
    this->name = "root";
    this->password = "";
    this->enable = 1;
    this->ipmi = 1;
    this->priv = 4;
    this->link_auth = 1;
    this->callin = 1;
    return true;
  } else if (index == 1) {
    this->name = "admin";
    this->password = "admin";
    this->enable = 1;
    this->ipmi = 1;
    this->priv = 4;
    this->link_auth = 1;
    this->callin = 1;
    return true;
  } else
    return false;
}

bool Ipmiuser::plat_user_init(uint8_t index, std::string _username,
                              std::string _password) {
  if (index >= 2 && index <= 9) {
    this->name = _username;
    this->password = _password;
    this->enable = 1;
    this->ipmi = 0;
    this->priv = 0;
    this->link_auth = 0;
    callin = 0;
    return true;
  } else
    return false;
}

void Ipmiuser::deleteUser(uint8_t index) {
  if (index >= 2 && index <= 9) {
    this->name = "";
    this->password = "";
    this->enable = 0;
    this->ipmi = 0;
    this->priv = 0;
    this->link_auth = 0;
    this->callin = 0;
  }
  char buf[500] = {
      0,
  };
  sprintf(buf, "User Delete User User ID : %s ", this->name);
  // ipmiLogEventHandler.Event_Registration(IpmiLogEvent(buf, "User",
  // "User"));
  return;
}

uint8_t Ipmiuser::getUserPriv() { return this->priv; }

uint8_t Ipmiuser::getUserenable() { return this->enable; }

std::string Ipmiuser::getUsername() { return this->name; }

std::string Ipmiuser::getUserpassword() {
  // printUserInfo();
  return this->password;
}

uint8_t Ipmiuser::getUserCallin() { return this->callin; }

uint8_t Ipmiuser::getUserLinkAuth() { return this->link_auth; }

uint8_t Ipmiuser::getUserIpmi() { return this->ipmi; }

void Ipmiuser::printUserInfo(void) {
  std::cout << "print user info" << std::endl;
  std::cout << "username : " << this->name << std::endl;
  std::cout << "password : " << this->password << std::endl;
  std::cout << "enable : " << to_string(this->enable) << std::endl;
  std::cout << "callin : " << to_string(this->callin) << std::endl;
  std::cout << "link_auth : " << to_string(this->link_auth) << std::endl;
  std::cout << "ipmi : " << to_string(this->ipmi) << std::endl;
  std::cout << "priv : " << to_string(this->priv) << std::endl;
  std::cout << "limit : " << to_string(this->limit) << std::endl;
}

int get_user_cnt(void) {
  int cnt;
  for (cnt = 1; cnt < MAX_USER; cnt++) {
    if (ipmiUser[cnt].getUsername() == "")
      break;
  }
  return cnt;
}

int rest_make_user_json(char *res) {
  json::value obj = json::value::object();
  json::value USER = json::value::object();
  vector<json::value> INFO_VEC;
  Ipmiuser temp;
  int last_index = 0;
  int user_cnt = 0;

  //   last_index = user_loading();
  user_cnt = get_user_cnt();
  for (int i = 0; i < user_cnt; i++) {
    json::value INFO = json::value::object();

    INFO["INDEX"] = json::value::string(U(to_string(i + 1)));
    INFO["NAME"] = json::value::string(ipmiUser[i].getUsername());
    INFO["PASSWORD"] = json::value::string(ipmiUser[i].getUserpassword());
    INFO["ENABLE_STATUS"] =
        json::value::string(U(to_string(ipmiUser[i].getUserenable())));
    INFO["CALLIN"] =
        json::value::string(U(to_string(ipmiUser[i].getUserCallin())));
    INFO["LINKAUTH"] =
        json::value::string(U(to_string(ipmiUser[i].getUserLinkAuth())));
    INFO["IPMIMSG"] =
        json::value::string(U(to_string(ipmiUser[i].getUserIpmi())));
    INFO["PRIVILEGE"] =
        json::value::string(U(to_string(ipmiUser[i].getUserPriv())));

    INFO_VEC.push_back(INFO);
  }

  USER["INFO"] = json::value::array(INFO_VEC);
  obj["USER"] = USER;
  strncpy(res, obj.serialize().c_str(), obj.serialize().length());
  return strlen(res);
}

void plat_user_save(void) {
  cout << "plat_user_save " << endl;
  make_user_db();

  for (int i = 0; i < get_user_cnt(); i++) {
    Ipmiuser *user = &ipmiUser[i];
    string _uri = ODATA_ACCOUNT_ID + string("/") + to_string(i);

    if (record_is_exist(_uri)) {

      Account *account = (Account *)g_record[_uri];
      account->user_name = user->getUsername();
      account->password = user->getUserpassword();
      account->callin = (int)user->callin;
      account->ipmi = (int)user->ipmi;
      account->link_auth = (int)user->link_auth;
      account->priv = (int)user->priv;
      if ((int)user->enable)
        account->enabled = true;
      else
        account->enabled = false;
      resource_save_json(account);
    }
  }
}

void app_set_user_name_params(int index, char *name) {
  std::string s_name(name);
  ipmiUser[index].setUserName(s_name);
  plat_user_save();
  return;
}

void app_set_user_enable(int index, char enable) {
  ipmiUser[index].setUserEnable(enable);
  plat_user_save();
  return;
}

void app_set_user_passwd_params(ipmi_req_t *req, unsigned char *response) {
  ipmi_res_t *res = (ipmi_res_t *)response;
  int index = (req->data[0] & 0x1F) - 1;
  int operation = req->data[1] & 0x03;
  char password[20];

  switch (operation) {
  case 0:
    ipmiUser[index].setUserEnable(req->data[1]);
    res->cc = CC_SUCCESS;
    break;
  case 1:
    ipmiUser[index].setUserEnable(req->data[1]);
    res->cc = CC_SUCCESS;
    break;
  case 2: {
    memcpy(password, req->data + 2, 20);
    string s_passwd(password);
    ipmiUser[index].setUserPasswd(s_passwd);
    res->cc = CC_SUCCESS;
    break;
  }
  case 3: {
    memcpy(password, req->data + 2, 20);
    string s_passwd(password);
    if (s_passwd == ipmiUser[index].getUserpassword())
      res->cc = CC_SUCCESS;
    break;
  }
  }
  plat_user_save();
  return;
}

void app_set_user_access_params(ipmi_req_t *req) {
  int index = req->data[1] - 1;
  ipmiUser[index].setUserAccess(req);
  return;
}

void app_del_user(int index) {
  if (index < 2)
    fprintf(stderr, "Error : user %s cannot be deleted\n",
            ipmiUser[index].getUsername().c_str());
  else {
    int user_cnt;
    user_cnt = get_user_cnt();
    if (index < user_cnt - 1) {
      for (int i = index; i < user_cnt; i++) {
        ipmiUser[i] = Ipmiuser(ipmiUser[i + 1]);
      }
      ipmiUser[user_cnt - 1].deleteUser(user_cnt - 1);
    } else
      ipmiUser[index].deleteUser(index);
  }
  usercount = usercount - 1;

  plat_user_save();
  return;
}
int name_callback(void *data, int argc, char **argv, char ** /*azColName*/) {
  // 데이터를 저장할 std::string을 캐스팅하여 받아옵니다.
  std::string &name = *static_cast<std::string *>(data);

  // 콜백 함수에서는 첫 번째 레코드의 "name" 열에 해당하는 데이터를 추출하여
  // 저장합니다.
  if (argc > 0 && argv[0]) {
    name = argv[0];
  }

  return 0; // 콜백이 더 이상 호출되지 않도록 0을 반환합니다.
}
// static int user_callback(void *data, int argc, char **argv, char **colNames)
// {
//   IpmiUser *user = static_cast<IpmiUser *>(data);

//   if (argc >= 7) {
//     user->enable = std::stoi(argv[2]);
//     user->ipmi = std::stoi(argv[3]);
//     user->priv = std::stoi(argv[4]);
//     user->link_auth = std::stoi(argv[5]);
//     user->callin = std::stoi(argv[6]);

//     user->name = argv[0] ? argv[0] : "";
//     user->password = argv[1] ? argv[1] : "";
//   }

//   return 0;
// }
/**
 * @brief 유저읽기
 * @bug IPMI와 redfish간에 동기화문제 발생
 * 처음 수행시 redfish가 없을때를 대비한 소스코드 필요함
 * 읽어들이는 방식이아닌 무조건 root/ admin을 생성하지만 redfish에서 road json
 * 수행시 redfish를 읽어서 생성함
 */
int user_loading(void) {
  int last_index = 0;
  log(info) << "user_loading start";
  int i = 0;
  FILE *fp;
  sqlite3 *db;
  char *err_msg = 0;
  char query[500] = {0};
  int callback_value = 0;
  int rc = sqlite3_open(LOG_DB, &db);
  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"Usertable\";");
  rc = sqlite3_exec(db, query, check_reading_db_callback1, &callback_value,
                    &err_msg);

  if (callback_value == 0) {
    {
      ipmiUser[0].plat_user_init(0);
      ipmiUser[1].plat_user_init(1);
      cout << "\\nn\n\n\nreal????\n\n\n\nn\n\n" << endl;
      last_index += 2;
      plat_user_save();
    }
    return last_index;
  } else {
    for (int i = 0; i < MAX_USER; i++) {
      std::string name; // 결과를 저장할 문자열 변수
      std::string password;
      std::string enable;
      std::string ipmi;
      std::string priv;
      std::string link_auth;
      std::string callin;

      sprintf(query, "SELECT name FROM Usertable WHERE id=\"%s\";",
              to_string(i + 1).c_str());
      sqlite3_exec(db, query, name_callback, &name, &err_msg);
      ipmiUser[i].name = name;

      sprintf(query, "SELECT password FROM Usertable WHERE id=\"%s\";",
              to_string(i + 1).c_str());
      sqlite3_exec(db, query, name_callback, &password, &err_msg);
      ipmiUser[i].password = password;

      sprintf(query, "SELECT enable FROM Usertable WHERE id=\"%s\";",
              to_string(i + 1).c_str());
      sqlite3_exec(db, query, name_callback, &enable, &err_msg);
      try {
        ipmiUser[i].enable = stoi(enable);
      } catch (const std::exception &e) {
        // Handle stoi exception (e.g., invalid_argument, out_of_range)
        std::cerr << "Error parsing enable value for user " << i << ": "
                  << e.what() << std::endl;
        // You might want to set a default value or handle the error
        // appropriately
      }

      sprintf(query, "SELECT ipmi FROM Usertable WHERE id=\"%s\";",
              to_string(i + 1).c_str());
      sqlite3_exec(db, query, name_callback, &ipmi, &err_msg);
      try {
        ipmiUser[i].ipmi = stoi(ipmi);
      } catch (const std::exception &e) {
        // Handle stoi exception
        std::cerr << "Error parsing ipmi value for user " << i << ": "
                  << e.what() << std::endl;
        // Handle the error appropriately
      }

      // Similar try-catch blocks for priv, link_auth, and callin parsing...

      sprintf(query, "SELECT priv FROM Usertable WHERE id=\"%s\";",
              to_string(i + 1).c_str());
      sqlite3_exec(db, query, name_callback, &priv, &err_msg);
      try {
        ipmiUser[i].priv = stoi(priv);
      } catch (const std::exception &e) {
        // Handle stoi exception
        std::cerr << "Error parsing priv value for user " << i << ": "
                  << e.what() << std::endl;
        // Handle the error appropriately
      }

      sprintf(query, "SELECT link_auth FROM Usertable WHERE id=\"%s\";",
              to_string(i + 1).c_str());
      sqlite3_exec(db, query, name_callback, &link_auth, &err_msg);
      try {
        ipmiUser[i].link_auth = stoi(link_auth);
      } catch (const std::exception &e) {
        // Handle stoi exception
        std::cerr << "Error parsing link_auth value for user " << i << ": "
                  << e.what() << std::endl;
        // Handle the error appropriately
      }
      sprintf(query, "SELECT callin FROM Usertable WHERE id=\"%s\";",
              to_string(i + 1).c_str());
      sqlite3_exec(db, query, name_callback, &callin, &err_msg);
      try {
        ipmiUser[i].callin = stoi(callin);
      } catch (const std::exception &e) {
        // Handle stoi exception
        std::cerr << "Error parsing callin value for user " << i << ": "
                  << e.what() << std::endl;
        // Handle the error appropriately
      }
    }
    for (int i = 0; i < get_user_cnt(); i++) {
      ipmiUser[i].printUserInfo();
    }
    plat_user_save();
    log(info) << "user_loading complete";
    return 10;
  }
}