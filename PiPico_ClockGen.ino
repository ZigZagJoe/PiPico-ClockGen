/* Simple clock generator using Pi Pico board.
 *  
 *  Circuit: GP21 is your clock output.
 * 
 * Connect to COM port and input new clock frequencies.
 */

#include "pico/stdlib.h"

#define MHZ2KHZ 1000
#define SEARCH_RANGE (1*MHZ2KHZ)

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
  Serial.println();
  Serial.print("Current frequency: ");
  Serial.print(((float)currSpKhz) / MHZ2KHZ);
  Serial.println(" MHz");
  Serial.print("New frequency (decimal MHz): ");

  String result = "";
  char ch;

  // read a string from serial, only allowing numeric, handling backspace and any line endings
  while (result.length() < 10) {
      if (Serial.available()) {
        ch = (char)Serial.read();

        // handle backspace
        if (ch == 8 || ch == 127) {
           if (result.length()) {
             result = result.substring(0,result.length()-1);
             // clear the last char
             Serial.print("\x8 \x8");
           }
           continue;
        }

        // submit
        if (ch == '\n' || ch =='\r') 
          break;
          
        // filter only valid chars
        if ((ch < '0' || ch > '9') && ch != '.')
          continue;

        result += ch;
        Serial.print(ch);
      }
  }

  Serial.println();
  
  float sp = result.toFloat();

  if (sp <= 0) {
    Serial.println("Input out of range.");
    return;
  }

  Serial.print("Input: ");
  Serial.print(sp);
  Serial.println(" MHz");

  // convert float to speed in KHZ
  int spKhz = sp * MHZ2KHZ;

  // can we even reach this speed?
  uint vco, postdiv1, postdiv2;
  if (!check_sys_clock_khz(spKhz, &vco, &postdiv1, &postdiv2)) {
      Serial.print("Not possible: ");

      // search for a nearby possible frequency
      float freq = 0;
      for (int s = 0; s < SEARCH_RANGE; s++) {
          if (check_sys_clock_khz(spKhz+s, &vco, &postdiv1, &postdiv2)){
              freq = ((float)(spKhz+s))/MHZ2KHZ;
              break;
          } else if (check_sys_clock_khz(spKhz-s, &vco, &postdiv1, &postdiv2)){
              freq = ((float)(spKhz-s))/MHZ2KHZ;
              break;
          }
      }

      if (freq) {
        Serial.print("Use ");
        Serial.print(freq);
        Serial.println(" MHz instead");
      } else {
        Serial.println("May be out of range.");
      }
      
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
