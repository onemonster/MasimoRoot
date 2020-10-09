#include "SerialPort.h"

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

using namespace std;

SerialPort::SerialPort(
    const std::string &path,
    speed_t baud_rate,
    uint8_t data_bits,
    uint8_t stop_bit,
    uint8_t parity
) {
  path_ = path;
  baud_rate_ = baud_rate;
  data_bits_ = data_bits;
  stop_bit_ = stop_bit;
  parity_ = parity;
}

int SerialPort::open() {
  fd_ = ::open(path_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd_ < 0) {
    perror("SerialPort file open failed");
    return errno;
  }

  termios tty{};
  if (tcgetattr(fd_, &tty)) {
    perror("tcgetattr");
    return errno;
  }

  if (cfsetspeed(&tty, baud_rate_)) {
    perror("cfsetspeed");
    return errno;
  }
  cfmakeraw(&tty);

  // payload_ bits
  tty.c_cflag &= ~CSIZE; // char size
  if (data_bits_ == 8) tty.c_cflag |= CS8;
  else if (data_bits_ == 7) tty.c_cflag |= CS7;
  else {
    printf("data_bits should be either 7 or 8\n");
    return -1;
  }

  // stop bit
  tty.c_cflag &= ~CSTOPB; // 1 stop bit (CSTOPB 가 1이면 2비트 정지비트이므로 clear 한다)
  tty.c_cflag |= CLOCAL | CREAD | HUPCL; // Local line - do not change "owner" of port

  // parity
  tty.c_cflag &= ~(PARODD | PARENB | CMSPAR); // no parity
  if (parity_ == 2) {
    tty.c_cflag |= PARENB; // even parity
  } else if (parity_ == 1) {
    tty.c_cflag |= PARODD | PARENB; // odd parity
  }

  // Raw output is selected by resetting the OPOST option in the c_oflag member:
  // tty.c_oflag &= ~(OPOST | ONLCR | OCRNL) ; // No Output Processing
  tty.c_oflag = 0;

  // if software flow control
  // tty.c_iflag |= (IXON | IXOFF | IXANY);
  // You may set the control characters using the c_cc part.
  // Set the software flow control characters in the VSTARTand VSTOP
  // input flags
  // tty.c_iflag &= ~(INLCR | IGNCR | ICRNL | IGNBRK | INPCK | ISTRIP);            // 패리티 오류 무시
  tty.c_iflag = 0;

  // software flow control
  // tty.c_cflag &= ~CRTSCTS
  // tty.c_iflag |= (IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software XON/XOFF flow control both i/p and o/p

  // local mode flags
  // tty.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHONL | ECHOPRT | IEXTEN | IUCLC | PARMRK);  // Non Cannonical mode
  tty.c_lflag = 0;

  // set “read timeout between characters” as 100 ms.
  // read returns either when the specified number of chars
  // are received or the timout occurs
  // tty.c_cc[VMIN] = 64; // 64 글자를 받거나
  // tty.c_cc[VTIME] = 1; // 100ms 가 되면 read 함수가 리턴
  tty.c_cc[VMIN] = 0; // 항상 즉각 리턴
  tty.c_cc[VTIME] = 1; // 100ms 가 되면 즉각 리턴

  // hardware flow control
  if (false) {
    printf("hardware handshake");
    tty.c_cflag |= CRTSCTS;
  } else {
    tty.c_cflag &= ~CRTSCTS;
  }

  /*
  printf("cflag = %o\n"
         "iflag = %o\n"
         "oflag = %o\n"
         "lflag = %o\n"
         "vmin = %d\n"
         "vtime = %d\n",
         tty.c_cflag, tty.c_iflag, tty.c_oflag, tty.c_lflag, tty.c_cc[VMIN], tty.c_cc[VTIME]);
  */

  if (tcsetattr(fd_, TCSANOW, &tty)) { // 즉시 변경
    perror("terminal: set attr failed");
    return errno;
  }

//   Set the RTS pin
  if (ioctl(fd_, TIOCMBIS, TIOCM_RTS)) {
    perror("ioctl");
  }

  if (tcflush(fd_, TCIFLUSH)) {
    perror("tcflush");
    return errno;
  }

//fcntl(fd, F_SETFL, O_NONBLOCK);
  return 0;
}

int SerialPort::close() {
  if (fd_) {
    tcflush(fd_, TCOFLUSH);
    ::close(fd_);
    fd_ = 0;
  }

  clearBuffer();
  return 0;
}

int SerialPort::read(size_t max_len) {
  char buffer[max_len + 1];
  auto nread = ::read(fd_, buffer, max_len);

  if (nread > 0) {
    lock_guard lock(lock_);
    buffer_.insert(buffer_.end(), buffer, buffer + nread);
  }

  return nread;
}

int SerialPort::popByte(uint8_t *b) {
  lock_guard lock(lock_);
  if (buffer_.empty()) return -1;
  *b = buffer_.front();
  buffer_.pop_front();
  return 0;
}

int SerialPort::write(std::string s) {
  if (s.empty()) return 0;
  if (!fd_) return -1;
  return ::write(fd_, s.c_str(), s.size());
}

size_t SerialPort::getBufferSize() {
  return buffer_.size();
}

int SerialPort::clearBuffer() {
  lock_guard lock(lock_);
  buffer_.clear();
  return 0;
}
