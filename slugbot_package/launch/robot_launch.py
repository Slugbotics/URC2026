import os
import shutil
import launch
from launch_ros.actions import Node
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from ament_index_python.packages import get_package_share_directory, get_package_prefix

def generate_launch_description():
    slugbot_node = Node(
        package='slugbot_package',
        executable='slugbot_driver_node',
    )

    serial = Node(
        package='slugbot_package',
        executable='SerialNode.py',
    )

    return LaunchDescription([
        slugbot_node,
        serial
    ])