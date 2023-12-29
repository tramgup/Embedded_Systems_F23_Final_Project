#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>

#include <Mouse.h>
#include <Wire.h>
#include <SPI.h>

using namespace std; 

//The device being used is the Adafruit Circuit Playground classic
//This device comes with the ATmega32u4 Processor


//setting position arrays to size of 3, to allow for more modular gestures
//a size of 3 means the gestures should incorporate 3 actions or be close to 3 seconds long
float X_values[3] = {0};
float Y_values[3] = {0};
float Z_values[3] = {0};

//moving average filter
//fixes errors for innacurate accelerometer readings.
float movavgx[5] = {0};
float movavgy[5] = {0};
float movavgz[5] = {0};

int filter_size = 5; 

int led_number= -1; //variable to store the current LED which is being triggered from the associated gesture


//created three functions for the x y z positions to filter out the data
float insert_new_val_ret_avg_x(float val) {
  float avgx = 0;

  //mving avg filter equation
  for(int x = 1; x < filter_size; x++){
    avgx += movavgx[x];
    movavgx[x-1] = movavgx[x]; 
  }

  avgx += val;
  avgx = avgx/filter_size;
  movavgx[filter_size-1] = val;
  
  return avgx;
}

float insert_new_val_ret_avg_y(float val) {
  float avgy = 0;

  for(int x = 1; x < filter_size; x++){
    avgy += movavgy[x];
    movavgy[x-1] = movavgy[x]; 
  }

  avgy += val;
  avgy = avgy/filter_size;
  movavgy[filter_size-1] = val;

  return avgy;
}

float insert_new_val_ret_avg_z(float val) {
  float avgz = 0;

  for(int x = 1; x <filter_size; x++){
    avgz += movavgz[x];
    movavgz[x-1] = movavgz[x]; 
  }

  avgz += val;
  avgz = avgz/filter_size;
  movavgz[filter_size-1] = val;

  return avgz;
}


float X, Y, Z;

//function that detects the 10 gestures
//different encoding for each of the 10 gestures
//tracking the various acceleration values for each associated gestures
void detect_message(float X_values[], float Y_values[], float Z_values[], int& m){
  // gesture 1: 
  if (X_values[0] < 0 && Y_values[0] < 0 && Z_values[0] > 0) {
    if (Z_values[1] < -6 || Z_values[2] < -6){
      led_number= 0; 
    }
  }

  // gesture 2
  if (X_values[0] < 0 && Y_values[0] < 0 && Z_values[0] > 0) {
    if (X_values[1] < -6 || X_values[2] < -6){
      led_number= 1; 
    }
  } 

  // gesture 3
  if (X_values[0] < -6){
    if (Y_values[1] < -5){
      if (Y_values[2]>6){
        led_number=2; 
      }
    }
  }


  // gesture 4
  if (Y_values[0] < -6 && Z_values[0] < 2 && X_values[0] < 3){
    if (X_values[1] < 1){
      if (Y_values[2]>6){
        led_number=3; 
      }
    }
  }

  // gesture 5
  if (Y_values[0] < -6 && Z_values[0] > 2 && X_values[0] < 0){
    if (X_values[1] < -6 && Y_values[1] < 4){
      if (Y_values[2] > 6){
        led_number= 4; 
      }
    }
  }

  // gesture 6
  if (Y_values[0] < 1 && X_values[0] < 1 && Z_values[0] > 6){
    if (X_values[1] < -6 || X_values[2] < -6){
      if (Y_values[2] < 2 && Z_values[2] < 2){
        led_number= 5; 
      }
    }
  }

  // gesture 7
  if (Y_values[0] < 1 && X_values[0] < 1 && Z_values[0] > 6){
    if (X_values[1] < 1){
      if (Z_values[2] > 6){
        led_number= 6; 
      }
    }
  }

  // gesture 8
  if (X_values[0] < -6 && Y_values[0] < 0){
    if (X_values[2] < -6 && Y_values[2] > 0){
      led_number= 7;
    }
  }

  // gesture 9
  if (Y_values[0] < -6 && X_values[0] < 1 && Z_values[0] < 4){
    if (X_values[1] < -6 || X_values[2] < -6){
      led_number= 8; 
    }
  }

  // gesture 10
  if (Y_values[0] > 6 && X_values[0] > -1 && Z_values[0] > -1){
    if (X_values[1] < -6 || X_values[2] < -6){
      led_number= 9; 
    }
  }

}

void light_led(int led_number){
  if (led_number>= 0){
    CircuitPlayground.setPixelColor(led_number, 128, 128,   0); //setting the associated LED based on the gesture.
  }

  CircuitPlayground.playTone(500, 100); //triggering a sound on the playground to indicate to the user that the correct gesture was done
}

//function reads acceleration and stores values into the x,y,z arrays
void read_acc(float X_values[], float Y_values[], float Z_values[]) {
  for (int i  = 0; i < 3; i++){
    X_values[i] = CircuitPlayground.motionX(); //.motion is the playground call that returns acceleromter readings
    Serial.println(X_values[i]); //printing the x acceleration on serial monitor
    Y_values[i] = CircuitPlayground.motionY();
    Serial.println(Y_values[i]);
    Z_values[i] = CircuitPlayground.motionZ();
    Serial.println(Z_values[i]);
    delay(1000); 
  }

}


void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();

}


void loop() {
  if (CircuitPlayground.leftButton()){ //starting to detect once the left button is pressed
    Serial.println("Start"); //signaling when detection begins
    read_acc(X_values,Y_values,Z_values);
    detect_message(X_values,Y_values,Z_values,led_number);
    light_led(led_number);
  } 

  Serial.println("The message is: "); 
  Serial.println(led_number); 

  Serial.print("X: ");
  Serial.print(CircuitPlayground.motionX());
  Serial.print("Y: ");
  Serial.print(CircuitPlayground.motionY());
  Serial.print("  Z: ");
  Serial.println(CircuitPlayground.motionZ());

  //delay for cleaner readings
  delay(500); 

} 