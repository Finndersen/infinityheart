
void newFrame() {
  static LedPattern* pattern;
  //static ColourScheme* colour_scheme;
  static bool start=true;
  //DPRINT("Frames left: ");
  //DPRINTLN(pattern->frames_left);
  // Change pattern when frames run out
  if (start | (!pattern->frames_left & !pattern->colour_scheme->frames_left))  {
    //printMemory();
    pattern=LedPattern::GetRandom();
    //printMemory();
    start=false;
  }
  /*
  // Change colour scheme when frames run out
  if (!colour_scheme->frames_left) {    
    colour_scheme = pattern->getColourMode();
    printMemory();
  }
  */
  pattern->newFrame();
  //colour_scheme->newFrame();
  // Convert LED array from HSV  to RGB
  hsv2rgb_rainbow(leds_hsv, leds, NUM_LEDS);
  FastLED.show();  
  frame++;
}
