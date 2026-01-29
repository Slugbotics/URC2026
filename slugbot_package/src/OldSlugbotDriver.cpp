/*
#include "slugbot_package/SlugbotDriver.hpp"

#include "rclcpp/rclcpp.hpp"
#include <algorithm>
#include <cmath>
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "messages/msg/controller_input.hpp"

const double HALF_DISTANCE_BETWEEN_WHEELS = 0.24;
const double FORWARD_DISTANCE_BETWEEN_WHEELS = 0.30;
const double WHEEL_RADIUS = 0.06;
const double MAX_WHEEL_SPEED = 25.0;
const double MAX_LINEAR_SPEED = (MAX_WHEEL_SPEED * WHEEL_RADIUS);
const double MAX_TURN_ANGLE = (M_PI / 6.0);
const bool IGNORE_AVOID_MESSAGE = true;

SlugbotDriver::SlugbotDriver()
    : Node("slugbot_driver") {
  // This assumes a 4 wheel drive with the front wheels turning, but it will also work with 6 wheels
  back_left_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/back_left_wheel", 10);
  back_right_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/back_right_wheel", 10);
  front_left_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/front_left_wheel", 10);
  front_right_wheel_publisher = this->create_publisher<std_msgs::msg::Float64>("/front_right_wheel", 10);
  left_turn_angle_publisher = this->create_publisher<std_msgs::msg::Float64>("/left_turn_angle", 10);
  right_turn_angle_publisher = this->create_publisher<std_msgs::msg::Float64>("/right_turn_angle", 10);

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
    angle = rx * std::abs(rx);
  } else {
    if (keys_pressed.find('w') != std::string::npos){
      speed = MAX_LINEAR_SPEED;
    }
    if (keys_pressed.find('s') != std::string::npos){
      speed -= MAX_LINEAR_SPEED;
    }
    if (keys_pressed.find('a') != std::string::npos){
      angle = -1;
    }
    if (keys_pressed.find('d') != std::string::npos){
      angle += 1;
    }
  }

  if (IGNORE_AVOID_MESSAGE) {
    cmd_vel_msg_avoid.linear.x = 0.0;
    cmd_vel_msg_avoid.angular.z = 0.0;
  }

  speed += cmd_vel_msg_avoid.linear.x;
  speed = std::clamp(speed, -MAX_LINEAR_SPEED, MAX_LINEAR_SPEED);
  double max_angular_speed = std::abs(speed) / (HALF_DISTANCE_BETWEEN_WHEELS + FORWARD_DISTANCE_BETWEEN_WHEELS/std::tan(MAX_TURN_ANGLE));
  angle *= max_angular_speed;
  angle += cmd_vel_msg_avoid.angular.z;
  angle = std::clamp(angle, -max_angular_speed, max_angular_speed);

  double back_left, back_right, front_left, front_right, left_angle, right_angle;
  if(std::abs(angle) < 1e-5) {
    back_left = speed;
    back_right = speed;
    front_left = speed;
    front_right = speed;
    left_angle = 0.0;
    right_angle = 0.0;
  }else{
    double radius = speed / angle;
    double left_r = radius - HALF_DISTANCE_BETWEEN_WHEELS;
    double right_r = radius + HALF_DISTANCE_BETWEEN_WHEELS;
    back_left = angle * left_r;
    back_right = angle * right_r;
    double front_left_r = std::hypot(left_r, FORWARD_DISTANCE_BETWEEN_WHEELS);
    double front_right_r = std::hypot(right_r, FORWARD_DISTANCE_BETWEEN_WHEELS);
    front_left = angle * front_left_r * std::abs(left_r) / left_r;
    front_right = angle * front_right_r * std::abs(right_r) / right_r;
    left_angle = std::atan(FORWARD_DISTANCE_BETWEEN_WHEELS / left_r);
    right_angle = std::atan(FORWARD_DISTANCE_BETWEEN_WHEELS / right_r);
    double highest_speed = std::max({std::abs(back_left), std::abs(back_right), std::abs(front_left), std::abs(front_right)});
    if (highest_speed > MAX_LINEAR_SPEED) {
      double scale = MAX_LINEAR_SPEED / highest_speed;
      back_left *= scale;
      back_right *= scale;
      front_left *= scale;
      front_right *= scale;
    }
  }

  auto back_left_msg = std_msgs::msg::Float64();
  back_left_msg.data = back_left / WHEEL_RADIUS;
  back_left_wheel_publisher->publish(back_left_msg);

  auto back_right_msg = std_msgs::msg::Float64();
  back_right_msg.data = back_right / WHEEL_RADIUS;
  back_right_wheel_publisher->publish(back_right_msg);

  auto front_left_msg = std_msgs::msg::Float64();
  front_left_msg.data = front_left / WHEEL_RADIUS;
  front_left_wheel_publisher->publish(front_left_msg);

  auto front_right_msg = std_msgs::msg::Float64();
  front_right_msg.data = front_right / WHEEL_RADIUS;
  front_right_wheel_publisher->publish(front_right_msg);

  auto left_angle_msg = std_msgs::msg::Float64();
  left_angle_msg.data = left_angle;
  left_turn_angle_publisher->publish(left_angle_msg);

  auto right_angle_msg = std_msgs::msg::Float64();
  right_angle_msg.data = right_angle;
  right_turn_angle_publisher->publish(right_angle_msg);
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SlugbotDriver>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
*/