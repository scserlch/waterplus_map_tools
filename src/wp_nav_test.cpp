#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include <visualization_msgs/Marker.h>
#include <waterplus_map_tools/Waypoint.h>
#include <waterplus_map_tools/GetNumOfWaypoints.h>
#include <waterplus_map_tools/GetWaypointByIndex.h>
#include <waterplus_map_tools/GetWaypointByName.h>
#include <string>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "wp_nav_test");

    ros::NodeHandle nh;
    ros::ServiceClient cliGetNum = nh.serviceClient<waterplus_map_tools::GetNumOfWaypoints>("/waterplus/get_num_waypoint");
    ros::ServiceClient cliGetWPIndex = nh.serviceClient<waterplus_map_tools::GetWaypointByIndex>("/waterplus/get_waypoint_index");
    ros::ServiceClient cliGetWPName = nh.serviceClient<waterplus_map_tools::GetWaypointByName>("/waterplus/get_waypoint_name");
    ///////////////
    //waterplus_map_tools::GetNumOfWaypoints srvN;
    // if (cliGetNum.call(srvN))
    // {
    //     ROS_INFO("Num_wp = %d", (int)srvN.response.num);
    // }
    // else
    // {
    //     ROS_ERROR("Failed to call service get_num_waypoints");
    // }
    // for(int i=0;i<srvN.response.num;i++)
    // {
    //      waterplus_map_tools::GetWaypointByIndex srvI;
    //      srvI.request.index = i;
    //     if (cliGetWPIndex.call(srvI))
    //     {
    //         std::string name = srvI.response.name;
    //         float x = srvI.response.pose.position.x;
    //         float y = srvI.response.pose.position.y;
    //         ROS_INFO("Get_wp_index: name = %s (%.2f,%.2f)", name.c_str(),x,y);
    //     }
    //     else
    //     {
    //         ROS_ERROR("Failed to call service get_wp_index");
    //     }
    // }
    waterplus_map_tools::GetWaypointByName srvN;
    for(int i=0;i<10;i++)
    {
        std::ostringstream stringStream;
        stringStream << i;
        std::string wp_index = stringStream.str();
        srvN.request.name = wp_index;
        if (cliGetWPName.call(srvN))
        {
            std::string name = srvN.response.name;
            float x = srvN.response.pose.position.x;
            float y = srvN.response.pose.position.y;
            ROS_INFO("Get_wp_name: name = %s (%.2f,%.2f)", wp_index.c_str(),x,y);
        }
        else
        {
            ROS_ERROR("Failed to call service get_waypoint_name");
        }
    }
    ////////////////

    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);

    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0)))
    {
        if(!ros::ok())
            break;
        ROS_INFO("Waiting for the move_base action server to come up");
    }

    int nWPIndex = 0;
    int nNumOfWaypoints = 0;
    move_base_msgs::MoveBaseGoal goal;
    

    while(ros::ok())
    {
        waterplus_map_tools::GetNumOfWaypoints srvN;
        if (cliGetNum.call(srvN))
        {
            ROS_INFO("Num_wp = %ld", (long int)srvN.response.num);
        }
        else
        {
            ROS_ERROR("Failed to call service get_num_waypoint");
        }

        if(nWPIndex >= nNumOfWaypoints)
        {
            nWPIndex = 0;
            continue;
        }

        ROS_INFO("Go to the WayPoint[%d]",nWPIndex);
        ac.sendGoal(goal);

        ac.waitForResult();

        if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("Arrived at WayPoint[%d] !",nWPIndex);
            nWPIndex ++;
        }
        else
            ROS_INFO("Failed to get to WayPoint[%d] ...",nWPIndex );
    }

    return 0;
}