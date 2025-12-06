#ifndef SLUGBOT_DRIVER_HPP
#define SLUGBOT_DRIVER_HPP

#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "messages/msg/controller_input.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"

#include "rclcpp/rclcpp.hpp"

class SlugbotDriver : public rclcpp::Node {
public:
  explicit SlugbotDriver();
  void update();

private:
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr left_wheel_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr right_wheel_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr turn_angle_publisher;
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