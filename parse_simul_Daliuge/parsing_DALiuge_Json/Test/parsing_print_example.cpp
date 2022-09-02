#include "parse_Json.h"
#include <iostream>

int main(int argc, char** argv)
{

  parse_json(argv[1], argv[2]);

  printf("--------------- DATA DROPS ---------------\n");

  printf("Task_id |    Task_name    | Data_size | Data_type | Input_mailbox | Output_mailbox\n");

  for (Data_Drop& drop_obj : DataDrops) {
    printf("%-15s %-15s %-10d %-9s", drop_obj.id.c_str(), drop_obj.name.c_str(), drop_obj.data_size,
           drop_obj.data_type.c_str());

    for (auto i : drop_obj.in_ctrl_mailboxes)
      printf("(%s)", i.c_str());

    printf("%-9s", "");

    for (auto i : drop_obj.out_ctrl_mailboxes)
      printf("(%s)", i.c_str());

    printf("\n");
  }

  printf("--------------- APP DROPS ---------------\n");
  printf("Task_id |    Task_name    | Data_size | Data_type |  Exec_time | Exec_try_nb |Number_cors| Input_mailbox | "
         "Output_mailbox\n");

  for (APP_Drop& drop_obj : APPDrops) {
    printf("%-12s %-15s %-14d %-12s %-11d %-10d %-10d ", drop_obj.id.c_str(), drop_obj.name.c_str(), drop_obj.data_size,
           drop_obj.data_type.c_str(), drop_obj.nb_exec_try, drop_obj.exec_time, drop_obj.nb_cores);

    for (auto i : drop_obj.in_ctrl_mailboxes)
      printf("(%s)", i.c_str());

    printf("%-16s", "");

    for (auto i : drop_obj.out_ctrl_mailboxes)
      printf("(%s)", i.c_str());

    printf("\n");
  }
}
