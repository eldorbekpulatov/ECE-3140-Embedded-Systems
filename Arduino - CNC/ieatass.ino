/*
 * Multi-motor control (experimental)
 *
 * Move two or three motors at the same time.
 * This module is still work in progress and may not work well or at all.
 *
 * Copyright (C)2017 Laurentiu Badea
 * 
 * MUST INSTALL LIBRARY:
 * 1. Go to TOOLS->MANAGE LIBRARIES
 * 2. Search "DRV8825" 
 * 3. Install, and Done!
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include <Servo.h>
#include "BasicStepperDriver.h"
#include "DRV8825.h"
#include "MultiDriver.h"
#include "SyncDriver.h"

// Motor steps per revolution.
#define MOTOR_STEPS 20
// 1=full step, 2=half step etc.
#define MICROSTEPS 32
// Target RPM for FAST Motor
#define MAX_RPM 360


// Servo
#define PIN_S 13

// X motor
#define DIR_X 8
#define STEP_X 9
#define M2_X 10
#define M1_X 11
#define M0_X 12



// Y motor
#define DIR_Y 3
#define STEP_Y 4
#define M2_Y 5
#define M1_Y 6
#define M0_Y 7



struct Point{
  Point(int x, int y) {this->x = x; this->y = y; }
  int x;
  int y;
};

// GLOBALS
Point current_pos(0.0,0.0);
Servo servo;
DRV8825 stepper_X(MOTOR_STEPS, DIR_X, STEP_X, M0_X, M1_X, M2_X);
DRV8825 stepper_Y(MOTOR_STEPS, DIR_Y, STEP_Y, M0_Y, M1_Y, M2_Y);


// Pick one of the two controllers below
// each motor moves independently, trajectory is a hockey stick
// MultiDriver controller(stepperX, stepperY);
// OR
// synchronized move, trajectory is a straight line
SyncDriver controller(stepper_X, stepper_Y);


Point interpolate(Point a, Point b, double scale){
  double dx = b.x - a.x;
  double dy = b.y - a.y;
  return Point(scale * dx + a.x, scale * dy + a.y);
}


void moveTo(Point p){
  double dx = p.x - current_pos.x;
  double dy = p.y - current_pos.y;

  if(abs(dx) > abs(dy)){
    double r = abs(dy)/abs(dx);
    stepper_X.setRPM(MAX_RPM);
    stepper_Y.setRPM((int)MAX_RPM*r);
  }else if(abs(dy) > abs(dx)){
    double r = abs(dx)/abs(dy);
    stepper_Y.setRPM(MAX_RPM);
    stepper_X.setRPM((int)MAX_RPM*r);
  }else{
    stepper_X.setRPM(MAX_RPM);
    stepper_Y.setRPM(MAX_RPM);
  }
  controller.rotate(dx, dy);
  current_pos = p;
}

void moveXY(int x, int y){
  double i = 0.0;
  int num = 100;
  while(i<=1){
    moveTo(interpolate(current_pos, Point((double)x, (double)y), i));
    i+=1.0/num;
  }
}

void penUp(){
  servo.write(90);
  delay(300);
}

void penDown(){
  servo.write(77);
  delay(300);
}

void setup() {
    stepper_X.begin(MAX_RPM, MICROSTEPS);
    stepper_Y.begin(MAX_RPM, MICROSTEPS);
    servo.attach(PIN_S);
    penUp();
}

void loop() {
  // I
  moveTo(Point(300, 4300));
  penDown();
  moveTo(Point(300, 3900));
  moveTo(Point(600, 3900));
  moveTo(Point(600, 2900));
  moveTo(Point(300, 2900));
  moveTo(Point(300, 2500));
  moveTo(Point(1200, 2500));
  moveTo(Point(1200, 2900));
  moveTo(Point(900, 2900));
  moveTo(Point(900, 3900));
  moveTo(Point(1200, 3900));
  moveTo(Point(1200, 4300));
  moveTo(Point(300, 4300));
  penUp();

  // E
  moveTo(Point(1800, 4300));
  penDown();
  moveTo(Point(1800, 2500));
  moveTo(Point(2500, 2500));
  moveTo(Point(2500, 2800));
  moveTo(Point(2100, 2800));
  moveTo(Point(2100, 3200));
  moveTo(Point(2500, 3200));
  moveTo(Point(2500, 3500));
  moveTo(Point(2100, 3500));
  moveTo(Point(2100, 3900));
  moveTo(Point(2500, 3900));
  moveTo(Point(2500, 4300));
  moveTo(Point(1800, 4300));
  penUp();


 //A
  moveTo(Point(2600, 2500));
  penDown();
  moveTo(Point(3100, 4300));
  moveTo(Point(3600, 2500));
  moveTo(Point(3400, 2500));
  moveTo(Point(3200, 3000));
  moveTo(Point(3000, 3000));
  moveTo(Point(2800, 2500));
  moveTo(Point(2600, 2500));
  penUp();
  moveTo(Point(3000, 3100));
  penDown();
  moveTo(Point(3200, 3100));
  moveTo(Point(3100, 3400));
  moveTo(Point(3000, 3100));
  penUp();

  // T
  moveTo(Point(3500, 4300));
  penDown();
  moveTo(Point(3500, 3900));
  moveTo(Point(3850, 3900));
  moveTo(Point(3850, 2500));
  moveTo(Point(4150, 2500));
  moveTo(Point(4150, 3900));
  moveTo(Point(4500, 3900));
  moveTo(Point(4500, 4300));
  moveTo(Point(3500, 4300));
  penUp();

  //A
  moveTo(Point(0, 0));
  penDown();
  moveTo(Point(500, 2000));
  moveTo(Point(700, 2000));
  moveTo(Point(1200, 0));
  moveTo(Point(900, 0));
  moveTo(Point(800, 500));
  moveTo(Point(400, 500));
  moveTo(Point(300, 0));
  moveTo(Point(0, 0));
  penUp();
  moveTo(Point(400, 800));
  penDown();
  moveTo(Point(800, 800));
  moveTo(Point(600, 1300));
  moveTo(Point(400, 800));
  penUp();

  // S
  moveTo(Point(1500, 500));
  penDown();
  moveTo(Point(1500, 0));
  moveTo(Point(2700, 0));
  moveTo(Point(2700, 1200));
  moveTo(Point(1900, 1200));
  moveTo(Point(1900, 1600));
  moveTo(Point(2300, 1600));
  moveTo(Point(2300, 1500));
  moveTo(Point(2700, 1500));
  moveTo(Point(2700, 2000));
  moveTo(Point(1500, 2000));
  moveTo(Point(1500, 800));
  moveTo(Point(2300, 800));
  moveTo(Point(2300, 400));
  moveTo(Point(1900, 400));
  moveTo(Point(1900, 500));
  moveTo(Point(1500, 500));
  penUp();
  
  // S
  moveTo(Point(3000, 500));
  penDown();
  moveTo(Point(3000, 0));
  moveTo(Point(4200, 0));
  moveTo(Point(4200, 1200));
  moveTo(Point(3400, 1200));
  moveTo(Point(3400, 1600));
  moveTo(Point(3800, 1600));
  moveTo(Point(3800, 1500));
  moveTo(Point(4200, 1500));
  moveTo(Point(4200, 2000));
  moveTo(Point(3000, 2000));
  moveTo(Point(3000, 800));
  moveTo(Point(3800, 800));
  moveTo(Point(3800, 400));
  moveTo(Point(3400, 400));
  moveTo(Point(3400, 500));
  moveTo(Point(3000, 500));
  penUp();
  
  moveTo(Point(0,0));
  while(1){}; // stall
  
}
