/* Simple clock generator using Pi Pico board.
 *  
 *  Circuit: GP21 is your clock output.
 * 
 * Connect to COM port and input new clock frequencies.
 */

#include "pico/stdlib.h"

#define MHZ2KHZ 1000

// initial frequency to use
long currSpKhz = 50 * MHZ2KHZ;

void setup() {
  Serial.begin(9600);

  // set default freq
  set_sys_clock_khz(currSpKhz,true);

  // set up the clock output
  pinMode(21,OUTPUT);
  clock_gpio_init(21, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 1);
}

void loop() {
  Serial.print("Current speed: ");
  Serial.print(((float)currSpKhz) / MHZ2KHZ);
  Serial.println(" MHZ");
  Serial.println("Enter new clock frequency in MHZ (decimals OK)");

  Serial.setTimeout(60000);
  
  // read a line
  String result = Serial.readStringUntil('\n');

  // not valid....
  if (!result || !result.length())
    return;
  
  float sp = result.toFloat();

  if (!sp)
    return;

  Serial.print("Input: ");
  Serial.print(sp);
  Serial.println(" mhz");

  // convert float to speed in KHZ
  int spKhz = sp * MHZ2KHZ;

  // can we even reach this speed?
  uint vco, postdiv1, postdiv2;
  if (!check_sys_clock_khz(spKhz, &vco, &postdiv1, &postdiv2)) {
      Serial.println("Clock speed is not attainable. Try a different value.");
      return;
  }

  // yup, looks good... final confirm
  Serial.println("Enter Y to set speed, anything else to abort.");
  while (!Serial.available()) ;
  
  if (Serial.read() == 'Y') {
      Serial.print("Doing the thing... ");
      set_sys_clock_khz(spKhz,true);
      delay(1000);
      Serial.println("Done");
      currSpKhz = spKhz;
  } else 
      Serial.println("Aborted");

  // clear any further junk out of the buffer
  while (Serial.available()) Serial.read();   
}
