// include the library code:
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <math.h> 
// initialize the library with the numbers of the interface pins
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
float L = 18.0;

//encoders for wheel turn counting
#define l_enc 44
#define r_enc 45

#define button 25 
#define pot A7


// dist = r x a x pi/180
#define pi M_PI
float rad = 8.6;

unsigned long previousMillis = 0;    
unsigned long startMillis = millis();

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


void setup() {
    // set up the LCD's number of columns and rows:
    pinMode(lm_dir, OUTPUT);
    pinMode(rm_dir, OUTPUT);
    pinMode(l_enc, INPUT);
    pinMode(r_enc, INPUT);
    
    Serial.begin(115200); 
    
    pinMode(button, INPUT);
    pinMode(pot, INPUT);
    
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


//get da angle
int getNewAngle(int curY, int nexY, int curX, int nexX){
        int newAngle = (atan2((nexY - curY),(nexX - curX))) * (180/pi);
        return newAngle;
}

void loop() {
      delay(300);

      int but = 1;
      int dur;

      
      boolean pressed = false;
      while (!pressed){
      //get left wheel speed
      l_speed = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("L Speed:      ");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      l_speed = map(l_speed, 0, 1023, 0, 255);
      lcd.setCursor(0,1);
      lcd.print(l_speed);
      lcd.print("                 ");
      if(but == 0){
        pressed = true;  
      }    
      }
      delay(60);
      pressed = false;
      
      but = digitalRead(button); //0 = on 1 = off
      while (!pressed){
      //get right wheel speed
      r_speed = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("R Speed:     ");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      r_speed = map(r_speed, 0, 1023, 0, 255);
      lcd.setCursor(0,1);
      lcd.print(r_speed);
       lcd.print("                ");
      if(but == 0){
        pressed = true;  
      }   
      }
      delay(30);
      pressed = false;
      
      but = digitalRead(button); //0 = on 1 = off
      while (!pressed){
      //get time of driving
      dur = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("Time:         ");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      dur = map(dur, 0, 1023, 1, 5);
      lcd.setCursor(0,1);
      lcd.print(dur);
      lcd.print("                ");
      if(but == 0){
        pressed = true;  
      }   
      }
      unsigned long currentMillis = millis(); 
      startMillis = currentMillis;
      while (currentMillis - startMillis <= (dur * 1000)) {
          currentMillis = millis();
          forward(lm_dir, lm_pin, l_speed);
          encR = digitalRead(r_enc);
          encL = digitalRead(l_enc);
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
          forward(rm_dir, rm_pin, r_speed);
      }

      backward(lm_dir, lm_pin, 200);
      backward(rm_dir, rm_pin, 200);
      delay(60);
      release(lm_pin);
      release(rm_pin);
      delay(30);

    //  float rICC = L * (tickL/(tickR-tickL)) + (L/2);
      float distR = tickR *0.0609;
      float distL = tickL *0.0609;
     // float angle = (0.01892 * (tickR-tickL));
      float angle = ((tickR-tickL)) / L;
     
      
      int totDist = (distR + distL) /2;
     
      angle = angle * 2;
      if(angle < 0) angle+= 360;
      but = 1; //0 = on 1 = off
      delay(30);
      pressed = false;
      while(!pressed){
        but = digitalRead(button); //0 = on 1 = off
        lcd.setCursor(0,0);
        lcd.print("Angle:     ");
        lcd.setCursor(10,0);
        lcd.print(angle);
        lcd.setCursor(0,1);
        lcd.print("Distance: ");
        lcd.setCursor(12,1);
        lcd.print(totDist);
        if(but == 0) pressed = true;
      }


      //chad as gone the right amount of time forward

      //first angle
/*      int angle = (atan2 (y1, x1)) * 180/pi;  // arc tangent of y/x in radians
      if(angle < 0)  {
        angle += 360; 
      } 
      int angleDist = rad * angle * (pi/180); //finds the distance to travel to make the angle    
      int distance = sqrt((x1 * x1)+ (y1 * y1)); //finds distance to travel after turn
      turnLeft(angleDist); 
      delay(50);
      iCanGoTheDistance(distance);
      int lastAngle = angle;
      
      int newAngle = getNewAngle(y1, y2, x1, x2);      
      angle = newAngle - lastAngle;  
      lastAngle = newAngle;   
      if(angle < 0)  {
        angle += 360; 
      } 
      angleDist = rad * angle * (pi/180);
      distance =  sqrt(sq(x2-x1) + sq(y2-y1));
      turnLeft(angleDist); 
      delay(50);
      iCanGoTheDistance(distance);


      newAngle = getNewAngle(y2, y3, x2, x3);      
      angle = newAngle - lastAngle;    
      lastAngle = newAngle;   
      if(angle < 0)  {
        angle += 360; 
      }    
      angleDist = rad * angle * (pi/180);
      distance =  sqrt(sq(x3-x2) + sq(y3-y2));
      turnLeft(angleDist); 
      delay(50);
      iCanGoTheDistance(distance);


      newAngle = getNewAngle(y3, 0, x3, 0);      
      angle = newAngle - lastAngle;    
      lastAngle = newAngle;   
      if(angle < 0)  {
        angle += 360; 
      }    
      angleDist = rad * angle * (pi/180);
      distance =  sqrt(sq(0-x3) + sq(0-y3));
      turnLeft(angleDist); 
      delay(50);
*/
      
      while(1){}      
}





  
