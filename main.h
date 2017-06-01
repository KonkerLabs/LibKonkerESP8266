#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "BlinkerID.h"
#include "parseURL.h"
#include "jsonhelper.h"
#include "chanTuple.h"
#include "subChanHelper.h"


//Trigger da configuracao via Json -- Tambem precisa ser uma variavel global para rodar em duas maquinas de estado.
bool shouldSaveConfig = 0;

bool failedComm=-1;

String configured_server;
char configured_device_login[32];
char configured_device_pass[32];
char fwVersion[7]="";
//flag de entrada de msg
int received_msg=0;
//tópico de entrada ;
char receivedTopic[32];
//Buffer de entrada ;
char receivedTopicMsg[2048];

//Definindo os objetos de Wifi e pubsubclient
WiFiClient espClient;
unsigned long previousMillis = 0;

#ifdef MQTT
  //SÓ PARA MQTT
  PubSubClient client(espClient);
#endif
#ifdef REST
  //SÓ PARA REST
  int restSubscribePeriord=1000;
#endif

WiFiManager wifiManager;

//Variaveis da Configuracao em Json
char server[64];
char device_login[32];
char device_pass[32];

//Criando as variaveis dentro do WiFiManager
WiFiManagerParameter custom_server("server", "server", server, 64);
WiFiManagerParameter custom_device_login("login", "device login", device_login, 32);
WiFiManagerParameter custom_device_pass("password", "device pass", device_pass, 32);



#include "msgTuple.h"




char chan0[7] = "fw";

char channelStatus[7] = "status";

const char sub_dev_modifier[4] = "sub";
const char pub_dev_modifier[4] = "pub";

String NAME="Sensor";
String chipid = NAME + String(ESP.getChipId());




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
    strcpy(receivedTopic, topic);
    received_msg = 1;
    Serial.println("");
  }
#endif


#ifdef REST

  //SÓ PARA REST
  // Funcao de Callback
  void callback(char* topic, String payload) {

    int i;
    int state=0;

    Serial.print("Message received [");
    Serial.print(topic);
    Serial.print("] ");

    payload.toCharArray(receivedTopicMsg, 2048);

    strcpy(receivedTopic, topic);
    received_msg = 1;
    Serial.println("");


  }
#endif







//----------------- Funcao de Trigger para salvar configuracao no FS ---------------------------
void saveConfigCallback() {
  Serial.println("Save the configuration");
  shouldSaveConfig = 1;
}


#include "topicBuilder.h"

#ifdef MQTT
  //SÓ PARA MQTT
  #include "MQTT\pubsubMQTTfunctions.h"

  #include "MQTT\testMQTTConn.h"
  //
#endif

#ifdef REST
  //SÓ PARA REST
  #include "REST\pubsubRESTfunctions.h"

  #include "REST\testRESTConn.h"
  //
#endif



#include "deviceManagement.h"

#include "checkMsgs.h"





//----------------- Copiando parametros configurados via HTML ---------------------------
void copyHTMLPar(char server[], char device_login[], char device_pass[], WiFiManagerParameter custom_server, WiFiManagerParameter custom_device_login, WiFiManagerParameter custom_device_pass){
  strcpy(server, custom_server.getValue());
  strcpy(device_login, custom_device_login.getValue());
  strcpy(device_pass, custom_device_pass.getValue());

}




//----------------- Montando o sistema de arquivo e lendo o arquivo config.json ---------------------------
void spiffsMount() {


  if (SPIFFS.begin()) {
    //Serial.println("Sistema de Arquivos Montado");
    if (SPIFFS.exists("/config.json")) {
      //Serial.println("Arquivo já existente, lendo..");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        //Serial.println("Arquivo aberto com sucesso");
        size_t size = configFile.size();
        // Criando um Buffer para alocar os dados do arquivo.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        //json.printTo(Serial);
        if (json.success()) {
          //Serial.println("\nparsed json");

          if (json.containsKey("server")) strcpy(server, json["server"]);
          if (json.containsKey("device_login")) strcpy(device_login, json["device_login"]);
          if (json.containsKey("device_pass")) strcpy(device_pass, json["device_pass"]);


          configured_server=String(server);
          strcpy(configured_device_login,device_login);
          strcpy(configured_device_pass, device_pass);


        } else {
          Serial.println("Can't read the config file!");
        }
      }
    }
  } else {
    Serial.println("Failed to build system file!");
  }

}



//----------------- Salvando arquivo de configuracao ---------------------------
void saveConfigtoFile(char server[], char device_login[], char device_pass[]){

  Serial.println("Saving config to file..");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  configured_server=String(server);
  strcpy(configured_device_login,device_login);
  strcpy(configured_device_pass,device_pass);
  json["server"] = server;
  json["device_login"] = device_login;
  json["device_pass"] = device_pass;



  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open file with write permitions!");
  }

  //json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
}


//----------Le o arquivo de configuração da memória
JsonObject& readConfigFile(){
  if (SPIFFS.begin()) {
    //Serial.println("Sistema de Arquivos Montado");
    if (SPIFFS.exists("/config.json")) {
      //Serial.println("Arquivo já existente, lendo..");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        //Serial.println("Arquivo aberto com sucesso");
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(configFile.readString());
        //json.printTo(Serial);
        if (json.success()) {
          //Serial.println("\nparsed json");

          return json;

        }
        else {
          Serial.println("Can't read the config file!");
        }
      }
    }
  } else {
    Serial.println("Failed to build system file!");
  }

}



//-----------------Abre o arquivo de configuração da memória e atualiza com o JSON recebido
int updateConfigJSONToFile(JsonObject& jsonToSave){
  int saved=0;
  JsonObject& filejson = readConfigFile();

  updateJSON(jsonToSave, filejson);


  if(saved==1){
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("Failed to open file with write permitions!");
    }

    filejson.printTo(Serial);
    filejson.printTo(configFile);
    configFile.close();
  }
}




void startKonkerAP(String apName, char server[], char device_login[], char device_pass[], WiFiManagerParameter custom_server, WiFiManagerParameter custom_device_login, WiFiManagerParameter custom_device_pass){
  if(wifiManager.autoConnect(blinkMyID(apName))==0) {
    //Copiando parametros
    strcpy(server, custom_server.getValue());
    strcpy(device_login, custom_device_login.getValue());
    strcpy(device_pass, custom_device_pass.getValue());

    delay(3000);
    ESP.reset();
  }
}


bool pubStatus(){
  StaticJsonBuffer<220> jsonBuffer;
  JsonObject& jsonMSG = jsonBuffer.createObject();


  delay(10);

  jsonMSG["deviceId"] = chipid;
  jsonMSG["fw"] = fwVersion;

  char *mensagemjson;
  mensagemjson = buildJSONmsg(jsonMSG);


  return PUB(channelStatus, mensagemjson);

}


//////////////////////////////
//função de inicialização
bool initialize(char firmwareVersion[]){
  bool success=1;
  Serial.println("Konker lib initialization");
  addSUBchan(chan0,0);

  //------------------- Montando Sistema de arquivos e copiando as configuracoes  ----------------------
  strcpy(fwVersion,firmwareVersion);

  spiffsMount();

  //Criando as variaveis dentro do WiFiManager
  WiFiManagerParameter custom_server("server", "server", server, 64);
  WiFiManagerParameter custom_device_login("login", "device login", device_login, 32);
  WiFiManagerParameter custom_device_pass("password", "device pass", device_pass, 32);

  //wifiManager.resetSettings();

  //wifiManager.resetSettings();SPIFFS.format();


  //------------------- Configuracao do WifiManager ----------------------
  wifiManager.setTimeout(800);
  wifiManager.setBreakAfterConfig(1);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_server);
  wifiManager.addParameter(&custom_device_login);
  wifiManager.addParameter(&custom_device_pass);



  if (!checkConnection(10, chipid)) {

    Serial.println("Can't connect! Reseting wifi setings..");
    wifiManager.resetSettings();//SPIFFS.format();
    delay(3000);
    startKonkerAP(NAME, server, device_login, device_pass, custom_server, custom_device_login, custom_device_pass);
  }



  Serial.println("Saving config to file....");
  saveConfigtoFile(server, device_login, device_pass);
  //------------------- Caso tudo mais falhe copie os dados para o FS ----------------------
  //Copiando parametros
  copyHTMLPar(server, device_login, device_pass, custom_server, custom_device_login, custom_device_pass);

  //Salvando Configuracao
  if (shouldSaveConfig) {
    Serial.println("Saving config to file....");
    saveConfigtoFile(server,device_login,device_pass);
  }
  delay(200);


  Serial.println();
  startBlinkID(0);// pass zero to stop
  //keep LED on
  digitalWrite(_STATUS_LED, HIGH);


  success=SUB(subChanArr);


  if(pubStatus()!=1){
    success=0;
  }

  Serial.println("End of Konker lib initialization");
  return success;
}


bool initialize(){
  char fw[7]="undef";
  return initialize(fw);
}
