#include "zeitkatze.hpp"

#include <atomic>
#include <iostream>
#include <memory> // make_unique
#include <string>

// define external variables and operator
bool color_enabled{true};
std::atomic<bool> interrupted{false};

std::ostream &operator<<(std::ostream &oss, Color c) {
  if (color_enabled) {
    switch (c) {
    case Color::Normal:
      oss << "\033[0m";
      break;
    case Color::Cat:
      oss << "\033[34m";
      break;
    case Color::Cat_hold:
      oss << "\033[34;1m";
      break;
    case Color::Total:
      oss << "\033[37;1m";
      break;
    case Color::Running:
      oss << "\033[32m";
      break;
    case Color::Running_lap:
      oss << "\033[33m";
      break;
    case Color::Split:
      oss << "\033[32;1m";
      break;
    case Color::Split_lap:
      oss << "\033[33;1m";
      break;
    default:
      break;
    }
  }
  return oss;
}

void interruptCallback(int) { interrupted = true; }

int main(int argc, char **argv) {
  std::string instructions =
      "Zeitkatze\n\n"
      "\ttime cat -- literally\n\n"
      "Arguments:\n"
      "[-n | --no-color] (default true) If passed, disables colours\n"
      "[-p | --precision] (default 2) How many decimals to use when measuring "
      "time\n"
      "Wrong/Invalid argument prints usage message and exits\n\n"
      "Instructions:\n"
      "\tRun the executable to start measuring time.\n"
      "\t-- Ctr + c = Split time/lap time\n "
      "\t-- Ctr + d = Stop and exit\n"
      "\t-- r = Reset current lap\n\n";

  // TODO: write an arg parser class
  bool argColorEnabled = true;
  int argPrecision = 2;
  bool argOneLine = false;
  char **arg = argv;
  auto argEqual = [](char **arg, const std::string &str) -> bool {
    return !std::string(*arg).compare(str);
  };
  while (--argc > 0) {
    if (argEqual(arg, "-n") || argEqual(arg, "--no-color"))
      argColorEnabled = false;
    if (argEqual(arg, "-p") || argEqual(arg, "--precision")) {
      if (argc >= 1)
        argPrecision = std::stoi(std::string(*++arg));
    }
    if (argEqual(arg, "-o") || argEqual(arg, "--one-line"))
      argOneLine = true;
    if (argEqual(arg, "-h") || argEqual(arg, "--help")) {
      std::cout << instructions;
      return 0;
    }
    arg++;
  }

  color_enabled = argColorEnabled;
  auto z = std::make_unique<Zeitkatze>(color_enabled, argPrecision, argOneLine);
  z->run();
  return 0;
}
