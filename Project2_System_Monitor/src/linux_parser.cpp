#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
// #include <filesystem>
#include <unordered_map>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
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

string LinuxParser::Kernel() {
  std::string os, kernel;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}


std::vector<int> LinuxParser::Pids() {
    vector<int> pids;
    DIR *directory = opendir(kProcDirectory.c_str());
    struct dirent *file;
    while ((file = readdir(directory)) != nullptr)
    {
      // Is this a directory?
      if (file->d_type == DT_DIR)
      {
        // Is every character of the name a digit?
        string filename(file->d_name);
        if (std::all_of(filename.begin(), filename.end(), isdigit))
        {
          int pid = stoi(filename);
          pids.push_back(pid);
        }
      }
    }
    closedir(directory);
    return pids;
}

//  Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::ifstream memInfo(kProcDirectory + kMeminfoFilename);
  
  double totalMemory = 0.0;
  double availableMemory = 0.0;

  if(memInfo.is_open()){
    std::string line;
    
    while (getline(memInfo, line)) {
          std::istringstream linestream(line);
          std::string key,value;
          linestream >> key >> value;

          if (key == "MemTotal:") {
              totalMemory = std::stod(value);
          } else if (key == "MemAvailable:") {
              availableMemory = std::stod(value);
          }

          // Break the loop if both values are found
          if (totalMemory > 0.0 && availableMemory > 0.0) {
              break;
          }
        }
      } 
    if (totalMemory == 0.0) {
        return 0.0;
    }
    double memoryUtilization =  (totalMemory - availableMemory) / totalMemory ;
    return memoryUtilization;
    }
  

// Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime;

  std::ifstream upTimeFile(kProcDirectory + kUptimeFilename);

  if(upTimeFile.is_open()){
    string line;
    std::getline(upTimeFile, line);
    std::istringstream upTimeFileStream(line);
    upTimeFileStream >> uptime;
  }
  return uptime; }

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long totalNumberOfJiffies = 0; 
  std::ifstream JiffiesFile(kProcDirectory + kStatFilename);
  if (JiffiesFile.is_open()){
    string line;
    std::getline(JiffiesFile, line);
    std::istringstream iss(line);
    std::string cpuLabel;
    long  temp;
    
    iss >> cpuLabel;
    while(iss >> temp){
      totalNumberOfJiffies += temp;
    }    
  }
  return totalNumberOfJiffies;
}


//  Read and return the number of active jiffies for a Process ID
long LinuxParser::ActiveJiffies(int pid) { 
    std::string line;
    std::string value;
    long utime, stime;
    std::ifstream file(kProcDirectory + std::to_string(pid) + kStatFilename);

    if (file.is_open()) {
        std::getline(file, line);
        std::istringstream linestream(line);
        for (int i = 1; i <= 13; ++i) {  // Skipping the first 13 values
            linestream >> value;
        }
        linestream >> utime >> stime;
    } else {
        return 0;  
    }

    return utime + stime;  // Sum of user and kernel times }
}
// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long totalNumberOfJiffies = LinuxParser::Jiffies();
  long idleJiffies = LinuxParser::IdleJiffies();
  long activeJiffies = totalNumberOfJiffies - idleJiffies;
  
  return activeJiffies; }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  
  std::ifstream JiffiesFile(kProcDirectory + kStatFilename);
  long idleJiffies = 0 ;
  if (JiffiesFile.is_open()){
    string line;
    std::getline(JiffiesFile, line);
    std::istringstream linestream(line);
    std::string cpuLabel;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    linestream >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    idleJiffies = idle + iowait;
  }
  return idleJiffies;
  }

// Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    std::vector<std::string> cpuStats;

    if (file.is_open()) {
        std::getline(file, line);

        std::istringstream linestream(line);
        std::string token;
        while (linestream >> token) {
            if (token != "cpu") {
                cpuStats.push_back(std::to_string(std::stoll(token)));
            }
        }

        file.close();
    }

    return cpuStats;
}

// Read and return the CpuUtilization of a process
float LinuxParser::CpuUtilizationProcess(int pid) {
    std::string line;
    std::string value;

    float cpuUsage = 0.0;
    long utime, stime, starttime;

    std::string path = kProcDirectory + std::to_string(pid) + kStatFilename;
    std::ifstream file(path);
  
    if(file.is_open()){
      std::getline(file, line);
      std::istringstream linestream(line);
      std::vector<std::string> values;

      while (linestream >> value) {
              values.push_back(value);
          }
      utime = std::stol(values[13]);
      stime = std::stol(values[14]);
      starttime = std::stol(values[21]);
    }
    
    file.close();

    long uptime = LinuxParser::UpTime(); // Get system uptime
    long total_time = utime + stime;
    long seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));

    if (seconds > 0) {
          cpuUsage =  ((total_time / sysconf(_SC_CLK_TCK)) / static_cast<float>(seconds));
      }

      return cpuUsage;
  }
// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcesses = 0;
  std::ifstream statFile(kProcDirectory + kStatFilename);
  
  if(statFile.is_open()){
    std::string line;
    while(getline(statFile, line)){
      std::istringstream linestream(line);
      std::string key, value;
      linestream >> key >> value ;
      if(key == "processes"){
        totalProcesses = std::stoi(value);
        break;
      }
      }
    }
  return totalProcesses; 
  }

int LinuxParser::RunningProcesses()
{
  std::string procRunning, procRunningNum;
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open())
  {
    while (getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> procRunning >> procRunningNum;
      if (procRunning == "procs_running")
        break;
    }
  }
  return std::stoi(procRunningNum);
}

// Read and return the command associated with a process
std::string LinuxParser::Command(int pid) {
    std::string cmd;
    std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename); // Missing semicolon here

    if (file.is_open()) {
        getline(file, cmd);
        file.close();
    }

    return cmd;
}

// Read and return the memory used by a process
std::string LinuxParser::Ram(int pid) {
    std::string path = kProcDirectory + std::to_string(pid) + kStatusFilename;
    std::ifstream file(path);
    std::string line;
    std::string ram_usage;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            long value;

            // Look for lines containing "VmRSS:" (Resident Set Size)
            if (iss >> key >> value && key == "VmRSS:") {
                // Memory usage is in kilobytes, you can convert to other units if needed
                ram_usage = std::to_string(value / 1000)   + " MB ";
                break; 
            }
        }
        file.close();
    } else {
        ram_usage = "Error: Unable to open file for PID " + std::to_string(pid);
    }

    return ram_usage;
}

// Read and return the user ID associated with a process
std::string LinuxParser::Uid(int pid) {
    std::string path = kProcDirectory + std::to_string(pid) + kStatusFilename;
    std::ifstream file(path);
    std::string line;
    std::string uid;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key, value;

            if (iss >> key >> value && key == "Uid:") {
                uid = value;
                break; // Exit the loop once UID is found
            }
        }
        file.close();
    } else {
        uid = "Error: Unable to open file for PID " + std::to_string(pid);
    }

    return uid;
}

//  Read and return the user associated with a process
std::string LinuxParser::User(int pid) {
  std::unordered_map<std::string, std::string> uidToUsernameMap;
    if (uidToUsernameMap.empty()) {
        // If it's empty, populate it by reading /etc/passwd or another relevant file
        std::ifstream passwdFile(kPasswordPath);
        std::string line;

        while (std::getline(passwdFile, line)) {
            std::istringstream iss(line);
            std::string username, _, uid;
            if (std::getline(iss, username, ':') && std::getline(iss, _, ':') && std::getline(iss, uid, ':')) {
                // Store the UID-to-username mapping in the map
                uidToUsernameMap[uid] = username;
            }
        }

        passwdFile.close();
    }

    // Now, get the UID associated with the process and find the corresponding username
    std::string uid = Uid(pid);
    auto it = uidToUsernameMap.find(uid);

    if (it != uidToUsernameMap.end()) {
        return it->second; // Return the username
    } else {
        return "Unknown"; // Return "Unknown" if the UID is not found in the mapping
    }
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
    std::string path = kProcDirectory + std::to_string(pid) + kStatFilename;
    std::ifstream file(path);
    std::string line;
    long uptime = 0;

    if (file.is_open()) {
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string _; // Placeholder for unused fields
            long starttime;

            for (int i = 1; i <= 22; ++i) {
                if (i == 22) {
                    iss >> starttime;
                } else {
                    iss >> _;
                }
            }
            // Calculate process uptime using system uptime
            uptime = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
        }

        file.close();
    } else {
        uptime = -1;
    }

    return uptime;
}

