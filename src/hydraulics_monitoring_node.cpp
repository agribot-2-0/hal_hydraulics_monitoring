/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <ros/ros.h>
#include <ros/console.h>
#include <hydraulics_monitoring/HydraulicsMonitoring.h>
#include <agribot_msgs/AgribotHydraulics.h>
#include <string>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "hydraulics_monitoring");
  ros::NodeHandle nh;
  ros::Rate loopRate(10);
  ros::Publisher hydraulicsPub = nh.advertise<agribot_msgs::AgribotHydraulics>("agribot/hydraulics", 1);
  agribot_msgs::AgribotHydraulics msg;

  if (argc < 2)
  {
    ROS_INFO("[Hydraulics Monitoring] Specify can channel [can0 or can1]");
    return -1;
  }

  std::string sCanChannel = std::string(argv[1]);
  HydraulicsMonitoring hydraulics(sCanChannel);

  OilSensorsData* ptrTempPressureData;

  while (ros::ok())
  {
    if (hydraulics.update())
    {
      if (hydraulics.bGetOilData(&ptrTempPressureData))
      {
        msg.header.frame_id = "hydraulics_monitor_link";
        msg.header.stamp = ros::Time::now();
        msg.oil_pressure_track_FL = ptrTempPressureData->fPressureValueFL;
        msg.oil_pressure_track_FR = ptrTempPressureData->fPressureValueFR;
        msg.oil_pressure_track_BL = ptrTempPressureData->fPressureValueBL;
        msg.oil_pressure_track_BR = ptrTempPressureData->fPressureValueBR;
        msg.oil_pressure_manifold_F = ptrTempPressureData->fPressureValueManifoldF;
        msg.oil_pressure_manifold_B = ptrTempPressureData->fPressureValueManifoldB;
        msg.oil_cooler_temperature = ptrTempPressureData->fTempOilCooler;
        msg.oil_tank_temperature = ptrTempPressureData->fTempOilTank;
        hydraulicsPub.publish(msg);
      }
    }
    ros::spinOnce();
    loopRate.sleep();
  }
  return 0;
}
