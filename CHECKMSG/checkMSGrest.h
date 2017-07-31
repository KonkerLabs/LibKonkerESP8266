#ifndef checkMSG
#define checkMSG

bool CheckMSGChan(MsgTuple &returnTuple, char *channel){
  Serial.println("Sem alterar");
  Serial.println(String(returnTuple.msg));
  if (millis() - previousMillis >= restSubscribePeriord) {
    previousMillis = millis();

    clearTuple(returnTuple);
    Serial.println("Cleared");
    Serial.println(String(returnTuple.msg));

    if(SUB(returnTuple, chan0)==0){
      return 0;
    }else{

      if(returnTuple.chan!='\0'){
        if (returnTuple.msg!='\0') {
          Serial.println("Pos Sub");
          Serial.println(String(returnTuple.msg));
          char strTS[13];
          parse_JSON_timestamp(String(returnTuple.msg),strTS,13);
          Serial.println("Pos parse_JSON_timestamp");
          Serial.println(String(returnTuple.msg));
          long long ts =char2LL(strTS);
          Serial.println("Pos char2LL");
          Serial.println(String(returnTuple.msg));

          if(setSUBchanNewTime(chan0,ts)){
            Serial.println("New ts!");
            Serial.println(String(returnTuple.msg));
            Serial.println();

            trataMsgFw(returnTuple.msg);
          }
        }
      }
    }

    clearTuple(returnTuple);
    if(SUB(returnTuple, channel)==1){
      return 1;
    }else{
      return 0;
    }



  }

  return 1;
}


bool CheckMSGs(MsgTuple *returnTuples){


  if (millis() - previousMillis >= restSubscribePeriord) {
    previousMillis = millis();


    MsgTuple returnTuplefw;
    if(SUB(returnTuplefw, chan0)==0){
      return 0;
    }else{
      if(returnTuplefw.chan!='\0'){
        if (returnTuplefw.msg!='\0') {
          char strTS[13];
          parse_JSON_timestamp(String(returnTuplefw.msg),strTS,13);
          long long ts =char2LL(strTS);
          if(setSUBchanNewTime(chan0,ts)){

            trataMsgFw(returnTuplefw.msg);
          }
        }
      }
    }



    clearTuples(returnTuples);
    if(SUB(returnTuples,subChanArr)==0){
        return 0;
    }
  }

  return 1;
}


bool CheckMSGs(){
  MsgTuple *returnTuple;
  return CheckMSGs(returnTuple);
}

#endif