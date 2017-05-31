int interpretHTTPCode(int httpCode){
  if (httpCode > 0 && httpCode<300) { //Check the returning code
    return 1;

  }else{
    return 0;
  }
}


int PUB(char channel[], char msg[]){
  int pubCode=-1;
  char topic[32];

  buildPUBTopic(configured_device_login, channel, topic);

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.setAuthorization(configured_device_login, configured_device_pass);
  http.begin(configured_server + "/" + String(topic));  //Specify request destination

  int httpCode=http.POST(String(msg));
  Serial.println("Publishing to " + String(topic) + "; Body: " + String(msg) + "; httpcode: " + String(httpCode));
  Serial.print(">");
  http.end();   //Close connection

  pubCode=interpretHTTPCode(httpCode);

  if (pubCode!=1){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
  }else{
    Serial.println("sucess");
    Serial.println("");
  }

  return pubCode;
}

int PUB(MsgTuple pTuple[]){
  int size=sizeof(pTuple)/sizeof(pTuple[0]);
  for (int i = 0; i < size; i++){
    char* chan=pTuple[i].chan;
    char* msg=pTuple[i].msg;
    PUB(chan, msg);
  }
}


int SUB(char channel[]){
  int subCode=-1;
  char topic[32];

  buildSUBTopic(configured_device_login, channel, topic);

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(configured_device_login, configured_device_pass);
  http.begin(configured_server + "/" + String(topic));  //Specify request destination
  int httpCode = http.GET();

  Serial.println("Subscribing to: " + String(topic) + "; httpcode:" + String(httpCode));
  Serial.print(">");

  subCode=interpretHTTPCode(httpCode);

  if (subCode!=1){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
  }else{
    Serial.println("sucess");
    Serial.println("");
  }


  if (httpCode > 0) { //Check the returning code
    String strPayload = http.getString();
     http.end();   //Close connection
     if (strPayload!="[]"){
      long long ts =char2LL(parse_JSON_timestamp(strPayload));
       if(setSUBchanNewTime(channel,ts)){
         callback(topic, strPayload);
       }
     }

   }else{
     return 0;
   }


  return subCode;
}


int SUB(ChanTuple chanArr[]){
  int success=1;
  for (int i = 0; i < sizeof(chanArr)-1; i++){
    if(strlen(chanArr[i].chan) != 0){
      if(SUB(chanArr[i].chan)!=1){
        success=0;
        failedComm=1;
      }
    }
  }
  return success;
}
