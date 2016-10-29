/************************************************************************/
/*				                        		*/
/*	SRK_Basic                 		                        */
/*						                  	*/
/*					                        	*/
/************************************************************************/
/*	Author: Jordan Retz						*/
/*	Copyright 2012, Digilent Inc.					*/
/************************************************************************/
/*  Module Description: 			             		*/
/*					                        	*/
/* This module implements a simple demo application that demonstrates   */
/* how to program the Servo Robot Kit.      		                */
/*									*/
/*	Functionality:							*/
/*									*/
/* The application uses the PmodBTN to control two continuous rotation  */
/* servo motors. Based on which button is being pressed,                */
/* the robot will perform actions, specified below:                     */
/*                                                                      */
/*  Instruction Code    BTN Pressed        Function                     */
/*        0x0            None              Stop                         */
/*        0x1            BTN0              Move Forward                 */
/*        0x2            BTN1              Pivot Right                  */
/*        0x4            BTN2              Move Backward                */
/*        0x8            BTN3              Pivot Left                   */
/*                                                                      */
/* Pressing multiple buttons at a time will have no function.           */
/*                                                                      */
/* The PmodBTN must be connected to the top row of header JD. The       */
/* PmodCON3 must be connected to the bottom row of header JC.           */
/*                                                                      */
/*     Programming:                                                     */
/*                                                                      */
/* Make sure that the MX3cK is plugged into the computer via USB cable, */
/* and that the board is powered.                                       */
/*                                                                      */
/* From the toolbar above, select "Tools->Board->MX3cK". From the Start */
/* menu, navitgate to "Start->Control Panel->Hardware and Sound->Device */
/* Manager". Select "Ports", and take note of what COM port (COMx) is   */
/* labeled "USB Serial Port". If you are unsure, unplug the MX3cK from  */
/* the computer and watch which COM port disappears, and then           */
/* reappears, when you plug the MX3cK back into the computer.           */
/* From the toolbar above, select "Tools->Serial Port" and then the COM */
/* port that you just found. Apply power to the MX3cK and click on the  */
/* "Upload" button above.                                               */
/*                                                                      */
/************************************************************************/
/*  Revision History:			        			*/
/*					                        	*/
/*	6/21/2012 (JordanR): created		       			*/
/*  12/17/2012 (MichaelK): Name Change                  */
/*						                     	*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*                Include File Definitions                      */
/* ------------------------------------------------------------ */
#include <plib.h>
#include <Servo.h>

/* ------------------------------------------------------------ */
/*				Local Type Definitions		*/
/* ------------------------------------------------------------ */
/* These definitions are valid if the PmodCON3 is connected to the
** bottom row of header JC
*/
#define    pinServoLeft   23
#define    pinServoRight  22

/* These definitions are valid if the PmodBTN is connected to the
** top row of header JD
*/
#define    pinBTN0        24
#define    pinBTN1        25
#define    pinBTN2        26
#define    pinBTN3        27

/* ------------------------------------------------------------ */
/*                Global Variables                              */
/* ------------------------------------------------------------ */
/*Define Servos*/
Servo      left;
Servo      right;
byte       bInstruction;

/* ------------------------------------------------------------ */
/*                Forward Declarations                          */
/* ------------------------------------------------------------ */
void CheckButtons();
void StopRobot();
void Forward();
void Backward();
void TurnRight();
void TurnLeft();


/* ------------------------------------------------------------ */
/*                Procedure Definitions                         */
/* ------------------------------------------------------------ */

/***  void setup()
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Initialize the system.
*/
void setup()
{
  /*Setup Input/Output on Pins
  */
  pinMode(pinServoLeft, OUTPUT);
  pinMode(pinServoRight, OUTPUT);

  pinMode(pinBTN0, INPUT);
  pinMode(pinBTN1, INPUT);
  pinMode(pinBTN2, INPUT);
  pinMode(pinBTN3, INPUT);

}

/* ------------------------------------------------------------ */
/***  void loop();
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Main application loop.
*/
void loop()
{
  CheckButtons();

  if(bInstruction == 0x0) {      //stop

    StopRobot();
  }
  else if(bInstruction == 0x1) { //move backward

    Forward();
  }
  else if(bInstruction == 0x2) { // move forward

    Backward();
  }
  else if(bInstruction == 0x4) { // turn right

    TurnRight();
  }
  else if(bInstruction == 0x8) { // turn left

    TurnLeft();
  }
}

/* ------------------------------------------------------------ */
/***  byte CheckButtons();
**
**  Parameters:
**    none
**
**  Return Value:
**  none
**
**  Errors:
**  none
**
**  Description:
**    Fetches the state of the buttons.
*/
void CheckButtons() {

    byte bBtn0 = digitalRead(pinBTN0)*(0x1);
    byte bBtn1 = digitalRead(pinBTN2)*(0x2);
    byte bBtn2 = digitalRead(pinBTN1)*(0x4);
    byte bBtn3 = digitalRead(pinBTN3)*(0x8);

    bInstruction = (bBtn0 | bBtn1 | bBtn2 | bBtn3);
}

/* ------------------------------------------------------------ */
/***	void StopRobot();
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Stops robot.
*/
void StopRobot() {

  left.write(90);
  right.write(90);

  left.detach();
  right.detach();
}

/* ------------------------------------------------------------ */
/***	void Forward();
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Moves robot forward
*/
void Forward() {

    left.attach(pinServoLeft);
    right.attach(pinServoRight);

    left.write(107);
    right.write(73);
}

/* ------------------------------------------------------------ */
/***	void Backward();
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Moves robot forward
*/
void Backward() {

    left.attach(pinServoLeft);
    right.attach(pinServoRight);

    left.write(73);
    right.write(107);
}

/* ------------------------------------------------------------ */
/***	void TurnRight();
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Turns robot right
*/
void TurnRight() {

    left.attach(pinServoLeft);
    right.attach(pinServoRight);

    left.write(107);
    right.write(107);
}

/* ------------------------------------------------------------ */
/***	void TurnLeft();
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   Turns robot left
*/
void TurnLeft() {

    left.attach(pinServoLeft);
    right.attach(pinServoRight);

    left.write(73);
    right.write(73);
}
