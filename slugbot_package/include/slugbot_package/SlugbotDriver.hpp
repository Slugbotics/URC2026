#ifndef WEBOTS_ROS2_PLUGIN_EXAMPLE_HPP
#define WEBOTS_ROS2_PLUGIN_EXAMPLE_HPP

#include "rclcpp/macros.hpp"
#include "webots_ros2_driver/PluginInterface.hpp"
#include "webots_ros2_driver/WebotsNode.hpp"

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

namespace slugbot_driver {
class SlugbotDriver : public webots_ros2_driver::PluginInterface {
public:
  void step() override;
  void init(webots_ros2_driver::WebotsNode *node,
            std::unordered_map<std::string, std::string> &parameters) override;

private:

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
      cmd_vel_subscription_avoid_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
      cmd_vel_subscription_input_;
  geometry_msgs::msg::Twist cmd_vel_msg_avoid;
  geometry_msgs::msg::Twist cmd_vel_msg_input;

  WbDeviceTag right_motors[3];
  WbDeviceTag *right_side;

  WbDeviceTag left_motors[3];
  WbDeviceTag *left_side;
  // distance sensors for debugging
  WbDeviceTag ds_left;
  WbDeviceTag ds_right;
  int time_step_ms = 0;
};
} // namespace slugbot_driver
#endif