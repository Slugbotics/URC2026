import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    package_dir = get_package_share_directory('slugbot_webots')
    world_path = os.path.join(package_dir, 'worlds', 'slugbot_world.wbt')
    
    return LaunchDescription([
        ExecuteProcess(
            cmd=['webots', world_path],
            output='screen'
        ),
        Node(
            package='slugbot_webots',
            executable='example_controller',
            output='screen'
        ),
    ])
