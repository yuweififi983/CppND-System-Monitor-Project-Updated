#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {};
// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
void Process::CpuUtilization(long activeJeff, long systemJeff) {
 	long duration_active = activeJeff - lastActiveJeff;
  	long duration = systemJeff - lastSystemJeff;
  	cpu_ = static_cast<float>(duration_active) / duration;
  	lastActiveJeff = activeJeff;
  	lastSystemJeff = systemJeff;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_; }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const & a) const {
  return CpuUtilization() < a.CpuUtilization();
}

bool Process::operator>(Process const & a) const {
  return CpuUtilization() > a.CpuUtilization();
}

