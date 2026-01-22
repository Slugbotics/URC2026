#ifndef SLUGBOT_DRIVER_HPP
#define SLUGBOT_DRIVER_HPP

#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "messages/msg/controller_input.hpp"
#include "messages/msg/wheel_states.hpp"

class SlugbotDriver : public rclcpp::Node {
public:
  explicit SlugbotDriver();
  void update();

private:
  void optimize_wheel_states(messages::msg::WheelStates& wheels);

  messages::msg::WheelStates current_wheel_states;

  rclcpp::Publisher<messages::msg::WheelStates>::SharedPtr wheel_states_publisher;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr keyboard_subscription;
  rclcpp::Subscription<messages::msg::ControllerInput>::SharedPtr controller_subscription;
  std::string keys_pressed;
  messages::msg::ControllerInput controller_input;

  rclcpp::TimerBase::SharedPtr timer;
  bool recieved_input = false;
};

#endif