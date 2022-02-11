#include "zeitkatze.hpp"
#include <atomic>     // atomic<T>
#include <chrono>     // chrono::duration, chrono::duration_cast
#include <cmath>      // pow(), floor()
#include <csignal>    // signal()
#include <fcntl.h>    //fcntl()
#include <filesystem> // std::filesystem::absolute, C++17+
#include <fstream>    // ifstream
#include <iomanip>    // setw(), setfill()
#include <iostream>   // ostream()
#include <memory>     // make_unique()
#include <poll.h>     // poll()
#include <sstream>
#include <string>    // string
#include <termios.h> // struct termios, tcgetattr()
#include <unistd.h>  // read()
#include <vector>    // vector<T>

using std::setfill;
using std::setw;
using std::chrono::duration;
using std::chrono::duration_cast;

void Zeitkatze::PrintTime(const CatIndex cat_index, const Color color) {
  steady_clock::time_point now(steady_clock::now());
  std::stringstream sbuf;
  sbuf << Color::Cat_hold << cat_emotes_[cat_index] << Color::Cat_hold << "   "
       << color << FormatSeconds(Elapsed()) << Color::Normal << "  ("
       << Color::Split_lap
       << FormatSeconds(
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

void Zeitkatze::PrintCurrentTime() {
  if (!one_line_) {
    if (split_printed_) {
      std::cout << std::endl;
      split_printed_ = false;
    }
  }
  std::stringstream sbuf;
  sbuf << Color::Cat << cat_emotes_[0] << "   " << Color::Running
       << FormatSeconds(Elapsed()) << Color::Normal;
  if (had_lap_) {
    auto current_lap =
        duration_cast<duration<double>>(steady_clock::now() - last_lap_);
    sbuf << "  (" << Color::Running_lap << FormatSeconds(current_lap.count())
         << Color::Normal << ") <- LAP";
  }
  std::string &&line = sbuf.str();
  std::cout << "\r" << std::string(last_line_len_, ' ') << "\r" << line
            << std::flush;
  last_line_len_ = line.size();
}

double Zeitkatze::Elapsed() {
  duration<double> time_span =
      duration_cast<duration<double>>(steady_clock::now() - start_);
  return time_span.count();
}

std::string Zeitkatze::FormatSeconds(double seconds) {
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

CatIndex Zeitkatze::SomeCatIndex() {
  return static_cast<CatIndex>(Elapsed() * 100) % (cat_emotes_.size() - 2) + 1;
}

void Zeitkatze::ResetLaps() {
  last_lap_ = steady_clock::now();
  had_lap_ = false;
}

void Zeitkatze::Init(bool enable_color) {
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

void Zeitkatze::Run() {
  pollfd fds[] = {{STDIN_FILENO, POLLIN, 0}};
  unsigned char x = 0;
  auto clearScreen = []() { std::cout << "\x1b[H\x1b[J" << std::flush; };
  while (running_) {
    constexpr int timeout_ms = 42;
    if (poll(fds, 1, timeout_ms) == 1) {
      if ((fds[0].revents & POLLIN) && read(0, &x, 1) == 1) {
        switch (x) {
        case '\n':
        case '\r':
          if (one_line_)
            clearScreen();
          PrintSplitTime();
          break;
        case 'r':
          ResetLaps();
          break;
        case 4: // ^D
        case 'q':
          running_ = false;
        }
      }
    }
    if (!one_line_) {
      if (Elapsed() - last_interrupt_ > kExitTimeout_)
        PrintCurrentTime();
    }

    if (interrupted) { // if we press Ctr+C^ twice with less time than
                       // kExitTimeout_ between them
      if (Elapsed() - last_interrupt_ < kExitTimeout_) {
        running_ = false;
        print_newline_ = true;
      } else {
        PrintSplitTime();
      }
      last_interrupt_ = Elapsed();
      interrupted = false;
    }
  }
  if (print_newline_)
    std::cout << std::endl;
  PrintEndTime();
  std::cout << std::endl;
}

CatVector Zeitkatze::ReadCats(std::string cat_file) {
  std::ifstream file(cat_file);
  CatVector ret;
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line))
      ret.push_back(line);
    file.close();
  } else {
    return cats_emotes_default_;
  }
  return ret;
}
