#include <vector>
#include "clock.h"

Clock::Clock()
{
  m_clocked_list = std::vector<ClockedIf *>();
}

void Clock::add_clocked(ClockedIf *clocked)
{
  m_clocked_list.push_back(clocked);
}

void Clock::tick()
{
  for (auto it = m_clocked_list.begin();
       it != m_clocked_list.end();
       it++) {
    (*it)->execute();
  }
}
