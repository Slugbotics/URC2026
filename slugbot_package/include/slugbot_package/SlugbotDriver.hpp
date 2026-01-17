#ifndef SLUGBOT_DRIVER_HPP
#define SLUGBOT_DRIVER_HPP

#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "messages/msg/controller_input.hpp"

class SlugbotDriver : public rclcpp::Node {
public:
  explicit SlugbotDriver();
  void update();

private:
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr back_left_wheel_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr back_right_wheel_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr front_left_wheel_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr front_right_wheel_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr left_turn_angle_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr right_turn_angle_publisher;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr keyboard_subscription;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_subscription_avoid;
  rclcpp::Subscription<messages::msg::ControllerInput>::SharedPtr controller_subscription;

  geometry_msgs::msg::Twist cmd_vel_msg_avoid;
  std::string keys_pressed;
  messages::msg::ControllerInput controller_input;

  rclcpp::TimerBase::SharedPtr timer;
  bool recieved_input = false;
};

#endif