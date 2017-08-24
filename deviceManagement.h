#ifndef deviceManagement
#define deviceManagement


String helthMonitorFile="/health.json";

int _netFailureCount=0;
int _mqttFailureCount=0;

void FailureUpdate(bool netFail, bool mqttFail){
  char chrNetFail[5]="";//getJsonItemFromFile(helthMonitorFile,(char*)"n");
  char chrMqttFail[5]="";//getJsonItemFromFile(helthMonitorFile,(char*)"m");

   getJsonItemFromFile(helthMonitorFile,(char*)"n",chrNetFail);
   getJsonItemFromFile(helthMonitorFile,(char*)"m",chrMqttFail);


  _netFailureCount=(chrNetFail[0] != '\0') ? atoi(chrNetFail) + netFail : netFail;
  _mqttFailureCount=(chrMqttFail[0] != '\0') ? atoi(chrMqttFail) + mqttFail : mqttFail;

  //nothing to update if everithing is zero
  if(_netFailureCount==0 && _mqttFailureCount==0){
    return;
  }

  if (_netFailureCount>9999){
    _netFailureCount=1;
  }
  if (_mqttFailureCount>9999){
    _mqttFailureCount=1;
  }

  String json="{\"n\":" + (String)_netFailureCount + ",\"m\":" + (String)_mqttFailureCount + "}";
  char charJson[24];
  json.toCharArray(charJson, 24);
  updateJsonFile(helthMonitorFile,charJson);
}


void healthUpdate(char *_health_channel){
  Serial.println("Health check..");
  FailureUpdate(0,0);

  //nothing to update if everithing is zero
  if(_netFailureCount==0 && _mqttFailureCount==0){
    return;
  }

  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& jsonMSG = jsonBuffer.createObject();

  delay(10);

	//jsonMSG["chipid"] = String(getChipId());
  jsonMSG["networkFail"] = _netFailureCount;
  jsonMSG["mqttFail"] = _mqttFailureCount;

  char mensagemjson[1024];
	jsonMSG.printTo(bufferJ, sizeof(bufferJ));
  strcpy(mensagemjson,bufferJ);
	Serial.println("Publishing on channel:" + (String)_health_channel);
	Serial.println("The message:");
	Serial.println(mensagemjson);
	if(PUB(_health_channel, mensagemjson)){

    //IMPLEMENTING THIS CODE BELOW, THE COUNTER IS ZEROED.
    //IF COMMENTED IT WILL ACCUMULATE
    int _netFailureCount=0;
    int _mqttFailureCount=0;
    String json="{\"n\":" + (String)_netFailureCount + ",\"m\":" + (String)_mqttFailureCount + "}";
    char charJson[24];
    json.toCharArray(charJson, 24);
    updateJsonFile(helthMonitorFile,charJson);
  }

}




void checkFWUpdates(String SERVER_URI, int PORT, String BIN_PATH){
    Serial.println(SERVER_URI);
    Serial.print(BIN_PATH);
    Serial.println("");
    t_httpUpdate_return ret = ESPhttpUpdate.update(SERVER_URI, PORT, BIN_PATH);
    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.println("[update] Update failed.");
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[update] Update no Update.");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("[update] Update ok."); // may not called we reboot the ESP
            break;
    }
  }


void checkFWUpdates(String SERVER_URI, String BIN_PATH){
  checkFWUpdates(SERVER_URI,80,BIN_PATH);
}

#ifdef REST
char *parseCom(String msg){
  char *RESTcmd;
  const char *cmd;

  StaticJsonBuffer<1024> jsonBuffer;
  JsonArray& array = jsonBuffer.parseArray(msg);
  JsonObject& jsonMSG = array[0]["data"];

  if (jsonMSG.containsKey("command")) cmd = jsonMSG["command"];

  RESTcmd = (char*) cmd;
  return RESTcmd;
}



void trataMsgFw(char msg[]){
  String msgstr =String(msg);
  Serial.println("msgstr");
  Serial.println(msgstr);

  char *type = parseCom(msgstr);
  Serial.println("type=" + String(type));
	/*
  if (String(type)=="reset") {
      wifiManager.resetSettings();
      delay(500);
      ESP.restart();
  }*/

  if (String(type)=="update") {
    Serial.println("Checking FW");
    String SERVER_URI=String(parse_JSON_dataItem(msg,(char*)"u"));
    String PORT=String(parse_JSON_dataItem(msg,(char*)"p"));
    String BIN_PATH=String(parse_JSON_dataItem(msg,(char*)"b"));

    Serial.println("SERVER_URI:" +SERVER_URI);
    Serial.println("PORT:" +PORT);
    Serial.println("BIN_PATH:" +BIN_PATH);
    if (BIN_PATH.charAt(0)!='/'){
      BIN_PATH="/" + BIN_PATH;
    }

    checkFWUpdates(SERVER_URI, PORT.toInt(), BIN_PATH);
  }
}



#else





void trataMsgFw(char msg[]){
  char type[64] ;
  parse_JSON_item(msg,(char*)"command",type);
  Serial.println("type=" + String(type));

  /*if (String(type)=="reset") {
      wifiManager.resetSettings();
      delay(500);
      ESP.restart();
  }*/

  if (String(type)=="update") {
    Serial.println("Checking FW");
    char SERVER_URI[64];
    char PORT[7];
    char BIN_PATH[1024];
    parse_JSON_item(msg,(char*)"u",SERVER_URI);
    parse_JSON_item(msg,(char*)"p",PORT);
    parse_JSON_item(msg,(char*)"b",BIN_PATH);

    Serial.println("SERVER_URI:" +(String)SERVER_URI);
    Serial.println("PORT:" +(String)PORT);
    Serial.println("BIN_PATH:" +(String)BIN_PATH);




    if (BIN_PATH[0]!='/'){
      char BIN_PATH_tmp[1024]={'/'};
      strcat(BIN_PATH_tmp, BIN_PATH);
      strcpy(BIN_PATH,BIN_PATH_tmp);
    }

    checkFWUpdates((String)SERVER_URI, atoi(PORT), (String)BIN_PATH);
  }
}



#endif
#endif
