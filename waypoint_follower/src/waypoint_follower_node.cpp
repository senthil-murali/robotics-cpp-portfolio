#include "waypoint_follower/waypoint_follower_node.hpp"

// === CONSTRUCTOR ===
// Initialize action client, define waypoints, start a one-shot timer
// that kicks off navigation once Nav2 is ready
WaypointFollowerNode::WaypointFollowerNode()
    : rclcpp::Node("waypoint_follower"),
      action_client_(rclcpp_action::create_client<NavigateToPose>(
          this, "navigate_to_pose")) {

  waypoints_.push_back({-1.5, -0.5, 0.0, 0.0, 0.0, 0.0});
  waypoints_.push_back({-0.5, -0.5, 0.0, 0.0, 0.0, 1.57});
  waypoints_.push_back({-0.5, 0.5, 0.0, 0.0, 0.0, 3.14});
  waypoints_.push_back({-1.5, 0.5, 0.0, 0.0, 0.0, -1.57});

  timer_ = create_wall_timer(std::chrono::seconds(1), [this]() {
    timer_->cancel();
    if (!action_client_->wait_for_action_server(std::chrono::seconds(5))) {
      RCLCPP_ERROR(get_logger(), "Nav2 action server not available!");
      return;
    }
    send_next_goal();
  });

  RCLCPP_INFO(get_logger(), "Waypoint Follower Node has been started.");
}

// === SEND_NEXT_GOAL ===
// Check if all waypoints are done. If not, convert the current
// waypoint to a PoseStamped, attach the three callbacks, and
// send the goal to Nav2. Then return — Nav2 takes over from here.
void WaypointFollowerNode::send_next_goal() {

  if (current_waypoint_index_ >= waypoints_.size()) {
    RCLCPP_INFO(get_logger(), "All waypoints reached!");
    return;
  }

  geometry_msgs::msg::PoseStamped next =
      waypoint_to_pose_stamped(waypoints_[current_waypoint_index_]);

  current_goal_.pose = next;

  RCLCPP_INFO(get_logger(), "Sending goal to waypoint %zu (%.2f,%.2f)",
              current_waypoint_index_, next.pose.position.x,
              next.pose.position.y);

  auto send_goal_options =
      rclcpp_action::Client<NavigateToPose>::SendGoalOptions();
  send_goal_options.goal_response_callback =
      std::bind(&WaypointFollowerNode::goal_response_callback, this,
                std::placeholders::_1);
  send_goal_options.feedback_callback =
      std::bind(&WaypointFollowerNode::feedback_callback, this,
                std::placeholders::_1, std::placeholders::_2);
  send_goal_options.result_callback = std::bind(
      &WaypointFollowerNode::result_callback, this, std::placeholders::_1);

  action_client_->async_send_goal(current_goal_, send_goal_options);
}

// === WAYPOINT_TO_POSE_STAMPED ===
// Convert our simple (x, y, yaw) struct into the PoseStamped
// message Nav2 expects. Yaw/roll/pitch → quaternion via tf2.
geometry_msgs::msg::PoseStamped
WaypointFollowerNode::waypoint_to_pose_stamped(const Waypoint &wp) {

  geometry_msgs::msg::PoseStamped pose_stamped;
  pose_stamped.header.frame_id = "map";
  pose_stamped.pose.position.x = wp.x;
  pose_stamped.pose.position.y = wp.y;
  pose_stamped.pose.position.z = wp.z;

  tf2::Quaternion q;
  q.setRPY(wp.roll, wp.pitch, wp.yaw);
  pose_stamped.pose.orientation.x = q.x();
  pose_stamped.pose.orientation.y = q.y();
  pose_stamped.pose.orientation.z = q.z();
  pose_stamped.pose.orientation.w = q.w();

  return pose_stamped;
}

// === GOAL_RESPONSE_CALLBACK ===
// Triggered once after sending. Nav2 tells us: accepted or rejected.
void WaypointFollowerNode::goal_response_callback(
    GoalHandle::SharedPtr goal_handle) {
  if (!goal_handle) {
    RCLCPP_ERROR(get_logger(), "Goal was rejected by Nav2!");
    return;
  }
  RCLCPP_INFO(get_logger(), "Goal accepted by Nav2!");
}

// === FEEDBACK_CALLBACK ===
// Triggered repeatedly while robot is moving. Nav2 sends distance
// remaining.Throttled to avoid spamming th eoncole with too many messages.
void WaypointFollowerNode::feedback_callback(
    GoalHandle::SharedPtr goal_handle,
    const std::shared_ptr<const NavigateToPose::Feedback> feedback) {

  (void)goal_handle;
  double distance_remaining = feedback->distance_remaining;

  static double last_logged_distance = -1.0;
  if (std::abs(distance_remaining - last_logged_distance) > 0.1) {
    RCLCPP_INFO(get_logger(), "Distance remaining: %.2f meters",
                distance_remaining);
    last_logged_distance = distance_remaining;
  }
}

// === RESULT_CALLBACK ===
// Triggered once when navigation finishes. On success, increment
// index and call send_next_goal() — this chains waypoints together.
void WaypointFollowerNode::result_callback(
    const GoalHandle::WrappedResult &result) {

  switch (result.code) {
  case rclcpp_action::ResultCode::SUCCEEDED:
    RCLCPP_INFO(get_logger(), "Goal succeeded!");
    break;
  case rclcpp_action::ResultCode::ABORTED:
    RCLCPP_ERROR(get_logger(), "Goal was aborted!");
    return;
  case rclcpp_action::ResultCode::CANCELED:
    RCLCPP_ERROR(get_logger(), "Goal was canceled!");
    return;
  default:
    RCLCPP_ERROR(get_logger(), "Unknown result code!");
    return;
  }

  current_waypoint_index_++;
  send_next_goal();
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<WaypointFollowerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}