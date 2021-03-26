/*
 * Copyright (C) 2020-2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/AgribotComponent.h>
#include <agribot_common/Can.h>

#include <queue>
#include <string>

struct OilSensorsData
{
  float fPressureValueFL;
  float fPressureValueFR;
  float fPressureValueBL;
  float fPressureValueBR;
  float fPressureValueManifoldF;
  float fPressureValueManifoldB;
  float fTempOilTank;
  float fTempOilCooler;
};

class HydraulicsMonitoring : public AgribotComponent
{
private:
  std::string sCanChannel;
  Can canHandler;
  OilSensorsData sSensorsData;
  uint8_t u8PressDataFlags;
  bool bOilDataBufferInitialized;

  static constexpr float tempereture_oil_tank_offset{ 90 };
  static constexpr float tempereture_oil_cooler_offset{ 90 };

public:
  explicit HydraulicsMonitoring(std::string NewCanChannelName);
  virtual int configure();
  virtual bool startup();
  virtual bool update();
  virtual bool stop();
  virtual bool cleanup();

  float fParsePressureData(can_frame* ptrFrame, uint8_t u8FirstDataByte);
  float fParseTemperatureData(can_frame* ptrFrame, uint8_t u8FirstDataByte);
  bool bGetOilData(OilSensorsData** dataBufor);
};
