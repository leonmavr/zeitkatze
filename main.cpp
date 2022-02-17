#include "zeitkatze.hpp"

#include <atomic>
#include <iostream>
#include <memory> // make_unique
#include <string>

// define external variables and operator
bool color_enabled{true};
std::atomic<bool> interrupted{false};
// this callback is later bound to `interrupted` by the SIGINT signal
void interruptCallback(int) { interrupted = true; }

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

int main(int argc, char **argv) {
  std::string instructions =
      "Zeitkatze\n\n"
      "\ttime cat -- literally\n\n"
      "Arguments:\n"
      "[-n | --no-color] (default true) If passed, disables colours\n"
      "[-p | --precision] (default 2) How many decimals to use when measuring "
      "time\n"
      "[-o | --one-line ] (default false) Whether or not to print the output "
      "in one line (as lap times)\n"
      "Wrong/Invalid argument prints usage message and exits\n\n"
      "Instructions:\n"
      "\tRun the executable to start measuring time.\n"
      "\t-- Ctr + c = Split time/lap time\n "
      "\t-- Enter = Split time/lap time\n "
      "\t-- Ctr + d = Stop and exit\n"
      "\t-- Ctr + c twice = Stop and exit\n"
      "\t-- q = Stop and exit\n"
      "\t-- r = Reset current lap\n\n";

  // variables prefixed by arg_ are set by the CLI arguments
  // and then passed to the Zeitkatze constructor
  bool arg_color_enabled = true;
  int arg_precision = 2;
  bool arg_one_line = false;
  bool arg_reset_emotes = false;
  char **arg = argv + 1; // program name is at index 0
  auto argEqual = [](char **arg, const std::string &str) -> bool {
    return !std::string(*arg).compare(str);
  };
  while (--argc > 0) {
    // on/off specifiers
    if (argEqual(arg, "-n") || argEqual(arg, "--no-color"))
      arg_color_enabled = false;
    else if (argEqual(arg, "-o") || argEqual(arg, "--one-line"))
      arg_one_line = true;
    else if (argEqual(arg, "-h") || argEqual(arg, "--help")) {
      std::cout << instructions;
      return 0;
    } else if (argEqual(arg, "-r") || argEqual(arg, "--reset-emotes")) {
      arg_reset_emotes = true;
    }
    // specifiers followed by argument
    else if (argEqual(arg, "-p") || argEqual(arg, "--precision")) {
      arg_precision = std::stoi(std::string(*++arg));
      argc--;
    }
    arg++;
  }

  color_enabled = arg_color_enabled;
  auto zeitkatze =
      std::make_unique<Zeitkatze>(color_enabled, arg_precision, arg_one_line);
  if (!arg_reset_emotes) {
    // do the work
    zeitkatze->Run();
  } else {
    // reset text file with emotes and exit
    zeitkatze->ResetEmotes();
  }
  return 0;
}
