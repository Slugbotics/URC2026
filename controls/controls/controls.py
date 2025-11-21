import rclpy
from rclpy.node import Node
from messages.msg import ControllerInput
import pygame

class ControlsNode(Node):
    def __init__(self):
        super().__init__('controls_node')
        self.publisher = self.create_publisher(ControllerInput, 'controller_input', 10)
        timer_period = 0.02  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        pygame.init()
        pygame.joystick.init()
        self.joystick = None
        self.connect_joystick()
        if self.joystick is None:
            self.get_logger().warning('No joystick connected at startup.')

    def timer_callback(self):
        self.connect_joystick()
        msg = ControllerInput()
        if self.joystick is not None:
            pygame.event.pump()
            msg.left_x = self.joystick.get_axis(0)
            msg.left_y = self.joystick.get_axis(1)
            msg.right_x = self.joystick.get_axis(3)
            msg.right_y = self.joystick.get_axis(4)
            msg.left_trigger = self.joystick.get_axis(2)
            msg.right_trigger = self.joystick.get_axis(5)
            msg.a = bool(self.joystick.get_button(0))
            msg.b = bool(self.joystick.get_button(1))
            msg.x = bool(self.joystick.get_button(2))
            msg.y = bool(self.joystick.get_button(3))
            msg.left_bumper = bool(self.joystick.get_button(4))
            msg.right_bumper = bool(self.joystick.get_button(5))
            msg.back = bool(self.joystick.get_button(6))
            msg.start = bool(self.joystick.get_button(7))
            msg.left_stick_pressed = bool(self.joystick.get_button(8))
            msg.right_stick_pressed = bool(self.joystick.get_button(9))
            msg.guide = bool(self.joystick.get_button(10))
            hat = self.joystick.get_hat(0)
            msg.dpad_up = hat[1] == 1
            msg.dpad_down = hat[1] == -1
            msg.dpad_left = hat[0] == -1
            msg.dpad_right = hat[0] == 1

        self.publisher.publish(msg)
    
    def connect_joystick(self):
        if pygame.joystick.get_count() > 0 and self.joystick is None:
            self.joystick = pygame.joystick.Joystick(0)
            self.joystick.init()
            self.get_logger().info(f'Joystick connected: {self.joystick.get_name()}')
        elif pygame.joystick.get_count() == 0 and self.joystick is not None:
            self.get_logger().info('Joystick disconnected')
            self.joystick = None

def main(args=None):
    rclpy.init(args=args)
    node = ControlsNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print()
    finally:
        node.destroy_node()
        # Only call shutdown if the rclpy context is still OK (avoids double-shutdown)
        if rclpy.ok():
            rclpy.shutdown()
        pygame.quit()

if __name__ == '__main__':
    main()