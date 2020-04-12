#include <FastLED.h>
#include <MemoryFree.h>
#define micPin 15
#define potPin A0
#define LED_DATA_PIN 10
#define NUM_LEDS 37
#define LEDS_PER_SIDE 18
#define PATTERN_FRAMES 500
#define COLOUR_FRAMES 200
#define FRAMEDELAY 20
#define LOOP_RATE_MS 1
#define DURATION_FACTOR 4
#define LEFT_SIDE 1
#define RIGHT_SIDE 2
#define BOTH_SIDES 3

//#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__); Serial.flush(); //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__); Serial.flush();   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif


CRGB leds[NUM_LEDS];      //Full LED array in RGB format
CHSV leds_hsv[NUM_LEDS];  //Full LED array in HSV format

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(micPin, INPUT);
  pinMode(potPin, INPUT);
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
  randomSeed(analogRead(potPin));
}
byte beat_level, max_brightness=255;
unsigned int frame;

// the loop function runs over and over again forever
void loop() {
  static byte frame_delay_counter;
  static unsigned int pot_read_delay;
  static unsigned long micro_time;
  static unsigned int duration;
  micro_time=micros();
  detectBeat();
  if (frame_delay_counter)  {
    frame_delay_counter -=1;
  } else  {
    newFrame();
    frame_delay_counter=FRAMEDELAY;
  }
  if (pot_read_delay)  {
    pot_read_delay -=1;
  } else  {
    max_brightness = analogRead(potPin)>>2;
    pot_read_delay=100;
  }
  
  duration = micros() - micro_time;
  //Serial.println(duration);
  if (duration < LOOP_RATE_MS*1000) {
    delayMicroseconds((LOOP_RATE_MS*1000)-duration);
  }
}

class BaseLEDController  {
  //Base class for ColourSchemes and Patterns
  private:
  virtual void frameAction() {}
  virtual void setLED(byte i) {} 
  protected: 
  byte framerate_delay, skip_frames;
  //Helper function to detect beat only once
  bool already_seen_beat;
  bool newBeat()  {
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
  
  public:
  unsigned int frames_left;
  BaseLEDController(unsigned int duration, byte framerate_delay): framerate_delay(framerate_delay) {
    frames_left=DURATION_FACTOR*duration;
  }  
  void newFrame() {
    //Serial.println("LED Controller Frame");
    if (skip_frames) {
      skip_frames--;
    } else  {
      frameAction();
      for (byte i=0; i<LEDS_PER_SIDE; i++) {
        setLED(i);
      }
      skip_frames=framerate_delay;
    }
    if (frames_left)  {
      frames_left--;
    }
  }    
};
