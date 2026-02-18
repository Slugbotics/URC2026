#ifndef SLUGBOT_DRIVER_HPP
#define SLUGBOT_DRIVER_HPP

#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64.hpp"
#include "messages/msg/controller_input.hpp"

class SlugbotDriver : public rclcpp::Node {
public:
  explicit SlugbotDriver();
  void update();

private:
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr left_wheel_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr right_wheel_publisher;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr keyboard_subscription;
  rclcpp::Subscription<messages::msg::ControllerInput>::SharedPtr controller_subscription;
  std::string keys_pressed;
  messages::msg::ControllerInput controller_input;

  rclcpp::TimerBase::SharedPtr timer;
  bool recieved_input = false;
};

#endif