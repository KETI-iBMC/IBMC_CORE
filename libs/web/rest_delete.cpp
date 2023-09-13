#include "ipmi/rest_delete.hpp"
#define USB_HID_STORED_DIRECTORY_PATH                                          \
  "/sys/kernel/config/usb_gadget/mass-storage"
extern Ipmiuser ipmiUser[10];
bool Ipmiweb_DEL::Del_Usb(json::value request_json) {

  string vm_id;
  get_value_from_json_key(request_json, "ID", vm_id);

  cout << "vm_id : " << vm_id << endl;

  string odata_id = ODATA_MANAGER_ID;
  odata_id.append("/VirtualMedia/").append(vm_id);

  VirtualMedia *vm = (VirtualMedia *)g_record[odata_id];
  string image_path = vm->image_name;
  string hid = USB_HID_STORED_DIRECTORY_PATH;
  string hid_path = hid.append(vm_id);
  // string cmd="echo > "+hid_path+"/functions/mass_storage.usb0/lun.0/file";
  string cmd = "echo > " + hid_path + "/UDC";
  system(cmd.c_str());
  cout << "cmd =" << cmd << endl;

  vm->inserted = false;
  vm->image = "";
  vm->size = "";
  resource_save_json(vm);

  return true;
}
bool Ipmiweb_DEL::Del_Usb_permanent() {
  string odata_id = ODATA_MANAGER_ID;
  odata_id.append("/").append("VirtualMedia");

  bool exist = false;

  if (!record_is_exist(odata_id)) {
    log(warning) << "[Web Del_Usb_permanent] : No USB";
    return false;
  }
  Collection *usb_col = (Collection *)g_record[odata_id];
  cout << "usb count =" << usb_col->members.size() << endl;
  ;
  // std::vector<Resource *>::iterator iter;
  //   for(iter = usb_col->members.begin(); iter != usb_col->members.end();
  //   iter++)
  for (int i = 0; i < usb_col->members.size(); i++) {
    VirtualMedia *acc = (VirtualMedia *)usb_col->members[i];
    cout << "id =" << acc->id << endl;
    if (acc->inserted) {
      string hid = USB_HID_STORED_DIRECTORY_PATH;
      string hid_path = hid.append(acc->id);
      string cmd = "echo > " + hid_path + "/UDC";
      system(cmd.c_str());
      cout << "cmd =" << cmd << endl;
    }

    cout << "2" << endl;
    string delete_name = odata_id;
    delete_name.append("/").append(acc->id).append(".json");
    fs::remove(delete_name);
    cout << "3 name =" << delete_name << endl;

    cout << "4" << endl;
    // // delete(*iter);
    // cout<<"5"<<endl;
    // usb_col->members.erase(usb_col->members.begin()+i);
    // cout<<"6"<<endl;
    // resource_save_json(usb_col);
    // cout<<"7"<<endl;
  }
  cout << "5" << endl;
  usb_col->members.clear();
  cout << "6" << endl;
  resource_save_json(usb_col);
  cout << "7" << endl;
  return true;
}
bool Ipmiweb_DEL::Del_User(int index) {

  app_del_user(index + 1);

  // [테스트] Account 레드피시로 적용
  // index가 곧 id인 셈이니까 id로 찾아가서 delete해버리면 된다.

  // 해당 index(id)의 계정이 없을때 false
  // 그 다음엔 account collection에서 id 일치하는놈 찾아서 삭제
  string odata_id = ODATA_ACCOUNT_ID;
  odata_id.append("/").append(to_string(index));
  bool exist = false;

  if (!record_is_exist(odata_id)) {
    log(warning) << "[Web User Delete] : No Account";
    return false;
  }

  Collection *account_col = (Collection *)g_record[ODATA_ACCOUNT_ID];
  std::vector<Resource *>::iterator iter;
  for (iter = account_col->members.begin(); iter != account_col->members.end();
       iter++) {
    Account *acc = (Account *)*iter;

    if (acc->id == to_string(index)) {
      // find user
      exist = true;
      break;
    }
  }

  if (exist) {
    unsigned int id_num;
    id_num = improved_stoi(((Account *)g_record[odata_id])->id);
    delete_numset_num(ALLOCATE_ACCOUNT_NUM, id_num);
    account_col->members.erase(iter);
    ipmiUser[id_num].setUserName("");
    ipmiUser[id_num].setUserPasswd("");
    ipmiUser[id_num].callin = 0;
    ipmiUser[id_num].link_auth = 0;
    ipmiUser[id_num].ipmi = 0;
    ipmiUser[id_num].priv = 0;
    ipmiUser[id_num].enable = 0;
    plat_user_save();
    // 위에서 아래로 이동
    delete (*iter);
    delete_resource(odata_id);
    resource_save_json(account_col);
    return true;
  } else
    return false;
}
