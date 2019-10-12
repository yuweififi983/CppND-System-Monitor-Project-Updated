#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
	float utiliz = 0;
	long activeJeff = LinuxParser::ActiveJiffies();
    long idleJeff = LinuxParser::IdleJiffies();
    long durActive = activeJeff - lastActiveJeff;
    long durIdle = idleJeff - lastIdleJeff;
    long durTotal = durActive + durIdle;
    utiliz = static_cast<float>(durActive) / durTotal;
    lastActiveJeff = activeJeff;
    lastIdleJeff = idleJeff;
    return utiliz;

}