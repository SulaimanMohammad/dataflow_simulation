#include "parse_Json.h"
#include "simgrid/s4u.hpp"
#include <bits/stdc++.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_actor_create, "The logging channel used in this example");
int token_size = 1;
long nb_hosts;
double one_cycle_time;
long nb_master_chunks = 0;
double time_exact     = 0;

std::vector<simgrid::s4u::Host*> hosts; // needd to be global to have it in the master
                                        // because the host names are need the engin

#define data_msg_waited_from_App "COMPLETED" // data need the termination of App
#define App_msg_waited_from_Data "FINISHED"  // App need the termination of Data
sig_atomic_t volatile terminate_stream = 0;

std::string get_id_from_mailboxName(std::string mailbox_name, std::string current_task)
{
  int left_key        = mailbox_name.find(current_task); // +1 to dont count "-"
  int right_key       = mailbox_name.length();
  std::string task_id = mailbox_name.substr(0, left_key);
  return task_id;
}
template <class T> std::vector<const char**> recive_messages(T task, std::vector<std::string> recive_from)
{
  std::vector<const char**> message_received(recive_from.size());
  std::vector<simgrid::s4u::CommPtr> pendingcomms;

  for (int i = 0; i < recive_from.size(); i++) {
    simgrid::s4u::Mailbox* rec_mailbox = simgrid::s4u::Mailbox::by_name(recive_from[i]);
    simgrid::s4u::CommPtr comm         = rec_mailbox->get_async<const char*>(&message_received[i]);
    pendingcomms.push_back(comm);
  }
  simgrid::s4u::Comm::wait_all(pendingcomms); // all messages should be arrived so actor can work

  for (int i = 0; i < message_received.size(); i++)
    XBT_INFO("task %s %s rec %s", task.name.c_str(), task.id.c_str(), *(message_received[i]));
  return message_received;
}

std::vector<std::string> messges_desired_rec(std::vector<const char**> messages_vector, const char* waited_msg)
{
  std::vector<std::string> mailboxes_sent_waited_msg;
  std::vector<std::string> msgg;

  std::string copy_string;
  // std::string rec_msg;
  std::string rec_msg;
  std::string mailbox_of_msg;
  unsigned pos_mailbox;
  unsigned msg_length;

  // for (int i = 0; i < messages_vector.size(); i++)
  //   XBT_INFO("messages_vector %s waited_msg %s", *messages_vector[i], waited_msg);

  for (int i = 0; i < messages_vector.size(); i++) {
    // XBT_INFO(" message recived   %s", *messages_vector[i]);
    copy_string = *messages_vector[i];

    pos_mailbox = copy_string.find("-"); // position where mailbox's name starts
    msg_length  = copy_string.length();

    // XBT_INFO("messages_vector[i]%s pos_mailbox%u,msg_length%u ", *copy_string, pos_mailbox, msg_length);
    if (pos_mailbox < 4294967295) { // find nothing
      rec_msg = (copy_string.substr(0, pos_mailbox));
      //*messages_vector[i] = rec_msg;
      mailbox_of_msg = (copy_string.substr(pos_mailbox, msg_length - pos_mailbox));

      // XBT_INFO("rec_msg %s mailbox_of_msg %s, ", rec_msg.c_str(), mailbox_of_msg.c_str());

      if (std::strcmp(rec_msg.c_str(), waited_msg) == 0) { // just push the name of the mailbox that sent the right msg
        // XBT_INFO("from if rec_msg %s waited_msg %s, ", rec_msg.c_str(), waited_msg);
        mailboxes_sent_waited_msg.push_back(mailbox_of_msg);
        // msgg.push_back(rec_msg);
      }
    }

    // std::cout << "messages_vector" << *messages_vector[i] << std::endl;
    // messages_vector.erase(messages_vector.begin() + i);
  }
  // for (int i = 0; i < mailboxes_sent_waited_msg.size(); i++)
  //   XBT_INFO("check the mailboxes recived%s mailboxe%s, ", msgg[i].c_str(), mailboxes_sent_waited_msg[i].c_str());

  return mailboxes_sent_waited_msg;
}

template <class T> long extract_data_recived(T task, std::vector<const char**> message_received)
{
  long data_size_read = 0;
  for (int i = 0; i < message_received.size(); i++) {
    std::string msg = *message_received[i];
    //  the msg recived in pattern ( data-size) the size is extracted after 4 positions
    // the size of data should be accumuated from all the input
    data_size_read += std::stol(msg.substr(5, sizeof(msg)));
  }
  return data_size_read;
}

template <class T> void sending_ctrl_messages(T task, std::vector<std::string> sending_to)
{

  simgrid::s4u::CommPtr comm;
  for (int i = 0; i < sending_to.size(); i++) {

    XBT_INFO("%s with id \'%s\' sends control message %s to task with id \'%s\'", task.name.c_str(), task.id.c_str(),
             task.status.c_str(), (get_id_from_mailboxName(sending_to[i], task.id)).c_str());

    simgrid::s4u::Mailbox* se_mailbox = simgrid::s4u::Mailbox::by_name(sending_to[i]);
    // the state of drops in Daluige are sent as integer numbers with size of 24 for each number
    std::string messsage_to_send = std::string(task.status.c_str()) + sending_to[i];

    // send the message and the mailbox of it
    comm = se_mailbox->put_init(new std::string(messsage_to_send), task.ctrl_messages_size)->detach();
    // no wait the actor can work after sendeing messages so it can execute
  }
}

template <class T> void sending_ctrl_messages_backward(T task, std::vector<std::string> sending_to)
{

  // const char* task_stat_waited;
  for (int i = 0; i < sending_to.size(); i++) {
    simgrid::s4u::Mailbox* se_mailbox = simgrid::s4u::Mailbox::by_name(sending_to[i]);
    // simgrid::s4u::CommPtr comm =
    // the state of drops in Daluige are sent as integer numbers with size of 24 for each number
    std::string messsage_to_send = std::string(task.status.c_str()) + sending_to[i];
    XBT_INFO("Message backward- %s to %s", messsage_to_send.c_str(), sending_to[i].c_str());
    simgrid::s4u::CommPtr comm = se_mailbox->put_init(new std::string(messsage_to_send), 0)->detach();
  }
}

template <class T> void send_data(T task, std::vector<std::string> sending_to, int nb_tokens_to_consumer = 0)
{
  simgrid::s4u::CommPtr comm;

  if (nb_tokens_to_consumer == 0)
    nb_tokens_to_consumer = task.data_size;
  // data drop when it send data to the consumer it should wait until it is read from it
  //  and that can represents consumer read from the memory of datadrop
  //  const char* task_stat_waited;
  for (int i = 0; i < sending_to.size(); i++) {
    simgrid::s4u::Mailbox* se_mailbox = simgrid::s4u::Mailbox::by_name(sending_to[i]);
    if (task.drop_type == "DATA") {
      comm = se_mailbox->put_async(new std::string("data-" + std::to_string(nb_tokens_to_consumer)),
                                   nb_tokens_to_consumer);
      comm->wait();
    } else if (task.drop_type == "APP") { // app doesnt need to wait data to arrive to memory
      comm = se_mailbox->put_init(new std::string("data-" + std::to_string(task.data_size)), task.data_size)->detach();
    }
  }
}

template <class T> void sending_data_from_appdrop(T task, std::vector<std::string> sending_to)
{ // app doesnt need to wait data to be read from the datadrop becauseit is like writing process in it
  // const char* task_stat_waited;
  for (int i = 0; i < sending_to.size(); i++) {
    simgrid::s4u::Mailbox* se_mailbox = simgrid::s4u::Mailbox::by_name(sending_to[i]);
    // this will roduce so much the time of the execution especially in big data
    simgrid::s4u::CommPtr comm =
        se_mailbox->put_init(new std::string("data-" + std::to_string(task.data_size)), task.data_size)->detach();
  }
}

template <class T> void after_terminate_actor(T task, const char* waited_msg)
{
  std::vector<const char**> ctrl_message_received = recive_messages(task, task.out_ctrl__mailboxes, waited_msg);
  sending_ctrl_messages(task, task.in_ctrl_mailboxes);
  if (task.out_ctrl__mailboxes.size() == ctrl_message_received.size()) {
    task.set_status("DELETED");
    XBT_INFO("%s", task.status.c_str());
    simgrid::s4u::this_actor::exit();
  }
}

template <class T> bool stream_condition(T task, const char* waited_msg)
{
  bool out = false;
  std::vector<std::string> message_received_back;
  //      will return th match messages
  message_received_back = messges_desired_rec(recive_messages(task, task.read_ctrl_back_mailboxes), waited_msg);
  // in case of terminal actors read_ctrl_back_mailboxes will be zero so already no message_received_back so they will
  // be both to ensure that this match is done  !message_received_back.empty() is provided
  if (task.read_ctrl_back_mailboxes.size() == message_received_back.size() && !message_received_back.empty())
    out = true;

  // the terminal drops doesnt need to have messagres back they should be always one as stream
  if (message_received_back.empty() && (task.status == "FINISHED" || task.status == "COMPLETED"))

    out = true;

  return out;
}

void signal_callback_handler(int signum)
{
  terminate_stream = 1;
  XBT_INFO("Intrupt the stream");
}

std::vector<std::string> derive_data_mailboxes(std::vector<std::string> mailboxes)
{
  std::vector<std::string> mailboxes_data;
  for (int i = 0; i < mailboxes.size(); i++)
    mailboxes_data.push_back(mailboxes[i] + "-data");

  return mailboxes_data;
}
std::vector<std::string> derive_backward_mailboxes(std::vector<std::string> mailboxes)
{
  std::vector<std::string> mailboxes_backward;
  for (int i = 0; i < mailboxes.size(); i++)
    mailboxes_backward.push_back(mailboxes[i] + "backward");
  return mailboxes_backward;
}

// here the task is updated but it sent in messages no need to change it permenently because it will be after changed
// after the function , i add the reference just incase you need to track or to use it in somehow after
template <class T> void tokens_streaming_app(T& task, std::vector<std::string> mailboxes, int& tokens_recived)
{
  std::vector<const char**> data_message_received;
  std::vector<std::string> mailboxes_data     = derive_data_mailboxes(mailboxes);
  std::vector<std::string> mailboxes_backward = derive_backward_mailboxes(mailboxes);
  // XBT_INFO("AAdd, start reciving tokens %s conusmed %d ", task.id.c_str(), task.nb_consumed_tokens);
  tokens_recived = 0;
  while (1) { // reciving all chunks
    XBT_INFO("App treaming new while iteration tokens_recived %d", tokens_recived);

    if (tokens_recived < task.nb_consumed_tokens) {
      data_message_received = recive_messages(task, mailboxes_data);
      task.data_size        = extract_data_recived(task, data_message_received);
      tokens_recived += task.data_size;
      // the tokens_recived is incereaside by the number of tokens recived,
      // for now the data drop send tokens one by one, but that can be changed

      //  it will send all the data size it has then it can quinized if necessarey
      task.set_status("TOKENS_ACK");
      sending_ctrl_messages_backward(task, mailboxes_backward);

    } else if (tokens_recived == task.nb_consumed_tokens) {
      task.set_status("TOKENS_DONE");
      sending_ctrl_messages_backward(task, mailboxes_backward);
      break;
    }
  }
  XBT_INFO("Total tokens_recived %d after dinishing all the while ", tokens_recived);
}

template <class T> void sending_messages_of_size(T task, std::vector<std::string> sending_to)
{
  simgrid::s4u::CommPtr comm;
  // data drop when it send data to the consumer it should wait until it is read from it
  //  and that can represents consumer read from the memory of datadrop
  //  const char* task_stat_waited;
  for (int i = 0; i < sending_to.size(); i++) {
    simgrid::s4u::Mailbox* se_mailbox = simgrid::s4u::Mailbox::by_name(sending_to[i]);
    comm = se_mailbox->put_init(new std::string("data-" + std::to_string(task.data_size)), 0)->detach();
  }
}

template <class T> void tokens_streaming_data(T task, int& buffer_size, int nb_tokens)
{
  bool stream_state;
  // in case of non-streaming all data will be sent as one token
  int streaming_tokens_counter = 0;
  token_size                   = 1;
  // XBT_INFO("AAdd, start DATA_TOKENS sending from %s size of data %d", task.id.c_str(), task.data_size);
  task.set_status("DATA_TOKENS");
  sending_ctrl_messages(task, task.out_ctrl_mailboxes);
  sending_messages_of_size(task, task.out_ctrl_mailboxes);
  int x = 0, y = 0;
  do {
    do {
      // XBT_INFO("AAdd, start sending from %s", task.id.c_str());

      XBT_INFO("Data, new data will be  sent streaming_tokens_counter%d", streaming_tokens_counter);
      send_data(task, task.out_data_mailboxes, token_size); // send one token each time
      y++;
      // if (streaming_tokens_counter > nb_tokens) // like the data is sent from  the buffer,  if  the token of the
      //                                           // produced tokens finished you can send from bufer
      //   buffer_size--;

      // sending_ctrl_messages_backward(task, task.send_ctrl_back_mailboxes);
      if (!task.out_ctrl_mailboxes.empty())
        streaming_tokens_counter++;

      XBT_INFO("Data,checking backward to see if new tokens should be sent");

      stream_state = stream_condition(task, "TOKENS_ACK"); // means need more token not doen yet

      // if (!stream_state) // recived token_done instade of TOKENS_ACK and mean that conusmer had all to execute so
      //                    // another round can starts
      // {
      //   if (!task.out_ctrl_mailboxes.empty())
      //     buffer_size += nb_tokens - streaming_tokens_counter;
      // }

      XBT_INFO("Data, after cheking  streaming_tokens_counter%d, nb_tokens%d buffer size%d, stream state%d ",
               streaming_tokens_counter, nb_tokens, buffer_size, stream_state);

    } while (buffer_size >= 0 && stream_state); //(y != 5); //
    XBT_INFO("new round");
    x++;
  } while (x < 2); //(streaming_tokens_counter < nb_tokens);
  XBT_INFO("   Final streaming_tokens_counter%d", streaming_tokens_counter);
  task.set_status("FINISHED");
  sending_ctrl_messages_backward(task, task.send_ctrl_back_mailboxes);
}

template <class T>
void check_do_cycle(T task, std::vector<std::string> mailboxes_send_to, const char* waited_msg, bool& cycle)
{
  if (stream_condition(task, waited_msg)) {
    sending_ctrl_messages_backward(task, mailboxes_send_to);
  } else // no messages in the backward mailbox , break no more cycles
    cycle = false;
}

template <class T> void set_flops(T& task, double speed)
{
  task.FLOPS = task.exec_time * speed;
}

void execute_send_ctrl_data(APP_Drop& task)
{
  // note pass by reference to save the status because without it after the call of the function the state is not
  // saved host_name->execute(task.FLOPS);
  simgrid::s4u::this_actor::execute(task.FLOPS);

  XBT_INFO("  Execute %s with id \'%s\' on host \'%s\'", task.name.c_str(), task.id.c_str(),
           simgrid::s4u::Host::current()->get_cname());
  task.set_status("COMPLETED");
  sending_ctrl_messages(task, task.out_ctrl_mailboxes);
  send_data(task, task.out_data_mailboxes);
}
/*-------------------- DATA Drop routine ---------------------*/
static void data_drop(Data_Drop task)
{
  XBT_INFO("Data drop on host %s", simgrid::s4u::Host::current()->get_cname());
  // number of tokens should be from the data recived see line 165, it is the data recived from the producer that will
  // be quantizized
  int nb_tokens;
  bool stream_state_round;
  std::vector<const char**> ctrl_message_received;
  std::vector<std::string> nb_complete_message_received;
  std::vector<const char**> data_message_received;

  if (task.id == "-3_0") {
    task.streaming = true;
  }

  // should not be reset after each round because it is buffer // neede to accmulate the data from many exec
  int buffer_size = 0;

  do {
    task.set_status("INITIALIZED"); // need to set for each new iteration
    ctrl_message_received = recive_messages(task, task.in_ctrl_mailboxes);
    // recive complete from producer
    nb_complete_message_received = messges_desired_rec(ctrl_message_received, data_msg_waited_from_App);
    task.update_status(); // DataDrop status is writing

    // still we need this because data drop cant do anything until the
    // producer is done executing and sending data
    if (task.in_ctrl_mailboxes.size() == nb_complete_message_received.size()) {
      // recive all data ,find the size and quantize it as tokens
      data_message_received = recive_messages(task, task.in_data_mailboxes);
      // the size of the data drop should be equal to what is recived.
      task.data_size = extract_data_recived(task, data_message_received);
      // the data drop should send same size od recived data ans the size is part of the message
      XBT_INFO("AAdd, task %s size %d", task.id.c_str(), task.data_size);
      if (task.streaming == false)
        token_size = task.data_size;
      else
        token_size = 1;

      nb_tokens = task.data_size / token_size;

      if (task.streaming) {
        tokens_streaming_data(task, buffer_size, nb_tokens);
      } else {
        task.set_status("FINISHED");
        sending_ctrl_messages(task, task.out_ctrl_mailboxes);
        send_data(task, task.out_data_mailboxes, token_size); // send one token each time
        sending_ctrl_messages_backward(task, task.send_ctrl_back_mailboxes);
      }

    } else { // expired is sent
      task.set_status("EXPIRED");
      XBT_INFO("add sending expired from task %s", task.id.c_str());
      sending_ctrl_messages(task, task.out_ctrl_mailboxes);
      break;
    }
    stream_state_round = stream_condition(task, data_msg_waited_from_App);
    XBT_INFO("task %s stream_state_round%d", task.id.c_str(), stream_state_round);
    // if recived different than  data_msg_waited_from_App then if will not be true , like sending -1
  } while (stream_state_round); // anounc that data can start sending agian

  XBT_INFO(" Exit %s%s", task.name.c_str(), task.id.c_str());
}

/*-------------------- APP Drops routine -------------------*/
static void app_drop(APP_Drop task)
{
  if (task.name == "CopyApp") {
    task.streaming = true;
  }

  // in case of non straming there will be one token to consum and all the data in one tokens from the datadrop

  // remeber that the copy will send the 100 as data , so you should recived the number of tokens sent
  int nb_tokens = task.data_size / token_size;
  std::vector<const char**> ctrl_message_received;
  std::vector<const char**> data_message_received;
  std::vector<const char**> size_data_streaming;
  std::vector<std::string> nb_expired_mailboxes;
  // std::vector<std::string> messages_mailbox;
  std::vector<std::string> stream_mailboxes;
  std::vector<std::string> finished_mailboxes;
  bool cycle = true;
  // double FLOPS;
  int tries;
  bool stre = 1;

  set_flops(task, simgrid::s4u::this_actor::get_host()->get_speed());

  bool stream_state;
  int tokens_recived          = 0;
  bool complet_msg_is_recived = false;
  bool complet_msg_stream     = false;
  int total_tokens            = 0;
  task.set_status("INITIALIZED");
  ctrl_message_received = recive_messages(task, task.in_ctrl_mailboxes);
  finished_mailboxes    = messges_desired_rec(ctrl_message_received, App_msg_waited_from_Data);
  stream_mailboxes      = messges_desired_rec(ctrl_message_received, "DATA_TOKENS");
  nb_expired_mailboxes  = messges_desired_rec(ctrl_message_received, "EXPIRED");

  if (task.streaming == true)
    task.nb_consumed_tokens = 5;
  // one execution is requiered not possible without , where no reading back is done ,after you can read back

  if (task.in_ctrl_mailboxes.size() >= finished_mailboxes.size() && !finished_mailboxes.empty()) {

    // task has two ports one normal and one streaming
    if (task.streaming) {
      complet_msg_is_recived = true;
      data_message_received  = recive_messages(task, derive_data_mailboxes(finished_mailboxes));
      task.data_size         = extract_data_recived(task, data_message_received);

    } else { // all the ports are normal
      data_message_received = recive_messages(task, task.in_data_mailboxes);
      task.data_size        = extract_data_recived(task, data_message_received);
      execute_send_ctrl_data(task);
      sending_ctrl_messages_backward(task, derive_backward_mailboxes(finished_mailboxes));
    }

    if (complet_msg_stream && complet_msg_is_recived /*if the app has both stream input and normal*/
        ||
        (complet_msg_stream && finished_mailboxes.size() == 0) /*only strame and no normal and strame is all recived */
        ||
        (complet_msg_is_recived && stream_mailboxes.size() == 0)) { /*only normal, no stream and normal is all done */
      execute_send_ctrl_data(task);
      sending_ctrl_messages_backward(task, task.send_ctrl_back_mailboxes);
    }
  }

  if (task.in_ctrl_mailboxes.size() >= stream_mailboxes.size() && !stream_mailboxes.empty()) {

    std::vector<const char**> recive_size_f_data = recive_messages(task, task.in_ctrl_mailboxes);
    long size_data_streaming                     = extract_data_recived(task, recive_size_f_data);

    // XBT_INFO("AAdd, stream ready to execute");
    while (total_tokens < size_data_streaming) {
      task.set_status("RUNNING");
      tokens_streaming_app(task, stream_mailboxes, tokens_recived);
      total_tokens += tokens_recived;
      complet_msg_stream = true;
      task.data_size     = tokens_recived;

      if (complet_msg_stream && complet_msg_is_recived /*if the app has both stream input and normal*/
          || (complet_msg_stream &&
              finished_mailboxes.size() == 0) /*only strame and no normal and strame is all recived */
          ||
          (complet_msg_is_recived && stream_mailboxes.size() == 0)) { /*only normal, no stream and normal is all done */
        execute_send_ctrl_data(task);
      }
      complet_msg_stream = false;
    }
    task.set_status("COMPLETED");
    sending_ctrl_messages_backward(task, task.send_ctrl_back_mailboxes);
  }

  // need to add expired case
  while (cycle) {
    XBT_INFO("entering the cycle");

    // shudl be reset after each cycle
    complet_msg_is_recived = false;
    complet_msg_stream     = false;

    tokens_recived = 0; // it should rest after each round is finished
    task.set_status("INITIALIZED");

    ctrl_message_received = recive_messages(task, task.in_ctrl_mailboxes);
    finished_mailboxes    = messges_desired_rec(ctrl_message_received, App_msg_waited_from_Data);
    stream_mailboxes      = messges_desired_rec(ctrl_message_received, "DATA_TOKENS");
    nb_expired_mailboxes  = messges_desired_rec(ctrl_message_received, "EXPIRED");

    for (int i = 0; i < ctrl_message_received.size(); i++)
      XBT_INFO(" recived from cycle task %s %s rec %s", task.name.c_str(), task.id.c_str(),
               *(ctrl_message_received[i]));

    task.update_status(); // running state

    if (task.in_ctrl_mailboxes.size() >= finished_mailboxes.size() && !finished_mailboxes.empty()) {

      XBT_INFO("AAdd, task entered the finish");
      // task has two ports one normal and one streaming
      if (task.streaming) {
        complet_msg_is_recived = true;
        data_message_received  = recive_messages(task, derive_data_mailboxes(finished_mailboxes));
        task.data_size         = extract_data_recived(task, data_message_received);

      } else { // all the ports are normal
        data_message_received = recive_messages(task, task.in_data_mailboxes);
        task.data_size        = extract_data_recived(task, data_message_received);
        execute_send_ctrl_data(task);
        sending_ctrl_messages_backward(task, derive_backward_mailboxes(finished_mailboxes));
      }
    }
    // STREAMIN PRODUCER CONUMER
    if (task.in_ctrl_mailboxes.size() >= stream_mailboxes.size() && !stream_mailboxes.empty()) {

      std::vector<const char**> recive_size_f_data = recive_messages(task, task.in_ctrl_mailboxes);
      long size_data_streaming                     = extract_data_recived(task, recive_size_f_data);

      // XBT_INFO("AAdd, stream ready to execute");
      while (total_tokens < size_data_streaming) {
        tokens_streaming_app(task, stream_mailboxes, tokens_recived);
        total_tokens += tokens_recived;
        complet_msg_stream = true;
        task.data_size     = tokens_recived;
        XBT_INFO("AAdd, stream ready to execute");

        if (complet_msg_stream && complet_msg_is_recived /*if the app has both stream input and normal*/
            || (complet_msg_stream &&
                finished_mailboxes.size() == 0) /*only strame and no normal and strame is all recived */
            || (complet_msg_is_recived &&
                stream_mailboxes.size() == 0)) { /*only normal, no stream and normal is all done */
          execute_send_ctrl_data(task);
          sending_ctrl_messages_backward(task, task.send_ctrl_back_mailboxes);
        }
        XBT_INFO("AAdd, done execution , rexivedtokens %d", total_tokens);
        complet_msg_stream = false;
      }
      XBT_INFO(" ADD leaving the task scream  %s", task.name.c_str());
    }

    if (complet_msg_stream && complet_msg_is_recived) {
      execute_send_ctrl_data(task);
      check_do_cycle(task, task.send_ctrl_back_mailboxes, App_msg_waited_from_Data, cycle);
    }

    if (task.in_ctrl_mailboxes.size() >= nb_expired_mailboxes.size() && !nb_expired_mailboxes.empty()) {
      XBT_INFO(" ADD reciving expired  %s", task.name.c_str());

      task.set_status("EXPIRED");
      sending_ctrl_messages(task, task.out_ctrl_mailboxes);
      break;
    }
  }
  XBT_INFO(" Exit %s%s", task.name.c_str(), task.id.c_str());
}

/*-----------------Master node that provid Data----------------*/
// master is already stream nature for app based on the number of chuncks
template <class T> static void master(T task) // master can be Appdrop or data drop
{
  const char* msg_waited;
  // task.read_ctrl_back_mailboxes = task.send_ctrl_back_mailboxes;
  int buffer_size = 0;
  int nb_tokens;
  task.data_size = 10;
  double time_ex = 0;
  if (nb_master_chunks == 0) // if input is time then this var will be calculated, otherwise it is input
    nb_master_chunks = long(time_exact / one_cycle_time);

  set_flops(task, simgrid::s4u::this_actor::get_host()->get_speed());

  // master case should execute without any expected data
  if (task.drop_type == "APP") {
    msg_waited = App_msg_waited_from_Data;
    XBT_INFO("------------- Chunck from master-------------");
    simgrid::s4u::this_actor::execute(task.FLOPS);
    XBT_INFO(" Master execute %s with id \'%s\' on host \'%s\'", task.name.c_str(), task.id.c_str(),
             simgrid::s4u::Host::current()->get_cname());
    task.set_status("COMPLETED");
    sending_ctrl_messages(task, task.out_ctrl_mailboxes);
    send_data(task, task.out_data_mailboxes);
    signal(SIGINT, signal_callback_handler);
    long chunks = 1; // one already done

    while ((chunks < nb_master_chunks)) {
      chunks++;
      simgrid::s4u::this_actor::execute(task.FLOPS);
      bool stream_state = stream_condition(task, msg_waited);

      if (stream_state) {
        XBT_INFO("-------------New Chunck from master-------------");
        sending_ctrl_messages(task, task.out_ctrl_mailboxes);
        send_data(task, task.out_data_mailboxes);
        if (terminate_stream)
          break;
      }
    }
  }
  if (task.drop_type == "DATA") {

    msg_waited = data_msg_waited_from_App;

    if (task.streaming == false)
      token_size = task.data_size;
    else
      token_size = 1;

    nb_tokens = task.data_size / token_size;

    XBT_INFO("------------- Chunck from master-------------");
    if (task.streaming) {
      tokens_streaming_data(task, buffer_size, nb_tokens);
    } else {
      task.set_status("FINISHED");
      sending_ctrl_messages(task, task.out_ctrl_mailboxes);
      send_data(task, task.out_data_mailboxes, token_size); // send one token each time
    }
    long chunks = 1;
    while ((chunks < nb_master_chunks)) {
      chunks++;
      bool stream_state = stream_condition(task, msg_waited);
      if (stream_state) {
        if (task.streaming) {
          XBT_INFO("-------------New Chunck from master-------------");
          tokens_streaming_data(task, buffer_size, nb_tokens);
        } else {
          XBT_INFO("-------------New Chunck from master-------------");
          task.set_status("FINISHED");
          sending_ctrl_messages(task, task.out_ctrl_mailboxes);
          send_data(task, task.out_data_mailboxes, token_size); // send one token each time
        }
        if (terminate_stream)
          break;
      }
    }
  }

  // no difference between both APP or data for this
  if (stream_condition(task, msg_waited)) {
    XBT_INFO("-------------Master Finished-------------");
    task.set_status("EXPIRED");
    sending_ctrl_messages(task, task.out_ctrl_mailboxes);
  }

  XBT_INFO("Exit master %s%s", task.name.c_str(), task.id.c_str());
}

template <class T> static void master_test_time_cycle(T task)
{ // exec time =0 always for data drop
  task.FLOPS = task.exec_time * simgrid::s4u::this_actor::get_host()->get_speed();

  double start = simgrid::s4u::Engine::get_clock();
  // master case should execute without any expected data
  simgrid::s4u::this_actor::execute(task.FLOPS);
  task.set_status("COMPLETED");
  sending_ctrl_messages(task, task.out_ctrl_mailboxes);
  sending_data_from_appdrop(task, task.out_data_mailboxes);

  if (stream_condition(task, App_msg_waited_from_Data)) {
    task.set_status("EXPIRED");
    sending_ctrl_messages(task, task.out_ctrl_mailboxes);
    // no data is sent here because it is just to stop the actors
  }
  one_cycle_time = simgrid::s4u::Engine::get_clock() - start;
}

int main(int argc, char* argv[])
{
  simgrid::s4u::Engine e(&argc, argv);

  e.load_platform(argv[1]);

  parse_json(argv[2], argv[3]); // parse the graph and store task in vectors of objects
  if (argv[4] != nullptr && std::strcmp(argv[4], "time") == 0) {

    if (argv[5] != nullptr)
      time_exact = std::stod(argv[5]);

    // run one time to see how long it takes each one cycle
    for (APP_Drop& drop : APPDrops) {
      if (drop.in_ctrl_mailboxes.empty()) {
        simgrid::s4u::Actor::create(drop.name, e.host_by_name("Tremblay"), &master_test_time_cycle<APP_Drop>, drop);
      } else {
        simgrid::s4u::Actor::create(drop.name, e.host_by_name("Tremblay"), &app_drop, drop);
      }
    }
    for (Data_Drop& drop : DataDrops) {
      if (drop.in_ctrl_mailboxes.empty()) {
        simgrid::s4u::Actor::create(drop.name, e.host_by_name("Tremblay"), &master_test_time_cycle<Data_Drop>, drop);
      } else {
        simgrid::s4u::Actor::create(drop.name, e.host_by_name("Tremblay"), &data_drop, drop);
      }
    }

    e.run();
    XBT_INFO("Simulation is over");

  } else if (argv[4] != nullptr && std::strcmp(argv[4], "chunks") == 0) {
    nb_master_chunks = std::stol(argv[5]);
  }

  hosts    = e.get_all_hosts();
  nb_hosts = e.get_host_count();

  std::vector<std::tuple<int, simgrid::s4u::Host*>> partition_host;
  for (int i = 0; i < partitions.size(); i++)
    partition_host.push_back(std::tuple<int, simgrid::s4u::Host*>(partitions[i], hosts[i]));

  simgrid::s4u::Host* drop_host;
  // assign_drops_to_actors(e);
  for (APP_Drop& drop : APPDrops) {
    for (auto& key : partition_host) {
      if (std::get<0>(key) == drop.partition)
        drop_host = std::get<1>(key);
    }

    if (drop.in_ctrl_mailboxes.empty()) {
      simgrid::s4u::Actor::create(drop.name, drop_host, &master<APP_Drop>, drop);
    } else {
      simgrid::s4u::Actor::create(drop.name, drop_host, &app_drop, drop);
    }
  }
  for (Data_Drop& drop : DataDrops) {

    for (auto& key : partition_host) {
      if (std::get<0>(key) == drop.partition)
        drop_host = std::get<1>(key);
    }

    if (drop.in_ctrl_mailboxes.empty()) {
      simgrid::s4u::Actor::create(drop.name, drop_host, &master<Data_Drop>, drop);
    } else {
      simgrid::s4u::Actor::create(drop.name, drop_host, &data_drop, drop);
    }
  }
  e.run();
  XBT_INFO("Simulation is over");

  return 0;
}