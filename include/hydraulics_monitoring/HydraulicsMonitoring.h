/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/AgribotComponent.h>
#include <agribot_common/Can.h>
#include <queue>

struct SOilSensorsData
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
  char* _pchCanChannel;
  Can _CanHandler;
  SOilSensorsData SensorsData;
  uint8_t u8PressDataFlags;

public:
  explicit HydraulicsMonitoring(char* pchCanChannel);

  virtual int configure();
  virtual bool startup();
  virtual bool update();
  virtual bool stop();
  virtual bool cleanup();

  bool bGetOilData(SOilSensorsData* dataBufor);
};