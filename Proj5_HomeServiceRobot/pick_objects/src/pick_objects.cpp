#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include "std_msgs/Bool.h"
#include <actionlib/client/simple_action_client.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "pick_objects");
  ros::NodeHandle n;
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  ros::Publisher goal_pub = n.advertise<std_msgs::Bool>("goal/flag", 1);
  std_msgs::Bool goal_flag;

  // Wait 5 sec for move_base action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  // set up the frame parameters
  goal.target_pose.header.frame_id = "/map";
  goal.target_pose.header.stamp = ros::Time::now();

  // Define a position and orientation for the robot to reach
  goal.target_pose.pose.position.x = 3.36;
  goal.target_pose.pose.position.y = 0.0;
  goal.target_pose.pose.orientation.w = 1.0;

   // Send the goal position and orientation for the robot to reach
  ROS_INFO("Sending 1st goal");
  goal_flag.data = false;
  goal_pub.publish(goal_flag);
  ac.sendGoal(goal);

  // Wait an infinite time for the results
  ac.waitForResult();

  // Check if the robot reached its goal
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
    ROS_INFO("Reached 1st goal");
    goal_flag.data = true;
    goal_pub.publish(goal_flag);
  }
  else ROS_INFO("The base failed to move to the 1st goal for some reason");

  ros::Duration(5).sleep();

  // Define a position and orientation for the robot to reach
  goal.target_pose.pose.position.x = 4.00;
  goal.target_pose.pose.position.y = 4.00;
  goal.target_pose.pose.orientation.w = 1.0;

   // Send the goal position and orientation for the robot to reach
  ROS_INFO("Sending 2nd goal");
  ac.sendGoal(goal);
  goal_flag.data = false;
  goal_pub.publish(goal_flag);

  // Wait an infinite time for the results
  ac.waitForResult();

  // Check if the robot reached its goal
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
    ROS_INFO("Reached the 2nd goal");
    goal_flag.data = true;
    goal_pub.publish(goal_flag);
  }
  else ROS_INFO("The base failed to moveto the 2nd goal for some reason");

  sleep(5);
  return 0;
}
