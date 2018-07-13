#ifndef testRESTConn
#define testRESTConn

bool testConn(String id){
  Serial.println("testConn");
  int result=0;

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(device_login, device_pass);
  Serial.print(" U:" + String(device_login) + " P:" + String(device_pass));

  char topic[32];

  buildSUBTopic(device_login, subChanArr[0].chan, topic);

  Serial.print(" HTTP.GET> " + server + "/" + String(topic));
  http.begin(server + "/" + String(topic)); //Specify request destination
  int httpCode = http.GET();
  Serial.println(", httpcode=" + String(httpCode));

  bool connectCode=interpretHTTPCode(httpCode);
  http.end();   //Close connection
  if (connectCode) { //Check the returning code
    return 1;
  }else{
    return 0;
  }
  return 0;


}


bool checkConnection(int tentatives, String channel){
  if (WiFi.SSID()!="") {
    if(failedComm==0){
      return 1;
    }
    Serial.println("Failed connection");
    Serial.println("Trying to reconnect..");
    for(int i=0;i<tentatives;i++){
      if (testConn(channel)) {
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

#endif
