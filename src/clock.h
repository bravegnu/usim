#ifndef CLOCK_H
#define CLOCK_H

#include <vector>

class ClockedIf
{
 public:
  virtual void execute(void) = 0;
};

class Clock
{
 private:
  std::vector<ClockedIf *> m_clocked_list;
 public:
  Clock();
  void add_clocked(ClockedIf *clocked);
  void tick();
};

#endif
