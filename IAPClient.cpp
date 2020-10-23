#include <cstring>
#include <zconf.h>
#include <csignal>
#include <utility>
#include <iostream>
#include <map>
#include "IAPClient.h"
#include "IAPProtocol.h"
#include "response_message/ChannelWaveformResponse.h"
#include "response_message/ChannelWaveformDataResponse.h"

using namespace std;
using namespace IAP;

IAPClient::IAPClient(Socket *socket) {
  socket_ = socket;
  if (socket_->open()) {
    fprintf(stderr, "Socket open failed.\n");
    exit(-1);
  }
}

IAPClient::~IAPClient() {
  socket_->close();
  delete socket_;
}

int IAPClient::command(string message) {
  return socket_->write(move(message));
}

static void unix_error(const string &msg) {
  fprintf(stderr, "%s: %s\n", msg.c_str(), strerror(errno));
  exit(0);
}

static pid_t Fork() {
  pid_t pid;
  if ((pid = fork()) < 0)
    unix_error("Fork error");
  return pid;
}

static int msleep(long msec) {
  struct timespec ts{};
  int res;

  if (msec < 0) {
    errno = EINVAL;
    return -1;
  }

  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;

  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);

  return res;
}

void IAPClient::startKeepAlive() {
  pid_t pid = Fork();
  if (pid) { // Parent
    keep_alive_pid_ = pid;
  } else { // Child
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
#ifdef SIGBREAK
    signal(SIGBREAK, SIG_DFL);
#endif
    printf("keepAlive pid: %d\n", getpid());
    while (true) {
      command(CommandBuilder::keepAlive());
      sleep(2);
    }
  }
}

int IAPClient::killKeepAlive(int sig) const {
  printf("kill (%d) keep_alive pid: %d\n", sig, keep_alive_pid_);
  return kill(keep_alive_pid_, sig);
}

void IAPClient::startRead() {
  pid_t pid = Fork();
  map<uint8_t, pair<uint32_t, uint16_t>> stream_handle_to_id;
  map<uint8_t, uint8_t> stream_handle_to_ordinal;
  if (pid) { // Parent
    read_pid_ = pid;
  } else { // Child
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
#ifdef SIGBREAK
    signal(SIGBREAK, SIG_DFL);
#endif
    printf("startRead pid: %d\n", getpid());
    MessageParser parser{};
    while (true) {
      msleep(100);
      socket_->read(5000);
      unsigned char b = 0;
      while (socket_->getBufferSize() > 0) {
        socket_->popByte(&b);
        parser.addByte(b);
        if (parser.isValid()) {
          if (auto response = parser.build()) {
            if (typeid(*response) == typeid(ChannelWaveformResponse)) {
              auto waveform_response = dynamic_cast<ChannelWaveformResponse*>(response);
              auto handle = waveform_response->get_handle();
              auto id = waveform_response->get_id();
              stream_handle_to_id[handle] = id;
              stream_handle_to_ordinal[handle] = 250;
              response->record();
            }
            else if (typeid(*response) == typeid(ChannelWaveformDataResponse)) {
              auto data_response = dynamic_cast<ChannelWaveformDataResponse*>(response);
              auto handle = data_response->get_handle();
              auto id_pair = stream_handle_to_id[handle];
              auto previous_ordinal = stream_handle_to_ordinal[handle];
              data_response->record(id_pair.first, id_pair.second, previous_ordinal);
              stream_handle_to_ordinal[handle] = data_response->get_ordinal_number();
            }
            else {
              response->record();
            }
            delete response;
          }
        }
      }
    }
  }
}

int IAPClient::killRead(int sig) const {
  printf("kill (%d) read pid: %d\n", sig, read_pid_);
  return kill(read_pid_, sig);
}
