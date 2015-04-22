#include <GSM.h>
#include "USSDAccess.h"
#include "configure.h"

USSDAccess ussd_acc;

char* ussd_code;		                               //	USSD Service code
char* ussd_code_notification;                                  //	USSD Service code
char* ussd_notification_data;
char* ussd_reistration_data;
char* ussd_response;

char* notify_request = "Ntfy";
char* command_request = "Cmd";
char* register_request = "Reg:coffie machine 01";

int loop_delay;
int max_resp_len = 175;	                                       //       maximum length of the USSD response

String received_command = "";


// for demo only
int ldr = 0;                                                   //analog pin to which LDR is connected
int ldr_value = 0;                                             //variable to store LDR values
int ussd = 0;

int demo_pin;
int demo_pin_value;
int demo_ussd = 0;
String demo_pin_type;
// end of demo


// the setup routine runs once when you press reset:
void setup() {  
  Serial.begin(9600);
  set_loop_delay(); 
  register_device();
  Serial.println("End of registration");  
  delay(20000);
  poll_command(); 
  
  //demo_setup();
}


// the loop routine runs over and over again forever:
void loop() {  
  delay(500); 
  demo();  
  
  //demo_loop();
}

void demo_setup(){
  //received_command = "CmdL:print a0;";                                     //  CmdL:print d7; //  
  String extracted_command =getValue(received_command,':',0);
  if (extracted_command=="Cmd" || extracted_command=="CmdL") {
      Serial.println("Command recieved");  
      Serial.println(received_command);
      String instruction = getValue(received_command,':',1);
      String temp = instruction.substring(0, instruction.length() - 1);   
      String current_command =  getValue(temp,';',0) ;
      Serial.println(current_command);
      String command_action = getValue(current_command,' ',0);
      String command_pin = getValue(current_command,' ',1);
      demo_pin = String(command_pin[1]).toInt();
      demo_pin_type = (String)command_pin[0];
  } 
  
}


void demo_loop(){
  if (demo_pin_type == "a")
     demo_pin_value = analogRead(ldr);                
  else if (demo_pin_type == "d")
     demo_pin_value = digitalRead(ldr);                

  Serial.println(ldr_value);
  if ((demo_pin_type == "a" && demo_pin_value< 200) || (demo_pin_type == "d" && demo_pin_value== 1)){
    if (demo_ussd ==0){
      Serial.println("send ussd");
      demo_ussd =1; 
      String pin = "level=";
      String x = "NtfyL:";
      x.concat(pin);
      String temp = String(demo_pin_value);               
      x.concat(temp);
      char charBuf[50];                              // charBuf temporary char buffer
      x.toCharArray(charBuf,50);
      ussd_notification_data = charBuf;     
      Serial.println(ussd_notification_data);
      send_notification();
  }
}
else{
 demo_ussd = 0;
}
}

void demo(){
ldr_value = analogRead(ldr);
Serial.println(ldr_value);
if (ldr_value< 200){
  if (ussd ==0){
    Serial.println("send ussd");
    ussd =1; 
    String pin = "level=";
    String x = "NtfyL:";
    x.concat(pin);
    String temp = String(ldr_value);      
    x.concat(temp);
    char charBuf[50];                                         
    x.toCharArray(charBuf,50);
    ussd_notification_data = charBuf;     
    Serial.println(ussd_notification_data);
    send_notification();
  }
}
else{
 ussd = 0;
}

}


void set_loop_delay(){
  loop_delay = main_loop_delay;
}

void register_device(){
  start_ussd_session();                                      // not a notification therefore 0
  get_ussd_response();  
  if (received_command == "Cont") {
    Serial.println("Sending register request");    
    send_ussd_response(register_request);
    get_ussd_response();
    if (received_command == "Fin") {
      Serial.println("End of the session");
  }
 }  
}

//
void poll_command(){
  Serial.println("Polling..");
  start_ussd_session();                                      // not a notification therefore 0
  get_ussd_response();    
  if (received_command == "Cont") {    
    send_ussd_response(command_request);
    get_ussd_response();
  }  
}

void start_ussd_session(){
    ussd_code = application_code;
    Serial.println(ussd_code);
    ussd_acc.initSession(ussd_code);
}

//
void send_notification(){  
  Serial.println("Sending a ussd response");
  Serial.println(ussd_notification_data);
  start_ussd_session();                                        // a notification therefore 1
  get_ussd_response();  
    if (received_command == "Cont") {   
    Serial.println("Sending the notification request Ntfy");    
    send_ussd_response(ussd_notification_data);
         get_ussd_response();           
         if (received_command == "Fin") {
           Serial.println("End of the session");
         }
  }
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

void execute_command(){
  
  received_command = "CmdL:print a0;";
  String extracted_command =getValue(received_command,':',0);
  Serial.println("Executing the command");
  Serial.println(extracted_command);
  
  if (received_command=="Fin") {
      Serial.println("No command received. Nothing to do.");
     // do nothing
  }
  else if (extracted_command=="Cmd" || extracted_command=="CmdL") {
      Serial.println(received_command);
      Serial.println("Command recieved");  
      extract_and_execute_instructions_new(getValue(received_command,':',1));
  } 
}

void extract_and_execute_instructions_new(String instruction){
    Serial.println("instructions");     
    String temp = instruction.substring(0, instruction.length() - 1);   
    String current_command =  getValue(temp,';',0) ;
    Serial.println(current_command);
    String command_action = getValue(current_command,' ',0);
    String command_pin = getValue(current_command,' ',1);

    
    if (command_action=="print"){
      Serial.println("Read the sensor value: ");
      int x = read_input_pin(String(command_pin[1]).toInt(),(String)command_pin[0]);
      String r1 = "NtfyL:" ;                     // r1 temporary string
      r1.concat(command_pin);
      r1.concat("=");
      r1.concat(String(x));
      
      char charBuf[50];                           // charBuf temporary char buffer
      r1.toCharArray(charBuf,50);
      ussd_notification_data = charBuf;

      Serial.println(ussd_notification_data);
      send_notification();
    }
} 

int read_input_pin(int pin, String read_type){
  Serial.println("read function");
  if (read_type == "d")
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




