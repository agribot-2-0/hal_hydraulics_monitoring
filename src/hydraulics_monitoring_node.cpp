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
  ros::NodeHandle nh;
  ros::Rate loopRate(10);
  ros::Publisher hydraulicsPub = nh.advertise<agribot_msgs::AgribotHydraulics>("agribot/hydraulics", 1);
  agribot_msgs::AgribotHydraulics msg;

  if (argc < 3)
  {
    ROS_INFO("[Hydraulics Monitoring] Specify can channel "
             "[can0 or can1] and proper ID.");
    return -1;
  }

  HydraulicsMonitoring hydraulics(argv[1]);

  OilSensorsData sTempPressureData;

  while (ros::ok())
  {
    if (hydraulics.update())
    {
      if (hydraulics.bGetOilData(&sTempPressureData))
      {
        msg.header.frame_id = "hydraulics_monitor_link";
        msg.header.stamp = ros::Time::now();
        msg.oil_pressure_track_FL = sTempPressureData.fPressureValueFL;
        msg.oil_pressure_track_FR = sTempPressureData.fPressureValueFR;
        msg.oil_pressure_track_BL = sTempPressureData.fPressureValueBL;
        msg.oil_pressure_track_BR = sTempPressureData.fPressureValueBR;
        msg.oil_pressure_manifold_F = sTempPressureData.fPressureValueManifoldF;
        msg.oil_pressure_manifold_B = sTempPressureData.fPressureValueManifoldB;
        msg.oil_cooler_temperature = sTempPressureData.fTempOilCooler;
        msg.oil_tank_temperature = sTempPressureData.fTempOilTank;
        hydraulicsPub.publish(msg);
      }
    }
    ros::spinOnce();
    loopRate.sleep();
  }
  return 0;
}
