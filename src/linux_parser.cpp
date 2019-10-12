#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float mem_total = 1;
  float mem_free = 0;
  float buffers = 0;
  string strTmp;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (stream >> strTmp) {
      if (strTmp == "MemTotal:") {
        if (stream >> strTmp) mem_total = stof(strTmp);
      } else if (strTmp == "MemFree:") {
        if (stream >> strTmp) mem_free = stof(strTmp);
      } else if (strTmp == "Buffers:") {
        if (stream >> strTmp) buffers = stof(strTmp);
      }
    }
  }
  return 1 - mem_free / (mem_total - buffers);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string strTmp;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream stream(line);
    if (stream >> strTmp) {
      return stoi(strTmp);
    }
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK);; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, token;
  vector<string> values;
  long jiffies = 0;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> token)
      values.push_back(token);
  }
  
  if (values.size() > 21) {
    long user = stol(values[13]);
    long kernel = stol(values[14]);
    long children_user = stol(values[15]);
    long children_kernel = stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }
  return jiffies;
 }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
	long activeJiffies = 0;
	 vector<string> cpu_usd = CpuUtilization();

	activeJiffies = (stol(cpu_usd[CPUStates::kUser_]) + stol(cpu_usd[CPUStates::kNice_]) +
          stol(cpu_usd[CPUStates::kSystem_]) + stol(cpu_usd[CPUStates::kIRQ_]) +
          stol(cpu_usd[CPUStates::kSoftIRQ_]) + stol(cpu_usd[CPUStates::kSteal_]) +
          stol(cpu_usd[CPUStates::kGuest_]) + stol(cpu_usd[CPUStates::kGuestNice_]));

	return activeJiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
	vector<string> values = CpuUtilization();
	long idleJiffies = (stol(values[CPUStates::kIdle_]) + stol(values[CPUStates::kIOwait_]));
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
        if (key == "processes")
        return stoi(value);
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
      std::istringstream linestream(line);
      string key, value;
      while (linestream >> key >> value) {
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
  string strTmp;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> strTmp) {
      if (strTmp == "VmSize:") {
        if (stream >> strTmp) return std::to_string(stoi(strTmp) / 1024);
      }
    }
  }
  return string("0");
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
 string strTmp;
 std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> strTmp) {
      if (strTmp == "Uid:") {
        if (stream >> strTmp) return strTmp;
      }
    }
  }
  return string("");
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
  return "0";
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

        time = stol(strTmp) / sysconf(_SC_CLK_TCK);
        return time;
      }
  }
  return time;
}