#include <Adafruit_NeoPixel.h>
#include <fix_fft.h>

#define PIN 8
#define X  8
#define Y  5

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  strip.setBrightness(50);
  
  analogReference(INTERNAL);
}

char im[128], data[128];

char data_avgs[14];

void loop() {

  for (byte i=0; i < 128; i++){
    char val = analogRead(A5);
    data[i] = val;
    im[i] = 0;
  };

  fix_fft(data,im,7,0);

  for (byte i=0; i< 64;i++){
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); // this gets the absolute value of the values in the array, so we're only dealing with positive numbers
  };


  // average bars together
  for (byte i=0; i<8; i++) {
    data_avgs[i] = data[i<<3] + data[(i<<3) + 1] + data[(i<<3) + 2] + data[(i<<3) + 3] + data[(i<<3) + 4] + data[(i<<3) + 5] + data[(i<<3) + 6] + data[(i<<3) + 7]; // average together
    data_avgs[i] = map(data_avgs[i]*log(((i<<3)+4)), 0, 50, 0, 5); // remap values for LoL
  }



  // set LoLShield

  for (byte x=0; x < X; x++) {
    for (byte y=0; y < Y; y++) {
      if (y < data_avgs[x]) { // 13-x reverses the bars so low to high frequences are represented from left to right.
        strip.setPixelColor(xyToPixel(x,y), strip.Color(y*63 + millis(), x*32 + millis(), 255 - y*63 + millis()));
      } 
      else {
        strip.setPixelColor(xyToPixel(x,y), strip.Color(0, 0, 0));
      }
    }
  }

  strip.show();

}

byte xyToPixel(byte x, byte y)
{
  byte comb = 0;

  if(y&1) //odd
  {
    comb = (y + 1)*8 - 1 - (7 - x);
  }
  else //even
  {
    comb = (7 - x) + y*8;
  }

  return comb;
}



