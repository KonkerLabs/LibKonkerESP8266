bool PUB(char channel[], char msg[]){
  int pubCode=-1;
  char topic[32];

  buildPUBTopic(configured_device_login, channel, topic);

  Serial.println("Publishing to: " + String(topic) + " msg: " + msg );
  Serial.print(">");
  pubCode=client.publish(topic, msg);

  if (pubCode!=1){
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
    char* chan=pTuple[i].chan;
    char* msg=pTuple[i].msg;
    if(!PUB(chan, msg){
      return 0;
    }
  }
  return 1;
}

bool SUB(char channel[]){
  int subCode=-1;
  char topic[32];

  buildSUBTopic(configured_device_login, channel, topic);

  Serial.println("Subscribing to: " + String(topic));
  Serial.print(">");
  subCode=client.subscribe(topic);

  if (subCode!=1){
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

bool SUB(ChanTuple chanArr[]){
  bool success=1;
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
