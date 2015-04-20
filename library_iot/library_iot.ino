#include <GSM.h>
#include "USSDAccess.h"
#include "configure.h"

char* ussd_code;		//	USSD Service code
char* ussd_code_notification;   //	USSD Service code
char* ussd_response;

int loop_delay;
int max_resp_len = 175;	        //	maximum length of the USSD response
String received_command = "";
char* ussd_notification_data;

char* notification = "0#";

USSDAccess ussd_acc;





// the setup routine runs once when you press reset:
void setup() {   
  Serial.begin(9600);  
  set_application_code();
  set_loop_delay(); 
  //ussd_code_notification = strcat(ussd_code,notification);
}


// the loop routine runs over and over again forever:
void loop() {
  
  poll_command(); 
  execute_command();
  Serial.println("done");
  ussd_acc.endSession();
  
  //// for demonstration ////
//      check_coffie_machine(5);  // when 5th pin is connected to the sensor input(digital)
//      delay (60000);
  
  delay (loop_delay);
 
}


void check_coffie_machine(int pin){  
  pinMode(pin, INPUT);
  int x = digitalRead(pin);
  Serial.println("coffie");
  if (x==1){
    Serial.println("no coffie");
    ussd_notification_data = "1";
    send_notification();
    ussd_acc.endSession();
  }
}



// get the application short code from the configure.h file
void set_application_code(){
  ussd_code = application_code;
}

void set_loop_delay(){
  loop_delay = main_loop_delay;
}

//
void poll_command(){
  start_ussd_session(0);  // not a notification therefore
  get_ussd_response();  
}

void start_ussd_session(int x){
  if (x==0){
  Serial.println(ussd_code);
  ussd_acc.initSession(ussd_code);
  }else
  ussd_acc.initSession(ussd_code_notification);
}

//
void send_notification(){
  Serial.println("Sending a ussd response");
  Serial.println(ussd_notification_data);
  start_ussd_session(1);  // since a notification
  get_ussd_response();  
  send_ussd_response(ussd_notification_data);
  Serial.println("USSD response sent");
}

void get_ussd_response(){
  char resp[max_resp_len];
  Serial.println("Receiving..");
  ussd_acc.getResponse(resp);
  Serial.println(resp);
  received_command = resp;  
}
//
void send_ussd_response(char* x){
  ussd_response = x;
  ussd_acc.respond(ussd_response);  
}
//
void end_ussd_session(){
  ussd_acc.endSession();
  Serial.println("Session terminated");
}


void execute_command(){
  
  if (received_command=="poll") {
     ussd_acc.endSession();
  }
  else if (received_command=="continue") {
     ussd_response = "2";
     ussd_acc.respond(ussd_response);
     ussd_response = "";
     get_ussd_response();
  }
  else if (received_command=="error") {
     ussd_acc.endSession();
  }
  else{
     ussd_acc.endSession();
     extract_and_execute_instructions(received_command);
  }  
}


void extract_and_execute_instructions(String instruction){
     Serial.println("instructions");
     Serial.println(instruction);
     
     //instruction = "run:set-10-1";
     instruction = "run:get-10";
     
     String instruction_status = getValue(instruction,':',0); 
     String instruction_content = getValue(instruction,':',1);
     if (instruction_status == "error"){
       Serial.println("An error has occured");
       Serial.println(instruction_content);
     }
     else if (instruction_status == "run"){
       Serial.println(instruction_content);
       
       if (getValue(instruction_content,'-',0) == "get"){
           int pin = getValue(instruction_content,'-',1).toInt();
           pinMode(pin, INPUT);
           int x = digitalRead(pin);
           if (x==1){
           ussd_notification_data = "1";}
           else if (x==0){
           ussd_notification_data = "0";}
           Serial.println("ussd_notification_data is :");
           Serial.println(ussd_notification_data);
           send_notification();
       }
       else if (getValue(instruction_content,'-',0) == "set"){
           int pin = getValue(instruction_content,'-',1).toInt();
           int value = getValue(instruction_content,'-',2).toInt();
           pinMode(pin, OUTPUT);
           digitalWrite(pin, value);
       }
       
     } 
}


void set_pins(String Pins, uint8_t Type){
  int x = 0;
  while (x!=-1){
    String y = getValue(Pins,',',x);
    if ( y!= ""){ 
      x +=1;  
      pinMode(y.toInt(), Type);
    }
    else{
      x =-1;
    }
  }
}


int read_input_pin(int pin, String read_type){
  Serial.println("read function");
  if (read_type == "dr")
     return digitalRead(pin); 
  else
     return analogRead(pin);   
}

void write_output_pin(int pin, String write_type, int value){
  Serial.println("write function");
  if (write_type == "dw")
     digitalWrite(pin, value); 
  else
     analogWrite(pin, value);   
}


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}




