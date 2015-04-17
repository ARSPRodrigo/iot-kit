#ifndef USSDAccess_H
#define USSDAccess_H
#include <GSM.h>
#include <GSM3ShieldV1DirectModemProvider.h>

class USSDAccess : public GSM , public GSM3ShieldV1DirectModemProvider {
private:
	void connectModule();
	uint8_t string_length(char* string);
	void setATCommand(char* code, char type, char* dcs, char ATCommand[], int length_code, int length_dcs);
	int sendATCommand(char* ATcommand); 
	char decoder(int character);                    
public:
	USSDAccess();                
	int initSession(char* ussd_code);
	int endSession();
	int endSession(char* end_response);
	int respond(char* message);
	void getResponse(char response[]);            
};

#endif
