from launch import LaunchDescription
from launch.actions import EmitEvent, RegisterEventHandler, TimerAction
from launch.event_handlers import OnProcessStart
from launch_ros.actions import LifecycleNode
from launch_ros.events.lifecycle import ChangeState
from lifecycle_msgs.msg import Transition
import launch.events

def generate_launch_description():
    # Launch the lifecycle node with configurable parameters
    obstacle_detector_node = LifecycleNode(
        package='obstacle_detector',
        executable='obstacle_detector_exec',
        name='obstacle_detector',
        namespace='',
        parameters=[{
            'obstacle_distance_threshold': 0.5
        }],
        output='screen'
    )

    # Auto-configure after node starts
    configure_event = EmitEvent(
        event=ChangeState(
            lifecycle_node_matcher=launch.events.matches_action(obstacle_detector_node),
            transition_id=Transition.TRANSITION_CONFIGURE,
        )
    )

    # Auto-activate 1 second after configure
    activate_event = TimerAction(
        period=1.0,
        actions=[
            EmitEvent(
                event=ChangeState(
                    lifecycle_node_matcher=launch.events.matches_action(obstacle_detector_node),
                    transition_id=Transition.TRANSITION_ACTIVATE,
                )
            )
        ]
    )

    # Trigger configure when node process starts
    on_start_handler = RegisterEventHandler(
        OnProcessStart(
            target_action=obstacle_detector_node,
            on_start=[configure_event, activate_event]
        )
    )

    return LaunchDescription([
        obstacle_detector_node,
        on_start_handler,
    ])