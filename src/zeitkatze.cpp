#include "zeitkatze.hpp"
#include <atomic>   // atomic<T>
#include <chrono>   // chrono::duration, chrono::duration_cast
#include <cmath>    // pow(), floor()
#include <csignal>  // signal()
#include <fcntl.h>  //fcntl()
#include <iomanip>  // setw(), setfill()
#include <iostream> // ostream()
#include <memory>   // make_unique()
#include <poll.h>   // poll()
#include <sstream>
#include <string>    // string
#include <termios.h> // struct termios, tcgetattr()
#include <unistd.h>  // read()
#include <vector>    // vector<T>

using std::setfill;
using std::setw;
using std::chrono::duration;
using std::chrono::duration_cast;

void Zeitkatze::print_time(const CatIndex cat_index, const Color color) {
  steady_clock::time_point now(steady_clock::now());
  std::stringstream sbuf;
  sbuf << Color::Cat_hold << kCats_[cat_index] << Color::Cat_hold << "   "
       << color << format_seconds(elapsed()) << Color::Normal << "  ("
       << Color::Split_lap
       << format_seconds(
              duration_cast<duration<double>>(now - last_lap_).count())
       << Color::Normal << ")";
  std::string &&line = sbuf.str();
  std::cout << "\r" << std::string(last_line_len_, ' ') << "\r" << line
            << std::flush;
  last_lap_ = now;
  split_printed_ = true;
  had_lap_ = true;
  last_line_len_ = line.size();
}

void Zeitkatze::print_current_time() {
  if (split_printed_) {
    std::cout << std::endl;
    split_printed_ = false;
  }
  std::stringstream sbuf;
  sbuf << Color::Cat << kCats_[0] << "   " << Color::Running
       << format_seconds(elapsed()) << Color::Normal;
  if (had_lap_) {
    auto current_lap =
        duration_cast<duration<double>>(steady_clock::now() - last_lap_);
    sbuf << "  (" << Color::Running_lap << format_seconds(current_lap.count())
         << Color::Normal << ") <- LAP";
  }
  std::string &&line = sbuf.str();
  std::cout << "\r" << std::string(last_line_len_, ' ') << "\r" << line
            << std::flush;
  last_line_len_ = line.size();
}

double Zeitkatze::elapsed() {
  duration<double> time_span =
      duration_cast<duration<double>>(steady_clock::now() - start_);
  return time_span.count();
}

std::string Zeitkatze::format_seconds(double seconds) {
  double full_seconds = floor(seconds);
  double fractional_seconds = seconds - full_seconds;
  double minutes = floor(full_seconds / 60.0);
  unsigned min = static_cast<unsigned>(minutes);
  unsigned sec = static_cast<unsigned>(full_seconds) % 60;
  unsigned frs =
      static_cast<unsigned>(fractional_seconds * pow(10, precision_));

  std::ostringstream oss;
  if (min > 0)
    oss << min << ":";
  oss << setfill('0') << setw(2) << sec << "." << setw(precision_) << frs;

  return oss.str();
}

CatIndex Zeitkatze::some_cat_index() {
  return static_cast<CatIndex>(elapsed() * 100) % (kCats_.size() - 2) + 1;
}

void Zeitkatze::reset_laps() {
  last_lap_ = steady_clock::now();
  had_lap_ = false;
}

void Zeitkatze::init(bool enable_color) {
  color_enabled = enable_color;
  char *color_env = getenv("ZEITKATZE_COLOR");
  if (color_env != nullptr && std::string(color_env) == "0")
    color_enabled = false;

  signal(SIGINT, interruptCallback);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  // setup the terminal
  struct termios tio;
  if (tcgetattr(1, &tio) == 0) {
    tio.c_lflag &= ~(ICANON);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    tcsetattr(1, TCSANOW, &tio);
  }
}

void Zeitkatze::run() {
  pollfd fds[] = {{STDIN_FILENO, POLLIN, 0}};
  unsigned char x = 0;
  while (running_) {
    constexpr int timeout_ms = 42;
    if (poll(fds, 1, timeout_ms) == 1) {
      if ((fds[0].revents & POLLIN) && read(0, &x, 1) == 1) {
        switch (x) {
        case '\n':
        case '\r':
          print_split_time();
          break;
        case 'r':
          reset_laps();
          break;
        case 4: // ^C
        case 'q':
          running_ = false;
        }
      }
    }
    if (elapsed() - last_interrupt_ > kExitTimeout_)
      print_current_time();

    if (interrupted) { // if we press Ctr+C^ twice with less time than
                       // kExitTimeout_ between them
      if (elapsed() - last_interrupt_ < kExitTimeout_) {
        running_ = false;
        print_newline_ = true;
      } else {
        print_split_time();
      }
      last_interrupt_ = elapsed();
      interrupted = false;
    }
  }
  if (print_newline_)
    std::cout << std::endl;
  print_end_time();
  std::cout << std::endl;
}
