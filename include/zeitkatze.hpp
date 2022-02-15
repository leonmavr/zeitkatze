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
// Class interface
class Zeitkatze : public ZeitkatzeBase {
public:
  // constructors
  Zeitkatze();
  Zeitkatze(bool enable_color);
  Zeitkatze(bool enable_color, unsigned precision);
  Zeitkatze(bool enable_color, unsigned precision, bool one_line);
  // implemented interface
  virtual void Init(bool enable_color);
  virtual void Run();
  // other public methods
  void ResetEmotes();
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
  const std::string dir_cat_emotes_ =
      std::string(getenv("HOME")) + "/.config/zeitkatze/";
  const std::string file_cat_emotes_ = "cats.txt";
  // how many decimals when formating seconds in Zeitkatze instance
  const unsigned precision_;
  bool enable_color_;
  // output the laps in one line instead of a scrolling screen
  const bool one_line_;
  CatVector ReadCats();
  CatVector cats_emotes_default_{
      "=(^.^)=", "=(o.o)=", "=(^.^)\"", "=(x.x)=", "=(o.o)m", " (o,o) ",
      "=(0.0)=", "=(@.@)=", "=(*.*)=",  "=(-.-)=", "=(v.v)=", "=(o.O)=",
      "=[˙.˙]=", "=(~.~)=", "=(ˇ.ˇ)=",  "=(=.=)="};
  CatVector cat_emotes_{std::move(ReadCats())};
  bool reset_cat_emote_file_{false};

  // methods
  /**
   * @brief Prints total time and current lap time including interrupt
   *
   * @param cat_index A index to CatVector, preferably random
   * @param color A Color enum type that determines the terminal color
   */
  void PrintTime(const CatIndex cat_index, const Color color);
  /**
   * @brief Prints total time and lap time formatted according to the class's
   * options
   */
  void PrintCurrentTime();
  double Elapsed();
  /**
   * @brief Formats seconds into minutes, seconds, and fractions of second
   *
   * @param seconds (double) seconds
   *
   * @return A string that contains all of the above mentioned in brief
   */
  std::string FormatSeconds(double seconds);
  CatIndex SomeCatIndex();
  void PrintSplitTime() { PrintTime(SomeCatIndex(), Color::Split); }
  void PrintEndTime() { PrintTime(cat_emotes_.size() - 1, Color::Total); }
  void ResetLaps();
};

#endif /* ZEITKATZE_HPP */
