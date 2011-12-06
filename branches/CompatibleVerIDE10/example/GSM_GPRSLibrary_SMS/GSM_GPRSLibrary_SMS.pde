#include "SIM900.h"
#include <NewSoftSerial.h>
//If not used, is better to exclude the HTTP library,
//for RAM saving.
//If your sketch reboots itself proprably you have finished,
//your memory available.
//#include "inetGSM.h"

//If you want to use the Arduino functions to manage SMS, uncomment the lines below.
//#include "sms.h"
//SMSGSM sms;

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to send and receive SMS.

char msg[200];
int numdata;

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(4800))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
  
  //Enable this two lines if you want to send an SMS.
  //if (gsm.sendSMS("3471234567", "Arduino SMS"))
    //Serial.println("\nSMS sent OK");

};

void loop() 
{
  char smsbuffer[160];
  char n[20];
  //Read if there are messages on SIM card and print them.
  if(gsm.readSMS(smsbuffer, 160, n, 20))
  {
    Serial.println(n);
    Serial.println(smsbuffer);
  }
  delay(1000);
};

