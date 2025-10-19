from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

def generate_launch_description():
    return LaunchDescription([
        ExecuteProcess(
            cmd=['webots', '--world', '/root/slugbotics_ws/src/2025-2026-URC/slugbot_webots/worlds/slugbot_world.wbt'],
            output='screen'
        ),
        Node(
            package='slugbot_webots',
            executable='example_controller',
            output='screen'
        ),
    ])
