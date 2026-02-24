#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
import serial
import time
from std_msgs.msg import Float64
from serial.tools import list_ports
import glob
import os

# This must match the value in SlugbotDriver.cpp
MAX_WHEEL_SPEED = 25.0

class SerialNode(Node):
    def __init__(self):
        super().__init__('serial_node')
        self.left_subscriber = self.create_subscription(
            Float64, "/left_wheel", self.left_callback, 10)
        self.right_subscriber = self.create_subscription(
            Float64, "/right_wheel", self.right_callback, 10)
        self.arm_pivot_subscriber = self.create_subscription(
            Float64, "/arm_pivot", self.arm_pivot_callback, 10)
        self.arm_joint1_subscriber = self.create_subscription(
            Float64, "/arm_joint1", self.arm_joint1_callback, 10)
        self.arm_joint2_subscriber = self.create_subscription(
            Float64, "/arm_joint2", self.arm_joint2_callback, 10)
        self.wrist_yaw_subscriber = self.create_subscription(
            Float64, "/wrist_yaw", self.wrist_yaw_callback, 10)
        self.wrist_pitch_subscriber = self.create_subscription(
            Float64, "/wrist_pitch", self.wrist_pitch_callback, 10)
        self.wrist_roll_subscriber = self.create_subscription(
            Float64, "/wrist_roll", self.wrist_roll_callback, 10)
        self.gripper_subscriber = self.create_subscription(
            Float64, "/gripper", self.gripper_callback, 10)
        self.left_wheel = 500
        self.right_wheel = 500
        self.arm_pivot = 500
        self.arm_joint1 = 500
        self.arm_joint2 = 500
        self.wrist_yaw = 500
        self.wrist_pitch = 500
        self.wrist_roll = 500
        self.gripper = 500

        self.ser = None
        self.connect_serial()
        self.create_timer(0.02, self.timer_callback)

    def find_arduino_port(self):
        try:
            ids = glob.glob('/dev/serial/by-id/*arduino*') + glob.glob('/dev/serial/by-id/*Arduino*') + glob.glob('/dev/serial/by-id/*ARDUINO*')
            if ids:
                return os.path.realpath(ids[0])
        except Exception:
            pass

        for p in list_ports.comports():
            desc = (p.description or '') + ' ' + (p.manufacturer or '')
            if 'arduino' in desc.lower():
                return p.device

        for pattern in ('/dev/ttyACM*', '/dev/ttyUSB*'):
            matches = glob.glob(pattern)
            if matches:
                return matches[0]

        return None

    def connect_serial(self):
        if self.ser is None or not self.ser.is_open:
            try:
                port = self.find_arduino_port()
                if port is None:
                    self.get_logger().error("Arduino not found on any serial port")
                    return
                self.ser = serial.Serial(port, 115200, timeout=1)
                time.sleep(2)
                self.get_logger().info("Serial connection established on port: " + port)
            except serial.SerialException as e:
                self.get_logger().error(f"Failed to connect to serial port: {e}")
                self.ser = None

    def timer_callback(self):
        self.connect_serial()
        if self.ser is None:
            time.sleep(1)
            return
        msg = f"-1 {self.left_wheel} {self.left_wheel} {self.right_wheel} {self.right_wheel} {self.arm_pivot} {self.wrist_yaw} {self.wrist_pitch} {self.wrist_roll} {self.arm_joint1} {self.arm_joint2} {self.gripper}\n"
        try:
            self.ser.write(msg.encode())
        except Exception as e:
            self.get_logger().error(f"Failed to write to serial: {e}")
            self.close_serial()

    def close_serial(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.ser = None

    def destroy_node(self):
        self.close_serial()
        super().destroy_node()

    def left_callback(self, msg):
        self.left_wheel = round(msg.data / MAX_WHEEL_SPEED * 500 + 500)
    def right_callback(self, msg):
        self.right_wheel = round(msg.data / MAX_WHEEL_SPEED * 500 + 500)
    def arm_pivot_callback(self, msg):
        self.arm_pivot = round(msg.data * 500 + 500)
    def arm_joint1_callback(self, msg):
        self.arm_joint1 = round(msg.data * 500 + 500)
    def arm_joint2_callback(self, msg):
        self.arm_joint2 = round(msg.data * 500 + 500)
    def wrist_yaw_callback(self, msg):
        self.wrist_yaw = round(msg.data * 500 + 500)
    def wrist_pitch_callback(self, msg):
        self.wrist_pitch = round(msg.data * 500 + 500)
    def wrist_roll_callback(self, msg):
        self.wrist_roll = round(msg.data * 500 + 500)
    def gripper_callback(self, msg):
        self.gripper = round(msg.data * 500 + 500)

def main(args=None):
    rclpy.init(args=args)
    node = SerialNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print()
    node.destroy_node()
    if rclpy.ok():
        rclpy.shutdown()


if __name__ == '__main__':
    main()
