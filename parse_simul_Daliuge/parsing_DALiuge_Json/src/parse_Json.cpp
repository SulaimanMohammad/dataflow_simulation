
#include "parse_Json.h"
#include <algorithm>

std::vector<APP_Drop> APPDrops;
std::vector<Data_Drop> DataDrops;
std::vector<int> partitions;

static const char* kTypeNames[] = {"Null", "False", "True", "Object", "Array", "String", "Number"};
// these variable to extract the key of the drop dependency
// for example 2022-08-06T19:19:45_-5_0 for output means "-5" is the dependcnny drop
unsigned left_key;
unsigned right_key;
std::string drop_key;
std::string depend_drop;
void extract_number(int& object_var, rapidjson::Value::ConstMemberIterator itr_parse)
{
  if (std::strcmp(kTypeNames[itr_parse->value.GetType()], "Number") == 0)
    object_var = itr_parse->value.GetInt64();
  if (std::strcmp(kTypeNames[itr_parse->value.GetType()], "String") == 0)
    object_var = std::stoi(itr_parse->value.GetString());
}

void extract_number(double& object_var, rapidjson::Value::ConstMemberIterator itr_parse)
{
  if (std::strcmp(kTypeNames[itr_parse->value.GetType()], "Number") == 0)
    object_var = itr_parse->value.GetDouble();
  if (std::strcmp(kTypeNames[itr_parse->value.GetType()], "String") == 0)
    object_var = std::stod(itr_parse->value.GetString());
}

void parse_json(char* in2, char* in)
{

  static const char* kTypeNames[] = {"Null", "False", "True", "Object", "Array", "String", "Number"};

  FILE* fp = fopen(in, "rb"); // non-Windows use "r"

  char readBuffer[65536];
  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

  rapidjson::Document doc;
  doc.ParseStream(is);

  if (doc.IsObject()) {

    int data_task_id = 0;
    // check all the subgraphs
    for (rapidjson::Value::ConstMemberIterator itr_sub = doc.MemberBegin(); itr_sub != doc.MemberEnd(); ++itr_sub) {
      auto sub_graph     = itr_sub->value.GetObject(); // for each subgraph  result is DALiuGEGraph
      auto componentData = sub_graph["componentData"].GetObject();

      /*
      componentData: {
      task{
        applicationParameters {} //object  empty for data drop and has values for app
        cat: sth //elemeent
        componentParameters;{ objet of the paameter
          component1:{
            keys: values s
          }
        }
      }
      }
      */
      // use vec.back() because data of task are parsed in serialized way
      // acess the first main object componentData gives and iterate into the tasks one after one
      for (rapidjson::Value::ConstMemberIterator tasks = componentData.MemberBegin();
           tasks != componentData.MemberEnd(); ++tasks) {
        // acess second level of each tasklike applicationParameters and componentParameters
        for (rapidjson::Value::ConstMemberIterator tasks_inf = tasks->value.MemberBegin();
             tasks_inf != tasks->value.MemberEnd(); ++tasks_inf) { // enter each task Info
          // acess the third lebel which contains component1
          if (tasks_inf->value.IsObject()) { // we can iterate just incase of object
            for (rapidjson::Value::ConstMemberIterator task_obj_in = tasks_inf->value.MemberBegin();
                 task_obj_in != tasks_inf->value.MemberEnd(); ++task_obj_in) { // enter each task Info

              // ypu can acess the key value in two ways 1- task_obj_in->value["key"].function
              //                                         2- tasks_inf_obj->value.GetType() which should be one by one
              //                                         until arrive to it so you need if always

              // -----------------------------for Data drops -----------------------------------------------------------

              if (strcmp(task_obj_in->value["text"].GetString(), "Data volume") == 0) { // return 0 for the full match
                // because we are in still in the same object we can parse type and values
                DataDrops.push_back(Data_Drop(tasks->name.GetString())); // name of object from the task id
                DataDrops.back().data_type = task_obj_in->value["type"].GetString();
                DataDrops.back().data_size = task_obj_in->value["value"].GetInt();
                DataDrops.back().name =
                    tasks->value["name"]
                        .GetString(); // note here we back to the level before because not a object anymore
              }

              // -----------------------------for App drops -----------------------------------------------------------
              // because sometimes is class instade of C
              else if (strcmp(task_obj_in->value["text"].GetString(), "Application Class") == 0 ||
                       strcmp(task_obj_in->value["text"].GetString(), "Application class") == 0) {
                APPDrops.push_back(APP_Drop(tasks->name.GetString())); // constructor assign id value

                APPDrops.back().name = tasks->value["name"].GetString();
              }

              if (task_obj_in->value["text"] == "Execution Time") {

                APPDrops.back().exec_time = task_obj_in->value["value"].GetDouble();

                APPDrops.back().data_type = task_obj_in->value["type"].GetString();
              }

              if (task_obj_in->value["text"] == "No. of CPUs") {
                APPDrops.back().nb_cores = task_obj_in->value["value"].GetInt();
              }

              if (task_obj_in->value["text"] == "Input error rate (%)") {
                APPDrops.back().error_rate = task_obj_in->value["value"].GetInt();
              }

              if (task_obj_in->value["text"] == "Number of tries") {
                APPDrops.back().nb_exec_try = task_obj_in->value["value"].GetInt();
              }

              if (task_obj_in->value["text"] == "Size") {
                APPDrops.back().data_size = (task_obj_in->value["value"].GetInt());
              }
            }
          }
        }
      }

      // note the input and output is wzitten in way to be good to used as mailbox
      /*
      in this way
      + APP  output mailbox
      consider App task_id=-4 send to -5 ,-6, -8 ,
      it become outport + task id
      so send to mails -5-4 , -6-4 , -8-4

      +Data input mailbox
      for the side of recive consider Data task_id = -6 , inputport is -4
      should recieve form task_id + inutport
      so -6-4
      sameway for others
      +Data output mailbox
      consider Data ask_id=-6 and output to -9
      so it send to outputport + task_id
      so to -9-6
      + App input mailbox
      consider app task_id=-9 and recive from -6
      so recive from to take_id + inputport
      so -9-6
      */
      auto linkData = sub_graph["linkData"].GetObject();
      // rapidjson::GenericObject nodeData = sub_graph["nodeData"].GetObject();

      for (rapidjson::Value::ConstMemberIterator tasks_links = linkData.MemberBegin();
           tasks_links != linkData.MemberEnd(); ++tasks_links) {
        for (Data_Drop& drop : DataDrops) {
          if (tasks_links->value["srcNode"].GetString() == drop.id) {
            drop.outports.push_back(tasks_links->value["destNode"].GetString());
            drop.out_ctrl_mailboxes.push_back(tasks_links->value["destNode"].GetString() + drop.id);
            drop.out_data_mailboxes.push_back(tasks_links->value["destNode"].GetString() + drop.id + "-data");
            drop.read_ctrl_back_mailboxes.push_back(tasks_links->value["destNode"].GetString() + drop.id + "backward");
          }
          if (tasks_links->value["destNode"].GetString() == drop.id) {
            drop.inputports.push_back(tasks_links->value["srcNode"].GetString());
            drop.in_ctrl_mailboxes.push_back(drop.id + tasks_links->value["srcNode"].GetString());
            drop.in_data_mailboxes.push_back(drop.id + tasks_links->value["srcNode"].GetString() + "-data");

            // for a drop the name of the backword massaging is the name of the in_ctrl_mailboxes which is also the
            // name of the output of the predecessor + "backward"
            drop.send_ctrl_back_mailboxes.push_back(drop.id + tasks_links->value["srcNode"].GetString() + "backward");
          }
        }

        for (APP_Drop& drop : APPDrops) {
          if (tasks_links->value["srcNode"].GetString() == drop.id) {
            drop.outports.push_back(tasks_links->value["destNode"].GetString());
            drop.out_ctrl_mailboxes.push_back(tasks_links->value["destNode"].GetString() + drop.id);
            drop.out_data_mailboxes.push_back(tasks_links->value["destNode"].GetString() + drop.id + "-data");
            drop.read_ctrl_back_mailboxes.push_back(tasks_links->value["destNode"].GetString() + drop.id + "backward");
          }

          if (tasks_links->value["destNode"].GetString() == drop.id) {
            drop.inputports.push_back(tasks_links->value["srcNode"].GetString());
            drop.in_ctrl_mailboxes.push_back(drop.id + tasks_links->value["srcNode"].GetString());
            drop.in_data_mailboxes.push_back(drop.id + tasks_links->value["srcNode"].GetString() + "-data");
            drop.send_ctrl_back_mailboxes.push_back(drop.id + tasks_links->value["srcNode"].GetString() + "backward");
          }
        }
      }
    }
  }
  /*---------------------------------------------------------------------------------------------
                     // in case the file is jason of the physic grapgh
  ---------------------------------------------------------------------------------------------*/
  int data_task_id = 0;
  bool its_datadrop;
  bool its_appdrop;
  std::string oid;
  if (doc.IsArray()) {

    for (rapidjson::SizeType i = 0; i < doc.Size(); i++) // to iterate in the object of the array
    {
      its_datadrop = false;
      its_appdrop  = false;
      // iterate in each object
      for (rapidjson::Value::ConstMemberIterator itr = doc[i].MemberBegin(); itr != doc[i].MemberEnd(); ++itr) {

        // extract the id from oid and use it as the id constructor
        if (std::strcmp(itr->name.GetString(), "oid") == 0) {
          depend_drop = itr->value.GetString();
          left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
          right_key   = depend_drop.find(depend_drop.length());
          oid         = depend_drop.substr(left_key, right_key - left_key);
          // oid         = itr->value.GetString(); // save the oid of the drop
        }

        // -----------------------------for Data drops -----------------------------------------------------------
        // the drop is Datadrop  if there is member with name type and value plain
        // creat datadrop from the oid
        if (std::strcmp(itr->name.GetString(), "type") == 0 && std::strcmp(itr->value.GetString(), "plain") == 0) {
          DataDrops.push_back(oid);
          its_datadrop = true;
        }

        // the curent drop is data so start save the data in it
        if (its_datadrop) {
          if (std::strcmp(itr->name.GetString(), "data_volume") == 0)
            extract_number(DataDrops.back().data_size, itr);

          // if (std::strcmp(itr->name.GetString(), "lg_key") == 0)
          //   DataDrops.back().id = std::to_string(itr->value.GetInt64());

          if (std::strcmp(itr->name.GetString(), "nm") == 0)
            DataDrops.back().name = itr->value.GetString();

          if (std::strcmp(itr->name.GetString(), "producers") == 0) {
            // iterate in the array of objects
            for (rapidjson::SizeType j = 0; j < itr->value.Size(); j++) {
              if (itr->value[j].IsObject()) {
                for (rapidjson::Value::ConstMemberIterator itr_j = itr->value[j].MemberBegin();
                     itr_j != itr->value[j].MemberEnd(); ++itr_j) {
                  depend_drop = itr_j->name.GetString();
                  left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                  right_key   = depend_drop.find(depend_drop.length());
                  drop_key    = depend_drop.substr(left_key, right_key - left_key);

                  DataDrops.back().outports.push_back(drop_key);
                  DataDrops.back().inputports.push_back(drop_key);
                  DataDrops.back().in_ctrl_mailboxes.push_back(DataDrops.back().id + drop_key);
                  DataDrops.back().in_data_mailboxes.push_back(DataDrops.back().id + drop_key + "-data");
                  DataDrops.back().send_ctrl_back_mailboxes.push_back(DataDrops.back().id + drop_key + "backward");
                }
              } else {
                depend_drop = itr->value[j].GetString();
                left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                right_key   = depend_drop.find(depend_drop.length());
                drop_key    = depend_drop.substr(left_key, right_key - left_key);

                DataDrops.back().outports.push_back(drop_key);
                DataDrops.back().inputports.push_back(drop_key);
                DataDrops.back().in_ctrl_mailboxes.push_back(DataDrops.back().id + drop_key);
                DataDrops.back().in_data_mailboxes.push_back(DataDrops.back().id + drop_key + "-data");
                DataDrops.back().send_ctrl_back_mailboxes.push_back(DataDrops.back().id + drop_key + "backward");
              }
            }
          }

          if (std::strcmp(itr->name.GetString(), "consumers") == 0) {

            for (rapidjson::SizeType j = 0; j < itr->value.Size(); j++) {

              if (itr->value[j].IsObject()) { // it is array of objects
                for (rapidjson::Value::ConstMemberIterator itr_j = itr->value[j].MemberBegin();
                     itr_j != itr->value[j].MemberEnd(); ++itr_j) {

                  depend_drop = itr_j->name.GetString();
                  left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                  right_key   = depend_drop.find(depend_drop.length());
                  drop_key    = depend_drop.substr(left_key, right_key - left_key);

                  DataDrops.back().out_ctrl_mailboxes.push_back(drop_key + DataDrops.back().id);
                  DataDrops.back().out_data_mailboxes.push_back(drop_key + DataDrops.back().id + "-data");
                  DataDrops.back().read_ctrl_back_mailboxes.push_back(drop_key + DataDrops.back().id + "backward");
                }
              } else { // when it is just a string member of array
                depend_drop = itr->value[j].GetString();
                left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                right_key   = depend_drop.find(depend_drop.length());
                drop_key    = depend_drop.substr(left_key, right_key - left_key);

                DataDrops.back().out_ctrl_mailboxes.push_back(drop_key + DataDrops.back().id);
                DataDrops.back().out_data_mailboxes.push_back(drop_key + DataDrops.back().id + "-data");
                DataDrops.back().read_ctrl_back_mailboxes.push_back(drop_key + DataDrops.back().id + "backward");
              }
            }
          }
        }

        // -----------------------------for App drops -----------------------------------------------------------
        if (std::strcmp(itr->name.GetString(), "type") == 0 && std::strcmp(itr->value.GetString(), "app") == 0) {
          APPDrops.push_back(oid);
          its_appdrop = true;
        }

        if (its_appdrop) {

          if (std::strcmp(itr->name.GetString(), "tw") == 0)
            extract_number(APPDrops.back().data_size, itr);

          if (std::strcmp(itr->name.GetString(), "num_cpus") == 0)
            extract_number(APPDrops.back().nb_cores, itr);

          if (std::strcmp(itr->name.GetString(), "execution_time") == 0)
            extract_number(APPDrops.back().exec_time, itr);

          if (std::strcmp(itr->name.GetString(), "input_error_threshold") == 0)
            extract_number(APPDrops.back().error_rate, itr);

          if (std::strcmp(itr->name.GetString(), "n_tries") == 0)
            APPDrops.back().nb_exec_try = itr->value.GetInt64();

          if (std::strcmp(itr->name.GetString(), "applicationArgs") == 0)
            for (rapidjson::Value::ConstMemberIterator itr_arg = itr->value.MemberBegin();
                 itr_arg != itr->value.MemberEnd(); ++itr_arg)
              if (std::strcmp(itr_arg->name.GetString(), "size") == 0)
                for (rapidjson::Value::ConstMemberIterator itr_arg_value = itr_arg->value.MemberBegin();
                     itr_arg_value != itr_arg->value.MemberEnd(); ++itr_arg_value)
                  if (std::strcmp(itr_arg_value->name.GetString(), "value") == 0)
                    extract_number(APPDrops.back().data_size, itr_arg_value);

          if (std::strcmp(itr->name.GetString(), "nm") == 0)
            APPDrops.back().name = itr->value.GetString();

          if (std::strcmp(itr->name.GetString(), "inputs") == 0) {
            // iterate in the array of objects
            for (rapidjson::SizeType j = 0; j < itr->value.Size(); j++) {
              if (itr->value[j].IsObject()) {
                for (rapidjson::Value::ConstMemberIterator itr_j = itr->value[j].MemberBegin();
                     itr_j != itr->value[j].MemberEnd(); ++itr_j) {

                  depend_drop = itr_j->name.GetString();
                  left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                  right_key   = depend_drop.find(depend_drop.length());
                  drop_key    = depend_drop.substr(left_key, right_key - left_key);
                  APPDrops.back().inputports.push_back(drop_key);
                  APPDrops.back().in_ctrl_mailboxes.push_back(APPDrops.back().id + drop_key);
                  APPDrops.back().in_data_mailboxes.push_back(APPDrops.back().id + drop_key + "-data");
                  APPDrops.back().send_ctrl_back_mailboxes.push_back(APPDrops.back().id + drop_key + "backward");
                }
              } else {
                depend_drop = itr->value[j].GetString();
                left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                right_key   = depend_drop.find(depend_drop.length());
                drop_key    = depend_drop.substr(left_key, right_key - left_key);
                APPDrops.back().inputports.push_back(drop_key);
                APPDrops.back().in_ctrl_mailboxes.push_back(APPDrops.back().id + drop_key);
                APPDrops.back().in_data_mailboxes.push_back(APPDrops.back().id + drop_key + "-data");
                APPDrops.back().send_ctrl_back_mailboxes.push_back(APPDrops.back().id + drop_key + "backward");
              }
            }
          }

          if (std::strcmp(itr->name.GetString(), "outputs") == 0) {
            for (rapidjson::SizeType j = 0; j < itr->value.Size(); j++) {
              if (itr->value[j].IsObject()) {

                for (rapidjson::Value::ConstMemberIterator itr_j = itr->value[j].MemberBegin();
                     itr_j != itr->value[j].MemberEnd(); ++itr_j) {

                  depend_drop = itr_j->name.GetString();
                  left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                  right_key   = depend_drop.find(depend_drop.length());
                  drop_key    = depend_drop.substr(left_key, right_key - left_key);

                  APPDrops.back().outports.push_back(depend_drop);
                  APPDrops.back().out_ctrl_mailboxes.push_back(drop_key + APPDrops.back().id);
                  APPDrops.back().out_data_mailboxes.push_back(drop_key + APPDrops.back().id + "-data");
                  APPDrops.back().read_ctrl_back_mailboxes.push_back(drop_key + APPDrops.back().id + "backward");
                }
              } else {
                depend_drop = itr->value[j].GetString();
                left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
                right_key   = depend_drop.find(depend_drop.length());
                drop_key    = depend_drop.substr(left_key, right_key - left_key);

                APPDrops.back().outports.push_back(depend_drop);
                APPDrops.back().out_ctrl_mailboxes.push_back(drop_key + APPDrops.back().id);
                APPDrops.back().out_data_mailboxes.push_back(drop_key + APPDrops.back().id + "-data");
                APPDrops.back().read_ctrl_back_mailboxes.push_back(drop_key + APPDrops.back().id + "backward");
              }
            }
          }
        }
      }
    }
  }

  fclose(fp);

  // find the groups for multiple nodes
  if (std::strcmp(in2, "single") != 0) {

    fp = fopen(in2, "rb"); // non-Windows use "r"
    rapidjson::FileReadStream is2(fp, readBuffer, sizeof(readBuffer));
    doc.ParseStream(is2);

    for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {

      if (itr->name == "nodeDataArray") {

        for (rapidjson::SizeType i = 0; i < itr->value.Size(); i++) { // to iterate in the object of the array

          // make it false after each object
          its_datadrop = false;
          its_appdrop  = false;

          for (rapidjson::Value::ConstMemberIterator itr_arr = itr->value[i].MemberBegin();
               itr_arr != itr->value[i].MemberEnd(); ++itr_arr) { // access the object of the array
            if (std::strcmp(itr_arr->name.GetString(), "oid") == 0) {
              depend_drop = itr_arr->value.GetString();
              left_key    = depend_drop.find("_") + 1; // +1 to dont count "-"
              right_key   = depend_drop.find(depend_drop.length());
              oid         = depend_drop.substr(left_key, right_key - left_key);
            }

            if (std::strcmp(itr_arr->name.GetString(), "category") == 0 &&
                (std::strcmp(itr_arr->value.GetString(), "Data") == 0))
              its_datadrop = true;

            if (std::strcmp(itr_arr->name.GetString(), "category") == 0 &&
                (std::strcmp(itr_arr->value.GetString(), "Component") == 0))
              its_appdrop = true;

            if (std::strcmp(itr_arr->name.GetString(), "group") == 0) {

              if (!(std::find(partitions.begin(), partitions.end(), itr_arr->value.GetInt64()) != partitions.end())) {
                partitions.push_back(itr_arr->value.GetInt64());
              }

              if (its_datadrop) {
                for (Data_Drop& drop_obj : DataDrops) {
                  if (drop_obj.id == oid) {
                    drop_obj.partition = itr_arr->value.GetInt64();
                  }
                }
              }

              if (its_appdrop) {
                for (APP_Drop& drop_obj : APPDrops) {
                  if (drop_obj.id == oid)
                    drop_obj.partition = itr_arr->value.GetInt64();
                }
              }
            }
          }
        }
      }
    }
  }
  // single node
  else {
    partitions.push_back(1);
    for (Data_Drop& drop_obj : DataDrops) {
      drop_obj.partition = 1;
    }
    for (APP_Drop& drop_obj : APPDrops) {
      drop_obj.partition = 1;
    }
  }
}