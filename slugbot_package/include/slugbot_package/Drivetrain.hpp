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
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr left_wheel_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr right_wheel_subscription;
  double left_wheel_speed_msg = 0.0;
  double right_wheel_speed_msg = 0.0;

  rclcpp::TimerBase::SharedPtr timer;
};