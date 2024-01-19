#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>
#include <unistd.h> // sleep

float Processor::Utilization() { 
    static std::vector<std::string> prevCpuStats = LinuxParser::CpuUtilization();

    std::vector<std::string> currCpuStats = LinuxParser::CpuUtilization();

    // Ensure the vectors have the expected number of elements
    if (prevCpuStats.size() < 8 || currCpuStats.size() < 8) {
        return 0.0;
    }

    long long prevIdle = std::stoll(prevCpuStats[3]) + std::stoll(prevCpuStats[4]);
    long long currIdle = std::stoll(currCpuStats[3]) + std::stoll(currCpuStats[4]);

    long long prevNonIdle = std::stoll(prevCpuStats[0]) + std::stoll(prevCpuStats[1]) + std::stoll(prevCpuStats[2]) +
                            std::stoll(prevCpuStats[5]) + std::stoll(prevCpuStats[6]) + std::stoll(prevCpuStats[7]);
    long long currNonIdle = std::stoll(currCpuStats[0]) + std::stoll(currCpuStats[1]) + std::stoll(currCpuStats[2]) +
                            std::stoll(currCpuStats[5]) + std::stoll(currCpuStats[6]) + std::stoll(currCpuStats[7]);

    long long prevTotal = prevIdle + prevNonIdle;
    long long currTotal = currIdle + currNonIdle;

    long long totalDelta = currTotal - prevTotal;
    long long idleDelta = currIdle - prevIdle;

    if (totalDelta == 0) {
        return 0.0;
    }

    float cpuUtilization = (1.0 - static_cast<float>(idleDelta) / totalDelta); //did not multiply with 100 since the units are in KB, it cancels out each other.

    prevCpuStats = currCpuStats;

    return cpuUtilization;
}
