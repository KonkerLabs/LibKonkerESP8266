#ifndef MQTTConn
#define MQTTConn


bool connectMQTT(char r_server[], int r_port, char r_device_login[], char r_device_pass[]){
    Serial.print("Connecting to MQTT broker ");


	  Serial.print(" URI:" + String(server) + " Port:" + String(port) + ", ");

	  client.setServer(r_server, r_port);
	  client.setCallback(callback);

	  Serial.print(" U:" + String(r_device_login) + " P:" + String(r_device_pass));
	  
		int connectCode=client.connect(r_device_login, r_device_login, r_device_pass);

		Serial.println(", connectCode=" + String(connectCode));


    if (connectCode==1) { //Check the returning code
      return 1;
    }else{
      return 0;
		}
}


bool connectMQTT(){

	return connectMQTT(server,port,device_login,device_pass);

}




#ifdef konkerMQTTs
#include "HELPERS/secureHelper.h"
#endif

#endif
