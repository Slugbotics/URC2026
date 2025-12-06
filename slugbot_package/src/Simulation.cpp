#include "slugbot_package/Simulation.hpp"

#include "slugbot_package/Simulation.hpp"

#include "rclcpp/rclcpp.hpp"
#include <cstdio>
#include <functional>
#include <cmath>
#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/keyboard.h>
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"

#define WHEEL_RADIUS 0.06
#define WHEEL_COUNT 6

void set_position(WbDeviceTag *side, float value);
void set_velocity(WbDeviceTag *side, float value);

namespace slugbot_driver {
void Simulation::init(
    webots_ros2_driver::WebotsNode *node,
    std::unordered_map<std::string, std::string> &parameters) {

  left_motors[0] = wb_robot_get_device("FrontLeftWheel");
  left_motors[1] = wb_robot_get_device("MiddleLeftWheel");
  left_motors[2] = wb_robot_get_device("BackLeftWheel");
  left_side = left_motors;

  right_motors[0] = wb_robot_get_device("FrontRightWheel");
  right_motors[1] = wb_robot_get_device("MiddleRightWheel");
  right_motors[2] = wb_robot_get_device("BackRightWheel");
  right_side = right_motors;

  turn_motors[0] = wb_robot_get_device("FrontLeftArm");
  turn_motors[1] = wb_robot_get_device("FrontRightArm");
  turn_motors[2] = wb_robot_get_device("BackLeftArm");
  turn_motors[3] = wb_robot_get_device("BackRightArm");

  set_position(left_side, INFINITY);
  set_position(right_side, INFINITY);

  set_velocity(left_side, 0);
  set_velocity(right_side, 0);

  // subscribe to wheel/turn commands published by the logic node
  left_wheel_subscription = node->create_subscription<std_msgs::msg::Float64>(
      "/left_wheel", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->left_wheel_speed_msg = msg->data;
      }
  );

  right_wheel_subscription = node->create_subscription<std_msgs::msg::Float64>(
      "/right_wheel", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->right_wheel_speed_msg = msg->data;
      }
  );

  turn_angle_subscription = node->create_subscription<std_msgs::msg::Float64>(
      "/turn_angle", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->turn_angle_msg = msg->data;
      }
  );

  keyboard_publisher = node->create_publisher<std_msgs::msg::String>("/keyboard", 10);

  time_step_ms = static_cast<int>(wb_robot_get_basic_time_step());
  wb_keyboard_enable(time_step_ms);
}

void Simulation::step() {
  // Read and publish keyboard input
  std::string keys;
  int key;
  while((key = wb_keyboard_get_key()) != -1) {
    switch (key) {
      case WB_KEYBOARD_UP:
        keys.push_back('w');
        break;
      case WB_KEYBOARD_DOWN:
        keys.push_back('s');
        break;
      case WB_KEYBOARD_LEFT:
        keys.push_back('a');
        break;
      case WB_KEYBOARD_RIGHT:
        keys.push_back('d');
        break;
      default:
        if (std::isalpha(key) || std::isdigit(key) || key == ' ') {
          keys.push_back(char(tolower(key)));
        }
        break;
    }
  }
  auto msg = std::make_unique<std_msgs::msg::String>();
  msg->data = keys;
  keyboard_publisher->publish(std::move(msg));

  // Set speeds and angles
  set_velocity(left_side, left_wheel_speed_msg);
  set_velocity(right_side, right_wheel_speed_msg);
  // Manually set all 6 speeds
  for (int i = 0; i < 3; i++) {
    wb_motor_set_velocity(left_motors[i], left_wheel_speed_msg);
    wb_motor_set_velocity(right_motors[i], right_wheel_speed_msg);
  }

  for(int i = 0; i < 4; i++) {
    wb_motor_set_position(turn_motors[i], i < 2 ? turn_angle_msg : -turn_angle_msg);
  }
}
} // namespace slugbot_driver

void set_position(WbDeviceTag *side, float value) {
  for (int i = 0; i < WHEEL_COUNT>>1; i++) {
    wb_motor_set_position(*side, value);
    side++;
  }
}

void set_velocity(WbDeviceTag *side, float value) {
  for (int i = 0; i < WHEEL_COUNT>>1; i++) {
    wb_motor_set_velocity(*side, value);
    side++;
  }
}

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(slugbot_driver::Simulation,
                       webots_ros2_driver::PluginInterface)