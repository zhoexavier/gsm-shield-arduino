#include "QuectelM10.h"
#include <NewSoftSerial.h>

/*
 * We should put here some copyright stuff.
 *
 * This program is developed just to test-develop the Arduino-TID-GSM libraries.
*/

char msg[200];
int numdata;

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration.
  if (gsm.begin(4800))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
  
  //if (gsm.sendSMS("3920934102", "Arduino SMS"))
    //Serial.println("\nSMS sent OK");

};

void loop() 
{
  char smsbuffer[160];
  char n[20];

  if(gsm.readSMS(smsbuffer, 160, n, 20))
  {
    Serial.println(n);
    Serial.println(smsbuffer);
  }
  delay(1000);
};

