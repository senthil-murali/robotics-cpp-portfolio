#include <cmath>
#include <limits>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>
#include <rclcpp_lifecycle/lifecycle_publisher.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <std_msgs/msg/bool.hpp>

// ObstacleDetector — ROS2 Lifecycle Component Node
//
// Subscribes to /scan (LaserScan), checks readings against a configurable
// danger threshold, and publishes alerts to /obstacle_alert (Bool).
//
// Concepts demonstrated:
//   - Lifecycle node (on_configure, on_activate, on_deactivate, on_cleanup,
//   on_shutdown)
//   - Component node (no main(), registered via macro, loadable into container)
//   - LifecyclePublisher (has internal active/inactive state, silently drops
//     messages when inactive — separate from the node's lifecycle state)
//   - shared_ptr (ROS2 uses shared_ptr for publishers, subscribers, and
//   messages)
//   - Lambda capture ([this] to access member variables from callback)
//   - const auto& (range-based for loop over laser scan data)
//   - RAII (shared_ptr reset() in on_cleanup — no manual delete needed)
//   - ROS2 parameters (configurable at launch without recompiling)

class ObstacleDetector : public rclcpp_lifecycle::LifecycleNode {
public:
  using CallbackReturn =
      rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

  ObstacleDetector(const rclcpp::NodeOptions &options)
      : LifecycleNode("obstacle_detector", options) {
    RCLCPP_INFO(get_logger(), "Creating ObstacleDetector node");
  }

  CallbackReturn
  on_configure(const rclcpp_lifecycle::State &previous_state) override {
    RCLCPP_INFO(get_logger(), "Configuring from [%s] state...",
                previous_state.label().c_str());

    // Sets the parameter with a default value, and then retrieves it. This
    // allows users to override the default value when launching the node.
    this->declare_parameter("obstacle_distance_threshold", 0.5);

    // Reads teh paraemeter value and stores it in a member variable for later
    // use in the laser scan callback.
    obstacle_distance_threshold_ =
        this->get_parameter("obstacle_distance_threshold").as_double();

    auto reliable_qos = rclcpp::QoS(10).reliable();
    auto best_effort_qos = rclcpp::QoS(10).best_effort();

    // Publisher with queue depth of 10
    obstacle_pub_ = this->create_publisher<std_msgs::msg::Bool>(
        "/obstacle_alert", reliable_qos);

    // Subscription
    // [this] captures the class instance so the lambda can access member
    // variables (obstacle_distance_threshold_, obstacle_pub_, is_active_).
    // SharedPtr msg — ROS2 delivers incoming messages as shared_ptr.

    laser_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", best_effort_qos,
        [this](const sensor_msgs::msg::LaserScan::SharedPtr msg) {
          // Skip all processing when inactive — saves CPU.
          // LifecyclePublisher would block the publish anyway,
          // but this avoids looping through 360+ readings for nothing.
          if (!is_active_) {
            return;
          }
          bool obstacle_detected = false;
          float closest_range = std::numeric_limits<float>::max();
          for (const auto &range : msg->ranges) {
            // LiDAR sensors return NaN, inf, or zero for invalid readings
            // (reflections, out-of-range, sensor errors). Skip these.
            if (std::isnan(range) || std::isinf(range) || range <= 0.0f) {
              continue;
            }
            if (range < closest_range) {
              closest_range = range;
            }
            if (range < obstacle_distance_threshold_) {
              obstacle_detected = true;
              break;
            }
          }
          RCLCPP_INFO(get_logger(), "Closest: %.2f m | Alert: %s",
                      closest_range, obstacle_detected ? "TRUE" : "FALSE");
          std_msgs::msg::Bool alert_msg;
          alert_msg.data = obstacle_detected;
          obstacle_pub_->publish(alert_msg);
        });

    return CallbackReturn::SUCCESS;
  }

  CallbackReturn
  on_activate(const rclcpp_lifecycle::State &previous_state) override {
    RCLCPP_INFO(get_logger(), "Activating from [%s] state...",
                previous_state.label().c_str());
    is_active_ = true;
    // Tells the LifecyclePublisher to start allowing messages through.
    // Without this, publish() silently drops every message.

    obstacle_pub_->on_activate();

    return CallbackReturn::SUCCESS;
  }

  CallbackReturn
  on_deactivate(const rclcpp_lifecycle::State &previous_state) override {
    RCLCPP_INFO(get_logger(), "Deactivating from [%s] state...",
                previous_state.label().c_str());
    is_active_ = false;
    obstacle_pub_->on_deactivate();

    return CallbackReturn::SUCCESS;
  }

  CallbackReturn
  on_cleanup(const rclcpp_lifecycle::State &previous_state) override {
    RCLCPP_INFO(get_logger(), "Cleaning up from [%s] state...",
                previous_state.label().c_str());
    // RAII: reset() releases the shared_ptr, which destroys the subscriber
    // and publisher. No manual delete needed. After this, the node is back
    // to Unconfigured — on_configure must be called again before activating.
    laser_sub_.reset();
    obstacle_pub_.reset();
    // safety reset of the internal state.
    is_active_ = false;

    return CallbackReturn::SUCCESS;
  }

  CallbackReturn
  on_shutdown(const rclcpp_lifecycle::State &previous_state) override {

    RCLCPP_INFO(get_logger(), "Shutting Down from [%s] state...",
                previous_state.label().c_str());
    return CallbackReturn::SUCCESS;
  }

private:
  // Regular subscription — ROS2 has no LifecycleSubscription.
  // Subscriber callbacks fire in any state, hence the is_active_ guard.
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub_;

  // LifecyclePublisher — has its own active/inactive state.
  // Silently drops messages when deactivated, even if publish() is called.
  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Bool>::SharedPtr
      obstacle_pub_;

  double obstacle_distance_threshold_ = 0.5;

  // Manual guard for subscriber callback. Could use
  // obstacle_pub_->is_activated() instead, but a separate flag gives more
  // control if we add features later.
  bool is_active_ = false;
};

// Registers this class as a ROS2 component node.
// This macro allows the node to be:
//   1. Loaded into a component container at runtime (shared process with other
//   nodes)
//   2. Auto-generates an executable (obstacle_detector_exec) via CMakeLists.txt
// No main() needed — ROS2 handles node creation using the constructor that
// takes NodeOptions.
#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(ObstacleDetector)