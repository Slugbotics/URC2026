#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <unordered_map>
#include <string>

#include "rclcpp/macros.hpp"
#include "webots_ros2_driver/PluginInterface.hpp"
#include "webots_ros2_driver/WebotsNode.hpp"

#include "std_msgs/msg/string.hpp"
#include "messages/msg/wheel_states.hpp"
#include "rclcpp/rclcpp.hpp"
#include <webots/robot.h>

namespace slugbot_driver {
class Simulation : public webots_ros2_driver::PluginInterface {
public:
  void step() override;
  void init(webots_ros2_driver::WebotsNode *node,
            std::unordered_map<std::string, std::string> &parameters) override;

private:
  rclcpp::Subscription<messages::msg::WheelStates>::SharedPtr wheel_states_subscription;

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr keyboard_publisher;

  messages::msg::WheelStates wheel_states_msg;

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