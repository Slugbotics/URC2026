#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "rclcpp/macros.hpp"
#include "webots_ros2_driver/PluginInterface.hpp"
#include "webots_ros2_driver/WebotsNode.hpp"

#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "rclcpp/rclcpp.hpp"

namespace slugbot_driver {
class Simulation : public webots_ros2_driver::PluginInterface {
public:
  void step() override;
  void init(webots_ros2_driver::WebotsNode *node,
            std::unordered_map<std::string, std::string> &parameters) override;

private:

  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr left_wheel_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr right_wheel_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr turn_angle_subscription;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr keyboard_publisher;
  double left_wheel_speed_msg;
  double right_wheel_speed_msg;
  double turn_angle_msg;
  std::string keys_pressed;

  WbDeviceTag right_motors[3];
  WbDeviceTag *right_side;

  WbDeviceTag left_motors[3];
  WbDeviceTag *left_side;

  WbDeviceTag turn_motors[4];

  // distance sensors for debugging
  WbDeviceTag ds_left;
  WbDeviceTag ds_right;
  int time_step_ms = 0;
};
} // namespace slugbot_driver
#endif