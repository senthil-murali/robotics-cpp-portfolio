#include <functional>
#include <iostream>
#include <memory>
#include <vector>

struct SensorReading {
  double value;
  double timestamp;
  std::string sensor_id;
};

void firstway() {
  std::cout << "First way With double and std::function" << std::endl;
  std::cout << "----------------------------------" << std::endl;
  std::vector<double> rawData = {102.3, 101.8, 999.9, 102.1, 101.5};
  std::vector<double> processedData;

  double threshold = 102.0;
  auto filter = [threshold](double value) {
    return value > threshold ? -1.0 : value;
  };
  auto convert = [](double value) { return value > 0 ? value / 1000.0 : -1.0; };
  auto scale = [](double value) { return value * 2.0; };

  // Harder way to do the same and incase of order change it becomes diffcult to
  // maintain This approach is less flexible and more error-prone, as it
  // requires manual management of the processing steps and their order. If the
  // order of processing steps changes, it can lead to incorrect results or
  // require significant code changes to accommodate the new order.
  double testValue = 101.8;
  double result = filter(testValue);
  result = convert(result);
  std::cout << "Processed value: " << result << std::endl;

  // use a vector of functions to apply multiple processing steps in sequence
  // This approach allows for easy addition or removal of processing steps
  // without changing the core logic function<double(double)> is used to define
  // a vector of functions that take a double as input and return a double as
  // output incase of 2 values in and one value out the definition becoems
  // function<double(double, double)> and the processing steps need to be
  // updated accordingly to accommodate the new function signature
  std::vector<std::function<double(double)>> processingSteps = {filter, convert,
                                                                scale};

  for (const auto &value : rawData) {
    double result = value;
    for (const auto &step : processingSteps) {
      result = step(result);
    }
    processedData.push_back(result);
  }
  std::cout << "Raw Data -> Processed Data:" << std::endl;
  for (size_t i = 0; i < processedData.size(); ++i) {
    std::cout << rawData[i] << " -> " << processedData[i] << " " << std::endl;
  }
  std::cout << std::endl;
}

void secondway() {

  std::cout << "second way With struct and unique pointer" << std::endl;
  std::cout << "----------------------------------" << std::endl;

  // Unique ptr cannot be declared like this for  muliple values as it requires
  // a single object to manage and cannot be copied or assigned to another
  // unique pointer. Instead, we can use a vector of unique pointers to manage
  // multiple SensorReading objects. Each unique pointer in the vector will
  // manage a single SensorReading object, allowing us to store and process
  // multiple sensor readings while ensuring proper memory management and
  // avoiding issues with copying or assigning unique pointers.

  //   std::vector<std::unique_ptr<SensorReading>> rawData = {
  //       std::make_unique<SensorReading>(
  //           SensorReading{102.3, 1622548800.0, "sensor_1"}),
  //       std::make_unique<SensorReading>(
  //           SensorReading{101.8, 1622548860.0, "sensor_2"}),
  //       std::make_unique<SensorReading>(
  //           SensorReading{999.9, 1622548920.0, "sensor_3"}),
  //       std::make_unique<SensorReading>(
  //           SensorReading{102.1, 1622548980.0, "sensor_4"}),
  //       std::make_unique<SensorReading>(
  //           SensorReading{101.5, 1622549040.0, "sensor_5"})};

  std::vector<std::unique_ptr<SensorReading>> rawData;
  rawData.push_back(std::make_unique<SensorReading>(
      SensorReading{102.3, 1622548800.0, "sensor_1"}));
  rawData.push_back(std::make_unique<SensorReading>(
      SensorReading{101.8, 1622548860.0, "sensor_2"}));
  rawData.push_back(std::make_unique<SensorReading>(
      SensorReading{999.9, 1622548920.0, "sensor_3"}));
  rawData.push_back(std::make_unique<SensorReading>(
      SensorReading{102.1, 1622548980.0, "sensor_4"}));
  rawData.push_back(std::make_unique<SensorReading>(
      SensorReading{101.5, 1622549040.0, "sensor_5"}));

  std::vector<std::unique_ptr<SensorReading>> processedData;

  double threshold = 102.0;
  auto filter = [threshold](double value) {
    return value > threshold ? -1.0 : value;
  };
  auto convert = [](double value) { return value > 0 ? value / 1000.0 : -1.0; };
  auto scale = [](double value) { return value * 2.0; };

  // Harder way to do the same and incase of order change it becomes diffcult to
  // maintain This approach is less flexible and more error-prone, as it
  // requires manual management of the processing steps and their order. If the
  // order of processing steps changes, it can lead to incorrect results or
  // require significant code changes to accommodate the new order.
  double testValue = 101.8;
  double result = filter(testValue);
  result = convert(result);
  std::cout << "Processed value: " << result << std::endl;

  //Pipeline Approach 
  std::vector<std::function<double(double)>> processingSteps = {filter, convert,
                                                                scale};
																
  // After processing only the value from the original pointer we move the
  // data with timestamp and sensor name inot new pointer and push it to the
  // processed data vector this way we maintain the original data and also have
  // the processed data with all the details intact
  for (const auto &value : rawData) {
    double result = value->value;
    for (const auto &step : processingSteps) {
      result = step(result);
    }
    auto processed = std::make_unique<SensorReading>(
        SensorReading{result, value->timestamp, value->sensor_id});
    processedData.push_back(std::move(processed));
  }
  std::cout << "Raw Data -> Processed Data:" << std::endl;
  for (size_t i = 0; i < processedData.size(); ++i) {
    std::cout << rawData[i]->value << " -> " << processedData[i]->value << " "
              << std::endl;
  }
  std::cout << std::endl;
}

int main() {

  firstway();

  secondway();

  return 0;
}