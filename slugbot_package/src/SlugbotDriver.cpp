#include "slugbot_package/SlugbotDriver.hpp"

#include "rclcpp/rclcpp.hpp"
#include <algorithm>
#include <cmath>
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "messages/msg/controller_input.hpp"

const double WHEEL_RADIUS = 0.06;
const double MAX_WHEEL_SPEED = 25.0;
const double MAX_LINEAR_SPEED = (MAX_WHEEL_SPEED * WHEEL_RADIUS);

SlugbotDriver::SlugbotDriver()
    : Node("slugbot_driver") {
  // This assumes a 4 wheel drive with the front wheels turning, but it will also work with 6 wheels
  left_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/left_wheel", 10);
  right_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/right_wheel", 10);
  arm_pivot_publisher = this->create_publisher<std_msgs::msg::Float64>("/arm_pivot", 10);
  arm_joint1_publisher = this->create_publisher<std_msgs::msg::Float64>("/arm_joint1", 10);
  arm_joint2_publisher = this->create_publisher<std_msgs::msg::Float64>("/arm_joint2", 10);
  wrist_yaw_publisher = this->create_publisher<std_msgs::msg::Float64>("/wrist_yaw", 10);
  wrist_pitch_publisher = this->create_publisher<std_msgs::msg::Float64>("/wrist_pitch", 10);
  wrist_roll_publisher = this->create_publisher<std_msgs::msg::Float64>("/wrist_roll", 10);
  gripper_publisher = this->create_publisher<std_msgs::msg::Float64>("/gripper", 10);

  controller_subscription = this->create_subscription<messages::msg::ControllerInput>(
      "/controller_input", 10,
      [this](const messages::msg::ControllerInput::SharedPtr msg){
        this->controller_input = *msg;
        this->recieved_input = true;
      }
  );
  operator_subscription = this->create_subscription<messages::msg::ControllerInput>(
      "/operator_input", 10,
      [this](const messages::msg::ControllerInput::SharedPtr msg){
        this->operator_input = *msg;
      }
  );

  keyboard_subscription = this->create_subscription<std_msgs::msg::String>(
      "/keyboard", 10,
      [this](const std_msgs::msg::String::SharedPtr msg){
        this->keys_pressed = msg->data;
      }
  );

  // Run at 50Hz
  timer = this->create_wall_timer(std::chrono::milliseconds(20),
    std::bind(&SlugbotDriver::update, this));
}

double gripper_position = 0;
void SlugbotDriver::update() {
  double speed = 0;
  double angle = 0;
  double arm_pivot = 0;
  double arm_joint1 = 0;
  double arm_joint2 = 0;
  double wrist_yaw = 0;
  double wrist_pitch = 0;
  double wrist_roll = 0;
  double gripper = 0;

  if (recieved_input) {
    double ly = controller_input.left_y;
    double rx = controller_input.right_x;
    speed = -MAX_LINEAR_SPEED * ly * std::abs(ly);
    angle = -rx * std::abs(rx);
  } else {
    if (keys_pressed.find('w') != std::string::npos){
      speed = MAX_LINEAR_SPEED;
    }
    if (keys_pressed.find('s') != std::string::npos){
      speed -= MAX_LINEAR_SPEED;
    }
    if (keys_pressed.find('a') != std::string::npos){
      angle = 1;
    }
    if (keys_pressed.find('d') != std::string::npos){
      angle -= 1;
    }
  }

  arm_pivot = operator_input.left_x;
  arm_joint1 = operator_input.left_y;
  arm_joint2 = operator_input.right_y;
  wrist_yaw = (operator_input.left_trigger - operator_input.right_trigger) / 2;
  wrist_pitch = (int)operator_input.dpad_up - (int)operator_input.dpad_down;
  wrist_roll = (int)operator_input.dpad_right - (int)operator_input.dpad_left;
  gripper = operator_input.right_x;

  angle *= MAX_LINEAR_SPEED;
  double max_wheel = std::abs(speed) + std::abs(angle);
  if(max_wheel > MAX_LINEAR_SPEED){
    speed *= MAX_LINEAR_SPEED / max_wheel;
    angle *= MAX_LINEAR_SPEED / max_wheel;
  }

  auto left_msg = std_msgs::msg::Float64();
  left_msg.data = (speed - angle) / WHEEL_RADIUS;
  left_wheel_publisher->publish(left_msg);

  auto right_msg = std_msgs::msg::Float64();
  right_msg.data = (speed + angle) / WHEEL_RADIUS;
  right_wheel_publisher->publish(right_msg);

  auto arm_pivot_msg = std_msgs::msg::Float64();
  arm_pivot_msg.data = arm_pivot;
  arm_pivot_publisher->publish(arm_pivot_msg);

  auto arm_joint1_msg = std_msgs::msg::Float64();
  arm_joint1_msg.data = arm_joint1;
  arm_joint1_publisher->publish(arm_joint1_msg);

  auto arm_joint2_msg = std_msgs::msg::Float64();
  arm_joint2_msg.data = arm_joint2;
  arm_joint2_publisher->publish(arm_joint2_msg);

  auto wrist_yaw_msg = std_msgs::msg::Float64();
  wrist_yaw_msg.data = wrist_yaw;
  wrist_yaw_publisher->publish(wrist_yaw_msg);

  auto wrist_pitch_msg = std_msgs::msg::Float64();
  wrist_pitch_msg.data = wrist_pitch;
  wrist_pitch_publisher->publish(wrist_pitch_msg);

  auto wrist_roll_msg = std_msgs::msg::Float64();
  wrist_roll_msg.data = wrist_roll;
  wrist_roll_publisher->publish(wrist_roll_msg);

  auto gripper_msg = std_msgs::msg::Float64();
  gripper_position += gripper * 0.05;
  gripper_position = std::max(-1.0, std::min(1.0, gripper_position));
  gripper_msg.data = gripper_position;
  gripper_publisher->publish(gripper_msg);
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SlugbotDriver>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}