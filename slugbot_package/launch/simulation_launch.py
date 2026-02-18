import os
import shutil
import launch
from launch_ros.actions import Node
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from ament_index_python.packages import get_package_share_directory, get_package_prefix
from webots_ros2_driver.webots_launcher import WebotsLauncher
from webots_ros2_driver.webots_controller import WebotsController


def generate_launch_description():
    package_dir = get_package_share_directory('slugbot_package')
    robot_description_path = os.path.join(package_dir, 'resource', 'slugbot.urdf')

    world_path = os.path.join(package_dir, 'worlds', 'my_world.wbt')

    # Detect WSL environment. webots_ros2_driver assumes Windows Webots in WSL,
    # which fails if Linux Webots (e.g., snap) is installed inside WSL.
    is_wsl = 'microsoft-standard' in os.uname().release
    linux_webots = shutil.which('webots')  # e.g. /snap/bin/webots

    if is_wsl and linux_webots:
        # Launch Linux Webots directly to avoid WSL-specific path to webots.exe.
        webots = ExecuteProcess(
            output='screen',
            cmd=[
                linux_webots,
                '--port=1234',
                world_path,
                '--batch',
                ['--mode=', 'realtime'],
            ],
            name='webots'
        )

        # In this case the controller should talk locally (IPC) rather than TCP to a Windows host.
        controller_script = os.path.join(
            get_package_share_directory('webots_ros2_driver'), 'scripts', 'webots-controller'
        )
        slugbot_driver = ExecuteProcess(
            output='screen',
            cmd=[
                controller_script,
                '--robot-name=slugbot',
                '--protocol=ipc',
                '--port=1234',
                'ros2',
                '--ros-args',
                '-p', f'robot_description:={robot_description_path}',
            ],
            name='webots_controller_slugbot',
            additional_env={'WEBOTS_HOME': get_package_prefix('webots_ros2_driver')}
        )
    else:
        # Default behavior: use WebotsLauncher which resolves Webots path on native Linux / Windows setups
        webots = WebotsLauncher(
            world=world_path
        )

        slugbot_driver = WebotsController(
            robot_name='slugbot',
            parameters=[
                {'robot_description': robot_description_path},
            ]
        )

    slugbot_node = Node(
        package='slugbot_package',
        executable='slugbot_driver_node',
    )

    return LaunchDescription([
        webots,
        slugbot_driver,
        slugbot_node,
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=webots,
                on_exit=[launch.actions.EmitEvent(event=launch.events.Shutdown())],
            )
        )
    ])