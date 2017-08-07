#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "BlinkerID.h"
#include "HELPERS/parseURLHelper.h"
#include "HELPERS/jsonhelper.h"
#include "TUPLES/chanTuple.h"
#include "HELPERS/subChanHelper.h"



//Trigger da configuracao via Json -- Tambem precisa ser uma variavel global para rodar em duas maquinas de estado.
bool shouldSaveConfig = false;

bool failedComm=-1;

char server[64];
char device_login[32];
char device_pass[32];
char fwVersion[7]="";
//flag de entrada de msg
int received_msg=0;

#if defined(IGNORE_SAME_REST_TIMESTAMP) || defined(MQTT)
/// ignorar msgs com o mesmo timestamp
  //tópico de entrada ;
  char receivedTopic[32];
  //Buffer de entrada ;
  char receivedTopicMsg[2048];

#endif

//Definindo os objetos de Wifi e pubsubclient
#ifdef konkerMQTTs
	WiFiClientSecure espClient;
#else
	WiFiClient espClient;
#endif

unsigned long previousMillis = 0;

#ifdef MQTT
  //SÓ PARA MQTT
  PubSubClient client(espClient);
#endif
#ifdef REST
  //SÓ PARA REST
  int restSubscribePeriord=1000;
  //MsgTuple &returnTuple[subChanSize];
#endif



#include "TUPLES/msgTuple.h"




char chan0[7] = "fw";

char channelStatus[7] = "status";

const char sub_dev_modifier[4] = "sub";
const char pub_dev_modifier[4] = "pub";

String NAME="Device";

void setName(String newname){
  NAME =newname;
}

String getChipId(){
  return  NAME + String(ESP.getChipId());
}






#ifdef MQTT
//SÓ PARA MQTT
// Funcao de Callback
void callback(char* topic, byte* payload, unsigned int length) {
  int i;
  int state=0;

  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("] ");
  for (i = 0; i < length; i++) {
    receivedTopicMsg[i] = payload[i];
    Serial.print((char)payload[i]);
  }
  receivedTopicMsg[i] = '\0';
  strncpy(receivedTopic, topic, 32);
  received_msg = 1;
  Serial.println();
  Serial.println(".");
}
#endif


#ifdef REST
  #ifdef IGNORE_SAME_REST_TIMESTAMP
  /// ignorar msgs com o mesmo timestamp
  // Funcao de Callback
  void callback(char* topic, String payload) {

    int i;
    int state=0;

    Serial.print("Message received [");
    Serial.print(topic);
    Serial.print("] >");
    Serial.println(payload);
    Serial.println();
    payload.toCharArray(receivedTopicMsg, 2048);

    strncpy(receivedTopic, topic, 32);
    received_msg = 1;
  }
  #endif
#endif



#include "HELPERS/topicBuilderHelper.h"

#ifdef MQTT
  //SÓ PARA MQTT
  #include "MQTT/pubsubMQTTfunctions.h"

  #include "MQTT/MQTTConn.h"
  //
#endif

#ifdef REST
  //SÓ PARA REST
  #ifdef IGNORE_SAME_REST_TIMESTAMP
    // ignorar msgs com o mesmo timestamp
    #include "REST/pubsubRESTfunctionsTS.h"
  #else
    #include "REST/pubsubRESTfunctions.h"
  #endif

  #include "REST/testRESTConn.h"
  //
#endif



#include "deviceManagement.h"

#include "CHECKMSG/checkMSGs.h"


