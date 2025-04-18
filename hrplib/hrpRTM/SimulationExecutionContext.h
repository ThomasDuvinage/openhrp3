// -*- C++ -*-
#ifndef SimulationExecutionContext_h
#define SimulationExecutionContext_h

#include <rtm/OpenHRPExecutionContext.h>
#include "hrpCorba/ClockGenerator.hh"

namespace RTC
{
  class CorbaNaming;
  class SimulationExecutionContext : public virtual OpenHRPExecutionContext
  {
  public:
    virtual ReturnCode_t start();
    virtual ReturnCode_t stop();
    static OpenHRP::ClockGenerator_var m_cg;
  };
};

extern "C"
{
  DLL_EXPORT void SimulationECInit(RTC::Manager* manager);
};

#endif
