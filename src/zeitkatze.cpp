#include "zeitkatze.hpp"
#include "terminal_setter.hpp"
#include <atomic>   // atomic<T>
#include <chrono>   // chrono::duration, chrono::duration_cast
#include <cmath>    // pow(), floor()
#include <csignal>  // signal()
#include <cstdlib>  // getenv
#include <fcntl.h>  // fcntl()
#include <fstream>  // ifstream
#include <iomanip>  // setw(), setfill()
#include <iostream> // ostream()
#include <memory>   // make_unique()
#include <poll.h>   // poll()
#include <string>   // string
#include <unistd.h> // read()
#if __cplusplus >= 201703L
#include <filesystem> // create_directory
namespace fs = std::filesystem;
#elif __cplusplus >= 201103L
#include <experimental/filesystem> // create_directory
namespace fs = std::experimental::filesystem;
#endif

using std::setfill;
using std::setw;
using std::chrono::duration;
using std::chrono::duration_cast;

// constructor definitions
Zeitkatze::Zeitkatze()
    : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
      last_line_len_(0), precision_(2), enable_color_(true), one_line_(false) {
  Init(enable_color_);
}

Zeitkatze::Zeitkatze(bool enable_color)
    : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
      last_line_len_(0), precision_(2), enable_color_(enable_color),
      one_line_(false) {
  Init(enable_color_);
}

Zeitkatze::Zeitkatze(bool enable_color, unsigned precision)
    : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
      last_line_len_(0), precision_(precision), enable_color_(enable_color),
      one_line_(false) {
  Init(enable_color_);
}

Zeitkatze::Zeitkatze(bool enable_color, unsigned precision, bool one_line)
    : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
      last_line_len_(0), precision_(precision), enable_color_(enable_color),
      one_line_(one_line) {
  Init(enable_color_);
}

// method definitions
void Zeitkatze::PrintTime(const CatIndex cat_index, const Color color) {
  steady_clock::time_point now{steady_clock::now()};
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

void Zeitkatze::Init(const bool enable_color) {
  color_enabled = enable_color;
  char *color_env = getenv("ZEITKATZE_COLOR");
  if (color_env != nullptr && std::string(color_env) == "0")
    color_enabled = false;

  signal(SIGINT, interruptCallback);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  // setup the terminal for user inputs
  TerminalSetter::SetTerminal();
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

CatVector Zeitkatze::ReadCats() {
  std::string cat_filepath = dir_cat_emotes_ + file_cat_emotes_;
  std::ifstream file(cat_filepath);
  CatVector ret;
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line))
      ret.push_back(line);
    file.close();
  } else {
    fs::create_directory(dir_cat_emotes_);
    ResetEmotes();
    // write cats to file ~/.config/zeitkatze
    return cats_emotes_default_;
  }
  return ret;
}

void Zeitkatze::ResetEmotes() {
  fs::create_directory(dir_cat_emotes_);
  std::string cat_filepath = dir_cat_emotes_ + file_cat_emotes_;
  std::fstream file_to;
  file_to.open(cat_filepath, std::ios::out | std::ios::trunc);
  if (!file_to || file_to.fail())
    throw std::runtime_error("FATAL: Cannot create file " + file_cat_emotes_ +
                             " at " + dir_cat_emotes_);
  else {
    for (const auto &emote : cats_emotes_default_)
      file_to << emote + "\n";
    file_to.close();
  }
}
