#include "simgrid/s4u.hpp"
#include <bits/stdc++.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define stat_Data_drop_msg "completed";
#define stat_App_drop_msg "finished";

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_actor_create, "The logging channel used in this example");

long nb_hosts;
std::vector<simgrid::s4u::Host*> hosts; // needd to be global to have it in the master
                                        // because the host name need the engin

//**********************************************************
//-----------------------Functions--------------------------
//**********************************************************
inline std::vector<std::string> get_mails(std::vector<std::string> args, std::string mailbox_type)
{
  std::vector<std::string> mail_names;
  const char* namebox_s;
  std::string key_s =
      mailbox_type; // mailbox_type;  //
                    // https://stackoverflow.com/questions/22440067/concatenating-integers-to-const-char-strings
  // XBT_INFO("key mail is: %s", ("mail" + std::to_string(keyid)).c_str());
  namebox_s = simgrid::s4u::Actor::self()->get_property(key_s.c_str());
  if (namebox_s != nullptr) {

    std::string nameMailbox(namebox_s);
    std::stringstream nameMailbox_s(nameMailbox);

    while (nameMailbox_s.good()) {
      std::string substr;
      getline(nameMailbox_s, substr, ',');
      mail_names.push_back(std::string(substr));
    }
    // XBT_INFO("app_drop sender mail number %d is: %s",keyid_s, name_box_s.c_str()  );
  }

  return mail_names;
}

static void data_drop(std::vector<std::string> args)
{
  const char* statue = "INITIALIZED";
  double compute_cost;
  long communication_cost;

  if (args.size() > 1) { // master is app_drop contains so many data control
    compute_cost       = std::stod(args[1]);
    communication_cost = std::stol(args[2]);
  }

  //---------------------------------------------------------------
  // ---------------------recive messages -------------------------
  //---------------------------------------------------------------

  std::vector<const char*> message_received;
  std::vector<std::string> receive_mails = get_mails(args, "rec_mail");

  // std::cout<< "num" <<receive_mails.size()<<std::endl;

  for (int i = 0; i < receive_mails.size(); i++) {
    simgrid::s4u::Mailbox* re_mailbox = simgrid::s4u::Mailbox::by_name(receive_mails[i]);
    const char** msg;
    simgrid::s4u::CommPtr comm = re_mailbox->get_async<const char*>(&msg);
    XBT_INFO("Data__drop recive mail is: %s", receive_mails[i].c_str());
    comm->wait();
    message_received.push_back(*msg);
  }

  // app_drop recived end of data drop so it can compute his data
  for (int i = 0; i < message_received.size(); i++) {
    if (strcmp(message_received[i], "C-A") == 0 && i == message_received.size() - 1) {
      simgrid::s4u::this_actor::execute(compute_cost);
    }
  }

  // the sending message represent the end of the data drop
  //-----------------------------------------------------------
  //  ---------------------send messages------------------------
  //-----------------------------------------------------------

  std::vector<std::string> send_mails = get_mails(args, "send_mail");

  for (int i = 0; i < send_mails.size(); i++) {
    simgrid::s4u::Mailbox* se_mailbox = simgrid::s4u::Mailbox::by_name(send_mails[i]);
    XBT_INFO("Data__drop Send mail is: %s", send_mails[i].c_str());
    simgrid::s4u::CommPtr comm = se_mailbox->put_async(new const char*("C-D"), 10); // send D as complete of data drop
    comm->wait();
  }
  XBT_INFO("Exiting now data_drop.");
}

static void app_drop(std::vector<std::string> args)
{
  const char* statue = "INITIALIZED";
  double compute_cost;
  long communication_cost;

  if (args.size() > 1) { // master is app_drop contains so many data control
    compute_cost       = std::stod(args[1]);
    communication_cost = std::stol(args[2]);
  }

  // reading from multiple data drops based on the number of dependancy
  // and in ths way all the prescussor send to on mail and read it many times based on the number of dep
  //---------------------------------------------------------------
  // ---------------------recive messages -------------------------
  //---------------------------------------------------------------

  if (get_mails(args, "rec_mail").empty()) { // master case sould execute without any expected data
    simgrid::s4u::this_actor::execute(compute_cost);
    statue = "Done";
  } else {
    std::vector<const char*> message_received;
    std::vector<std::string> receive_mails = get_mails(args, "rec_mail");

    // std::cout<< "num" <<receive_mails.size()<<std::endl;

    for (int i = 0; i < receive_mails.size(); i++) {
      simgrid::s4u::Mailbox* re_mailbox = simgrid::s4u::Mailbox::by_name(receive_mails[i]);
      const char** msg;
      simgrid::s4u::CommPtr comm = re_mailbox->get_async<const char*>(&msg);
      XBT_INFO("APP__drop recive mail is: %s", receive_mails[i].c_str());
      comm->wait();
      message_received.push_back(*msg);
    }

    // app_drop recived end of data drop so it can compute his data

    for (int i = 0; i < message_received.size(); i++) {
      if (strcmp(message_received[i], "C-D") == 0 && i == message_received.size() - 1) {
        simgrid::s4u::this_actor::execute(compute_cost);
        statue = "Done";
      }
    }
  }
  //-----------------------------------------------------------
  // ---------------------send messages------------------------
  //-----------------------------------------------------------
  // multi sending from the app to data drops
  std::vector<std::string> send_mails = get_mails(args, "send_mail");
  // when the app drop finish execution then send ACK to Data Drop to start
  for (int i = 0; i < send_mails.size(); i++) {
    simgrid::s4u::Mailbox* se_mailbox = simgrid::s4u::Mailbox::by_name(send_mails[i]);
    XBT_INFO("APP__drop Send mail is: %s", send_mails[i].c_str());
    if (strcmp(statue, "Done") == 0) {
      simgrid::s4u::CommPtr comm = se_mailbox->put_async(new const char*("C-A"), 10);
      comm->wait();
    }
  }

  XBT_INFO("Exiting now app_drop .");
}

int main(int argc, char* argv[])
{
  simgrid::s4u::Engine e(&argc, argv);
  xbt_assert(argc > 2, "Usage: %s platform_file deployment_file\n", argv[0]);

  /* Register the functions representing the actors */
  e.register_function("data_drop", &data_drop);
  e.register_function("app_drop", &app_drop);

  /* Load the platform description and then deploy the application */
  e.load_platform(argv[1]);
  e.load_deployment(argv[2]);

  hosts    = e.get_all_hosts();  // global
  nb_hosts = e.get_host_count(); // global var number of hosts which will be number of actors

  /* Run the simulation */
  e.run();

  XBT_INFO("Simulation is over");

  return 0;
}
