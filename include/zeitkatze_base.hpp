#ifndef ZEITKATZE_BASE_HPP
#define ZEITKATZE_BASE_HPP

class ZeitkatzeBase {
public:
  virtual void Init(bool enable_color) = 0;
  virtual void Run() = 0;
  virtual ~ZeitkatzeBase() {}
};

#endif /* ZEITKATZE_BASE_HPP */
