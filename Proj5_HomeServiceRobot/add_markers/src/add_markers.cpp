#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "nav_msgs/Odometry.h"
#include "std_msgs/Bool.h"
#include <math.h>

float goal_x = 0.0;
float goal_y = 0.0;
bool goal = false;

void goal_callback(const std_msgs::Bool flag_msg)
{
  if (flag_msg.data == true)
    goal = true;
  else
    goal = false;
}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "add_markers");
  ros::NodeHandle n;
  ros::Rate r(1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

  //Due to poor map accuracy odometry is not accurate when delivering or picking up the object
  //I have used a flag published by the object pick up node to alert when the goal is reached
  //ros::Subscriber odometry_sub = n.subscribe("odom", 1000, odom_callback);
  ros::Subscriber goal_sub = n.subscribe("goal/flag", 1000, goal_callback);

  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;
  bool object = false;

  visualization_msgs::Marker marker;
  marker.header.frame_id = "/map";
  marker.header.stamp = ros::Time::now();

  // Set the namespace and id for this marker.  This serves to create a unique ID
  // Any marker sent with the same namespace and id will overwrite the old one
  marker.ns = "cube";
  marker.id = 0;

  // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
  marker.type = shape;
  marker.action = visualization_msgs::Marker::ADD;

  // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
  marker.pose.position.x = 3.36;
  marker.pose.position.y = 0.0;
  marker.pose.position.z = 0;
  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 1.0;

  // Set the scale of the marker -- 1x1x1 here means 1m on a side
  marker.scale.x = 0.2;
  marker.scale.y = 0.2;
  marker.scale.z = 0.2;

  // Set the color -- be sure to set alpha to something non-zero!
  marker.color.r = 0.0f;
  marker.color.g = 0.0f;
  marker.color.b = 1.0f;
  marker.color.a = 1.0;

  marker.lifetime = ros::Duration();

  // Publish the marker
  sleep(1);
  marker_pub.publish(marker);

  ROS_INFO("Object is ready to be picked up");

  goal_x = marker.pose.position.x;
  goal_y = marker.pose.position.y;

  while(ros::ok()){
    ros::spinOnce();
        if(goal){
            if(object){
                marker.action = visualization_msgs::Marker::ADD;
                marker_pub.publish(marker);
                ROS_INFO("Object has been delivered");
                break;
            }
            if(!object){
                marker.action = visualization_msgs::Marker::DELETE;
                marker_pub.publish(marker);
                ROS_INFO("Object has been picked up");
                goal_x = marker.pose.position.x;
                goal_y = marker.pose.position.y;

                //wait for secong goal to be sent and set new marker location
                sleep(2);
                marker.pose.position.x = 4.00;
                marker.pose.position.y = 4.00;
                goal = false;
                object = true;
                ROS_INFO("Ready to deliver object to 2nd location");
            }
        }
    }
  sleep(5);
  return 0;
}
