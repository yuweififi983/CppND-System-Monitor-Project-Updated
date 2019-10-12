#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    string result;
    long hour = seconds / (60 * 60);
    long minute = (seconds / 60) % 60;
    long secs = seconds % 60;
    std::stringstream outSs;
    outSs << std::setfill('0') << std::setw(2) << hour << ":"
        << std::setfill('0') << std::setw(2) << minute << ":"
        << std::setfill('0') << std::setw(2) << secs;

    outSs >> result; 

  return result;
}