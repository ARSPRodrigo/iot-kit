#include <GSM.h>
#include <bitlash.h>
#include "USSDAccess.h"
#include "configure.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #define serialPrintByte(b) Serial.write(b)
#else
  #define serialPrintByte(b) Serial.print(b,BYTE)
#endif


USSDAccess ussd_acc;

char* ussd_code;
char* ussd_notification_data;
char* command;
char* register_request;

char* saved = "saved";
char* notify_request = "NtfyL";
char* command_request = "Cmd";
char* stop_process = "{stop 0;}";

int loop_delay;
int max_resp_len = 175;	                                       
int exist_new_command = 1;
int notification_sent = 0;
int poll_timer = periodic_check_timer;
int is_registered = 0;

bool toggle = false;

String received_command = "";
String return_data="";


void set_bitlash_command(char* received_command){
   command = received_command ;
}

void run_bitlash_command(){ 
    doCommand(command);
}

void serialHandler(byte b) {
  if (b == 13 ){
    if (return_data.equals("saved")){
      return_data = "";       
    }
    else {
      char charBuf[200];                              
      return_data.toCharArray(charBuf,200);
      ussd_notification_data = charBuf;
    //  toggle = !toggle;
    //  if(toggle) {
    //    digitalWrite(7, 1);
    //  } else {
    //    digitalWrite(7, 0);
    //  }
      Serial.println("Preparing to send the notification");
      send_notification();

      return_data = "";
    }   
  }
  else {
    if (b == 10){
      //
    }
    else{
      return_data.concat((char)b);       
    }  
  }
}

void setup() {  
  initBitlash(9600);  
  addBitlashFunction("run_bitlash_command", (bitlash_function) run_bitlash_command);
  setOutputHandler(&serialHandler);
  register_device();
  //pinMode(7, OUTPUT);
}

void loop() {
  runBitlash();

  if (exist_new_command==1){
    execute_command();
    exist_new_command = 0;
  }
  
  poll_timer = poll_timer + main_loop_delay;
  delay(main_loop_delay);
}


void register_device(){
  start_ussd_session();                                      
  get_ussd_response();  
  if (received_command == "Cont") {
    Serial.println("Sending register request");
    String temp = "Reg:";  
    temp.concat(registered_name);
    char charBuf[200];                              
    return_data.toCharArray(charBuf,200);
    register_request = charBuf; 
    send_ussd_response(register_request);
    
    Serial.println("End of the registration session");
    get_ussd_response();
    
    if (received_command == "Fin") {
      is_registered = 1 ;
    }
  }  
}


void poll_command(){
  Serial.println("Polling..");
  start_ussd_session();                                      
  get_ussd_response();    
  if (received_command == "Cont") {    
    send_ussd_response(command_request);
    get_ussd_response();
    if (received_command == ""){
      exist_new_command == 0;
    }
    else{
      exist_new_command == 1;
      notification_sent = 0;
    }
  }  
}

void start_ussd_session(){
  ussd_code = application_code; 
  Serial.println(ussd_code);
  ussd_acc.initSession(ussd_code);
}


void send_notification(){  
  Serial.println("Sending a ussd response");
  Serial.println(ussd_notification_data);
  start_ussd_session();                                        
  get_ussd_response();  
  if (received_command == "Cont") {   
    Serial.println("Sending the notification Ntfy");    
    send_ussd_response(ussd_notification_data);
    get_ussd_response();           
    if (received_command == "Fin") {
      Serial.println("End of the session");
    }
  }
}

void send_ussd_response(char* ussd_response){
  ussd_acc.respond(ussd_response);  
}

void get_ussd_response(){
  char resp[max_resp_len];
  Serial.println("Receiving..");
  ussd_acc.getResponse(resp);
  Serial.println(resp);
  
  received_command = getValue(resp, '\"',1);
  Serial.println(received_command);
}



void extract_and_execute_instructions_new(String action, String instruction){
  if (action == "c"){
    Serial.println("Command is executing.."); 
    char charBuf[200];                              
    instruction.toCharArray(charBuf,200);
    char* bitlash_command = charBuf;
    doCommand(bitlash_command);
  }
  else if (action == "f"){ 
  String temp1 = "function iot_function {";        
  temp1.concat(instruction);                      // eg: "function iot_function {pinMode(13, 1); dw(13,1);  if(a0< 200){print \"level=\"a0;}; snooze(2000); }"
  temp1.concat("}");
  char charBuf[200];                              
  temp1.toCharArray(charBuf,200);
  char* bitlash_command = charBuf;

  set_bitlash_command(stop_process);
  set_bitlash_command(bitlash_command);
  run_bitlash_command();
  bitlash_command = "run iot_function;";
  set_bitlash_command(bitlash_command);
  run_bitlash_command();
  Serial.println("Bitlash command running..");  
} 
}


void execute_command(){  
  //received_command = "CmdL:while 1 {if(!d13){print \"light is toggling\";}; snooze(100);};";  
    received_command = "CmdL:f:while 1 {if(!d13){print \"light is toggling\";}; snooze(100);};";  
  // received_command = "CmdL:c: print d13;;"; 
  //perform the replacements:
  String temp_string = received_command;
  temp_string.replace("'", "\"");
  received_command = temp_string;
  
  String extracted_command =getValue(received_command,':',0);   
  
  if (received_command=="Fin") {
    Serial.println("No command received. Nothing to do.");     
  }
  else if (extracted_command=="Cmd" || extracted_command=="CmdL") {
    Serial.println("Command ready: " + getValue(received_command,':',1));  
    extract_and_execute_instructions_new(getValue(received_command,':',0),getValue(received_command,':',1));     
  } 
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

