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

int line_count = 0;

//encoders for wheel turn counting
#define l_enc 44
#define r_enc 45

#define button 25 
#define pot A7
#define line 14

#define intervallcd 120
#define intervalrun 4000 

// dist = r x a x pi/180
#define pi M_PI
float rad = 8.6;

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


void setup() {
    // set up the LCD's number of columns and rows:
    pinMode(lm_dir, OUTPUT);
    pinMode(rm_dir, OUTPUT);
    pinMode(l_enc, INPUT);
    pinMode(r_enc, INPUT);
    pinMode(line, INPUT);
    
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
      forward(rm_dir, rm_pin, r_speed);       
    } 
     
     //check if the right wheel is going to fast
     if(tickR > tickL){
        release(rm_pin); 
        forward(lm_dir, lm_pin, l_speed); 
      }
     
    if(tickL == tickR){
       forward(rm_dir, rm_pin, r_speed);
       forward(lm_dir, lm_pin, l_speed);   
    }
       
}
  else {    
    backward(rm_dir, rm_pin, 255);
    backward(lm_dir, lm_pin, 255);
    tickR = tickL =0;
    
    delay(75);
    release(lm_pin);
    release(rm_pin);
    go = false;  
  }
 }
}





//any angle above 180 turns left
void turnRight(int dist){
   int count = (dist/0.0609);
   boolean go = true;
   while(go){
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
          forward(lm_dir, lm_pin, l_speed); 
        }
       
      if(tickL == tickR){
         backward(rm_dir, rm_pin, r_speed);
         forward(lm_dir, lm_pin, l_speed);   
      }
    }
    else {
        forward(rm_dir, rm_pin, 200);
        backward(lm_dir, lm_pin, 200);
        delay(50);  
        release(lm_pin);
        release(rm_pin);
        //reset tick count
        tickR = tickL =0;
        go = false;
    }
  
   }
  
 }

//any angle below 180 turns left
void turnLeft(int dist){
        
   int count = (dist/0.0609);
   boolean go = true;
   while(go){
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
        forward(rm_dir, rm_pin, r_speed);       
      } 
       
       //check if the right wheel is going to fast
       if(tickR > tickL){
          release(rm_pin); 
          backward(lm_dir, lm_pin, l_speed); 
        }
       
      if(tickL == tickR){
         forward(rm_dir, rm_pin, r_speed);
         backward(lm_dir, lm_pin, l_speed);   
      }
    }
    else {
        backward(rm_dir, rm_pin, 200);
        forward(lm_dir, lm_pin, 200);
        delay(50);  
        release(lm_pin);
        release(rm_pin);
        tickR = tickL =0;
        go = false;
    }
  }  
}

//get da angle
int getNewAngle(int curY, int nexY, int curX, int nexX){
        int newAngle = (atan2((nexY - curY),(nexX - curX))) * (180/pi);
        return newAngle;
}

void loop() {
  delay(300);
      long x1,y1,x2,y2,y3,x3;
      int but = 1;
      boolean pressed = false;
      while (!pressed){
      //get x1
      x1 = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("X Coord 1:");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      x1 = map(x1, 0, 1023, -200, 200);
      lcd.setCursor(0,1);
      lcd.print(x1);
      lcd.print("        ");
      if(but == 0){
        pressed = true;  
      }    
      }
      delay(30);
      pressed = false;
      //but = digitalRead(button); //0 = on 1 = off
      while (!pressed){
      //get y1
      y1 = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("Y Coord 1:");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      y1 = map(y1, 0, 1023, -200, 200);
      lcd.setCursor(0,1);
      lcd.print(y1);
      lcd.print("        ");
      if(but == 0){
        pressed = true;  
      }   
      }
      delay(30);
      pressed = false;
      //but = digitalRead(button); //0 = on 1 = off
      while (!pressed){
      //get x1
      x2 = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("X Coord 2:");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      x2 = map(x2, 0, 1023, -200, 200);
      lcd.setCursor(0,1);
      lcd.print(x2);
      lcd.print("        ");
      if(but == 0){
        pressed = true;  
      }   
      }
      delay(30);
      pressed = false;
      //but = digitalRead(button); //0 = on 1 = off
      while (!pressed){
      //get y1
      y2 = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("Y Coord 2:");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      y2 = map(y2, 0, 1023, -200, 200);
      lcd.setCursor(0,1);
      lcd.print(y2);
      lcd.print("        ");
      if(but == 0){
        pressed = true;  
      }   
      }
      delay(30);
      pressed = false;
      //but = digitalRead(button); //0 = on 1 = off
      while (!pressed){
      //get x1
      x3 = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("X Coord 3:");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      x3 = map(x3, 0, 1023, -200, 200);
      lcd.setCursor(0,1);
      lcd.print(x3);
      lcd.print("        ");
      if(but == 0){
        pressed = true;  
      }   
      }
      delay(30);
      pressed = false;
     // but = digitalRead(button); //0 = on 1 = off`
      while (!pressed){
      //get y1
      y3 = analogRead(pot); //potentiometer reading
      but = digitalRead(button); //0 = on 1 = off
      lcd.setCursor(0,0);
      lcd.print("Y Coord 3:");
      lcd.setCursor(9,0);
      lcd.print("     ");
      lcd.setCursor(10,0);
      y3 = map(y3, 0, 1023, -200, 200);
      lcd.setCursor(0,1);
      lcd.print(y3);
      lcd.print("        ");
      if(but == 0){
        pressed = true;  
      }   
      }
      delay(30);
      x1= 92;
      y1= 41;
      x2= -61;
      y2= 59;
      x3 = 53;
      y3 = -107;
     
      //first angle
      int angle = (atan2 (y1, x1)) * 180/pi;  // arc tangent of y/x in radians
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
      iCanGoTheDistance(distance);
      while(1){}      
}





  
