/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <ros/ros.h>
#include <ros/console.h>
#include <hydraulics_monitoring/HydraulicsMonitoring.h>
#include <agribot_msgs/AgribotHydraulics.h>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "hydraulics_monitoring");
  ros::NodeHandle n;
  ros::Rate loop_rate(10);
  ros::Publisher hydraulics_pub =
      n.advertise<agribot_msgs::AgribotHydraulics>("agribot/hydraulics", 1);
  agribot_msgs::AgribotHydraulics msg;

  if (argc < 3)
  {
    ROS_INFO("[Hydraulics Monitoring] Specify can channel "
             "[can0 or can1] and proper ID.");
    return -1;
  }

  HydraulicsMonitoring Hydraulics(argv[1]);

  SOilSensorsData tempPressureData;
  float tempCoolerTemp = 0;
  float tempTankTemp = 0;
  while (ros::ok())
  {
    if (Hydraulics.update())
    {
      if (Hydraulics.bGetOilData(&tempPressureData))
      {
        msg.header.frame_id = "hydraulics_monitor_link";
        msg.header.stamp = ros::Time::now();
        msg.oil_pressure_track_FL = tempPressureData.fPressureValueFL;
        msg.oil_pressure_track_FR = tempPressureData.fPressureValueFR;
        msg.oil_pressure_track_BL = tempPressureData.fPressureValueBL;
        msg.oil_pressure_track_BR = tempPressureData.fPressureValueBR;
        msg.oil_pressure_manifold_F = tempPressureData.fPressureValueManifoldF;
        msg.oil_pressure_manifold_B = tempPressureData.fPressureValueManifoldB;
        msg.oil_cooler_temperature = tempPressureData.fTempOilCooler;
        msg.oil_tank_temperature = tempPressureData.fTempOilTank;
        hydraulics_pub.publish(msg);
      }
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
