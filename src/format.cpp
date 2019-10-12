#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hh = seconds / 3600;
  long hh_other = seconds % 3600;
  long mm = seconds / 60;
  long ss = hh_other % 60;
  std::string result =
      std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss);

  return result;
}