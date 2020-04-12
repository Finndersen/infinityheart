byte byteAdd(byte current, unsigned int add) {
  if ((add + current) > 255)  {
    return 255;
  } else {
    return current + add;
  }
}

byte byteSubtract(byte current, unsigned int subtract) {
  if (subtract > current)  {
    return 0;
  } else {
    return current - subtract;
  }
}

byte countSetBits(unsigned int n) {
  unsigned int count = 0; 
  while (n) 
  { 
      count += n & 1; 
      n >>= 1; 
  } 
  return count;
}

void printMemory()  {
  DPRINT("Free memory: ");
  DPRINTLN(freeMemory());
}
void printLevel(int level) {
  for (int i=0; i<=level; i++)  {
    Serial.print('*');
  }
  Serial.print('\n');
}


int running_max=0;
int arrayMax(int* int_array, int array_size) {
  int max=int_array[0];
  for (int i=1; i<array_size; i++)  {
    if (int_array[i] > max) {
      max = int_array[i];
    }
  }
  return max;

  
}
int runningAverage(int M) {
  #define LM_SIZE 40
  static int LM[LM_SIZE];      // LastMeasurements
  static byte index = 0;
  static long sum = 0;
  static byte count = 0;

  // keep sum updated to improve speed.
  sum -= LM[index];
  LM[index] = M;
  sum += LM[index];
  index++;
  index = index % LM_SIZE;
  if (count < LM_SIZE) count++;
  running_max = arrayMax(LM, LM_SIZE);

  return (int) (sum / count);
}
