#include "slugbot_package/Simulation.hpp"

#include "rclcpp/rclcpp.hpp"
#include <string>
#include <cctype>
#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/keyboard.h>
#include "std_msgs/msg/string.hpp"
#include "messages/msg/wheel_states.hpp"
#include "../../utils/math/Rotation2d.hpp"

#define WHEEL_RADIUS 0.06
#define WHEEL_COUNT 4

namespace slugbot_driver {
void Simulation::init(
    webots_ros2_driver::WebotsNode *node,
    std::unordered_map<std::string, std::string> &parameters) {

  motors[0] = wb_robot_get_device("FrontLeftWheel");
  motors[1] = wb_robot_get_device("FrontRightWheel");
  motors[2] = wb_robot_get_device("BackLeftWheel");
  motors[3] = wb_robot_get_device("BackRightWheel");

  turn_motors[0] = wb_robot_get_device("FrontLeftArm");
  turn_motors[1] = wb_robot_get_device("FrontRightArm");
  turn_motors[2] = wb_robot_get_device("BackLeftArm");
  turn_motors[3] = wb_robot_get_device("BackRightArm");

  for (int i = 0; i < WHEEL_COUNT; i++) {
    wb_motor_set_position(turn_motors[i], 0);
    wb_motor_set_position(motors[i], INFINITY);
    wb_motor_set_velocity(motors[i], 0);
  }

  // subscribe to wheel states message
  wheel_states_subscription = node->create_subscription<messages::msg::WheelStates>(
      "/wheel_states", rclcpp::SensorDataQoS().reliable(),
      [this](const messages::msg::WheelStates::SharedPtr msg){
        this->wheel_states_msg = *msg;
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

  // This simulation is currently only an approximation of swerve
  for(int i = 0; i < 4; i++) {
    double speed = wheel_states_msg.speeds[i];
    double angle = wheel_states_msg.angles[i];
    if(std::abs(angle) > M_PI/2){
      angle = Rotation2d(angle).invert().getRadians();
      speed *= -1;
    }
    wb_motor_set_velocity(motors[i], -speed);
    wb_motor_set_position(turn_motors[i], angle);
  }
}
} // namespace slugbot_driver

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(slugbot_driver::Simulation,
                       webots_ros2_driver::PluginInterface)