#ifndef ZEITKATZE_HPP
#define ZEITKATZE_HPP
#include "zeitkatze_base.hpp"
#include <atomic> // atomic<T>
#include <chrono> // chrono::duration, chrono::duration_cast
#include <memory> // make_unique()
#include <string> // string
#include <vector> // vector<T>

//-----------------------------------------------------------------------------
// External variable declarations
extern bool color_enabled;
extern std::atomic<bool> interrupted; // later bound to SIGINT
extern void interruptCallback(int);   // sets `interrupted` above

//-----------------------------------------------------------------------------
// Types/enums/operators
typedef std::vector<std::string> CatVector;
typedef std::vector<std::string>::size_type CatIndex;
using std::chrono::steady_clock;
enum class Color {
  Normal,
  Cat,
  Cat_hold,
  Total,
  Running,
  Running_lap,
  Split,
  Split_lap
};

extern std::ostream &operator<<(std::ostream &oss, Color c);

//-----------------------------------------------------------------------------
// Method definitions
class Zeitkatze : public ZeitkatzeBase {
public:
  Zeitkatze()
      : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
        last_line_len_(0), precision_(2), enable_color_(true),
        one_line_(false) {
    Init(enable_color_);
  }
  Zeitkatze(bool enable_color)
      : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
        last_line_len_(0), precision_(2), enable_color_(enable_color),
        one_line_(false) {
    Init(enable_color_);
  }
  Zeitkatze(bool enable_color, unsigned precision)
      : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
        last_line_len_(0), precision_(precision), enable_color_(enable_color),
        one_line_(false) {
    Init(enable_color_);
  }
  Zeitkatze(bool enable_color, unsigned precision, bool one_line)
      : split_printed_(false), start_(steady_clock::now()), last_lap_(start_),
        last_line_len_(0), precision_(precision), enable_color_(enable_color),
        one_line_(one_line) {
    Init(enable_color_);
  }
  // implemented interface
  virtual void Init(bool enable_color);
  virtual void Run();
  // accessors
  bool enable_color() const { return enable_color_; }
  unsigned precision() const { return precision_; }

private:
  // members
  bool running_ = true;
  // maximum duration (sec) between two ^C (Ctr-C) to consider them a double ^C
  const double kExitTimeout_ = 0.8;
  // Print a new line before the end_time. Should be done after ^C^C but not
  // after ^D (Ctr-D)
  bool print_newline_ = false;
  double last_interrupt_ = -kExitTimeout_;
  bool split_printed_, had_lap_;
  steady_clock::time_point start_, last_lap_;
  unsigned last_line_len_;
  // how many decimals when formating seconds in Zeitkatze instance
  const unsigned precision_;
  bool enable_color_;
  const bool one_line_;
  CatVector ReadCats(std::string cat_file = "cats.txt");
  CatVector cats_emotes_default_{
      "=(^.^)=", "=(o.o)=", "=(^.^)\"", "=(x.x)=", "=(o.o)m", " (o,o) ",
      "=(0.0)=", "=(@.@)=", "=(*.*)=",  "=(-.-)=", "=(v.v)=", "=(o.O)=",
      "=[˙.˙]=", "=(~.~)=", "=(ˇ.ˇ)=",  "=(=.=)="};
  CatVector cat_emotes_{std::move(ReadCats("cats.txt"))};
  // output the laps in one line instead of a scrolling screen
  // methods
  void PrintTime(const CatIndex cat_index, const Color color);
  void PrintCurrentTime();
  double Elapsed();
  std::string FormatSeconds(double seconds);
  CatIndex SomeCatIndex();
  void PrintSplitTime() { PrintTime(SomeCatIndex(), Color::Split); }
  void PrintEndTime() { PrintTime(cat_emotes_.size() - 1, Color::Total); }
  void ResetLaps();
};

#endif /* ZEITKATZE_HPP */
