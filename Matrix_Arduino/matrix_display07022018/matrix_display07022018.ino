//Current_pt
// This sketch has been largely adapted from Adafruit sketches.
// Demonstrates the drawing abilities of the RGBmatrixPanel library.
// For 32x64 RGB LED matrix.

// NOTE THIS CAN ONLY BE USED ON A MEGA! NOT ENOUGH RAM ON UNO!

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <string.h>
#define OE  9
#define LAT 8
#define CLK 11
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);

char currentTemp1[15] = "";
char Celsius[] = " C";

const int curxT = 8; //Cursor start for time line.
const int curxD = 2; //Cursor start for date line.
const int curTmp = 16; //Cursor start for temperature line.

char currentDate [] = "dd-mm-yyyy";
char currentTime [] = "hh:mm:ss";
char currentTemp [] = "-tt.t";
char currMesg [200];
char nextMesg [200];

char buffer [200];
unsigned int count;

unsigned long nextUpdate;

const int sensorPin = 7; //Pin for LDR.
const int minValue = 100;
const int maxValue = 700;
int red, green, blue = 1; //initial value for display colours at minimum brightness. (7 is max.)

char nullStr [] = "Starting up";
char spaceStr [] = " "; //Added by Pete.

const char *pText;
int textLen;
int textOff;
int textStep;

void setText (const char *pStr)
{
  //Serial.print("Str=");
  //Serial.println(pStr);
  textLen = strlen(pText = pStr);
  textOff = -12;
  textStep = 0;
}


void setup() 
{
  Serial.begin(9600); // Turn the Serial Protocol ON
  matrix.begin();
  matrix.fillScreen(0); // fill the screen with 'black'
  matrix.setTextColor(matrix.Color333(0,green,0)); 
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves  

  setText (nullStr);

  buffer [count = 0] = '\0';
  nextUpdate = millis () + 100;
}

void loop()
{
  int lightValue = analogRead(sensorPin);
  //lightValue = map(lightValue,100,700,minValue,maxValue);
  lightValue = map(lightValue,100,700,minValue,maxValue);
  lightValue = constrain(lightValue,minValue,maxValue);
  int red = lightValue/100;
  int green = lightValue/100;
  int blue = lightValue/100;

  unsigned long millisNow;

  if (Serial.available ()) {
    char ch = Serial.read ();
    switch (ch) {
    case '[':  
      count = 0; 
      break;

    case ']':
      {
        buffer [count] = '\0';
        if ((count == 8) && (buffer [2] == ':') && (buffer [5] == ':') && strcmp (currentTime, buffer))
          strcpy (currentTime, buffer);
        break; 
      }

    case '!':
      {
        buffer [count] = '\0';
        if ((count == 10) && (buffer [2] == '-') && (buffer [5] == '-') && strcmp (currentDate, buffer))
          strcpy (currentDate, buffer);
        break; 
      }

    case '^':
      {
        buffer [count] = '\0';
        if ((count >= 3) && (count <= 5) && strcmp (currentTemp, buffer))
          strcpy (currentTemp, buffer);
        break; 
      }
      
      case '~':
      {
        //Serial.println("found tilde");
        buffer [count] = '\0';
        
        if (strcmp (nextMesg, buffer))
          strcpy(nextMesg, buffer);
        break; 
      }
      
    default:
      if (count < (sizeof(buffer)-1)) buffer [count++] = ch;
    } 
  }

  millisNow = millis ();

  if ((millisNow == nextUpdate) || (millisNow == (nextUpdate + 1))) {

    matrix.setCursor(curxT,0); // Set cursor for line 1.
    matrix.fillScreen(0);// Fill screen with black
    matrix.setTextColor(matrix.Color333(red,green,blue));
    matrix.print(currentTime); //Print time - line1
    matrix.setCursor(curxD,8); //Set cursor for line2.
    matrix.setTextColor(matrix.Color333(0,green,0));
    matrix.print(currentDate); //Print data - line2
    strcpy(currentTemp1,currentTemp);
    strcat(currentTemp1,Celsius);

    int tmplnth = strlen(currentTemp1); //length of currentTemp1 in characters
    double lineStart=32-((tmplnth*6.0)/2.0); //gives x coordinate of start of temperature line
    double degreeCentre=32+((tmplnth*6.0)/2.0)-9; //gives x coordinate of centre of degree symbol

    if (strcmp(currentTemp1,"0.0 C") <= 0)
    {
      matrix.setTextColor(matrix.Color333(0,0,blue));         
      matrix.drawCircle(degreeCentre,17,1,matrix.Color333(0,0,blue)); //Draw 'degree' symbol
    }
    else 
    {
      matrix.setTextColor(matrix.Color333(red,0,0));   
      matrix.drawCircle(degreeCentre,17,1,matrix.Color333(red,0,0)); //Draw 'degree' symbol
    }
    //line 3
    matrix.setCursor(lineStart,16);//Set cursor for line3
    matrix.print(currentTemp1);// print temp - line3

    //line 4
    matrix.setCursor(-textStep,24);
    //Serial.println(textStep);
    matrix.setTextColor(matrix.Color333(red,green,0));

      for (int index = 0; index < 12; ++index) {
        int offset = textOff + index;
        matrix.print (((offset >= 0) && (offset < textLen)) ? pText[offset] : ' ');
    }
    
    if (++textStep == 5) {
      textStep = 0;
      if (++textOff > (textLen + 12)) {
        textOff = -12;
        
        if (strcmp (currMesg, nextMesg))
          setText (strcpy (currMesg, nextMesg));
        strcpy (nextMesg, spaceStr); //Added by Pete.
      }
    }

    matrix.swapBuffers(false);   
    nextUpdate += 100;
  } 
}




