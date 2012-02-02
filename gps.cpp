#include "gps.h"

char GPSGSM::attachGPS() 
{
	if(AT_RESP_ERR_DIF_RESP == gsm.SendATCmdWaitResp("AT+CGPSPWR=1", 500, 100, "OK", 5))
		return 0;
	if(AT_RESP_ERR_DIF_RESP == gsm.SendATCmdWaitResp("AT+CGPSRST=1", 500, 100, "OK", 5))
		return 0;
	return 1;
}

char GPSGSM::deattachGPS() 
{
	if(AT_RESP_ERR_DIF_RESP == gsm.SendATCmdWaitResp("AT+CGPSPWR=0", 500, 100, "OK", 5))
		return 0;
	return 1;
}

char GPSGSM::getStat() 
{
	char ret_val=0;
	gsm.SimpleWrite("AT+CGPSSTATUS?");
	if(gsm.IsStringReceived("Unknown"))
		ret_val=0;
	else if(gsm.IsStringReceived("Not"))
		ret_val=1;
	else if(gsm.IsStringReceived("2D"))
		ret_val=2;
	else if(gsm.IsStringReceived("3D"))
		ret_val=3;
	return ret_val;
}

char GPSGSM::getPar(char *str_long, char *str_lat, char *str_alt, char *str_time, char *str_speed) 
{
	char ret_val=0;
	char *p_char; 
	char *p_char1;
	gsm.SimpleWrite("AT+CGPSINF=0");
	if(gsm.IsStringReceived("OK"))
		ret_val=1;
		
	//longitude
	p_char = strchr((char *)(gsm.comm_buf),',');
	p_char1 = p_char+1;  //we are on the first char of longitude
	p_char = strchr((char *)(p_char), ',');
	if (p_char != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char = 0; 
    }
	strcpy(str_long, (char *)(p_char1));
	
	//latitude
	p_char++;
	p_char1 = strchr((char *)(p_char), ',');
	if (p_char1 != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char1 = 0; 
    }	
	strcpy(str_lat, (char *)(p_char));
	
	//altitude
	p_char1++;
	p_char = strchr((char *)(p_char1), ',');
	if (p_char != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char = 0; 
    }	
	strcpy(str_alt, (char *)(p_char1));
	
	//UTC time
	p_char++;
	p_char1 = strchr((char *)(p_char), ',');
	if (p_char1 != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char1 = 0; 
    }	
	strcpy(str_time, (char *)(p_char));	

	//TTFF
	p_char1++;
	p_char = strchr((char *)(p_char1), ',');
	if (p_char != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char = 0; 
    }	

	//num
	p_char++;
	p_char1 = strchr((char *)(p_char), ',');
	if (p_char1 != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char1 = 0; 
    }	

	//speed
	p_char1++;
	p_char = strchr((char *)(p_char1), ',');
	if (p_char != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char = 0; 
    }		
	strcpy(str_speed, (char *)(p_char1));	
	return ret_val;
}