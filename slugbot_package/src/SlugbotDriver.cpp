#include "slugbot_package/SlugbotDriver.hpp"

#include "rclcpp/rclcpp.hpp"
#include <cmath>
#include "std_msgs/msg/string.hpp"
#include "messages/msg/controller_input.hpp"
#include "messages/msg/wheel_states.hpp"
#include "../../utils/math/Translation2d.hpp"
#include "../../utils/math/Rotation2d.hpp"

const double TRACK_WIDTH = 0.5; // Meters
const double TRACK_LENGTH = 0.6; // Meters
const double WHEEL_RADIUS = 0.06; // Meters
const double MAX_WHEEL_SPEED = 25.0; // Radians per second
const double MAX_LINEAR_SPEED = (MAX_WHEEL_SPEED * WHEEL_RADIUS); // Meters per second
const double MAX_ROTATIONAL_SPEED = (MAX_LINEAR_SPEED / ((std::hypot(TRACK_WIDTH, TRACK_LENGTH) / 2))); // Radians per second

SlugbotDriver::SlugbotDriver()
    : Node("slugbot_driver") {
  // Swerve drive
  wheel_states_publisher = this->create_publisher<messages::msg::WheelStates>("/wheel_states", 10);

  current_wheel_states = messages::msg::WheelStates();
  
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
  // Forward, left, and CCW are positive
  Translation2d speeds = Translation2d();
  double rotation = 0.0;

  if (recieved_input) {
    double lx = controller_input.left_x;
    double ly = controller_input.left_y;
    double rx = controller_input.right_x;
    speeds = Translation2d(-ly * std::hypot(lx, ly) * MAX_LINEAR_SPEED,
                           -lx * std::hypot(lx, ly) * MAX_LINEAR_SPEED);
    rotation = -rx * std::abs(rx) * MAX_ROTATIONAL_SPEED;
  } else {
    // Percent speed, -1 to 1
    double x = 0, y = 0;
    if (keys_pressed.find('w') != std::string::npos){
      x = 1;
    }
    if (keys_pressed.find('s') != std::string::npos){
      x -= 1;
    }
    if (keys_pressed.find('a') != std::string::npos){
      y = 1;
    }
    if (keys_pressed.find('d') != std::string::npos){
      y -= 1;
    }
    if (keys_pressed.find('q') != std::string::npos){
      rotation = MAX_ROTATIONAL_SPEED / 2;
    }
    if (keys_pressed.find('e') != std::string::npos){
      rotation -= MAX_ROTATIONAL_SPEED / 2;
    }
    speeds = Translation2d(x, y).normalized() * MAX_LINEAR_SPEED;
  }

  Translation2d wheels[4] = {
    Translation2d(speeds.getX() - rotation * TRACK_WIDTH/2, speeds.getY() + rotation * TRACK_LENGTH/2) / WHEEL_RADIUS,  // Front Left
    Translation2d(speeds.getX() + rotation * TRACK_WIDTH/2, speeds.getY() + rotation * TRACK_LENGTH/2) / WHEEL_RADIUS,  // Front Right
    Translation2d(speeds.getX() - rotation * TRACK_WIDTH/2, speeds.getY() - rotation * TRACK_LENGTH/2) / WHEEL_RADIUS,  // Back Left
    Translation2d(speeds.getX() + rotation * TRACK_WIDTH/2, speeds.getY() - rotation * TRACK_LENGTH/2) / WHEEL_RADIUS   // Back Right
  };
  double max_wheel_speed = 0.0;
  for (int i = 0; i < 4; i++) {
    double wheel_speed = wheels[i].norm();
    if (wheel_speed > max_wheel_speed) {
      max_wheel_speed = wheel_speed;
    }
  }
  if (max_wheel_speed > MAX_WHEEL_SPEED) {
    for (int i = 0; i < 4; i++) {
      wheels[i] = wheels[i] * (MAX_WHEEL_SPEED / max_wheel_speed);
    }
  }

  auto wheel_msg = messages::msg::WheelStates();
  for(int i = 0; i < 4; i++) {
    wheel_msg.speeds[i] = wheels[i].norm();
    wheel_msg.angles[i] = wheels[i].getAngle();
  }
  optimize_wheel_states(wheel_msg);
  wheel_states_publisher->publish(wheel_msg);
}

void SlugbotDriver::optimize_wheel_states(messages::msg::WheelStates& wheels) {
  for(int i = 0; i < 4; i++){
    double a = Rotation2d(wheels.angles[i] - current_wheel_states.angles[i]).modulus().getRadians();
    if (std::abs(a) > M_PI / 2){
      wheels.angles[i] = Rotation2d(wheels.angles[i] + M_PI).modulus().getRadians();
      wheels.speeds[i] = -wheels.speeds[i];
    }
  }
  current_wheel_states = wheels;
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SlugbotDriver>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}