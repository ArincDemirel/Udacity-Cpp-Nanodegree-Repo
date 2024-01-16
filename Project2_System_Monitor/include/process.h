#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization();                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const; 

  Process(int pid, std::string ram, std::string usr, long int uptime, 
          std::string command, float processCpuInfo) {
    pid_ = pid;
    ram_ = ram;
    user_ = usr;
    uptime_ = uptime;
    command_ = command;
    cpu_utilization_ = processCpuInfo;
  }
  
 private:
   int pid_;
    std::string user_;
    std::string command_;
    float cpu_utilization_;
    std::string ram_;
    long int uptime_;
 
};

#endif