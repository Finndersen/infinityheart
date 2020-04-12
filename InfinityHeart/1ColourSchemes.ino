
//Colour pattern binary IDs for bitmasking
#define NUM_COLOUR_MODES 7
#define COLOUR_RAINBOW_STATIC 1
#define COLOUR_CYCLE 2
#define COLOUR_BEAT_CHANGE 4
#define COLOUR_RED_HEART 8
#define COLOUR_RANDOM 16
#define COLOUR_RAINBOW_MOVE 32
#define COLOUR_ALL 63

#define DEFAULT_COLOUR_SCHEME_DURATION 50
class ColourScheme : public BaseLEDController  {
  public:
  ColourScheme(unsigned int duration=DEFAULT_COLOUR_SCHEME_DURATION, byte framerate_delay=0): BaseLEDController(duration, framerate_delay)  {
    DPRINTLN("Initialising Colour Scheme");
  }
  ~ColourScheme() {
    DPRINTLN("Destroying colour scheme");
  }
};

class colourRainbow : public ColourScheme  {  
  //Create rainbow pattern that moves at varying rate
  public:
  colourRainbow(byte rainbow_len=2, byte rainbow_speed=1): rainbow_speed(rainbow_speed), ColourScheme(80)  {
    hue_step = 255 / (LEDS_PER_SIDE/rainbow_len);
  }
  private:
  byte hue_step;   //How much hue to increment per LED
  byte first_hue, rainbow_speed;
  void frameAction() {
    first_hue+=rainbow_speed<<1;
  }
  void setLED(byte i) {
    setLEDColour(BOTH_SIDES, i, first_hue + i*hue_step, 255);
  }
};

class colourRandom : public ColourScheme  {  
  //Every pixel is random colour and beats to white
  public:
  colourRandom(byte framerate_delay=2): ColourScheme(DEFAULT_COLOUR_SCHEME_DURATION, framerate_delay)   {}
  private:
  byte hue_step= 255/LEDS_PER_SIDE;   //How much hue to increment per LED
  byte first_hue, saturation;
  void frameAction()  {
    saturation = map(beat_level, 0, 255, 255, 128);
  }
  void setLED(byte i) {
    setLEDColour(BOTH_SIDES, i, random(0, 255), saturation);
  }
};

class colourCycleBeatRed : public ColourScheme  {
  // Cycle smoothly through colour hues and change to red on beat
  public:
  colourCycleBeatRed(byte cycle_speed=2): cycle_speed(cycle_speed) {}
  private:
  byte beat_hue, base_hue, cycle_speed;
  void frameAction() {
    base_hue+=cycle_speed + 1;
    if (beat_level) {
      beat_hue = 0;
    } else  {
      beat_hue = base_hue;
    }
    
  }
  void setLED(byte i) {
    setLEDColour(BOTH_SIDES, i, beat_hue, 255);
  } 
};

class colourChangeOnBeat : public ColourScheme {
  public:
  //colourChangeOnBeat() {}
  private:
  byte hue;
  void frameAction()  {
    if (newBeat()) {
      hue  += random(50,100);
    }
  }
  void setLED(byte i) {
    setLEDColour(BOTH_SIDES, i, hue, 255);
  }
};

class colourRedHeart : public ColourScheme {
  //Cycle through range of pink/orAnge/red colours
  public:
  colourRedHeart(byte cycle_speed=2): cycle_speed(cycle_speed) {}
  private:
  byte red_hue=0, cycle_speed;
  bool hue_direction= 1;
  void frameAction() {
    if (hue_direction)  {
      red_hue+=cycle_speed + 1;
      if ((red_hue > 20) & (red_hue < 230)) {
        hue_direction = 0;
      }
    } else {
      red_hue-=cycle_speed + 1;
      if ((red_hue < 230) & (red_hue > 20)) {
        hue_direction = 1;
      }
    }
  }
  void setLED(byte i) {
    setLEDColour(BOTH_SIDES, i, red_hue, 255);
  }
};

class colourStrobe : public ColourScheme {
  //Flashes quickly between R-G-B to create strobe effect
  public:
  colourStrobe() : ColourScheme(80) {};
  private:
  byte hue;
  void frameAction()  {
      if (hue == 0) {
        hue = 96;
      } else if (hue == 96) {
        hue = 160;
      } else {
        hue = 0;
      }
  }
  void setLED(byte i) {
    setLEDColour(BOTH_SIDES, i, hue, 255);
  }
};

class colourFire : public ColourScheme {
  //Yellow to red fire colours
  public:
  private:
  byte hue;
  void setLED(byte i) {
    setLEDColour(BOTH_SIDES, i, map(i, 0, LEDS_PER_SIDE, 38,0), 255);
  }
};

class colourMovingHueShifters : public ColourScheme {
  //two moving dots which shift hue
  public:
  colourMovingHueShifters() : ColourScheme(100)  {
    speed1=random(1,3);
    speed2=random(3,5);
    strength1=random(30,100);
    strength2=random(30,100);
    for (byte i; i<LEDS_PER_SIDE*2; i++)  {
      led_hues[i]=0;
    }
  }
  byte speed1, speed2;
  byte  strength1,strength2;
  private:
  byte led_hues[LEDS_PER_SIDE*2];
  byte pos1, pos2;
  void frameAction()  {
    static byte counter1=0, counter2=0;
    if (!counter1)  {
      if (pos1 >= LEDS_PER_SIDE*2)  {
        pos1 = 0;
      } else {
        pos1++;
      }
      counter1=speed1;
      led_hues[pos1] += strength1;
    } else {
      counter1--;
    }
    //DPRINT("Position1: ");
    //DPRINTLN(pos1);
    if (!counter2)  {
      if ((pos2 == 0) | (pos2 > LEDS_PER_SIDE*2))  {
        pos2 = LEDS_PER_SIDE*2;
      } else {
        pos2--;
      }
      led_hues[pos2] += strength2;
      counter2=speed2;
    } else{
      counter2--;
    }
    //DPRINT("Position2: ");
    //DPRINTLN(pos2);
  }
  void setLED(byte i) {
    static byte second_ind;
    second_ind = LEDS_PER_SIDE*2-i;
    setLEDColour(RIGHT_SIDE, i, led_hues[i], (i==pos1) | (i==pos2) ? 0 : 255);
    setLEDColour(LEFT_SIDE, i, led_hues[second_ind], (second_ind==pos1) | (second_ind==pos2) ? 0 : 255);
  }
};
