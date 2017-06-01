bool testConn(String id){
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
    parseURLtoURIePath(configured_server,strmqtt_server, strpath, strmqtt_port);
    char mqtt_server[64];
    char mqtt_port[5];
    strmqtt_server.toCharArray(mqtt_server, 64);
    strmqtt_port.toCharArray(mqtt_port, 5);

    Serial.print(" URI:" + String(mqtt_server) + " Port:" + String(atoi(mqtt_port)) + ", ");

    client.setServer(mqtt_server, atoi(mqtt_port));
    client.setCallback(callback);
    //try to connect
    Serial.print(" U:" + String(configured_device_login) + " P:" + String(configured_device_pass));
    int connectCode=client.connect(newid, configured_device_login, configured_device_pass);
    Serial.print(", connectCode=" + connectCode);

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


bool checkConnection(int tentatives, String id){
  if (WiFi.SSID()!="") {
    if(failedComm==0){
      return 1;
    }
    Serial.println("Failed connection");
    Serial.println("Trying to reconnect..");
    for(int i=0;i<tentatives;i++){
      if (!testConn(id)) {
        Serial.println("Connected!");
        failedComm=0;
        return 1;
      }
      if(i+1<tentatives){
          Serial.println("Failed! Trying again in 3 seconds.. try " + String(i+1) +" of " + String(tentatives));
          // Esperando 3 segundos antes de re-conectar
          delay(3000);
      }
    }
    Serial.println("End of tentatives!");
    failedComm=1;
  }
  Serial.println("No wifi configured!");
  return 0;
}
