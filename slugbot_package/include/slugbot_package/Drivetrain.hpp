#pragma once

#include <unordered_map>
#include <string>

#include "rclcpp/macros.hpp"

#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "rclcpp/rclcpp.hpp"

class Drivetrain : public rclcpp::Node {
public:
  Drivetrain();
  void update();

private:
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr front_left_wheel_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr front_right_wheel_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr back_left_wheel_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr back_right_wheel_subscription;

  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr left_turn_angle_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr right_turn_angle_subscription;

  double front_left_wheel_speed_msg = 0.0;
  double front_right_wheel_speed_msg = 0.0;
  double back_left_wheel_speed_msg = 0.0;
  double back_right_wheel_speed_msg = 0.0;

  double left_turn_angle_msg = 0.0;
  double right_turn_angle_msg = 0.0;
  rclcpp::TimerBase::SharedPtr timer;
};