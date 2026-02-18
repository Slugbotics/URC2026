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

  angle *= MAX_LINEAR_SPEED;
  double max_wheel = std::abs(speed) + std::abs(angle);
  if(max_wheel > MAX_LINEAR_SPEED){
    speed *= MAX_LINEAR_SPEED / max_wheel;
    angle *= MAX_LINEAR_SPEED / max_wheel;
  }

  auto left_msg = std_msgs::msg::Float64();
  left_msg.data = speed - angle;
  left_wheel_publisher->publish(left_msg);

  auto right_msg = std_msgs::msg::Float64();
  right_msg.data = speed + angle;
  right_wheel_publisher->publish(right_msg);
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SlugbotDriver>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}