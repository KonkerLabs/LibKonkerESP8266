#ifndef jsonhelper
#define jsonhelper

#include "fileHelper.h"
#include "httpHelper.h"

#include <ArduinoJson.h>
//Buffer das mensagens MQTT
char bufferJ[256];

/*
long long char2LL_old(char *str)
{
long long result = 0; // Initialize result
// Iterate through all characters of input string and update result
for (int i = 0; str[i] != '\0'; ++i){


result = result*10 + str[i] - '0';
}

return result;
}*/


long long char2LL(char *str){
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


char *parse_JSON_item(JsonObject& jsonMSG, const char itemName[]){
	const char *ival = "null";
	if (jsonMSG.containsKey(itemName)) ival = jsonMSG[itemName];
	char *itemval = (char*)ival;
	return itemval;
}


char  *parse_JSON_dataItem(String json, String itemName){
	const char *ival = "null";
	StaticJsonBuffer<2048> jsonBuffer;
	JsonArray& array = jsonBuffer.parseArray(json);
	JsonObject& jsonMSG = array[0]["data"];
	if (jsonMSG.containsKey(itemName)) ival = jsonMSG[itemName];
	char *it = (char*) ival;
	
	return it;
}


void  parse_JSON_timestamp(String json, char *chrTS, int chrTSsize){
	const char *ival = "null";
	StaticJsonBuffer<2048> jsonBuffer;
	JsonArray& array = jsonBuffer.parseArray(json);
	JsonObject& jsonMSG = array[0]["meta"];
	if (jsonMSG.containsKey("timestamp")) ival = jsonMSG["timestamp"];
	//char *it = (char*)ival;
	strncpy(chrTS,(char*)ival,chrTSsize);
	
}



bool updateJSON(JsonObject& jsonToUpdate,  String keyNameToSave,  String itemValue){
	if (jsonToUpdate.containsKey(keyNameToSave)){
		Serial.print("Key json found: " + keyNameToSave);
		String fileValue=jsonToUpdate[keyNameToSave];
		if(fileValue!=itemValue ){
			jsonToUpdate[keyNameToSave]=itemValue;
			Serial.println(", value updated!");
		}else{
			//Serial.println(", sem alterações de valor.");
		}
	}else{
		// Key not found... Creating it
		jsonToUpdate.createNestedObject(keyNameToSave);
		jsonToUpdate[keyNameToSave]=itemValue;			
	}
}



bool updateJSON(JsonObject& jsonToUpdate, JsonObject& jsonNewValues){
	//Serial.println("Checando valores recebidos..");
	for (JsonObject::iterator it=jsonNewValues.begin(); it!=jsonNewValues.end(); ++it) {
		String keyNameToSave=it->key;
		updateJSON(jsonToUpdate,keyNameToSave,(it->value));
	}
	
}



bool updateJsonFile(String filePath, JsonObject& jsonNewValues){
	String fileContens;
	//first read file...
	Serial.println("Opening file to update");		
	if(openFile(filePath,&fileContens)){
		Serial.println("Parsing: " +fileContens);
	}else{
		return 0;
	}
	
	//updating file
	DynamicJsonBuffer jsonBuffer;
	JsonObject& fileJson = jsonBuffer.parseObject(fileContens);
	if (fileJson.success()) {			
		Serial.println("Updating json readed from file");		
		updateJSON(fileJson,jsonNewValues);
		
		Serial.println("Saving file with changed values..");
		File myFile = SPIFFS.open(filePath, "w");
		Serial.println("Saving...: "  + String(buildJSONmsg(fileJson)));
		fileJson.printTo(myFile);
		myFile.close();
		
		return 1;
	}else{
		Serial.println("Failed to read Json file");
		return 0;
	}
}

#endif

