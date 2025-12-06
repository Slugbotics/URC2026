#include "slugbot_package/SlugbotDriver.hpp"

#include "rclcpp/rclcpp.hpp"
#include <algorithm>
#include <cmath>
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "messages/msg/controller_input.hpp"

#define HALF_DISTANCE_BETWEEN_WHEELS 0.24
#define WHEEL_RADIUS 0.06
#define MAX_WHEEL_SPEED 25.0
#define MAX_LINEAR_SPEED (MAX_WHEEL_SPEED * WHEEL_RADIUS)
#define MAX_TURN_ANGLE (M_PI / 6)
#define IGNORE_AVOID_MESSAGE true

SlugbotDriver::SlugbotDriver()
    : Node("slugbot_driver") {
  left_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/left_wheel", 10);
  right_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/right_wheel", 10);
  turn_angle_publisher = this->create_publisher<std_msgs::msg::Float64>("/turn_angle", 10);

  cmd_vel_subscription_avoid = this->create_subscription<geometry_msgs::msg::Twist>(
      "/cmd_vel_avoid", 10,
      [this](const geometry_msgs::msg::Twist::SharedPtr msg){
        this->cmd_vel_msg_avoid = *msg;
      }
  );

  controller_subscription = this->create_subscription<messages::msg::ControllerInput>(
      "/controller_input", 10,
      [this](const messages::msg::ControllerInput::SharedPtr msg){
        this->controller_input = *msg;
        this->recieved_input = true;
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

void SlugbotDriver::update() {
  double speed = 0;
  double angle = 0;

  if (recieved_input) {
    double ly = controller_input.left_y;
    double rx = controller_input.right_x;
    speed = -MAX_LINEAR_SPEED * ly * std::abs(ly);
    angle = MAX_TURN_ANGLE * rx * std::abs(rx);
  } else {
    if (keys_pressed.find('w') != std::string::npos){
      speed = MAX_LINEAR_SPEED;
    }
    if (keys_pressed.find('s') != std::string::npos){
      speed -= MAX_LINEAR_SPEED;
    }
    if (keys_pressed.find('a') != std::string::npos){
      angle = -MAX_TURN_ANGLE;
    }
    if (keys_pressed.find('d') != std::string::npos){
      angle += MAX_TURN_ANGLE;
    }
  }

  if (IGNORE_AVOID_MESSAGE) {
    cmd_vel_msg_avoid.linear.x = 0.0;
    cmd_vel_msg_avoid.angular.z = 0.0;
  }

  speed += cmd_vel_msg_avoid.linear.x;
  angle += cmd_vel_msg_avoid.angular.z;

  double wheelSpeed = std::clamp(speed / WHEEL_RADIUS, -MAX_WHEEL_SPEED, MAX_WHEEL_SPEED);

  auto left_msg = std::make_unique<std_msgs::msg::Float64>();
  left_msg->data = wheelSpeed;
  left_wheel_publisher->publish(std::move(left_msg));

  auto right_msg = std::make_unique<std_msgs::msg::Float64>();
  right_msg->data = wheelSpeed;
  right_wheel_publisher->publish(std::move(right_msg));

  auto turn_msg = std::make_unique<std_msgs::msg::Float64>();
  turn_msg->data = angle;
  turn_angle_publisher->publish(std::move(turn_msg));
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SlugbotDriver>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}