from controller import Robot
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist

class SlugbotController(Node):
    def __init__(self):
        super().__init__('slugbot_controller')
        self.robot = Robot()
        self.time_step = int(self.robot.getBasicTimeStep())
        self.get_logger().info("Slugbot Webots controller started.")
        self.create_subscription(Twist, '/cmd_vel', self.cmd_callback, 10)

    def cmd_callback(self, msg):
        # For now, just log the command
        self.get_logger().info(f"Received velocity command: linear={msg.linear.x:.2f}, angular={msg.angular.z:.2f}")

    def spin_robot(self):
        while self.robot.step(self.time_step) != -1:
            rclpy.spin_once(self, timeout_sec=0.001)

def main():
    rclpy.init()
    node = SlugbotController()
    node.spin_robot()
    node.destroy_node()
    rclpy.shutdown()
