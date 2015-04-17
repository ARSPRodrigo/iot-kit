#include <GSM.h>
#include "USSDAccess.h"
#include "configure.h"

char* ussd_code;		//	USSD Service code
int max_resp_len = 175;	        //	maximum length of the USSD response

USSDAccess ussd_acc;

// the setup routine runs once when you press reset:
void setup() {   
  Serial.begin(9600);  
  
  set_application_code();
  ussd_seesion();
 
}

// get the application short code from the configure.h file
void set_application_code(){
  ussd_code = application_code;
}

void ussd_seesion(){
  char resp[max_resp_len];
  Serial.println("Begin");
  ussd_acc.initSession(ussd_code);
  Serial.println("Receiving..");
  ussd_acc.getResponse(resp);   
  Serial.println(resp);
  ussd_acc.endSession();
  Serial.println("Session terminated");
}


// the loop routine runs over and over again forever:
void loop() {
//
}
