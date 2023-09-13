#include "psu_server.hpp"
#include <math.h>

/**
 *
 */
int extern bPowerGD;
int count_m, count_h, count_d = 0;
/**
 * @brief 초기 데이터베이스가 생성되어있는지 확인하는 함수
 */
void check_db();
/* function prototype */
void DoStuff(void);
int find_first_index(int flag, sqlite3_stmt *statement, sqlite3 *db);
int min_cursor[2], hour_cursor[2], day_cursor[2] = {0};

int table_check = 0; /* 테이블 체크한번수행*/
int power_watt[2] = {0};

void psu_delay(int milliseconds) {
  long pause;
  clock_t now, then;

  pause = milliseconds * (CLOCKS_PER_SEC / 1000);
  now = then = clock();
  while ((now - then) < pause)
    now = clock();
}
int sqlite_run(char *in_query, sqlite3_stmt *statement, sqlite3 *db) {
  int rc, rt_value = 0;
  char query[500] = {0};
  sprintf(query, in_query);
  sqlite3_prepare_v2(db, query, -1, &statement, NULL);
  while (1) {
    rc = sqlite3_step(statement);
    if (SQLITE_ROW == rc) {
      rt_value = sqlite3_column_int(statement, 0);
    } else if (SQLITE_BUSY == rc || SQLITE_LOCKED == rc)
      continue;
    else
      break;
  }
  sqlite3_finalize(statement);
  return rt_value;
}
void exit_cleanup() { exit(0); }
/*
void timer_handler(void){
        time_t ltime = time(NULL);
        struct tm tm_local = *localtime(&ltime);
        char query[500] = {0}; // sqlite query string
        char *zErrMsg = 0;	// sqlite query error message
        sqlite3 *db;
        sqlite3_stmt *statement = NULL;
        int rc = 0;
        rc = sqlite3_open(POWER_USAGE_DB, &db);
        ltime = time(NULL);
        tm_local = *localtime(&ltime);
        if(count > 2){
                sprintf(query, "SELECT count(*) FROM last_min_1;");
                v_count = sqlite_run(query, statement, db);
                sqlite3_finalize(statement);
                printf("vcount = %d\n", v_count);
                count = 0;
        }
        count++;
        sqlite3_close(db);
}
*/
void *psu_handler(void) {
  try {

    // signal(SIGINT, exit_cleanup);
    // signal(SIGKILL, exit_cleanup);
    // signal(SIGTERM, exit_cleanup);
    printf("PSU Monitoring server Starting...\n");
    if (ast_gpio_open() < 0)
      printf("GPIP OPEN Failed!!\n");
    ast_gpio_direction_input(PCH_PWR_GOOD);
    struct itimerval it_val; /* for setting itimer */
    sqlite3 *db;
    sqlite3_stmt *statement = NULL;
    int rc = 0;

    try {
      if (!access(POWER_USAGE_DB, F_OK))
        check_db();
    } catch (const std::exception &) {
      log(info) << "psu_handler not open error : check_db";
    }
    // cout<<"psu database check"<<endl;
    log(info) << "psu_handler step 1";

    // cout<<"psu database end"<<endl;
    try {
      rc = sqlite3_open(POWER_USAGE_DB, &db);
    } catch (const std::exception &) {
      log(info) << "psu_handler not open error";
      return;
    }
    log(info) << "psu_handler step 2";
    min_cursor[0] = find_first_index(MIN_1, statement, db);
    // cout<<"psu database 1"<<endl;
    min_cursor[1] = find_first_index(MIN_2, statement, db);
    // cout<<"psu database 2"<<endl;
    hour_cursor[0] = find_first_index(HOUR_1, statement, db);
    // cout<<"psu database 3 "<<endl;
    hour_cursor[1] = find_first_index(HOUR_2, statement, db);
    // cout<<"psu database 4"<<endl;
    day_cursor[0] = find_first_index(DAY_1, statement, db);
    // cout<<"psu database5"<<endl;
    day_cursor[1] = find_first_index(DAY_2, statement, db);
    log(info) << "psu_handler step 3";
    // cout<<"psu database find_first_index..!"<<endl;

    sqlite3_close(db);
    sqlite3_finalize(statement);
    log(info) << "psu_handler step 4";
    // cout<<"psu database sqlite3_finalize..!"<<endl;
    while (1) {
      log(info) << "psu_handler step 5";
      DoStuff();
      log(info) << "psu_handler step 6";
      psu_delay(10000);
    }
  } catch (const std::exception &) {
    log(info) << "psu_handler error ";
  }
  // pause();
}
int find_first_index(int flag, sqlite3_stmt *statement, sqlite3 *db) {

  try {

    struct tm *parseTime;
    int row_count, l, rc, t_max, i_max = 0;
    char query[500] = {0};
    // cout<<"find_first_index 1"<<endl;
    switch (flag) {
    case MIN_1:
      sprintf(query, "SELECT count(*) FROM last_min_1;");
      row_count = sqlite_run(query, statement, db);
      sprintf(query, "SELECT dt FROM last_min_1;");
      break;
    case MIN_2:
      sprintf(query, "SELECT count(*) FROM last_min_2;");
      row_count = sqlite_run(query, statement, db);
      sprintf(query, "SELECT dt FROM last_min_2;");
      break;
    case HOUR_1:
      sprintf(query, "SELECT count(*) FROM last_hour_1;");
      row_count = sqlite_run(query, statement, db);
      sprintf(query, "SELECT dt FROM last_hour_1;");
      break;
    case HOUR_2:
      sprintf(query, "SELECT count(*) FROM last_hour_2;");
      row_count = sqlite_run(query, statement, db);
      sprintf(query, "SELECT dt FROM last_hour_2;");
      break;
    case DAY_1:
      sprintf(query, "SELECT count(*) FROM last_day_1;");
      row_count = sqlite_run(query, statement, db);
      sprintf(query, "SELECT dt FROM last_day_1;");
      break;
    case DAY_2:
      sprintf(query, "SELECT count(*) FROM last_day_1;");
      row_count = sqlite_run(query, statement, db);
      sprintf(query, "SELECT dt FROM last_day_2;");
      break;
    }
    // cout << "find_first_index 2 row_count" << row_count << endl;
    if (row_count != 0) {
      char db_datetime[row_count][100];
      struct tm tm_l[row_count];
      time_t time[row_count];

      try {
        cout << "sqlite3_prepare" << endl;
        sqlite3_prepare(db, query, -1, &statement, NULL);
        cout << "sqlite3_prepare end" << endl;
        while (1) {
          rc = sqlite3_step(statement);
          if (SQLITE_ROW == rc) {
            strcpy(db_datetime[l], (char *)sqlite3_column_text(statement, 0));
            l++;
          } else if (SQLITE_BUSY == rc || SQLITE_LOCKED == rc)
            continue;
          else
            break;
        }
        sqlite3_finalize(statement);
      } catch (const std::exception &) {
        // cout << "find_first_index error: step1" << endl;
      }

      l = 0;

      for (l = 0; l < row_count; l++) {
        strptime(db_datetime[l], "%Y-%m-%d %H:%M:%S", &tm_l[l]);
        time[l] = mktime(&tm_l[l]);
      }

      t_max = time[0];

      for (l = 0; l < row_count; l++) {
        if (t_max < time[l]) {
          t_max = time[l];
          i_max = l;
        }
      }
    }
    // cout << "find_first_index " << row_count << endl;
    return i_max + 1;
  }

  catch (const std::exception &e) {
    // cout << "find_first_index error" << endl;
    return false;
  }
}
/*
 * DoStuff
 */
void time_function(uint8_t t_year, uint8_t t_mon, uint8_t t_days,
                   uint8_t t_hour, uint8_t t_min, uint8_t t_sec, char *output) {
  char month[3], days[3], hour[3], min[3], sec[3] = {0};
  if ((t_mon + 1) < 10)
    sprintf(month, "0%d", t_mon + 1);
  else
    sprintf(month, "%d", t_mon + 1);

  if (t_days < 10)
    sprintf(days, "0%d", t_days);
  else
    sprintf(days, "%d", t_days);

  if (t_hour < 10)
    sprintf(hour, "0%d", t_hour);
  else
    sprintf(hour, "%d", t_hour);

  if (t_min < 10)
    sprintf(min, "0%d", t_min);
  else
    sprintf(min, "%d", t_min);

  if (t_sec < 10)
    sprintf(sec, "0%d", t_sec);
  else
    sprintf(sec, "%d", t_sec);

  sprintf(output, "%d-%s-%s %s:%s:%s", t_year + 1900, month, days, hour, min,
          sec);
}
void DoStuff(void) {
  int ret = 0;
  time_t ltime = time(NULL);
  struct tm tm_local = *localtime(&ltime);

  char query[500] = {0}; // sqlite query string
  char time_string[100] = {0};
  char *zErrMsg = 0; // sqlite query error message
  int v_count = 0;
  // int bPowerGD = 0;
  sqlite3 *db;
  sqlite3_stmt *statement = NULL;
  int rc = 0;

  try {
    rc = sqlite3_open(POWER_USAGE_DB, &db);
  } catch (const std::exception &) {
    log(info) << "psu_handler not open error";
    return;
  }

  bPowerGD = ast_get_gpio_value(PCH_PWR_GOOD);
  log(debug) << "doStuff bPowerGD ==" << bool(bPowerGD);
  // lightning_sensor_read(
  //    FRU_NVA, NVA_SENSOR_PSU1_WATT,
  //    &power_watt[0]); // sdr_sensor_read(NVA_SENSOR_PSU1_TEMP);
  // lightning_sensor_read(FRU_NVA, NVA_SENSOR_PSU2_WATT, &power_watt[1]);
  if (bPowerGD == 0) {
    if (power_watt[0] == 13)
      power_watt[0] = 0;
    if (power_watt[1] == 13)
      power_watt[1] = 0;
  }
  // cout<<"psu DoStuff count_m..!"<<count_m<<endl;
  if (count_m > 2) {

    sprintf(query, "SELECT count(*) FROM last_min_1;");
    v_count = sqlite_run(query, statement, db);
    sqlite3_finalize(statement);
    time_function(tm_local.tm_year, tm_local.tm_mon, tm_local.tm_mday,
                  tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec,
                  time_string);
    // printf("psu DoStuff time string %s\n", time_string);
    printf("psu DoStuff v_count %d\n", v_count);
    if (v_count < LAST_MIN_COUNT_DB)
      sprintf(query, "INSERT INTO last_min_1 (watt, dt) values(%d, \"%s\");",
              power_watt[0] * 10, time_string);

    else if (v_count >= LAST_MIN_COUNT_DB) {

      sprintf(query, "UPDATE last_min_1 SET watt=%d, dt=\"%s\" WHERE ID=%d;",
              power_watt[0] * 10, time_string, min_cursor[0]);
      if (min_cursor[0] < LAST_MIN_COUNT_DB)
        min_cursor[0] += 1;
      else
        min_cursor[0] = 1;
    }
    // printf("query : %s / cursor : %d\n", query, min_cursor[0]);
    sqlite3_prepare_v2(db, query, -1, &statement, NULL);
    // cout<<"psu DoStuff sqlite3_prepare..!"<<endl;
    rc = sqlite3_step(statement);
    sqlite3_finalize(statement);
    // cout<<"psu DoStuff sqlite3_finalize..!"<<endl;

    sprintf(query, "SELECT count(*) FROM last_min_2;");
    sqlite3_prepare_v2(db, query, -1, &statement, NULL);
    v_count = sqlite_run(query, statement, db);
    // cout<<"psu DoStuff sqlite_run v_count="<<v_count<<endl;
    sqlite3_finalize(statement);
    // cout<<"psu DoStuff statement ..!"<<endl;
    if (v_count < LAST_MIN_COUNT_DB)
      sprintf(query, "INSERT INTO last_min_2 (watt, dt) values(%d, \"%s\");",
              power_watt[1] * 10, time_string);

    else if (v_count >= LAST_MIN_COUNT_DB) {
      sprintf(query, "UPDATE last_min_2 SET watt=%d, dt=\"%s\" WHERE ID=%d;",
              power_watt[1] * 10, time_string, min_cursor[1]);
      if (min_cursor[1] < LAST_MIN_COUNT_DB)
        min_cursor[1] += 1;
      else
        min_cursor[1] = 1;
    }

    sqlite3_prepare_v2(db, query, -1, &statement, NULL);
    rc = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (count_h > 3) {

      sprintf(query, "SELECT count(*) FROM last_hour_1;");
      sqlite3_prepare_v2(db, query, -1, &statement, NULL);
      v_count = sqlite_run(query, statement, db);
      sqlite3_finalize(statement);
      // cout<<"psu DoStuff SELECT count(*) FROM last_hour_1 ..!"<<endl;

      if (v_count < LAST_HOUR_COUNT_DB)
        sprintf(query, "INSERT INTO last_hour_1 (watt, dt) values(%d, \"%s\");",
                power_watt[0] * 10, time_string);
      else if (v_count >= LAST_HOUR_COUNT_DB) {

        sprintf(query, "UPDATE last_hour_1 SET watt=%d, dt=\"%s\" WHERE ID=%d;",
                power_watt[0] * 10, time_string, hour_cursor[0]);
        if (hour_cursor[0] < LAST_HOUR_COUNT_DB)
          hour_cursor[0] += 1;
        else
          hour_cursor[0] = 1;
      }

      sqlite3_prepare_v2(db, query, -1, &statement, NULL);
      rc = sqlite3_step(statement);
      sqlite3_finalize(statement);

      sprintf(query, "SELECT count(*) FROM last_hour_2;");
      sqlite3_prepare_v2(db, query, -1, &statement, NULL);
      v_count = sqlite_run(query, statement, db);
      sqlite3_finalize(statement);

      if (v_count < LAST_HOUR_COUNT_DB)
        sprintf(query, "INSERT INTO last_hour_2 (watt, dt) values(%d, \"%s\");",
                power_watt[1] * 10, time_string);

      else if (v_count >= LAST_HOUR_COUNT_DB) {

        sprintf(query, "UPDATE last_hour_2 SET watt=%d, dt=\"%s\" WHERE ID=%d;",
                power_watt[1] * 10, time_string, hour_cursor[1]);
        if (hour_cursor[1] < LAST_HOUR_COUNT_DB)
          hour_cursor[1] += 1;
        else
          hour_cursor[1] = 1;
      }

      sqlite3_prepare_v2(db, query, -1, &statement, NULL);
      rc = sqlite3_step(statement);
      sqlite3_finalize(statement);

      if (count_d > 10) {

        sprintf(query, "SELECT count(*) FROM last_day_1;");
        sqlite3_prepare_v2(db, query, -1, &statement, NULL);
        v_count = sqlite_run(query, statement, db);
        sqlite3_finalize(statement);

        if (v_count < LAST_DAY_COUNT_DB)
          sprintf(query,
                  "INSERT INTO last_day_1 (watt, dt) values(%d, \"%s\");",
                  power_watt[0] * 10, time_string);

        else if (v_count >= LAST_DAY_COUNT_DB) {

          sprintf(query,
                  "UPDATE last_day_1 SET watt=%d, dt=\"%s\" WHERE ID=%d;",
                  power_watt[0] * 10, time_string, day_cursor[0]);
          if (day_cursor[0] < LAST_DAY_COUNT_DB)
            day_cursor[0] += 1;
          else
            day_cursor[0] = 1;
        }

        sqlite3_prepare(db, query, -1, &statement, NULL);
        rc = sqlite3_step(statement);
        sqlite3_finalize(statement);

        sprintf(query, "SELECT count(*) FROM last_day_2;");
        sqlite3_prepare(db, query, -1, &statement, NULL);
        v_count = sqlite_run(query, statement, db);
        sqlite3_finalize(statement);

        if (v_count < LAST_DAY_COUNT_DB)
          sprintf(query,
                  "INSERT INTO last_day_2 (watt, dt) values(%d, \"%s\");",
                  power_watt[1] * 10, time_string);

        else if (v_count >= LAST_DAY_COUNT_DB) {

          sprintf(query,
                  "UPDATE last_day_2 SET watt=%d, dt=\"%s\" WHERE ID=%d;",
                  power_watt[1] * 10, time_string, day_cursor[1]);
          if (day_cursor[1] < LAST_DAY_COUNT_DB)
            day_cursor[1] += 1;
          else
            day_cursor[1] = 1;
        }

        sqlite3_prepare(db, query, -1, &statement, NULL);
        rc = sqlite3_step(statement);
        sqlite3_finalize(statement);

        count_d = 0;
      }
      count_h = 0;
      count_d++;
    }

    count_m = 0;
    count_h++;
  }

  count_m++;

  sqlite3_close(db);
}

// memcpy(backup_db, db, sizeof(db));
//

int check_db_callback(void *NotUsed, int argc, char **argv, char **azColName) {
  // log(info)<<"check_db_callback start=====";
  if (argv[0] != NULL)
    table_check = atoi(argv[0]);
  else
    table_check = 0;
  // log(info)<<"check_db_callback end=====";
  return 0;
}

void check_db() {
  int rc = 0;
  char *zErrMsg;
  char query[500] = {0};
  sqlite3 *db;
  // log(info)<<"check_db start =====";
  try {
    rc = sqlite3_open(POWER_USAGE_DB, &db);
  } catch (const std::exception &) {
    log(info) << "psu_handler not open error";
    return;
  }

  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"last_min_1;\"");
  sqlite3_exec(db, query, check_db_callback, 0, &zErrMsg);

  // cout<<"psuserver check Dababase table_check =="<<table_check<<endl;

  if (table_check == 0) {
    sprintf(query, "CREATE TABLE last_min_1 (ID INTEGER NOT NULL PRIMARY KEY "
                   "AUTOINCREMENT, watt INTEGER NOT NULL, dt TEXT NOT NULL);");
    sqlite3_exec(db, query, 0, 0, &zErrMsg);
    // cout<<"psuserver CREATE TABLE last_min_1"<<endl;
  }
  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"last_min_2;\"");
  sqlite3_exec(db, query, check_db_callback, 0, &zErrMsg);
  if (table_check == 0) {
    sprintf(query, "CREATE TABLE last_min_2 (ID INTEGER NOT NULL PRIMARY KEY "
                   "AUTOINCREMENT, watt INTEGER NOT NULL, dt TEXT NOT NULL);");
    sqlite3_exec(db, query, 0, 0, &zErrMsg);
    // cout<<"psuserver CREATE TABLE last_min_2"<<endl;
  }

  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"last_hour_1;\"");
  sqlite3_exec(db, query, check_db_callback, 0, &zErrMsg);
  if (table_check == 0) {
    sprintf(query, "CREATE TABLE last_hour_1 (ID INTEGER NOT NULL PRIMARY KEY "
                   "AUTOINCREMENT, watt INTEGER NOT NULL, dt TEXT NOT NULL);");
    sqlite3_exec(db, query, 0, 0, &zErrMsg);
    // cout<<"psuserver CREATE TABLE last_hour_1"<<endl;
  }

  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"last_hour_2;\"");
  sqlite3_exec(db, query, check_db_callback, 0, &zErrMsg);
  if (table_check == 0) {
    sprintf(query, "CREATE TABLE last_hour_2 (ID INTEGER NOT NULL PRIMARY KEY "
                   "AUTOINCREMENT, watt INTEGER NOT NULL, dt TEXT NOT NULL);");
    // cout<<"psuserver CREATE TABLE last_hour_2"<<endl;
    sqlite3_exec(db, query, 0, 0, &zErrMsg);
  }

  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"last_day_1;\"");
  sqlite3_exec(db, query, check_db_callback, 0, &zErrMsg);
  if (table_check == 0) {
    sprintf(query, "CREATE TABLE last_day_1 (ID INTEGER NOT NULL PRIMARY KEY "
                   "AUTOINCREMENT, watt INTEGER NOT NULL, dt TEXT NOT NULL);");
    // cout<<"psuserver CREATE TABLE last_day_1"<<endl;
    sqlite3_exec(db, query, 0, 0, &zErrMsg);
  }

  sprintf(query,
          "SELECT COUNT(*) FROM sqlite_master where name=\"last_day_2;\"");
  sqlite3_exec(db, query, check_db_callback, 0, &zErrMsg);
  if (table_check == 0) {
    sprintf(query, "CREATE TABLE last_day_2 (ID INTEGER NOT NULL PRIMARY KEY "
                   "AUTOINCREMENT, watt INTEGER NOT NULL, dt TEXT NOT NULL);");
    sqlite3_exec(db, query, 0, 0, &zErrMsg);
    // cout<<"psuserver CREATE TABLE last_day_2"<<endl;
  }

  sqlite3_close(db);
  sqlite3_free(zErrMsg);
  // log(info)<<"check_db end =====";
}