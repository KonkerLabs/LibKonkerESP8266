#ifndef MQTTConn
#define MQTTConn


bool connectMQTT(String id){
  int result=0;
	int MQTTcode = client.state();

  if (MQTTcode==0){
     result= 1;
  }else{
    Serial.print("Connecting to MQTT broker ");
    //generate ramdom ID
    int i=0;
    int random_n = random(10000);
    String temp_id = id + String (random_n);
    const int diment = temp_id.length()+1;
    char newid[diment];
    temp_id.toCharArray(newid, temp_id.length()+1);


    //mqtt://mqtt.demo.konkerlabs.net:1883
    String strmqtt_server;
    String strpath;
    String strmqtt_port;
    parseURLtoURIePath(server,strmqtt_server, strpath, strmqtt_port);
    char mqtt_server[64];
    char mqtt_port[5];
    strmqtt_server.toCharArray(mqtt_server, 64);
    strmqtt_port.toCharArray(mqtt_port, 5);

    Serial.print(" URI:" + String(mqtt_server) + " Port:" + String(atoi(mqtt_port)) + ", ");

    client.setServer(mqtt_server, atoi(mqtt_port));
    client.setCallback(callback);
    //try to connect
    Serial.print(" U:" + String(device_login) + " P:" + String(device_pass));
    int connectCode=client.connect(newid, device_login, device_pass);
    Serial.println(", connectCode=" + String(connectCode));

    if (connectCode==1) { //Check the returning code
      return 1;
    }else{
      return 0;
    }

  }

  if (MQTTcode==0) { //Check the returning code
    return 1;
  }else{
    return 0;
  }

}

#ifdef konkerMQTTs
#include "HELPERS/secureHelper.h"
#endif

#endif
