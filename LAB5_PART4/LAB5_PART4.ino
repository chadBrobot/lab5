#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "MedianFilterLib.h"
LiquidCrystal_I2C lcd(0x27, 16,2);
 //Arduino PWM Speed Control：
#define lm_pin 5   //PWM on M2
#define lm_dir 4   //direction control on M2
#define rm_pin 6   //PWM for M1
#define rm_dir 7   //direction control for M1 

#define interval 1500

/*left turn sppeds*/
//#define l_speed 128
//#defineint r_speed 100
/*right turn sppeds*/
int l_speed = 110;
int r_speed = 99;
int line_count = 0;

//encoders for wheel turn counting
#define l_enc 44
#define r_enc 45

#define button 25 
#define pot A7
#define sonar A12


#define TRIGGER_PIN  48  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     48  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 150 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define intervallcd 120
#define intervalrun 4000 

// dist = r x a x pi/180
#define pi M_PI
float rad = 8.6;

#include <SPI.h>
#include <SD.h>

File myFile;

unsigned long previousMillis = 0;    

int oldRenc = digitalRead(r_enc); 
int encR = digitalRead(r_enc);
int oldLenc = digitalRead(l_enc); 
int encL = digitalRead(l_enc);

int count = 0;
int tickL = 0;
int tickR = 0;
bool menu = true;
bool runfor = false;

int xCoord[3]; //holds xcoords
int yCoord[3]; //holds ycoords
int cm = 5;

//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
      // set up the LCD's number of columns and rows:
    pinMode(lm_dir, OUTPUT);
    pinMode(rm_dir, OUTPUT);
    pinMode(l_enc, INPUT);
    pinMode(r_enc, INPUT);
   
    Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
    pinMode(button, INPUT);
    pinMode(pot, INPUT);
    pinMode(sonar, INPUT);
    
    lcd.init();
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.backlight();

}

void release(int motor){
  analogWrite(motor, 0);
  }

void forward(int motor_dir, int motor_pin, int speedy){
  digitalWrite(motor_dir,HIGH);
  analogWrite(motor_pin, speedy);
  }

void backward(int motor_dir, int motor_pin, int speedy){
  digitalWrite(motor_dir,LOW);
  analogWrite(motor_pin, speedy);
  }

//go straight for a specified distance
void iCanGoTheDistance(int distance){
   distance = distance -7 ;
   count = distance/0.0609;
   boolean go = true;
   while(go) {
   if ((tickR <= count) && (tickL <= count)) {
  
   encR = digitalRead(r_enc);
   encL = digitalRead(l_enc);
   //this means left is going faster
   if(encR != oldRenc){
        if(encR == 1){
          tickR++; 
          }
          oldRenc = encR;    
        }

    if(encL != oldLenc){
        if (encL == 1){
          tickL++;
          }
        oldLenc = encL;
      }
    //check if the left wheel is going to fast
    if(tickL > tickR){
      release(lm_pin); 
      backward(rm_dir, rm_pin, r_speed);       
    } 
     
     //check if the right wheel is going to fast
     if(tickR > tickL){
        release(rm_pin); 
        backward(lm_dir, lm_pin, l_speed); 
      }
     
    if(tickL == tickR){
       backward(rm_dir, rm_pin, r_speed);
       backward(lm_dir, lm_pin, l_speed);   
    }
       
}
  else {    
    //forward(rm_dir, rm_pin, 255);
    //forward(lm_dir, lm_pin, 255);
    tickR = tickL =0;
    
    delay(75);
    release(lm_pin);
    release(rm_pin);
    go = false;  
  }
 }
}

float voltage2cm(int volt){
    return ((6787/(volt - 3.0)) - 4.0);  
}

void loop() {
    int but = 1;
    int value, value1, value2;
    float e = 2.71828;
    but = digitalRead(button);
    if (but == 0){  
    float median;
    MedianFilter<int> medianFilter(10);
      for(int i = 0; i < 10; i++){
        value = analogRead(sonar); //potentiometer reading
        delay(50);
        float y = 143 * pow(e, (-0.00783*value)); 
        float m = (0.895*y + 5.17);
        median = medianFilter.AddValue(m);
       }

      
      lcd.setCursor(0,0);
      lcd.print("CM:");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      lcd.setCursor(0,1);
      lcd.print(median);
      lcd.print("        ");  
      delay(30);
    }


        
        /*int but = 1;
        int value;
        but = digitalRead(button); //0 = on 1 = off
        lcd.setCursor(0,0);
        lcd.print("CM:");
        lcd.setCursor(9,0);
        lcd.print("     ");
        lcd.setCursor(10,0);
        lcd.setCursor(0,1);
        lcd.print(cm);
        lcd.print("        ");
        if(but == 0){       
          delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
          Serial.print("Sonar: ");
          value = analogRead(sonar); //potentiometer reading
          Serial.print(value); // Send ping, get distance in cm and print result (0 = outside set distance range)
          Serial.println("cm");
          myFile.println(value);
          cm += 5;
          delay(100);
          
          delay(5000);
        }
        if (cm >= 95){
          myFile.close();
          } 
               */
}
