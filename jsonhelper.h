//Buffer das mensagens MQTT
char bufferJ[256];



char *buildJSONmsg(JsonObject& jsonMSG){
  jsonMSG.printTo(bufferJ, sizeof(bufferJ));
  return bufferJ;
}



//----------------- Decodificacao da mensagem Json In -----------------------------
char *parse_JSON_item(String json, const char itemName[]){
  const char *ival = "null";
  StaticJsonBuffer<2048> jsonBuffer;
  JsonObject& jsonMSG = jsonBuffer.parseObject(json);
    if (jsonMSG.containsKey(itemName)) ival = jsonMSG[itemName];
  char *itemval = (char*)ival;
  return itemval;
}

char *parse_JSON_timestamp(String json){
  const char *ival = "null";
  StaticJsonBuffer<2048> jsonBuffer;
  JsonObject& jsonMSG = jsonBuffer.parseObject(json);
  if (jsonMSG.containsKey("timestamp")) ival = jsonMSG["timestamp"];
  char *itemval = (char*)ival;
  return itemval;
}


int updateJSON(JsonObject& jsonToUpdate, JsonObject& jsonNewValues){
  //Serial.println("Checando valores recebidos..");
  for (JsonObject::iterator it=jsonNewValues.begin(); it!=jsonNewValues.end(); ++it) {
     String keyNameToSave=it->key;
     if (jsonToUpdate.containsKey(keyNameToSave)){
       Serial.print("Key json encontrada: " + keyNameToSave);
       String fileValue=jsonToUpdate[keyNameToSave];
       if(fileValue!=(it->value) ){
         jsonToUpdate[keyNameToSave]=(it->value);
         Serial.println(", valor atualizado!");
       }else{
         //Serial.println(", sem alterações de valor.");
       }
     }
  }

}
