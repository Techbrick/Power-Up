#include "WPILib.h"

#ifndef SRC_PNEUMATICS_H_
#define SRC_PNEUMATICS_H_

class Pneumatics
{
public:
  Pneumatics(uint32_t InSoleChannel, uint32_t OutSoleChannel);
  void set(bool state);
  bool get();
private:
  Solenoid InSole;
  Solenoid OutSole;
};

#endif
