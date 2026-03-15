#pragma once
#include <cstddef>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <vector>

// Waypoint in RPY format — converted to PoseStamped before sending to Nav2
struct Waypoint {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double roll = 0.0;
  double pitch = 0.0;
  double yaw = 0.0;
};

// Type aliases to avoid repeating long template types
using NavigateToPose = nav2_msgs::action::NavigateToPose;
using GoalHandle = rclcpp_action::ClientGoalHandle<NavigateToPose>;

class WaypointFollowerNode : public rclcpp::Node {
public:
  WaypointFollowerNode();
  void send_next_goal();

private:
  // State
  size_t current_waypoint_index_ = 0;
  std::vector<Waypoint> waypoints_;
  NavigateToPose::Goal current_goal_;

  // ROS2 interfaces
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp_action::Client<NavigateToPose>::SharedPtr action_client_;

  // Converts Waypoint (RPY) to PoseStamped (quaternion) in the map frame
  geometry_msgs::msg::PoseStamped waypoint_to_pose_stamped(const Waypoint &wp);

  // Nav2 action callbacks
  void goal_response_callback(GoalHandle::SharedPtr goal_handle);

  void feedback_callback(
      GoalHandle::SharedPtr goal_handle,
      const std::shared_ptr<const NavigateToPose::Feedback> feedback);

  void result_callback(const GoalHandle::WrappedResult &result);
};