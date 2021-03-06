#include "digit_util.h"
#include <SPI.h>
#include "GD2.h"

// TODO: A possible error in this function.
//       I often see the uV area jump from low (0-100) to high (900-999) without
//       the mv changing... What's the precision in arduino calculations ?
void DigitUtilClass::separate(int *v, int *mv, int *uv, bool *neg, double rawMv) {
  *neg=false;
  if (rawMv < 0.0f) {
    rawMv = 0.0f - rawMv;
    *neg=true;
  }
  *v = (int)(rawMv / 1000.0f);
  *mv = (int)(rawMv - *v * 1000.0f);
  *uv = (rawMv - (int)rawMv) * 1000.0f;
}

void DigitUtilClass::separate2(int *v, int *mv, int *uv, int *nv, bool *neg, double rawMv) {
  float uv_f;
  *neg=false;
  if (rawMv < 0.0f) {
    rawMv = 0.0f - rawMv;
    *neg=true;
  }
  *v = (int)(rawMv / 1000.0f);
  *mv = (int)(rawMv - *v * 1000.0f);
  uv_f = (rawMv - (int)rawMv) * 1000.0f;
  *uv = uv_f;
  *nv = (uv_f - int(uv_f)) * 1000.0;
}

void DigitUtilClass::renderValue(int x,int y,float val, int size = 0, int type = 0) {

    int font = 26;
    int fontWidth = 10; 
    
    if (size == 1) {
       font = 28;
       fontWidth = 15;
    }
    else if (size == 2) {
       font = 29;
       fontWidth = 18;
    }
    else if (size == 3) {
       font = 30;
       fontWidth = 21;
    }
    else if (size == 4) {
       font = 31;
       fontWidth = 29;
    }
    
    int v, mV, uV, nV;
    bool neg;
    DIGIT_UTIL.separate2(&v, &mV, &uV, &nV, &neg, val);
    if(neg) {
      GD.cmd_text(x, y, font, 0,  "-");
    }

    x = x + fontWidth;
    if (v > 0) {
      GD.cmd_number(x, y, font, 2, v);
      x = x + fontWidth*1.7;
      GD.cmd_text(x, y, font, 0,  ".");
      x = x + fontWidth/3;
      GD.cmd_number(x, y, font, 3, mV);
      x = x + fontWidth * 2.9;
      GD.cmd_number(x, y, font, 3, uV);
      x = x + fontWidth * 2.6;
      if (type > -1) {
        GD.cmd_text(x, y, font, 0,  type == typeVoltage ? "V" : "A");
      }
    } else if (mV > 0 or type == typeVoltage) {
      
      if (mV < 100) {
        x = x + fontWidth*0.8;
        GD.cmd_number(x, y, font, 2, mV); // show only two least significant digits. Not the leading zero
              x = x + fontWidth*1.7;

      } else {
        GD.cmd_number(x, y, font, 3, mV);
              x = x + fontWidth*2.5;

      }

      
      GD.cmd_text(x-1, y, font, 0,  ".");
      x = x + fontWidth/3;
      GD.cmd_number(x, y, font, 3, uV);
      x = x + fontWidth * 2.6;

      // can show more resolution on current when using low current range....
      //TODO: Use different resolutions depending on range, not only measured value !
      if (type == typeCurrent) {
        x=x+5;
       GD.cmd_number(x, y, font, 2, nV / 10); // show two digits
        x = x + fontWidth * 1.9;
      }

      
      if (type > -1) {
        GD.cmd_text(x, y, font, 0,  type == typeVoltage ? "mV" : "mA");
      }
    } else {
      GD.cmd_number(x, y, font, 3, uV);
      x = x + fontWidth*2.5;
      GD.cmd_text(x-1, y, font, 0,  ".");
      x = x + fontWidth/3;
      GD.cmd_number(x, y, font, 3, nV);
      x = x + fontWidth * 2.6;
      if (type > -1) {
        GD.cmd_text(x, y, font, 0,  type == typeVoltage ? "uV" : "uA");
      }
    }

}


unsigned long compliance_blink_timer = millis();
unsigned long old_compliance_blink_timer = compliance_blink_timer ;

int DigitUtilClass::blinkColor(int colorHigh, int colorLow, unsigned int period) {
  unsigned int compliance_blink_timer = millis();
  unsigned int color = colorHigh;
  if (compliance_blink_timer - old_compliance_blink_timer > period) {
    old_compliance_blink_timer = compliance_blink_timer;
  } else if (compliance_blink_timer - old_compliance_blink_timer > period/2) {
    color = colorLow;
  }
  return color;
}

int DigitUtilClass::indicateColor(int normalColor, int indicatorColor,int period, int tag) {
  if  (tag != currentTag) {
    return normalColor;
  }
  if (timeSinceIndicated + 100 > millis()) {
    return indicatorColor;
  } else {
    return normalColor;
  }
}
void DigitUtilClass::startIndicator(int tag) {
  currentTag = tag;
  timeSinceIndicated= millis();
}



DigitUtilClass DIGIT_UTIL;
