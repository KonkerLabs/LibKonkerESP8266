int testMQTTConn(String id){
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


//----------------- Funcao para conectar ao broker MQTT e reconectar quando perder a conexao --------------------------------
int MQTTRetry(int retries, String id) {
  int i=0;

  //Loop ate estar conectado
  while (!testMQTTConn(id)) {
    Serial.print("Cheking connection with MQTT server...");
    if (testMQTTConn(id)) {
      Serial.println("Connected!");
      received_msg=0;
      return i;
    } else {
      Serial.println("Trying again in 3 seconds..");
      // Esperando 3 segundos antes de re-conectar
      delay(3000);
      if (i==retries) break;
      i++;
    }
}
  return i;
}

int checkConnection(int tentatives, String id){

  int result=0;
  if (WiFi.SSID()!="") {
    //MQTTPUB
    if (testMQTTConn(id)==0) {
      MQTTRetry(3, id);
      int auxTent=tentatives;
      while (tentatives>0 && testMQTTConn(id)==0) {
        Serial.println("Trying to reconnect.. " + String(auxTent-tentatives+1));
        WiFi.begin();
        delay(3000);
        tentatives--;
      }
      if (tentatives<=0) {
        Serial.println("End of tentatives");
        result= 0;
      }else{
        result= 1;
        //se teve que reconectar, faça subscribe denovo
        MQTTSUB(subChanArr);

      }
    }else{
      result= 1;
    }
  }else{
    Serial.println("Wifi not configured yet");
    result= 0;
  }
  return result;
}
