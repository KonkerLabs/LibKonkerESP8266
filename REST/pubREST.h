#ifndef pubREST
#define pubREST

bool interpretHTTPCode(int httpCode){
  if (httpCode > 0 && httpCode<300) { //Check the returning code
    return 1;

  }else{
    return 0;
  }
}


bool PUB(char channel[], char msg[]){
  bool pubCode=0;
  char topic[32];

  buildPUBTopic(device_login, channel, topic);

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.setAuthorization(device_login, device_pass);
  http.begin(server + "/" + String(topic));  //Specify request destination

  int httpCode=http.POST(String(msg));
  Serial.println("Publishing to " + String(topic) + "; Body: " + String(msg) + "; httpcode: " + String(httpCode));
  Serial.print(">");
  http.end();   //Close connection

  pubCode=interpretHTTPCode(httpCode);

  if (!pubCode){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
    return 0;
  }else{
    Serial.println("sucess");
    Serial.println("");
    return 1;
  }

}

bool PUB(MsgTuple pTuple[]){
  int size=sizeof(pTuple)/sizeof(pTuple[0]);
  for (int i = 0; i < size; i++){
    if (!PUB(pTuple[i].chan, pTuple[i].msg)){
      return 0;
    }
  }
  return 1;
}

#endif