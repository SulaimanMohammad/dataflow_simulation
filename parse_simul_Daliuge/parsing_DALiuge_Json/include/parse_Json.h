#ifndef PARSE_JSON_H
#define PARSE_JSON_H
#include "rapidjson/document.h" // rapidjson's DOM-style API
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "string.h"
#include <bits/stdc++.h>
#include <cstdio>
#include <vector>

// Map of the time of execution of all the App drops

class Drop {

public:
  std::vector<std::string> drop_status_str = {"INITIALIZED", "WRITING", "STREAMACK", "DAta_App_exc ",
                                              "EXPIRED",     "DELETED", "ERROR"};
  std::vector<std::string> drop_status     = {"INITIALIZED", "WRITING", "DAta_App_exc ", "EXPIRED",
                                              "DELETED",     "ERROR",   "TOKENS_State"};
  std::string id;
  std::string lg_key;

  std::string name;
  int data_size          = 0;
  int ctrl_messages_size = 28; // in case of daluige in python it is 26 an can change from other platform
  std::string data_type;
  bool streaming = false;

  std::vector<std::string> inputports;
  std::vector<std::string> outports;

  std::vector<std::string> in_ctrl_mailboxes;
  std::vector<std::string> out_ctrl_mailboxes;

  // back control mailbox name come form the name of the input
  // where the input name is same of the output of the predsecussor
  std::vector<std::string> send_ctrl_back_mailboxes;
  std::vector<std::string> read_ctrl_back_mailboxes;

  std::vector<std::string> in_data_mailboxes;
  std::vector<std::string> out_data_mailboxes;

  // long compute_cost = data_size;
  long communication_cost;
  int node_id;

  int partition;
  std::string host_name;
  std::string status = drop_status.front(); // all the drops starts from initi

  double exec_time = 0;
  double FLOPS     = 0; // assigned from the simulation and it stays zero for datadrop
  void update_status()
  {
    auto curr_index = std::find(drop_status.begin(), drop_status.end() - 1, status); //"error not included "
    int index       = curr_index - drop_status.begin();
    if (index <=
        drop_status.size() - 2) //-2 because we will increase more one , and last element normally is at size()-1
      status = drop_status.at(++index);
    else
      status = drop_status.back();
  }

  // Set a specific state
  void set_status(std::string stat)
  { // for set error because it is not a status shoul be arrived by update

    std::for_each(stat.begin(), stat.end(), [](char& c) { c = ::toupper(c); }); // in case of small letter input
    auto curr_index = std::find(drop_status.begin(), drop_status.end(), stat);  //"error included "
    int index       = curr_index - drop_status.begin();
    status          = drop_status.at(index);
  }
};

class Data_Drop : public Drop // there is no applicationParametersand  using the category decide
{
public:
  std::string drop_type = "DATA";
  Data_Drop(std::string task_id)
  {
    this->id = task_id;
    // drop_status_str = {"INITIALIZED", "WRITING", "STREAMACK_DATA", "FINISHED", "EXPIRED", "DELETED", "ERROR"};
    drop_status = {"INITIALIZED", "WRITING", "FINISHED", "EXPIRED", "DELETED", "ERROR", "DATA_TOKENS"};
  }
};

class APP_Drop : public Drop {
  // from componentParameters
public:
  int nb_cores;
  int nb_exec_try;            // number of trying the exec
  int error_rate;             // error in percent , for ex: 0 , no error allowed, 50 half allowed to fail
  int nb_consumed_tokens = 1; // no need to provide the produced because all will be saved in data

  std::string drop_type = "APP";
  APP_Drop(std::string task_id)
  {
    this->id = task_id;
    // drop_status_str = {"INITIALIZED", "WRITING", "STREAMACK_APP", "COMPLETED", "EXPIRED", "DELETED", "ERROR"};
    drop_status = {"INITIALIZED", "RUNNING", "COMPLETED", "EXPIRED", "DELETED", "ERROR", "TOKENS_ACK", "TOKENS_DONE"};
  }
};

extern std::vector<APP_Drop> APPDrops;
extern std::vector<Data_Drop> DataDrops;
extern std::vector<int> partitions;

void parse_json(char* in, char* in2);

#endif /* PARSE_JSON.H */
