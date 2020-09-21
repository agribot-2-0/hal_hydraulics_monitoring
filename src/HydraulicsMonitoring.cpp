/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <hydraulics_monitoring/HydraulicsMonitoring.h>

#define HYDRAULICS_PRESSURE_DATA_ID 0xFFD964
#define HYDRAULICS_TEMPERATURE_DATA_ID 0xFFDA64

#define PRESSURE_CONSTANT_DIVIDER 100.0
#define TEMPERATURE_OFFSET 100
#define TEMPERATURE_CONSTANT_DIVIDER 10.0

#define FIRST_SENSOR_ID_BYTE 0
#define FIRST_SENSOR_DATA_BYTE 1
#define SECOND_SENSOR_ID_BYTE 3
#define SECOND_SENSOR_DATA_BYTE 4

#define SENSOR_FL 0
#define SENSOR_FR 1
#define SENSOR_FLR_FLAG 0b11 << 0
#define SENSOR_BL 2
#define SENSOR_BR 3
#define SENSOR_BLR_FLAG 0b11 << 2
#define SENSOR_MF 4
#define SENSOR_MB 5
#define SENSOR_MFB_FLAG 0b11 << 4

#define COOLER_TEMP_FIRST_BYTE 0
#define TANK_TEMP_FIRST_BYTE 2
#define SENSOR_TEMP_FLAG 0b1 << 6

#define THREE_BYTES_MASK 0xFFFFFF

HydraulicsMonitoring::HydraulicsMonitoring(char* pchCanChannel)
{
  u8PressDataFlags = 0;
  _pchCanChannel = pchCanChannel;
}

int HydraulicsMonitoring::configure()
{
  return _CanHandler.iInitialize(_pchCanChannel);
}

bool HydraulicsMonitoring::startup()
{
  return false;
}

bool HydraulicsMonitoring::update()
{
  can_frame frame;
  while (_CanHandler.bReceiveFrame(&frame))
  {
    if ((frame.can_id & THREE_BYTES_MASK) == (HYDRAULICS_PRESSURE_DATA_ID & THREE_BYTES_MASK))
    {
      // first pair
      if (frame.data[FIRST_SENSOR_ID_BYTE] == SENSOR_FL && frame.data[SECOND_SENSOR_ID_BYTE] == SENSOR_FR)
      {
        SensorsData.fPressureValueFL =
            (static_cast<float>(frame.data[FIRST_SENSOR_DATA_BYTE] + (frame.data[FIRST_SENSOR_DATA_BYTE + 1] << 8)) /
             PRESSURE_CONSTANT_DIVIDER);
        SensorsData.fPressureValueFR =
            (static_cast<float>(frame.data[SECOND_SENSOR_DATA_BYTE] + (frame.data[SECOND_SENSOR_DATA_BYTE + 1] << 8)) /
             PRESSURE_CONSTANT_DIVIDER);
        u8PressDataFlags = u8PressDataFlags | (SENSOR_FLR_FLAG);
      }  // second pair
      else if (frame.data[FIRST_SENSOR_ID_BYTE] == SENSOR_BL && frame.data[SECOND_SENSOR_ID_BYTE] == SENSOR_BR)
      {
        SensorsData.fPressureValueBL =
            (static_cast<float>(frame.data[FIRST_SENSOR_DATA_BYTE] + (frame.data[FIRST_SENSOR_DATA_BYTE + 1] << 8)) /
             PRESSURE_CONSTANT_DIVIDER);
        SensorsData.fPressureValueBR =
            (static_cast<float>(frame.data[SECOND_SENSOR_DATA_BYTE] + (frame.data[SECOND_SENSOR_DATA_BYTE + 1] << 8)) /
             PRESSURE_CONSTANT_DIVIDER);
        u8PressDataFlags = u8PressDataFlags | (SENSOR_BLR_FLAG);
      }  // third pair
      else if (frame.data[FIRST_SENSOR_ID_BYTE] == SENSOR_MF && frame.data[SECOND_SENSOR_ID_BYTE] == SENSOR_MB)
      {
        SensorsData.fPressureValueManifoldF =
            (static_cast<float>(frame.data[FIRST_SENSOR_DATA_BYTE] + (frame.data[FIRST_SENSOR_DATA_BYTE + 1] << 8)) /
             PRESSURE_CONSTANT_DIVIDER);
        SensorsData.fPressureValueManifoldB =
            (static_cast<float>(frame.data[SECOND_SENSOR_DATA_BYTE] + (frame.data[SECOND_SENSOR_DATA_BYTE + 1] << 8)) /
             PRESSURE_CONSTANT_DIVIDER);
        u8PressDataFlags = u8PressDataFlags | (SENSOR_MFB_FLAG);
      }
    }  // temperature data
    else if ((frame.can_id & THREE_BYTES_MASK) == (HYDRAULICS_TEMPERATURE_DATA_ID & THREE_BYTES_MASK))
    {
      SensorsData.fTempOilTank =
          (static_cast<float>(frame.data[COOLER_TEMP_FIRST_BYTE] + (frame.data[COOLER_TEMP_FIRST_BYTE + 1] << 8) -
                              TEMPERATURE_OFFSET) /
           TEMPERATURE_CONSTANT_DIVIDER);
      SensorsData.fTempOilCooler =
          (static_cast<float>(frame.data[TANK_TEMP_FIRST_BYTE] + (frame.data[TANK_TEMP_FIRST_BYTE + 1] << 8) -
                              TEMPERATURE_OFFSET) /
           TEMPERATURE_CONSTANT_DIVIDER);
      u8PressDataFlags = u8PressDataFlags | (SENSOR_TEMP_FLAG);
    }
  }
  return true;
}

bool HydraulicsMonitoring::bGetOilData(SOilSensorsData* dataBufor)
{
  if (u8PressDataFlags & (SENSOR_FLR_FLAG | SENSOR_BLR_FLAG | SENSOR_MFB_FLAG | SENSOR_TEMP_FLAG))
  {
    *dataBufor = SensorsData;
    u8PressDataFlags = 0;
    return true;
  }
  else
    return false;
}

bool HydraulicsMonitoring::stop()
{
  return true;
}

bool HydraulicsMonitoring::cleanup()
{
  return true;
}
