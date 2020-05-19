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

    int white_pixel_location = 0;
    int white_pixel_sum = 0;
    int total_image_pixels = img.height*img.width;
    int col = 0;
    int row = 0;
    int pixel = 0;
    int mult = 0;

    // calculate border regions
    int image_section = img.width/3;
    int ball_avg_location = 0;

    // Loop through each pixel in the image
    for (row = 0; row < img.height; row++) {
        for (pixel = 0; pixel < img.step; pixel = pixel+3){
            col = pixel / 3;
            //need to increase mult to access pixel of next row
            mult = row*img.step;
            //scan if red, green and blue pixels are all 255
            if (img.data[pixel+mult] == 255 && img.data[pixel+1+mult] == 255 && img.data[pixel+2+mult] == 255){
                // sum of white pixels and their horizontal location
                white_pixel_location += col;
                white_pixel_sum ++;
            }
        }
    }

    //ROS_INFO("white pixel sum: %d", white_pixel_sum);

    if(white_pixel_sum == 0){
        ROS_INFO("The ball is not detected");
        drive_robot(0,0);
    }
    else{
        //if white pixels are more than 2/3 of the image we have reached the ball;
        if (white_pixel_sum < (2*total_image_pixels)/3){
            ROS_INFO("Chasing the ball");
            ball_avg_location = white_pixel_location / white_pixel_sum;

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
        else{
            ROS_INFO("Reached the ball");
            drive_robot(0,0);
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
