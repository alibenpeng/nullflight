#ifndef _FLIGHT_H
#define _FLIGHT_H
typedef struct {
  float x;
  float y;
  float z;
} Axis3f;          
void controllerInit(void);
void flightControl(void);
#endif
