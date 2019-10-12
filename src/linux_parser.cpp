#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using namespace std;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
     /* if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }*/
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memory_util = 0;
  int memory_total = 0;
  int memory_free = 0;
  string memory_total_name = "MemTotal";
  string memory_free_name = "MemFree";
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      if (line.compare(0, memory_total_name.size(), memory_total_name)) {
        std::istringstream buf(line);
        std::istream_iterator<string> beg(buf), end;
        std::vector<string> values(beg, end);
        memory_total = (stof(values[1]) / float(1024));
      }

      if (line.compare(0, memory_free_name.size(), memory_free_name)) {
        std::istringstream buf(line);
        std::istream_iterator<string> beg(buf), end;
        std::vector<string> values(beg, end);
        memory_free = (stof(values[1]) / float(1024));
      }
    }
  }
  memory_util = memory_total - memory_free;
  return memory_util;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime = 0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream buf(line);
      std::istream_iterator<string> beg(buf), end;
      std::vector<string> values(beg, end);
      std::istringstream str_v(values[0]);
      str_v >> uptime;
    }
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK);; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, token;
  vector<string> values;
  long jiffies = 0;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> token)
      values.push_back(token);
  }
  
  if (values.size() > 21) {
    cout << "active jiff id" << "\n";
    long user = stol(values[13]);
    long kernel = stol(values[14]);
    long children_user = stol(values[15]);
    long children_kernel = stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
    cout << "active jiff id end" << "\n";
  }
  return jiffies;
 }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
	long activeJiffies = 0;
	 vector<string> cpu_usd = CpuUtilization();
   std::cout << "idle jiff" << "\n";
	activeJiffies = (stol(cpu_usd[CPUStates::kUser_]) + stol(cpu_usd[CPUStates::kNice_]) +
          stol(cpu_usd[CPUStates::kSystem_]) + stol(cpu_usd[CPUStates::kIRQ_]) +
          stol(cpu_usd[CPUStates::kSoftIRQ_]) + stol(cpu_usd[CPUStates::kSteal_]) +
          stol(cpu_usd[CPUStates::kGuest_]) + stol(cpu_usd[CPUStates::kGuestNice_]));

	return activeJiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
   std::cout << "idle jiff" << "\n";
	vector<string> values = CpuUtilization();
  for (int i = 0; i < values.size(); i++)
    cout << "a:" << values[i] << "";
  
	long idleJiffies = (stol(values[CPUStates::kIdle_]) + stol(values[CPUStates::kIOwait_]));
  std::cout << "idle jiff end" << "\n";
	return idleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string  strTmp;
 vector<string> cpuUtil;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> strTmp) {
        if (strTmp == "cpu") {
          while (linestream >> strTmp)
            cpuUtil.push_back(strTmp);
          return cpuUtil;
        }
      }
    }
  }
  return cpuUtil;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
	string line;
	  string key;
	  string value;
	  std::ifstream stream(kProcDirectory + kStatFilename);
	  if (stream.is_open()) {
	    while (std::getline(stream, line)) {
	      std::istringstream stream(line);
	      while (stream >> key >> value) {
		if (key == "processes") {
		  return stoi(value);
		}
	      }
	    }
	  }
	return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
	string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream stream(line);
      string key, value;
      while (stream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  } 

return 0;
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    return line;
  }
  return "";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string name = "VmSize";
  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string value;
  float result = 0;
  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result = (stof(values[1]) / float(1024));
      break;
    }
  }
  return to_string(result);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string result = "";
  string name = "Uid";
  std::string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result = values[1];
      break;
    }
  }
  return "";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
 std::ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) {
    string line;
    string token = "x:" + Uid(pid);
    while (std::getline(stream, line)) {
      auto marker = line.find(token);
      if (marker != string::npos) {
        return line.substr(0, marker - 1);
      }
    }
  }
  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long int time{0};
  string strTmp;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatFilename);

  if (stream.is_open()) {
    for (int i = 0; stream >> strTmp; ++i)
      if (i == 13) {
        time = stol(strTmp);
        time /= sysconf(_SC_CLK_TCK);
        return time;
      }
  }
  return time;
}