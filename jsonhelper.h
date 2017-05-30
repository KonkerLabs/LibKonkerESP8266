//Buffer das mensagens MQTT
char bufferJ[256];

long long char2LL_old(char *str)
{
  long long result = 0; // Initialize result
  // Iterate through all characters of input string and update result
  for (int i = 0; str[i] != '\0'; ++i){


    result = result*10 + str[i] - '0';
  }

  return result;
}


long long char2LL(char *str)
{
  long long result = 0; // Initialize result
  long long m=1000000000000;
  // Iterate through all characters of input string and update result
  for (int i = 0; m>=1; ++i){
    result = result + str[i] * m;
    m=m/10;
  }

  return result;
}




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

char  *parse_JSON_timestamp(String json)
{
  const char *ival = "null";
  StaticJsonBuffer<2048> jsonBuffer;
  JsonArray& array = jsonBuffer.parseArray(json);
  JsonObject& jsonMSG = array[0]["meta"];
  if (jsonMSG.containsKey("timestamp")) ival = jsonMSG["timestamp"];
  char *it = (char*)ival;


  //OK I know the below code is bizzare, but I don't know why atoll() isn't working in PlataformIo
  return it;
}


int updateJSON(JsonObject& jsonToUpdate, JsonObject& jsonNewValues){
  //Serial.println("Checando valores recebidos..");
  for (JsonObject::iterator it=jsonNewValues.begin(); it!=jsonNewValues.end(); ++it) {
     String keyNameToSave=it->key;
     if (jsonToUpdate.containsKey(keyNameToSave)){
       Serial.print("Key json found: " + keyNameToSave);
       String fileValue=jsonToUpdate[keyNameToSave];
       if(fileValue!=(it->value) ){
         jsonToUpdate[keyNameToSave]=(it->value);
         Serial.println(", value updated!");
       }else{
         //Serial.println(", sem alterações de valor.");
       }
     }
  }

}
