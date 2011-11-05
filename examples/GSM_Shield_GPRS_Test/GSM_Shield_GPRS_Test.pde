
/* GSM Shield example
 
 created 2011
 by Boris Landoni and
    Marco Martines
 
 This example code is in the public domain.
 
 
 http://www.open-electronics.org
 http://www.futurashop.it
 */

#include <GSM_Shield.h>
long lasttime=0;
long lasttime2=0;
long interval=60000;
long interval2=10000;
char ip[20];
boolean firston=true;
int response=0;

//**************************************************************************
char number[]="+39123456789";  //Destination number 
char text[]="hello world";  //SMS to send
byte type_sms=SMS_UNREAD;      //Type of SMS
byte del_sms=0;                //0: No deleting sms - 1: Deleting SMS
//**************************************************************************

GSM gsm;
int inint;
char inchar;
char sms_rx[122]; //Received text SMS
char number_incoming[20];
int call;
char resp[]="A";
int error;
char term[2];
boolean cmdmode=false;

void setup() 
{
  Serial.begin(9600);
  Serial.println("system startup"); 
  gsm.TurnOn(4800);          //module power on
  gsm.InitParam(PARAM_SET_1);//configure the module  
  gsm.Echo(0);               //enable AT echo 
  lasttime=millis();
  lasttime2=millis();
  term[0]=0x1a;
  term[1]='\0';
}


void loop()
{ 
  if(firston){
    Serial.println("Initializing...");
    delay(15000);
    Serial.println("Setting APN...");
    response=gsm.SetAPN("internet.wind","","");
    if(response==1)
      Serial.println("APN OK");
    else
      Serial.println("APN NOT OK");
    delay(5000);
    Serial.println("Starting GPRS connection...");
    response=gsm.EnableGPRS(0);
    if(response==1)
      Serial.println("GPRS OK");
    else
      Serial.println("GPRS NOT OK");
    delay(5000);
    
    Serial.println("Checking IP...");
    response=gsm.GetIP(ip);
    if(response==1){
      Serial.println(ip);
    }
    else
      Serial.println("IP NOT OK");
    delay(5000);
    
    Serial.println("Creating connection...");
    response=gsm.OpenSocket("TCP",80,"204.13.248.112");
    if(response==1){
      Serial.println("CONNECTED");
    }
    else
      Serial.println("NOT CONNECTED");
    delay(10000);
    
    Serial.println("Publishing IP...");
    gsm.PublishIP("gsmtesthgh.dyndns.org","Z3NtdGVzdDpmb3J6YXJvbWE=",ip);
    firston=false;
    
  }
  
  if(millis()-lasttime>interval){
    lasttime=millis();
    Serial.println("Starting server...");
    gsm.SimpleWrite("AT+CIPSERVER=1,80");
    cmdmode=true;
  }
    
  char inSerial[100];
  int i=0;
  
  if(gsm.ReadAndSave(inint)==1)
    {
      inchar=(char)inint;
      if(inchar=='2'){
        digitalWrite(5,LOW);
        Serial.println("led off");
      }
      else if(inchar=='1'){
        digitalWrite(5,HIGH);
        Serial.println("led on");
      }
      else
        Serial.print(inchar);
    }
    
    
  
  
  if (Serial.available() > 0) 
  {             
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }
    inSerial[i]='\0';

    if(!strcmp(inSerial,"end")){
      Serial.print(0x1a);
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWrite(inSerial);
    }
    else{
      Serial.print("CMD: ");
      Serial.println(inSerial);
      gsm.SimpleWrite(inSerial);
    }
    inSerial[0]='\0';
    //Check_Protocol(inSerial);
  }

}  


