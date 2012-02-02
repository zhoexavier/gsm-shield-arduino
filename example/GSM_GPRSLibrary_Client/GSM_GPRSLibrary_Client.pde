#include "SIM900.h"
#include <NewSoftSerial.h>
#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to start a connection as client.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;

char msg[50];
int numdata;
char inSerial[30];
int i=0;

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
  
  //GPRS attach, put in order APN, username and password.
  //If no needed auth let them blank.
  if (gsm.attachGPRS("internet.wind", "", ""))
    Serial.println("status=ATTACHED");
  else Serial.println("status=ERROR");
  delay(5000);
  
  //TCP Client GET, send a GET request to the server and
  //save the reply.
  numdata=inet.httpGET("www.google.com", 80, "/", msg, 50);
  //Print the results.
  Serial.println("\nNumber of data received:");
  Serial.println(numdata);  
  Serial.println("Data received:"); 
  Serial.println(msg); 

  //Tweet
  //inet.tweet("*********************key************", "An Arduino at #cpes15");

};

void loop() 
{
  //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
  serialhwread();
  //Read for new byte on NewSoftSerial.
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
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWrite("AT+CSQ");
    }
    //Read last message saved.
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
