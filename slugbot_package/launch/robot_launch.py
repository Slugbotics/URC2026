import os
import shutil
import launch
from launch_ros.actions import Node
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from ament_index_python.packages import get_package_share_directory, get_package_prefix

def generate_launch_description():
    obstacle_avoider = Node(
        package='slugbot_package',
        executable='obstacle_avoider',
    )

    slugbot_node = Node(
        package='slugbot_package',
        executable='slugbot_driver_node',
    )

    drivetrain = Node(
        package='slugbot_package',
        executable='drivetrain_node',
    )

    return LaunchDescription([
        slugbot_node,
        obstacle_avoider,
        drivetrain
    ])