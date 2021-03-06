// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    

#include <Wire.h>
 
uint16_t samples[NUMSAMPLES];
 
void setup(void) {
  //Serial.begin(9600);
  analogReference(EXTERNAL);
}
 
void loop(void) {
  uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  steinhart = steinhart * 1.8 + 32;            // make it freedom units
  
  Serial.print("Temp "); 
  Serial.print(steinhart);
  Serial.println(" *F");
  Serial.println("              ");
 
  delay(1000);
}

void LCDinitialize(){
  Serial.write(0xFE);
  Serial.write(0x41);
  Serial.write(0xFE);
  Serial.write(0x51);
}

void LCDsetContrast(int contrast){
  if(contrast >50){
    return;
  }
  Serial.write(0xFE);
  Serial.write(0x52);
  Serial.write(contrast);
}

void LCDsetBrightness(int level){
  if(level > 8){
    return;
  }
  Serial.write(0xFE);
  Serial.write(0x53);
  Serial.write(level);
}

void LCDsetCursor(int line_num, int x){
  int g_index[4] = { 
    0x00, 0x40, 0x14, 0x54           };
  Serial.write(0xFE);
  Serial.write(0x45);
  Serial.write(g_index[line_num-1] + x -1);
}

void LCDclear(){
  Serial.write(0xFE);
  Serial.write(0x51);
}
