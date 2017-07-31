#ifndef deviceManagement
#define deviceManagement

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

  StaticJsonBuffer<2048> jsonBuffer;
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
    String SERVER_URI=String(parse_JSON_dataItem(msg,"u"));
    String PORT=String(parse_JSON_dataItem(msg,"p"));
    String BIN_PATH=String(parse_JSON_dataItem(msg,"b"));

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
  char *type = parse_JSON_item(msg,"command");
  Serial.println("type=" + String(type));

  /*if (String(type)=="reset") {
      wifiManager.resetSettings();
      delay(500);
      ESP.restart();
  }*/

  if (String(type)=="update") {
    Serial.println("Checking FW");
    String SERVER_URI=String(parse_JSON_item(msg,"u"));
    String PORT=String(parse_JSON_item(msg,"p"));
    String BIN_PATH=String(parse_JSON_item(msg,"b"));

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
