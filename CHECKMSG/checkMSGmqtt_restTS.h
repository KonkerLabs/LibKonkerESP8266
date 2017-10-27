#ifndef checkMSGmqtt_rest
#define checkMSGmqtt_rest

bool CheckMSGChan(MsgTuple &returnTuple, char channel[]){
  #ifdef MQTT
    //SÓ PARA MQTT
    if (!client.loop()){
      return 0;
    }
  #endif
  String payload="[]";
  if(received_msg==1){
    char topic[32];
    buildSUBTopic(device_login, channel, topic);

    if(strcmp(receivedTopic, topic)==0){

      payload= String(receivedTopicMsg);
    }
    received_msg=0;

  }

  if (payload!="[]"){
    setTuple(returnTuple,channel,receivedTopicMsg);
  }
	return 1;
}


bool CheckMSGs(MsgTuple &returnTuple){

  #ifdef MQTT
    //SÓ PARA MQTT
    if (!client.loop()){
      return 0;
    }
  #endif
  #ifdef REST
    //SÓ PARA REST
    if (millis() - previousMillis >= restSubscribePeriord) {
      previousMillis = millis();
      if (!SUB(subChanArr)){
        return 0;
      }
    }

  #endif

  for (int i = 0; i < subChanSize; i++){
    if(strlen(subChanArr[i].chan) != 0){
      if(returnTuple.chan!='\0'){

        if (returnTuple.msg!='\0') {
          //if(strcmp(returnTuple.chan, chan0) == 0){
          //  trataMsgFw(returnTuple.msg);//fixo  para sempre ser da mesma forma o update nativo
          //}else{
            CheckMSGChan(returnTuple,subChanArr[i].chan);
          //}
        }
      }
    }
  }
  return 1;
}


bool CheckMSGs(){
  MsgTuple returnTuple;
  return CheckMSGs(returnTuple);
}

#endif
