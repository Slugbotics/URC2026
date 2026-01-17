#include "slugbot_package/Drivetrain.hpp"

#include "rclcpp/rclcpp.hpp"
#include <string>
#include <cctype>
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"

Drivetrain::Drivetrain()
    : Node("drivetrain") {

  // TODO: Create motors and sensors

  // subscribe to wheel/turn commands published by the logic node
  front_left_wheel_subscription = this->create_subscription<std_msgs::msg::Float64>(
      "/front_left_wheel", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->front_left_wheel_speed_msg = msg->data;
      }
  );
  front_right_wheel_subscription = this->create_subscription<std_msgs::msg::Float64>(
      "/front_right_wheel", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->front_right_wheel_speed_msg = msg->data;
      }
  );

  back_left_wheel_subscription = this->create_subscription<std_msgs::msg::Float64>(
      "/back_left_wheel", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->back_left_wheel_speed_msg = msg->data;
      }
  );
  back_right_wheel_subscription = this->create_subscription<std_msgs::msg::Float64>(
      "/back_right_wheel", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->back_right_wheel_speed_msg = msg->data;
      }
  );

  left_turn_angle_subscription = this->create_subscription<std_msgs::msg::Float64>(
      "/left_turn_angle", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->left_turn_angle_msg = msg->data;
      }
  );
  right_turn_angle_subscription = this->create_subscription<std_msgs::msg::Float64>(
      "/right_turn_angle", rclcpp::SensorDataQoS().reliable(),
      [this](const std_msgs::msg::Float64::SharedPtr msg){
        this->right_turn_angle_msg = msg->data;
      }
  );

  timer = this->create_wall_timer(std::chrono::milliseconds(20),
    std::bind(&Drivetrain::update, this));
}

void Drivetrain::update() {
  // Set motor speeds
  // TODO: update real motors
}

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto drivetrain = std::make_shared<Drivetrain>();
  rclcpp::spin(drivetrain);
  rclcpp::shutdown();
  return 0;
}