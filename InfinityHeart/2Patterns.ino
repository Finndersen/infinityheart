#define DEFAULT_PATTERN_DURATION 120

class LedPattern : public BaseLEDController {
  public:
  static ColourScheme* colour_scheme;
  LedPattern(unsigned int duration=DEFAULT_PATTERN_DURATION, byte framerate_delay=0): BaseLEDController(duration, framerate_delay)   {
    DPRINTLN("Initialising Pattern");
    
    printMemory();
  }
  ~LedPattern() {
    DPRINTLN("Destroying pattern");
    delete colour_scheme;
    colour_scheme=NULL;
  }
  void newFrame() {
    BaseLEDController::newFrame();
    if (colour_scheme != NULL)  {
      colour_scheme->newFrame();
      //Change colour mode when expired
      if (frames_left & !colour_scheme->frames_left) {
        getColourMode();
      }
    } else {
      // Get colour mode when missing (initial)
      getColourMode();
    }
    
  }
  static LedPattern* GetRandom();
  private:
  virtual void getColourMode() {    
    delete colour_scheme;
    colour_scheme=NULL;
    //Get random colour scheme
    byte choice = random(0, 8);
    DPRINT("Choosing colour mode: ");
    DPRINTLN(choice+1);
    switch (choice) {
       case 0:
        colour_scheme= new colourMovingHueShifters(); 
        break;
      case 1:
        colour_scheme= new colourCycleBeatRed(random(0,3));
        break;
      case 2:
        colour_scheme= new colourChangeOnBeat();
        break;
      case 3:
        colour_scheme= new colourRedHeart();
        break;
      case 4:
        colour_scheme= new colourRandom();
        break;
      case 5:
      case 6:
        colour_scheme= new colourRainbow(random(1,4), random(1,4)); //Moving rainbow
        break;
      case 7:
        colour_scheme= new colourStrobe(); 
        break;

      default:
        colour_scheme= new colourRainbow(3, 3); 
        break;
    }
    //return colour_scheme;
  }
};
ColourScheme* LedPattern::colour_scheme = NULL;//new colourRainbow(3, 3);

class patternBeatHeight : public LedPattern  {
  byte height;
  void frameAction() {
      height = map(beat_level, 0, 255, 0, LEDS_PER_SIDE);
  }
  void setLED(byte i) {
      if (i <= height)  {
        setLEDValue(BOTH_SIDES, i, max_brightness);
      } else {
        setLEDValue(BOTH_SIDES, i, 0);
      }
  }
};

class patternSolidBeatPulse : public LedPattern  {
  private:
  byte brightness;
  void frameAction()  {
    brightness = map(beat_level, 0,255, max_brightness>>1, max_brightness);
  }
  void setLED(byte i) {      
    setLEDValue(BOTH_SIDES, i, brightness);
  }
};

/* RANDOM SPARKLE
 *  Sparkles randomly appear and then die down
 */

#define SPARKLE_FADE_FACTOR 30
class patternRandomSparkle : public LedPattern  {
  public:
  patternRandomSparkle(byte sparkle_intensity=3, byte framerate_delay=2): sparkle_intensity(sparkle_intensity), LedPattern(DEFAULT_PATTERN_DURATION, framerate_delay)  {}
  private:
  void getColourMode() {    
    delete colour_scheme;
    colour_scheme=NULL;
    //Get random colour scheme
    byte choice = random(0, 5);
    DPRINT("Choosing SParkle colour mode: ");
    DPRINTLN(choice+1);
    switch (choice) {
      case 0:
        colour_scheme= new colourRainbow(0, 3); //Static rainbow
        break;
      case 1:
        colour_scheme= new colourChangeOnBeat();
        break;
      case 2:
        colour_scheme= new colourRandom();
        break;
      case 3:
        colour_scheme= new colourRainbow(3, 3); //Moving rainbow
        break;
      case 4:
        colour_scheme= new colourStrobe(); 
        break;
    }
  }
  unsigned long sparkle_bitmask;
  byte sparkle_intensity;
  byte  left_leds[LEDS_PER_SIDE], right_leds[LEDS_PER_SIDE];
  void frameAction() {
      sparkle_side(left_leds);
      sparkle_side(right_leds);
      //sparkle_bitmask = random(0, ((unsigned long) 1)<<(LEDS_PER_SIDE));
  }
  void sparkle_side(byte led_array[LEDS_PER_SIDE])  {
    byte sparkle_id=random(0,LEDS_PER_SIDE*sparkle_intensity);
    for (byte i=0; i<LEDS_PER_SIDE; i++){
      if (i == sparkle_id)  {
        // New sparkle
        led_array[i]=255;
      } else {
        // Fade
        led_array[i]=byteSubtract(led_array[i], SPARKLE_FADE_FACTOR);
      }
    }
  }
  void setLED(byte i) {
    setLEDValue(LEFT_SIDE, i, map(left_leds[i], 0, 255, 0, max_brightness));
    setLEDValue(RIGHT_SIDE, i, map(right_leds[i], 0, 255, 0, max_brightness));
    /*
    bool led_on;
    led_on = ((((unsigned long) 1)<<i)&sparkle_bitmask);
    setLEDValue(BOTH_SIDES, i, led_on ? max_brightness : 0);
    */
  }
};

/* ROTATING SEGMENTS
 *  Segments which move around the heart and grow with beat
 */
class patternMovingSegments : public LedPattern  {

  public:
  patternMovingSegments(byte num_segments=2, int move_delay=2): LedPattern(DEFAULT_PATTERN_DURATION, 1), num_segments(num_segments), move_delay(move_delay)  {
    // Initialise segment positions
    for (byte i=0; i < num_segments; i++) {
      segment_positions[i] = i*(LEDS_PER_SIDE/num_segments);    
    }
  }
  private:
  byte num_segments, segment_extra_width, move_delay;
  bool overlap;
  bool led_states[LEDS_PER_SIDE];
  byte segment_positions[4];
  void frameAction() {
    static byte skip_count;
    byte segment_max, segment_min;
    //Clear LED values
    for (byte i=0; i < LEDS_PER_SIDE; i++) {
      led_states[i] = false;    
    }
    // Calculate segment width based on beat
    segment_extra_width = map(beat_level, 0, 255, 1, (byte) 6/num_segments);
    // Dont move segment position every frame
    if (skip_count) {
      skip_count--;
    } else {
      skip_count=move_delay;
    }
    //Process each segment
    for (byte i=0; i < num_segments; i++) {  
      // Increment position of segment (but not every frame)
      if (!skip_count)  {
        if (segment_positions[i] < LEDS_PER_SIDE-1) {
          segment_positions[i]++;
        } else {
          segment_positions[i]=0;
        }
      }
      //Set LED array values
      segment_max = segment_positions[i] + segment_extra_width;
      segment_min = segment_positions[i] - segment_extra_width;
      // Handle bottom overflow (segment_min underflows)
      if (segment_min > LEDS_PER_SIDE-1)  {
        for (byte j=0; j<256-segment_min; j++)  {
          led_states[LEDS_PER_SIDE-1-j] = true;
        }
        segment_min=0;
      }
      // Handle top overflow
      if (segment_max > LEDS_PER_SIDE-1)  {
        for (byte j=0; j<segment_max - LEDS_PER_SIDE-1; j++)  {
          led_states[j] = true;
        }
        segment_max=LEDS_PER_SIDE-1;
      }
      //Normal 
      for (byte j=0; j<(segment_max-segment_min+1); j++)  {
        led_states[segment_min+j] = true;
      }
    }
  }
  void setLED(byte i) {
    setLEDValue(LEFT_SIDE, i, led_states[i] ? max_brightness : 0 );
    setLEDValue(RIGHT_SIDE, LEDS_PER_SIDE-i-1, led_states[i] ? max_brightness : 0);
  }
};

/* MOVING SINE WAVE
 * Lights move in sine wave of varying length
 */
class patternMovingSineWave : public LedPattern {
  public:
  patternMovingSineWave(byte wave_length_factor=2): wave_length_factor(wave_length_factor)  {}
  private:
  byte frame_time, wave_length_factor;
  void frameAction()  {
    frame_time += 4;
  }
  void setLED(byte i) {
    setLEDValue(BOTH_SIDES, i, map(sin8(map(i, 0, LEDS_PER_SIDE,0,127<<wave_length_factor)+frame_time), 0, 255, 0 ,max_brightness));
  }
};

/* SINE WAVE PULSE
 * All lights pulsing brightness with sine wave
 */
class patternSineWavePulse : public LedPattern {
  public:
  patternSineWavePulse(byte extra_delay=3): extra_delay(extra_delay)  {}
  private:
  byte frame_time, extra_delay;
  void frameAction()  {
    frame_time += 6+ extra_delay;
  }
  void setLED(byte i) {
    setLEDValue(BOTH_SIDES, i, map(sin8(frame_time), 0, 255, 0, max_brightness));
  }
};

/* ALTERNATE SIDES
 * Half heart is lit then lights move to other side with beat
 */
class patternAlternateSides : public LedPattern {

  private:
  byte frame_time, animate_frame;
  bool side;
  void frameAction()  {
    if (newBeat()) {
        side = !side;
        animate_frame = LEDS_PER_SIDE/2;
    }
    if (animate_frame)  {
      animate_frame--;
    }
  }
  void setLED(byte i) {
    bool led_on_left, led_on_right;
    if (animate_frame)  {
      if (side) {
        // Transitioning from left to right side
        led_on_left = (i > (LEDS_PER_SIDE - animate_frame)) | (i < animate_frame);
        led_on_right = (i > (LEDS_PER_SIDE/2 + animate_frame)) | (i < (LEDS_PER_SIDE/2 - animate_frame));
      } else {
        led_on_right = (i > (LEDS_PER_SIDE - animate_frame)) | (i < animate_frame);
        led_on_left = (i > (LEDS_PER_SIDE/2 + animate_frame)) | (i < (LEDS_PER_SIDE/2 - animate_frame));
      
      }
    } else {
      led_on_left = !side;
      led_on_right = side;
    }
    setLEDValue(LEFT_SIDE, i, led_on_left ? max_brightness : 0);
    setLEDValue(RIGHT_SIDE, i, led_on_right ? max_brightness : 0);
  }
};

/* BEAT PULSE GENERATOR
 * Each beat creates a pulse which travels down heart (top to bottom)
 */
#define PULSE_LENGTH 5
#define NUM_LED_VALUES LEDS_PER_SIDE+1
#define MAX_PULSES 5
class patternBeatPulseGenerator : public LedPattern {
  public:
  patternBeatPulseGenerator(bool stack, unsigned int duration=200): LedPattern(duration), stack(stack), pulse_index(0) {}
  private:
  byte pulses[MAX_PULSES]; //Contains positions of pulses (up to LEDS_PER_SIDE for full stack)
  byte led_values[NUM_LED_VALUES];
  bool  stack;
  byte pulse_index=0, stack_level=0;
  void frameAction()  {
    int total_brightness;
    if (stack_level < LEDS_PER_SIDE) {
      framerate_delay=1;
      // Clear LED values
      for (byte i=0; i < NUM_LED_VALUES; i++)  {
        if (i >= (NUM_LED_VALUES-stack_level))  {
          led_values[i]=max_brightness;
        } else {
          led_values[i]=0;
        }  
      }
      //Move all existing pulses positions and set LED values
      for (byte i=0; i < MAX_PULSES; i++)  {
        if (pulses[i])  {
          if (pulses[i] == (NUM_LED_VALUES-stack_level-1)) {
            if (stack)  {
              stack_level++;
            }
            DPRINTLN("Pulse reached end");
             //Pulse ended
             pulses[i] = 0;  
          } else {
            // Pulse move
            pulses[i]++;
            //Set values
            for (byte j=0; j<=PULSE_LENGTH; j++)  {
              if (pulses[i] > j)  {
                led_values[pulses[i]-(j+1)]=max_brightness/(j+1);
              }
            }
          }      
        }
      }

      //Generate new pulse on beat
      if (newBeat()) {
        DPRINT("Making new pulse at index: ");
        DPRINTLN(pulse_index);
        pulses[pulse_index] = 1;
        led_values[0]=max_brightness;
        //SHift pulse index (alternative to appending/removign from array)
        if (pulse_index >= MAX_PULSES-1)  {
          pulse_index=0;
        } else {
          pulse_index++;
        }
      }
    } else {
      //When reach full stack, fade all away
      framerate_delay=3;
      total_brightness=0;
      for (byte i=0; i < NUM_LED_VALUES; i++)  {
        if (led_values[i])  {
          led_values[i]=led_values[i]>>random(0,2);
          total_brightness+=led_values[i];
        }
      }
      if (total_brightness==0)  {
        // Restart stack
        stack_level=0;
      }
    }
  }
  void setLED(byte i) {
    if (i == LEDS_PER_SIDE-1) {
      //Static part at top always on
      setLEDValue(BOTH_SIDES, i, map(beat_level, 0, 255, 80, 255));
    } else {
      setLEDValue(BOTH_SIDES, i, led_values[LEDS_PER_SIDE-i-1]);
    }
  }
};

class patternFire : public LedPattern {
  public:
  patternFire()  {
    cooling_factor=80;//random(60,100);
    sparking_factor=50;//random(50,100);
    framerate_delay=3;
  }
  
  private:
  void getColourMode() {    
    delete colour_scheme;
    colour_scheme=new colourFire();
  }
  byte led_values[LEDS_PER_SIDE];
  byte cooling_factor, sparking_factor;
  void frameAction()  {
    unsigned int cooldown;
    // Step 1.  Cool down every cell a little
    for( byte i = 0; i < LEDS_PER_SIDE; i++) {
      cooldown = random(0, ((cooling_factor * 10) / LEDS_PER_SIDE) + 2);
      led_values[i] = byteSubtract(led_values[i], cooldown);
    }
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( byte k= LEDS_PER_SIDE - 1; k >= 2; k--) {
      led_values[k] = (led_values[k - 1] + led_values[k - 2] + led_values[k - 2]) / 3;
    }
    // Step 3.  Randomly ignite new 'sparks' near the bottom
    for (byte s=0; s<7; s++)  {
      if( random(180+(10*s)) < sparking_factor ) {
        led_values[s] = byteAdd(led_values[s], random(160,255));
      }
    }
    
  }
  void setLED(byte i) {
    setLEDValue(BOTH_SIDES, i, map(led_values[i], 0, 255, 0, max_brightness));
  }
};

class patternBouncingPulses : public LedPattern {
  //Symmetric pulses bouncing top to bottom
  public:
  patternBouncingPulses(byte frame_delay=0):  LedPattern(DEFAULT_PATTERN_DURATION, frame_delay) {}
  private:
  byte pulse_position;
  bool pulse_direction=true;
  byte led_values[LEDS_PER_SIDE];
  void frameAction()  {
    // Dim all values
    for (byte i=0; i<LEDS_PER_SIDE; i++)  {
      led_values[i]=byteSubtract(led_values[i], 30);
    }
    // Set pulse position
    if (pulse_direction)  {
      if (pulse_position == LEDS_PER_SIDE-1)  {
        pulse_direction=false;
      } else {
        pulse_position++;
      }
    } else {
      if (pulse_position==0)  {
        pulse_direction=true;
      } else {
        pulse_position--;
      }
    }
    led_values[pulse_position] = map(beat_level, 0, 255, 160, 255);    
  }
  void setLED(byte i) {
    setLEDValue(BOTH_SIDES, i, map(led_values[i], 0, 255, 0, max_brightness));
  }
};

#define NUM_PATTERNS 14

LedPattern* LedPattern::GetRandom() {
  static LedPattern* current_pattern;
  delete current_pattern;
  current_pattern=NULL;
  Serial.println("Deleted old pattern");
  byte pattern_id =random(0,NUM_PATTERNS);
  Serial.print("Choosing pattern: ");
  Serial.println(pattern_id);
  switch (pattern_id) {
    case 0:
      current_pattern = new patternBeatPulseGenerator(true);
      break;
    case 1:
      current_pattern = new patternAlternateSides();
      break;
    case 2:
      current_pattern = new patternMovingSineWave();
      break;
    case 3:
      current_pattern = new patternBeatHeight();
      break;
    case 4:
      current_pattern = new patternFire();
      break;
    case 5:
      current_pattern = new patternMovingSegments(random(1,4), random(1,3));
      break;
    case 6:
      current_pattern = new patternRandomSparkle(random(1,3), random(0,2));
      break;
    case 7:
      current_pattern = new patternSineWavePulse(random(1,3));
      break;
    case 8:
      current_pattern = new patternBeatPulseGenerator(false);
      break;
    case 9:
      current_pattern = new patternBouncingPulses();
      break;
    case 10:
    case 11:
    case 12:
    case 13:
      current_pattern = new patternSolidBeatPulse();
      break;
    default:
      current_pattern = new patternRandomSparkle(random(1,3), random(0,2));
      break;
  }
  return current_pattern;
}
/*
LedPattern* led_patterns[] = {
  new patternBeatPulseGenerator(true),
  new patternAlternateSides(),    
  new patternMovingSineWave(),
  new patternBeatHeight(), 
  new patternSolidBeatPulse(),
  new patternMovingSegments(),
  new patternRandomSparkle(),
  new patternSineWavePulse(),
  new patternBeatPulseGenerator(false),
};
*/
