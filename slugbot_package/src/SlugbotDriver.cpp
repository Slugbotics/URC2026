#include "slugbot_package/SlugbotDriver.hpp"

#include "rclcpp/rclcpp.hpp"
#include <cstdio>
#include <functional>
#include <cmath>
#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/keyboard.h>

#define HALF_DISTANCE_BETWEEN_WHEELS 0.24
#define WHEEL_RADIUS 0.06
#define WHEEL_COUNT 6
#define IGNORE_AVOID_MESSAGE true

void set_position(WbDeviceTag *side, float value);
void set_velocity(WbDeviceTag *side, float value);

namespace slugbot_driver {
void SlugbotDriver::init(
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

  set_position(left_side, INFINITY);
  set_position(right_side, INFINITY);

  set_velocity(left_side, 0);
  set_velocity(right_side, 0);

  cmd_vel_subscription_avoid_ = node->create_subscription<geometry_msgs::msg::Twist>(
      "/cmd_vel_avoid", rclcpp::SensorDataQoS().reliable(),
      [this](const geometry_msgs::msg::Twist::SharedPtr msg){
        this->cmd_vel_msg_avoid.linear = msg->linear;
        this->cmd_vel_msg_avoid.angular = msg->angular;
      }
  );

  controller_subscription = node->create_subscription<messages::msg::ControllerInput>(
      "/controller_input", rclcpp::SensorDataQoS().reliable(),
      [this](const messages::msg::ControllerInput::SharedPtr msg){
        this->cmd_vel_msg_input.linear.x = -msg->left_y * std::abs(msg->left_y);
        this->cmd_vel_msg_input.angular.z = 2 * msg->right_x * std::abs(msg->right_x);
        this->recieved_input = true;
      }
  );

  time_step_ms = static_cast<int>(wb_robot_get_basic_time_step());
  wb_keyboard_enable(time_step_ms);
}

void SlugbotDriver::step() {
  if(!recieved_input) {
    int key;
    bool w=false, a=false, s=false, d=false;
    while((key = wb_keyboard_get_key()) != -1) {
      switch (key) {
        case 'W':
        case 'w':
        case WB_KEYBOARD_UP:
          w = true;
          break;
        case 'S':
        case 's':
        case WB_KEYBOARD_DOWN:
          s = true;
          break;
        case 'A':
        case 'a':
        case WB_KEYBOARD_LEFT:
          a = true;
          break;
        case 'D':
        case 'd':
        case WB_KEYBOARD_RIGHT:
          d = true;
          break;
      }
    }
    cmd_vel_msg_input.linear.x = 0.0;
    cmd_vel_msg_input.angular.z = 0.0;
    if (IGNORE_AVOID_MESSAGE) {
      cmd_vel_msg_avoid.linear.x = 0.0;
      cmd_vel_msg_avoid.angular.z = 0.0;
    }
    if (w) {
      cmd_vel_msg_input.linear.x = 1.0;
    }
    if (s) {
      cmd_vel_msg_input.linear.x += -1.0;
    }
    if (a && cmd_vel_msg_avoid.angular.z == 0.0) {
      cmd_vel_msg_input.angular.z = -2.0;
    }
    if (d && cmd_vel_msg_avoid.angular.z == 0.0) {
      cmd_vel_msg_input.angular.z = 2.0;
    }
  }

  auto forward_speed = cmd_vel_msg_avoid.linear.x + cmd_vel_msg_input.linear.x;
  auto angular_speed = cmd_vel_msg_avoid.angular.z + cmd_vel_msg_input.angular.z;

  auto command_motor_left =
      (forward_speed - angular_speed * HALF_DISTANCE_BETWEEN_WHEELS) /
      WHEEL_RADIUS;
  auto command_motor_right =
      (forward_speed + angular_speed * HALF_DISTANCE_BETWEEN_WHEELS) /
      WHEEL_RADIUS;

  set_velocity(left_side, command_motor_left);
  set_velocity(right_side, command_motor_right);
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
PLUGINLIB_EXPORT_CLASS(slugbot_driver::SlugbotDriver,
                       webots_ros2_driver::PluginInterface)