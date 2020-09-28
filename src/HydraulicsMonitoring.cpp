/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <hydraulics_monitoring/HydraulicsMonitoring.h>

constexpr uint uiHydraulicsPressureData = 0xFFD964;
constexpr uint uiHydraulicsTemperatureData = 0xFFDA64;

constexpr float fPressureConstantDivider = 100.0;
constexpr int iTemperatureOffset = 100;
constexpr float fTemperatureConstantDivider = 10.0;

constexpr uint8_t u8FirstSensorIDByte = 0;
constexpr uint8_t u8FirstSensorDataByte = 1;
constexpr uint8_t u8SecondSensorIDByte = 3;
constexpr uint8_t u8SecondSensorDataByte = 4;

constexpr uint8_t u8SensorFL = 0;
constexpr uint8_t u8SensorFR = 1;
constexpr uint8_t u8SensorFlagFLR = 0b11 << 0;
constexpr uint8_t u8SensorBL = 2;
constexpr uint8_t u8SensorBR = 3;
constexpr uint8_t u8SensorFlagBLR = 0b11 << 2;
constexpr uint8_t u8SensorMF = 4;
constexpr uint8_t u8SensorMB = 5;
constexpr uint8_t u8SensorFlagMFB = 0b11 << 4;
constexpr uint8_t u8CoolerTempFirstByte = 0;
constexpr uint8_t u8TankTempFirstByte = 2;
constexpr uint8_t u8SensorTempFlag = 0b1 << 6;

constexpr uint uiThreeBytesMask = 0xFFFFFF;

HydraulicsMonitoring::HydraulicsMonitoring(std::string NewCanChannelName)
{
  u8PressDataFlags = 0;
  bOilDataBufferInitialized = false;
  sCanChannel = NewCanChannelName;
  configure();
}

float HydraulicsMonitoring::fParsePressureData(can_frame* ptrFrame, uint8_t u8FirstDataByte)
{
  return (static_cast<float>(ptrFrame->data[u8FirstDataByte] + (ptrFrame->data[u8FirstDataByte + 1] << 8)) /
          fPressureConstantDivider);
}

float HydraulicsMonitoring::fParseTemperatureData(can_frame* ptrFrame, uint8_t u8FirstDataByte)
{
  return (static_cast<float>(ptrFrame->data[u8FirstDataByte] + (ptrFrame->data[u8FirstDataByte + 1] << 8)) /
          fTemperatureConstantDivider) -
         iTemperatureOffset;
}

int HydraulicsMonitoring::configure()
{
  return canHandler.iInitialize(sCanChannel.c_str());
}

bool HydraulicsMonitoring::startup()
{
  return false;
}

bool HydraulicsMonitoring::update()
{
  can_frame frame;
  while (canHandler.bReceiveFrame(&frame))
  {
    if ((frame.can_id & uiThreeBytesMask) == (uiHydraulicsPressureData & uiThreeBytesMask))
    {
      // first pair
      if (frame.data[u8FirstSensorIDByte] == u8SensorFL && frame.data[u8SecondSensorIDByte] == u8SensorFR)
      {
        sSensorsData.fPressureValueFL = fParsePressureData(&frame, u8FirstSensorDataByte);
        sSensorsData.fPressureValueFR = fParsePressureData(&frame, u8SecondSensorDataByte);
        u8PressDataFlags = u8PressDataFlags | u8SensorFlagFLR;
      }  // second pair
      else if (frame.data[u8FirstSensorIDByte] == u8SensorBL && frame.data[u8SecondSensorIDByte] == u8SensorBR)
      {
        sSensorsData.fPressureValueBL = fParsePressureData(&frame, u8FirstSensorDataByte);
        sSensorsData.fPressureValueBR = fParsePressureData(&frame, u8SecondSensorDataByte);
        u8PressDataFlags = u8PressDataFlags | u8SensorFlagBLR;
      }  // third pair
      else if (frame.data[u8FirstSensorIDByte] == u8SensorMF && frame.data[u8SecondSensorIDByte] == u8SensorMB)
      {
        sSensorsData.fPressureValueManifoldF = fParsePressureData(&frame, u8FirstSensorDataByte);
        sSensorsData.fPressureValueManifoldB = fParsePressureData(&frame, u8SecondSensorDataByte);
        u8PressDataFlags = u8PressDataFlags | u8SensorFlagMFB;
      }
    }  // temperature data
    else if ((frame.can_id & uiThreeBytesMask) == (uiHydraulicsTemperatureData & uiThreeBytesMask))
    {
      sSensorsData.fTempOilTank = fParseTemperatureData(&frame, u8CoolerTempFirstByte);
      sSensorsData.fTempOilCooler = fParseTemperatureData(&frame, u8TankTempFirstByte);
      u8PressDataFlags = u8PressDataFlags | u8SensorTempFlag;
    }
  }
  return true;
}

bool HydraulicsMonitoring::bGetOilData(OilSensorsData*& ptrDataBuffer)
{
  if (!bOilDataBufferInitialized)
  {
    ptrDataBuffer = &sSensorsData;
    bOilDataBufferInitialized = true;
  }
  if (static_cast<uint>(u8PressDataFlags) ==
      static_cast<uint>(u8SensorFlagFLR | u8SensorFlagBLR | u8SensorFlagMFB | u8SensorTempFlag))
  {
    u8PressDataFlags = 0;
    return true;
  }
  else
  {
    return false;
  }
}

bool HydraulicsMonitoring::stop()
{
  return true;
}

bool HydraulicsMonitoring::cleanup()
{
  return true;
}
