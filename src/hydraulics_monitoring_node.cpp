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
  ros::Rate LoopRate(10);
  ros::Publisher HydraulicsPub = nh.advertise<agribot_msgs::AgribotHydraulics>("agribot/hydraulics", 1);
  agribot_msgs::AgribotHydraulics Msg;

  if (argc < 3)
  {
    ROS_INFO("[Hydraulics Monitoring] Specify can channel "
             "[can0 or can1] and proper ID.");
    return -1;
  }

  HydraulicsMonitoring Hydraulics(argv[1]);

  OilSensorsData sTempPressureData;

  while (ros::ok())
  {
    if (Hydraulics.update())
    {
      if (Hydraulics.bGetOilData(&sTempPressureData))
      {
        Msg.header.frame_id = "hydraulics_monitor_link";
        Msg.header.stamp = ros::Time::now();
        Msg.oil_pressure_track_FL = sTempPressureData.fPressureValueFL;
        Msg.oil_pressure_track_FR = sTempPressureData.fPressureValueFR;
        Msg.oil_pressure_track_BL = sTempPressureData.fPressureValueBL;
        Msg.oil_pressure_track_BR = sTempPressureData.fPressureValueBR;
        Msg.oil_pressure_manifold_F = sTempPressureData.fPressureValueManifoldF;
        Msg.oil_pressure_manifold_B = sTempPressureData.fPressureValueManifoldB;
        Msg.oil_cooler_temperature = sTempPressureData.fTempOilCooler;
        Msg.oil_tank_temperature = sTempPressureData.fTempOilTank;
        HydraulicsPub.publish(Msg);
      }
    }
    ros::spinOnce();
    LoopRate.sleep();
  }
  return 0;
}
