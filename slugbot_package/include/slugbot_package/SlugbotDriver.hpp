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

  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr arm_pivot_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr arm_joint1_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr arm_joint2_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr wrist_yaw_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr wrist_pitch_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr wrist_roll_publisher;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr gripper_publisher;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr keyboard_subscription;
  rclcpp::Subscription<messages::msg::ControllerInput>::SharedPtr controller_subscription;
  rclcpp::Subscription<messages::msg::ControllerInput>::SharedPtr operator_subscription;
  std::string keys_pressed;
  messages::msg::ControllerInput controller_input;
  messages::msg::ControllerInput operator_input;

  rclcpp::TimerBase::SharedPtr timer;
  bool recieved_input = false;
};

#endif