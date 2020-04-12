
void setLEDValue(byte sides, byte led_number, byte value)  {
  int led_id;
  if (sides & LEFT_SIDE)  {
    led_id = getLeftSideLedID(led_number);
    leds_hsv[led_id].v = value;
    //leds_hsv[led_id].s = 255;
    if (led_id == 1)  {
      leds_hsv[0].v = value;
      //leds_hsv[0].s = 255;
    }
  }
  if (sides & RIGHT_SIDE)  {
    led_id = getRightSideLedID(led_number);
    leds_hsv[led_id].v = value;
    //leds_hsv[led_id].s = 255; 
  }
};

void setLEDColour(byte sides, byte led_number, byte hue, byte saturation)  {
  int id;
  if (sides & LEFT_SIDE)  {
    id = getLeftSideLedID(led_number);
    leds_hsv[id].h = hue;
    leds_hsv[id].s = saturation;
    if (id == 1) {
      leds_hsv[0].h = hue;
      leds_hsv[0].s = saturation;
    }
  }
  if (sides & RIGHT_SIDE)  {
    id = getRightSideLedID(led_number);
    leds_hsv[id].h = hue;
    leds_hsv[id].s = saturation;     
  }
};

void setLEDSaturation(byte sides, byte led_number, byte saturation)  {
  if (sides & LEFT_SIDE)  {
    int id = getLeftSideLedID(led_number);
    leds_hsv[id].s = saturation;
    if (id == 1) {
      leds_hsv[0].s = saturation;
    }
  }
  if (sides & RIGHT_SIDE)  {
    leds_hsv[getRightSideLedID(led_number)].s = saturation;    
  }
};

int getLeftSideLedID(byte number) {
  //Convert LED number to stip ID for left side
  static byte led_id;
  led_id = LEDS_PER_SIDE  - number;
  if (led_id > NUM_LEDS-1)  {
    Serial.println("Invalid LED index");
    return 0;
  } else {
    return led_id;
  }
}

int getRightSideLedID(byte number)  {
  //Convert LED number to strip ID for right side
  static byte led_id;
  led_id = LEDS_PER_SIDE + 1 + number;
  if (led_id > NUM_LEDS-1)  {
    Serial.println("Invalid LED index");
    return 0;
  } else {
    return led_id;
  }
}
