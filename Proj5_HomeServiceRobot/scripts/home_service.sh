#!/bin/sh
xterm -e "roslaunch home_service_robot world.launch" &

sleep 5

xterm -e "roslaunch home_service_robot amcl.launch" &

sleep 5

xterm -e "roslaunch home_service_robot navigation.launch" &

sleep 5

xterm -e "rosrun add_markers add_markers" &

sleep 5

xterm -e "rosrun pick_objects pick_objects"


