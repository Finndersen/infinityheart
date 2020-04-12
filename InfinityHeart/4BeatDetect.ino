#define BEAT_COOLDOWN 300
#define BEAT_RAMP_DELAY 5
#define BEAT_FADE_STEP 6

void detectBeat() {
  static unsigned int detect_delay;
  static bool beat;
  static byte ramp, ramp_delay, beat_level_linear;
  byte ramp_up_step;
  
  // Detect beat (after delay of previous beat)
  if (detect_delay) {
    detect_delay -=1;
  } else {
    
    if (digitalRead(micPin)) {
      // Detect new beat
      if (!beat)  {
        // Filter out short noise
        delayMicroseconds(200);
        if (digitalRead(micPin))  {
          Serial.println("Detect Beat");
          beat=1;
          ramp=1;
        }
      // Continue existing beat
      } else {
        ramp =1;
      }
    //Beat finished
    } else if (beat) {
      beat = 0;
      detect_delay=BEAT_COOLDOWN;
    }
  }

  if (ramp_delay) {
    ramp_delay--;
  } else {
    ramp_delay=BEAT_RAMP_DELAY;
    //Ramp beat level up
    if (ramp == 1)  {
       ramp_up_step = BEAT_FADE_STEP<<1;
      if (beat_level <= 255-ramp_up_step) {
        beat_level_linear = beat_level_linear + ramp_up_step;
      } else {
        beat_level_linear=255;
        ramp=2;
      }
    //Ramp beat level down
    } else  if (ramp == 2)   {
      if (beat_level_linear >= BEAT_FADE_STEP) {
        // Fade beat level
        beat_level_linear = beat_level_linear - BEAT_FADE_STEP;
      } else {
        beat_level_linear=0;
        ramp=0;
      }
    }
    beat_level = quadwave8(beat_level_linear>>1);
  }  
}

bool new_beat_colour() {
  //Helper function to detect beat only once
  static bool already_seen_beat;
   if (beat_level) {
    if (!already_seen_beat) {
      already_seen_beat = true;
      return true;
    }
  } else {
    already_seen_beat = false;
  }
  return false;
}

bool new_beat_pattern() {
  //Helper function to detect beat only once
  static bool already_seen_beat;
   if (beat_level) {
    if (!already_seen_beat) {
      already_seen_beat = true;
      return true;
    }
  } else {
    already_seen_beat = false;
  }
  return false;
}
