#ifndef deviceManagement
#define deviceManagement

String helthMonitorFile="/health.json";


void netFailureUpdate(){
  int _netFailureCount=0;
  _netFailureCount=atoi(getJsonItemFromFile(helthMonitorFile,(char*)"n"))+1;
  if (_netFailureCount>9999){
    _netFailureCount=0;
  }

  String json="{\"n\":" + (String)_netFailureCount + "}";
  char charJson[12];
  json.toCharArray(charJson, 12);
  updateJsonFile(helthMonitorFile,charJson);
}


void mqttFailureUpdate(){
  int _mqttFailureCount=0;
  _mqttFailureCount=atoi(getJsonItemFromFile(helthMonitorFile,(char*)"p"))+1;
  if (_mqttFailureCount>9999){
    _mqttFailureCount=0;
  }

  String json="{\"n\":" + (String)_mqttFailureCount + "}";
  char charJson[12];
  json.toCharArray(charJson, 12);
  updateJsonFile(helthMonitorFile,charJson);
}


void healthUpdate(char *_health_channel){
  int _netFailureCount=0;
  int _mqttFailureCount=0;

  _netFailureCount=atoi(getJsonItemFromFile(helthMonitorFile,(char*)"n"));
  _mqttFailureCount=atoi(getJsonItemFromFile(helthMonitorFile,(char*)"p"));

  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& jsonMSG = jsonBuffer.createObject();

  delay(10);

	jsonMSG["chipid"] = "\"" +  String(getChipId()) + "\"";
  jsonMSG["networkfail"] = _netFailureCount;
  jsonMSG["pubsubfail"] = _mqttFailureCount;

  char *mensagemjson;
	mensagemjson = buildJSONmsg(jsonMSG);
	Serial.println("Publishing on channel:" + (String)_health_channel);
	Serial.println("The message:");
	Serial.println(mensagemjson);
	PUB(_health_channel, mensagemjson);

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
  char *type = parse_JSON_item(msg,(char*)"command");
  Serial.println("type=" + String(type));

  /*if (String(type)=="reset") {
      wifiManager.resetSettings();
      delay(500);
      ESP.restart();
  }*/

  if (String(type)=="update") {
    Serial.println("Checking FW");
    String SERVER_URI=String(parse_JSON_item(msg,(char*)"u"));
    String PORT=String(parse_JSON_item(msg,(char*)"p"));
    String BIN_PATH=String(parse_JSON_item(msg,(char*)"b"));

    Serial.println("SERVER_URI:" +SERVER_URI);
    Serial.println("PORT:" +PORT);
    Serial.println("BIN_PATH:" +BIN_PATH);
    if (BIN_PATH.charAt(0)!='/'){
      BIN_PATH="/" + BIN_PATH;
    }

    checkFWUpdates(SERVER_URI, PORT.toInt(), BIN_PATH);
  }
}



#endif
#endif
