#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <unordered_map>
#include <string>

#include "rclcpp/macros.hpp"
#include "webots_ros2_driver/PluginInterface.hpp"
#include "webots_ros2_driver/WebotsNode.hpp"

#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64.hpp"
#include "rclcpp/rclcpp.hpp"
#include <webots/robot.h>

namespace slugbot_driver {
class Simulation : public webots_ros2_driver::PluginInterface {
public:
  void step() override;
  void init(webots_ros2_driver::WebotsNode *node,
            std::unordered_map<std::string, std::string> &parameters) override;

private:
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr keyboard_publisher;

  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr left_wheel_subscription;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr right_wheel_subscription;
  double left_wheel_speed_msg = 0.0;
  double right_wheel_speed_msg = 0.0;

  std::string keys_pressed;

  WbDeviceTag motors[4];
  WbDeviceTag turn_motors[4];

  // distance sensors for debugging (optional)
  WbDeviceTag ds_left;
  WbDeviceTag ds_right;

  int time_step_ms = 0;
};
} // namespace slugbot_driver

#endif