#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

class Robot {

  const std::string name_;
  const std::string ip_address_;
  std::string connection_status_;
  std::string fault_status_;
  std::string program_running_;
  std::chrono::steady_clock::time_point last_heartbeat_;

public:
  Robot(const std::string &name, const std::string &ip_address)
      : name_(name), ip_address_(ip_address),
        connection_status_("disconnected"), fault_status_("clear"),
        program_running_("no_program") {}

  void info() const {
    std::cout << "Robot Name: " << name_ << "\n";
    std::cout << "IP Address: " << ip_address_ << "\n";
    std::cout << "Connection Status: " << connection_status_ << "\n";
    std::cout << "Fault Status: " << fault_status_ << "\n";
    std::cout << "Program Running: " << program_running_ << "\n";
    std::cout << "Last Heartbeat: "
              << std::chrono::duration_cast<std::chrono::seconds>(
                     std::chrono::steady_clock::now() - last_heartbeat_)
                     .count()
              << " seconds ago\n";
  }

  void set_connection_status(const std::string &status) {
    connection_status_ = status;
    last_heartbeat_ = std::chrono::steady_clock::now();
  }

  void set_fault_status(const std::string &status) {
    fault_status_ = status;
    last_heartbeat_ = std::chrono::steady_clock::now();
  }

  void set_program_running(const std::string &program) {
    program_running_ = program;
    last_heartbeat_ = std::chrono::steady_clock::now();
  }

  void heartbeat() { last_heartbeat_ = std::chrono::steady_clock::now(); }

  const std::string &get_name() const { return name_; }

  const std::string &get_ip_address() const { return ip_address_; }
  const std::string &get_connection_status() const {
    return connection_status_;
  }
  const std::string &get_fault_status() const { return fault_status_; }
  const std::string &get_program_running() const { return program_running_; }
};

class ConnectionManager {
  std::vector<std::unique_ptr<Robot>> robots_;

public:
  void add_robot(const std::string &name, const std::string &ip_address) {
    robots_.push_back(std::make_unique<Robot>(name, ip_address));
  }

  void remove_robot(const std::string &name) {

    // Erase-remove idiom: remove_if moves non-matching elements to front,
    // erase chops the leftover from the returned iterator to end.

    robots_.erase(std::remove_if(robots_.begin(), robots_.end(),
                                 [&name](const std::unique_ptr<Robot> &robot) {
                                   return robot->get_name() == name;
                                 }),
                  robots_.end());
  }

  void list_robots() const {
    for (const auto &robot : robots_) {
      robot->info();
      std::cout << "-------------------------------\n";
    }
  }

  //THis method is const because it doesn't modify the ConnectionManager, it just looks up a robot. 
  const Robot *find_robot(const std::string &name) const {
    // std::find_if searches linearly, returns iterator to first match or end()
    // if not found.
    // .get() extracts raw pointer from unique_ptr without transferring
    // ownership.
    auto it = std::find_if(robots_.begin(), robots_.end(),
                           [&name](const std::unique_ptr<Robot> &robot) {
                             return robot->get_name() == name;
                           });
    if (it != robots_.end()) {
      return it->get();
    }
    return nullptr;
  }
  // This non-const version allows setters to modify the robot's status after finding it.

  Robot *find_robot(const std::string &name) {
    // std::find_if searches linearly, returns iterator to first match or end()
    // if not found.
    // .get() extracts raw pointer from unique_ptr without transferring
    // ownership.
    auto it = std::find_if(robots_.begin(), robots_.end(),
                           [&name](const std::unique_ptr<Robot> &robot) {
                             return robot->get_name() == name;
                           });
    if (it != robots_.end()) {
      return it->get();
    }
    return nullptr;
  }
};

int main() {
  ConnectionManager manager;
  manager.add_robot("Alpha", "192.168.1.100");
  manager.add_robot("Beta", "192.168.1.110");
  manager.add_robot("Gamma", "192.168.1.120");

  auto *RobotAlpha = manager.find_robot("Alpha");
  auto *RobotBeta = manager.find_robot("Beta");

  // Simulated delay between robot communications
  std::this_thread::sleep_for(std::chrono::seconds(2));

  auto *RobotGamma = manager.find_robot("Gamma");

  std::cout << (RobotAlpha ? GREEN "✓ Found Alpha" : RED "✗ Alpha not found")
            << RESET << "\n";
  std::cout << (RobotBeta ? GREEN "✓ Found Beta" : RED "✗ Beta not found")
            << RESET << "\n";
  std::cout << (RobotGamma ? GREEN "✓ Found Gamma" : RED "✗ Gamma not found")
            << RESET << "\n";

  if (RobotAlpha) {
    RobotAlpha->set_connection_status("connected");
  } else {
    std::cout << RED "✗ Alpha not found, cannot set status" << RESET << "\n";
  }

  // Simulated delay between robot communications
  std::this_thread::sleep_for(std::chrono::seconds(2));

  if (RobotBeta) {
    RobotBeta->set_connection_status("connected");
  } else {
    std::cout << RED "✗ Beta not found, cannot set status" << RESET << "\n";
  }

  // Simulated delay between robot communications
  std::this_thread::sleep_for(std::chrono::seconds(2));

  if (RobotGamma) {
    RobotGamma->set_connection_status("connected");
  } else {
    std::cout << RED "✗ Gamma not found, cannot set status" << RESET << "\n";
  }

  auto *RobotDelta = manager.find_robot("Delta");
  std::cout << (RobotDelta ? GREEN "✓ Found Delta" : RED "✗ Delta not found")
            << RESET << "\n";

  std::cout << CYAN "All robots:\n" << RESET;

  manager.list_robots();

  manager.remove_robot("Gamma");

  std::cout << CYAN "All robots after deletion:\n" << RESET;

  manager.list_robots();

  std::cout << " Connection manager closing down...\n" << RESET;

  return 0;
}
