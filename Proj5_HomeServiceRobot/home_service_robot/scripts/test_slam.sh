#!/bin/sh

xterm -e "roslaunch home_service_robot world.launch" &

sleep 5

xterm -e "roslaunch turtlebot_gazebo gmapping_demo.launch" &

sleep 5

xterm -e "roslaunch turtlebot_rviz_launchers view_navigation.launch" &

sleep 5

xterm -e "roslaunch turtlebot_teleop keyboard_teleop.launch"
