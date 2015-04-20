// This configuartion file is the only place the user has to change //


char application_code[] = "#771*476";     // the # code of the application to which the GSM module is to initialte the USSD session. "#771*476*0#"; // "#132#"
long main_loop_delay = 60000;          // periodic time in milliseconds at which the arduino will poll for a command from application
long periodic_check_timer = 120000;     // periodic time in milliseconds at which the arduino will initiate a notification ussd session

char sim_number[] = "0771234567";


