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
long lasttime;

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
  int i=0;
  while(i<20){
    gsm.SimpleRead();
    i++;
  }
  delay(5000);

  //Tweet
  //inet.tweet("*********************key************", "An Arduino at #cpes15");

  // TCP Server.
  gsm.write((const uint8_t*)"AT+CIFSR\r", 10);
  gsm.read(msg, 200);
  Serial.println(msg);
  delay(5000);
  if (gsm.connectTCPServer(80))
    Serial.println("status=TCPSERVERWAIT");
  else Serial.println("ERROR in Server");
  lasttime=millis();

};


void loop() 
 {
  //serialhwread();
  //serialswread();
 if (gsm.connectedClient())
 {
 Serial.println("A");
 gsm.read(msg, 200);
 Serial.println(msg);
 //gsm.write((const uint8_t*)"AT+CIPSTATUS\r", 14);
 }
 };

/*
void loop() 
{
  serialhwread();
  serialswread();
};
*/

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
      //Serial.println("_");
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


