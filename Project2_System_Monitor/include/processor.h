#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
#include <string>
class Processor {
 public:
  Processor() = default;
  Processor(const std::vector<std::string>& processor):prevCpuStats(processor){} 
  float Utilization(); 

 private:
 std::vector<std::string> prevCpuStats;
};

#endif