#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "map"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// sort processes according to CPU usage 
vector<int> System::sortPid(vector<int> pids) {
    std::map<int, int, std::greater <int> > pid_map;
    std::vector<int> sort_pid;
    int pids_size= pids.size();

    for (auto i = 0; i<pids_size; i++)
        pid_map.insert(std::make_pair(LinuxParser::ActiveJiffies(pids[i]), pids[i]));
        std::map<int,int> :: iterator it; 
        for (it = pid_map.begin(); it != pid_map.end() ; it++){ 
            //cout << "(" << (*it).first << ", " << (*it).second << ")" << endl;
            sort_pid.push_back((*it).second);
        }
    return sort_pid;
}
// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    vector<int> pids = LinuxParser::Pids();
    int pids_size= pids.size();
    vector<int> pids_sort;
    for (int i = 0; i < pids_size; i++)
        pids_sort.push_back(pids[i]);
    
    vector<int> pids_sorted = System::sortPid(pids_sort);

    for (int j = 0; j < pids_size; j++)
        processes_.emplace_back(pids_sorted[j]);

    return processes_; 
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel();}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization();}

// TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }