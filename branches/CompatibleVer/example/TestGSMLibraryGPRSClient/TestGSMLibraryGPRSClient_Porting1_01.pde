#include "QuectelM10.h"
#include <NewSoftSerial.h>
#include "inetGSM.h"
/*#include "LOG.h"
 
 #define __LOG_LEVEL 5
 LOG logme(__LOG_LEVEL);
 */
/*
 * We should put here some copyright stuff.
 *
 * This program is developed just to test-develop the Arduino-TID-GSM libraries.
 */

InetGSM inet;
char msg[100];
int numdata;
char inSerial[30];
int i=0;

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration.
  if (gsm.begin(4800))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");

  //GPRS attach
  if (gsm.attachGPRS("internet.wind", "", ""))
    Serial.println("status=ATTACHED");
  else Serial.println("status=ERROR");
  delay(5000);
  //TCP Client GET.
  numdata=inet.httpGET("www.google.com", 80, "/", msg, 100);
  Serial.println("\nNumber of data received:");
  Serial.println(numdata);  
  Serial.println("Data received:"); 
  Serial.println(msg); 

  //Tweet
  //inet.tweet("*********************key************", "An Arduino at #cpes15");

};

/*
void loop() 
 {
 if (gsm.connectedClient())
 {
 Serial.println("A");
 gsm.read(msg, 200);
 Serial.println(msg);
 gsm.write((const uint8_t*)"Read Data", 9);
 }
 };
*/
 
void loop() 
{
  serialhwread();
  serialswread();
};

void serialhwread(){
  i=0;
  if (Serial.available() > 0){            
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }

    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END")){
      Serial.println("_");
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWrite(inSerial);
    }

    if(!strcmp(inSerial,"TEST")){
      Serial.println("TESTING");
    }

    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg);
    }

    else{
      Serial.println(inSerial);
      gsm.SimpleWrite(inSerial);
    }    

    inSerial[0]='\0';
  }
}

void serialswread(){
  gsm.SimpleRead();
}


