#include "common.h"
#include "atan.h"

#define use_atan_360

#if defined use_atan_45
   // Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
   // Converts any XY values including 0 to a degree value that should be
   // within +/- 1 degree of the accurate value without needing
   // large slow trig functions like ArcTan() or ArcCos().
   // NOTE! X value must be greater than zero!
   // This is the simplest version, for one octant (half a quadrant)
   // so X must be >= Y, although any values of X and Y are usable
   // provided they are under 1456 so the 16bit multiply does not overflow.
   // the result will be 0-45 degrees range

   unsigned char tempdegree;
   unsigned char comp;
   unsigned int degree;     // this will hold the result
   //signed int x;            // these hold the XY vector at the start
   //signed int y;            //

   // 1. Calc the scaled "degrees"
   degree = (y * 45) / x; // note! X must be >= Y, result will be 0-45

   // 2. Compensate for the 4 degree error curve
   comp = 0;
   tempdegree = degree;     // use an unsigned char for speed!
   if(tempdegree > 22)      // if top half of range
   {
      if(tempdegree <= 44) comp++;
      if(tempdegree <= 41) comp++;
      if(tempdegree <= 37) comp++;
      if(tempdegree <= 32) comp++;  // max is 4 degrees compensated
   }
   else    // else is lower half of range
   {
      if(tempdegree >= 2) comp++;
      if(tempdegree >= 6) comp++;
      if(tempdegree >= 10) comp++;
      if(tempdegree >= 15) comp++;  // max is 4 degrees compensated
   }
   degree += comp;   // degree is now accurate to +/- 1 degree!
  return degree;
}
#elif defined use_atan_360

u16 fast_atan(s16 x, s16 y) {

   // Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
   // Converts any XY values including 0 to a degree value that should be
   // within +/- 1 degree of the accurate value without needing
   // large slow trig functions like ArcTan() or ArcCos().
   // NOTE! at least one of the X or Y values must be non-zero!
   // This is the full version, for all 4 quadrants and will generate
   // the angle in integer degrees from 0-360.
   // Any values of X and Y are usable including negative values provided
   // they are between -1456 and 1456 so the 16bit multiply does not overflow.

   unsigned char negflag;
   unsigned char tempdegree;
   unsigned char comp;
   unsigned int degree;     // this will hold the result
//   signed int x;            // these hold the XY vector at the start
//   signed int y;            // (and they will be destroyed)
   unsigned int ux;
   unsigned int uy;

   // Save the sign flags then remove signs and get XY as unsigned ints
   negflag = 0;
   if(x < 0)
   {
      negflag += 0x01;    // x flag bit
      x = (0 - x);        // is now +
   }
   ux = x;                // copy to unsigned var before multiply
   if(y < 0)
   {
      negflag += 0x02;    // y flag bit
      y = (0 - y);        // is now +
   }
   uy = y;                // copy to unsigned var before multiply

   // 1. Calc the scaled "degrees"
   if(ux > uy)
   {
      degree = (uy * 45) / ux;   // degree result will be 0-45 range
      negflag += 0x10;    // octant flag bit
   }
   else
   {
      degree = (ux * 45) / uy;   // degree result will be 0-45 range
   }

   // 2. Compensate for the 4 degree error curve
   comp = 0;
   tempdegree = degree;    // use an unsigned char for speed!
   if(tempdegree > 22)      // if top half of range
   {
      if(tempdegree <= 44) comp++;
      if(tempdegree <= 41) comp++;
      if(tempdegree <= 37) comp++;
      if(tempdegree <= 32) comp++;  // max is 4 degrees compensated
   }
   else    // else is lower half of range
   {
      if(tempdegree >= 2) comp++;
      if(tempdegree >= 6) comp++;
      if(tempdegree >= 10) comp++;
      if(tempdegree >= 15) comp++;  // max is 4 degrees compensated
   }
   degree += comp;   // degree is now accurate to +/- 1 degree!

   // Invert degree if it was X>Y octant, makes 0-45 into 90-45
   if(negflag & 0x10) degree = (90 - degree);

   // 3. Degree is now 0-90 range for this quadrant,
   // need to invert it for whichever quadrant it was in
   if(negflag & 0x02)   // if -Y
   {
      if(negflag & 0x01)   // if -Y -X
            degree = (180 + degree);
      else        // else is -Y +X
            degree = (180 - degree);
   }
   else    // else is +Y
   {
      if(negflag & 0x01)   // if +Y -X
            degree = (360 - degree);
   }

  return degree;
}
#endif
