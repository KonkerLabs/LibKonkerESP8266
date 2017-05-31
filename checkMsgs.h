void CheckMSGChan(MsgTuple &returnTuple, char channel[]){
  String payload="[]";
  if(received_msg==1){
    char topic[32];
    buildSUBTopic(configured_device_login, channel, topic);
    if(strcmp(receivedTopic, topic)==0){
      payload= String(receivedTopicMsg);
    }
    received_msg=0;
  }

  if (payload!="[]"){
    setTuple(returnTuple,channel,receivedTopicMsg);
  }
}


void CheckMSGs(MsgTuple &returnTuple){

  #ifdef MQTT
    //SÓ PARA MQTT
    client.loop();
  #endif
  #ifdef REST
    //SÓ PARA REST
    if (millis() - previousMillis >= restSubscribePeriord) {
      previousMillis = millis();
      SUB(subChanArr);
    }

  #endif

  for (int i = 0; i < subChanSize; i++){
    if(strlen(subChanArr[i].chan) != 0){
      CheckMSGChan(returnTuple,subChanArr[i].chan);
      if(returnTuple.chan!='\0'){

        if (returnTuple.msg!='\0') {
          if(strcmp(returnTuple.chan, chan0) == 0){
            trataMsgFw(returnTuple.msg);//fixo  para sempre ser da mesma forma o update nativo
          }
        }

        return;
      }
    }
  }
}


void CheckMSGs(){
  MsgTuple returnTuple;
  CheckMSGs(returnTuple);
}