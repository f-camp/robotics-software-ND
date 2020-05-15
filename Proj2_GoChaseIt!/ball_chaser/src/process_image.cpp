#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{

    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the command_robot service and pass the requested motor commands
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    // count of white pixel in image regions [left, center, right]
    int white_pixel_location = 0;
    int white_pixel_sum = 0;
    int col = 0;
    int row = 0;
    int pixel = 0;

    // calculate border regions
    int image_section = img.width/3;
    int ball_avg_location = 0;

    // Loop through each pixel in the image
    for (row = 0; row < img.height; row++) {
        for (pixel = 0; pixel < img.step; pixel = pixel+3){
            //col = pixel / 3;
            //scan if red, green and blue pixels are all 255
            if (img.data[pixel] == 255 && img.data[pixel+1] == 255 && img.data[pixel+2] == 255){
                // sum of white pixels and their horizontal location
                white_pixel_location += pixel / 3;
                white_pixel_sum ++;
            }
        }
    }

    if(white_pixel_sum = 0){
        drive_robot(0,0);
    }
    else{
        ball_avg_location = 0; // white_pixel_location / white_pixel_sum;

        if (ball_avg_location < image_section){
            drive_robot(0.2, 0.5);
        }
        else if (ball_avg_location > image_section && ball_avg_location < 2*image_section){
            drive_robot(0.2, 0.0);
        }
        else{
            drive_robot(0.2, -0.5);
        }
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
