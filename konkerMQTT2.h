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


//Trigger da configuracao via Json -- Tambem precisa ser uma variavel global para rodar em duas maquinas de estado.
bool shouldSaveConfig = 0;


String configured_mqtt_server;
char configured_device_login[32];
char configured_device_pass[32];


struct msgtuple
{
   char *chan={'\0'};
   char *msg={'\0'};
};

typedef struct msgtuple MsgTuple;

MsgTuple getMsgTuple(char chan[], char msg[]){
  MsgTuple newTuple;
  strcpy(newTuple.chan, chan);
  strcpy(newTuple.msg, msg);
  return newTuple;
}

void copyTuple(MsgTuple destiny, MsgTuple origin){
  destiny.chan=origin.chan;
  destiny.msg=origin.chan;
}

void setTuple(MsgTuple &destiny, char chan[], char msg[]){
  destiny.chan=chan;
  destiny.msg=msg;
}

char subChanArr[5][7] = {"fw"};



const char sub_dev_modifier[4] = "sub";
const char pub_dev_modifier[4] = "pub";


//flag de entrada de msg
int received_msg=0;
//tópico de entrada MQTT;
char receivedTopic[32];
//Buffer de entrada MQTT;
char receivedTopicMsg[2048];


//Definindo os objetos de Wifi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wifiManager;

String NOME="Sensor";
String chipid = NOME + String(ESP.getChipId());

//Variaveis da Configuracao em Json
char mqtt_server[64];
char device_login[32];
char device_pass[32];

//Criando as variaveis dentro do WiFiManager
WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 64);
WiFiManagerParameter custom_device_login("login", "device login", device_login, 32);
WiFiManagerParameter custom_device_pass("password", "device pass", device_pass, 32);



// Funcao de Callback para o MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  int i;
  int state=0;

  Serial.print("Mensagem recebida [");
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



void buildSUBTopic(char device_login[], char channel[], char *topic){
  String SString;
  SString = String(sub_dev_modifier) + String("/") + String(device_login) + String("/") + String(channel); //sub
  SString.toCharArray(topic, SString.length()+1);
}

char *buildSUBTopic(char channel[]){
  String SString;

  SString = String(sub_dev_modifier) + String("/") + String(device_login) + String("/") + String(channel); //pub
  char *topic;
  SString.toCharArray(topic, SString.length()+1);
  return topic;
}

void buildPUBTopic(char device_login[], char channel[], char *topic){
  String SString;
  SString = String(pub_dev_modifier) + String("/") + String(device_login) + String("/") + String(channel); //pub
  SString.toCharArray(topic, SString.length()+1);
}

char *buildPUBTopic(char channel[]){
  String SString;

  SString = String(pub_dev_modifier) + String("/") + String(device_login) + String("/") + String(channel); //pub
  char *topic;
  SString.toCharArray(topic, SString.length()+1);
  return topic;
}



int testMQTTConn(String id){
  int result=0;
  int MQTTcode = client.state();

  if (MQTTcode==0){
     result= 1;
  }else{
    Serial.print("Connecting to MQTT broker ");
    //generate ramdom ID
    int i=0;
    int random_n = random(10000);
    String temp_id = id + String (random_n);
    const int diment = temp_id.length()+1;
    char newid[diment];
    temp_id.toCharArray(newid, temp_id.length()+1);


    //mqtt://mqtt.demo.konkerlabs.net:1883
    String strmqtt_server;
    String strpath;
    String strmqtt_port;
    parseURLtoURIePath(configured_mqtt_server,strmqtt_server, strpath, strmqtt_port);
    char mqtt_server[64];
    char mqtt_port[5];
    strmqtt_server.toCharArray(mqtt_server, 64);
    strmqtt_port.toCharArray(mqtt_port, 5);

    Serial.print(" URI:" + String(mqtt_server) + " Port:" + String(atoi(mqtt_port)) + ", ");

    client.setServer(mqtt_server, atoi(mqtt_port));
    client.setCallback(callback);
    //try to connect
    Serial.print(" U:" + String(configured_device_login) + " P:" + String(configured_device_pass));
    int connectCode=client.connect(newid, configured_device_login, configured_device_pass);
    Serial.print(", connectCode=" + connectCode);

    if (connectCode==1) { //Check the returning code
      return 1;
    }else{
      return 0;
    }

  }

  if (MQTTcode==0) { //Check the returning code
    return 1;
  }else{
    return 0;
  }

}



//----------------- Funcao para conectar ao broker MQTT e reconectar quando perder a conexao --------------------------------
int MQTTRetry(int retries, String id) {
  int i=0;

  //Loop ate estar conectado
  while (!testMQTTConn(id)) {
    Serial.print("Checando conexão MQTT com o server...");
    if (testMQTTConn(id)) {
      Serial.println("Conectado!");
      received_msg=0;
      return i;
    } else {
      Serial.println("Tentando conectar novamente em 3 segundos");
      // Esperando 3 segundos antes de re-conectar
      delay(3000);
      if (i==retries) break;
      i++;
    }
}
  return i;
}






int MQTTSUB(char channel[]){
  int subCode=-1;
  char topic[32];

  buildSUBTopic(configured_device_login, channel, topic);

  Serial.println("Subscribing to: " + String(topic));
  Serial.print(">");
  subCode=client.subscribe(topic);

  if (subCode!=1){
    Serial.println("failed");
    Serial.println("");
  }else{
    Serial.println("sucess");
    Serial.println("");
  }

  return subCode;
}


int MQTTSUB(char chanArr[][7]){
  for (int i = 0; i < sizeof(chanArr)-1; i++){
    if(strlen(chanArr[i]) != 0){
      MQTTSUB(chanArr[i]);
    }
  }
}



void addSUBchan(char chan[]){
  for (int i = 1; i < sizeof(subChanArr)-1; i++){//sempre pula o canal fw que é o i=0
    if(strlen(subChanArr[i]) == 0){
      strcpy(subChanArr[i], chan);
      return;
    }
  }
}

int addSUBchanArr(char chanArr[][7]){
  for (int i = 0; i < sizeof(chanArr)-1; i++){
    if(strlen(chanArr[i]) != 0){
      addSUBchan(chanArr[i]);
    }
  }
}

void removeSUBchan(char chan[]){
  for (int i = 1; i < sizeof(subChanArr)-1; i++){//sempre pula o canal fw que é o i=0
    if(strcmp(subChanArr[i], chan)==0){
      memset(subChanArr[i], 0, sizeof(subChanArr[i]));
      return;
    }
  }
}

int MQTTPUB(char channel[], char msg[]){
  int pubCode=-1;
  char topic[32];

  buildPUBTopic(configured_device_login, channel, topic);

  Serial.println("Publishing to: " + String(topic) + " msg: " + msg );
  Serial.print(">");
  pubCode=client.publish(topic, msg);

  if (pubCode!=1){
    Serial.println("failed");
    Serial.println("");
  }else{
    Serial.println("sucess");
    Serial.println("");
  }

  return pubCode;
}

int MQTTPUB(MsgTuple pTuple[]){
  int size=sizeof(pTuple)/sizeof(pTuple[0]);
  for (int i = 0; i < size; i++){
    char* chan=pTuple[i].chan;
    char* msg=pTuple[i].msg;
    MQTTPUB(chan, msg);
  }
}




void MQTTCheckMSGChan(MsgTuple &returnTuple, char channel[]){
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




void trataMsgFw(char msg[]){
  char *type = parse_JSON_item(msg,"command");
  Serial.println("type=" + String(type));

  if (String(type)=="reset") {
      wifiManager.resetSettings();
      delay(500);
      ESP.restart();
  }

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

void MQTTCheckMSGs(MsgTuple &returnTuple){
  for (int i = 0; i < sizeof(subChanArr)-1; i++){
    if(strlen(subChanArr[i]) != 0){
      MQTTCheckMSGChan(returnTuple,subChanArr[i]);
      if(returnTuple.chan!='\0'){

        if (returnTuple.msg!='\0') {
          if(strcmp(returnTuple.chan, "fw") == 0){
            trataMsgFw(returnTuple.msg);//fixo  para sempre ser da mesma forma o update nativo
          }
        }

        return;
      }
    }
  }
}

void MQTTCheckMSGs(){
  MsgTuple returnTuple;
  for (int i = 0; i < sizeof(subChanArr)-1; i++){
    if(strlen(subChanArr[i]) != 0){
      MQTTCheckMSGChan(returnTuple,subChanArr[i]);
      if(returnTuple.chan!='\0'){

        if (returnTuple.msg!='\0') {
          if(strcmp(returnTuple.chan, "fw") == 0){
            trataMsgFw(returnTuple.msg);//fixo  para sempre ser da mesma forma o update nativo
          }
        }

        return;
      }
    }
  }
}

int checkConnection(int tentatives, String id){
  int result=0;
  if (WiFi.SSID()!="") {
    //MQTTPUB
    if (testMQTTConn(id)==0) {
      MQTTRetry(3, id);
      int auxTent=tentatives;
      while (tentatives>0 && testMQTTConn(id)==0) {
        Serial.println("Tentando reconectar.." + String(auxTent-tentatives+1));
        WiFi.begin();
        delay(3000);
        tentatives--;
      }
      if (tentatives<=0) {
        Serial.println("Fim de tentativas");
        result= 0;
      }else{
        result= 1;
        //se teve que reconectar, faça subscribe denovo
        MQTTSUB(subChanArr);

      }
    }else{
      result= 1;
    }
  }else{
    Serial.println("Wifi zerado");
    result= 0;
  }
  return result;
}





//----------------- Funcao de Trigger para salvar configuracao no FS ---------------------------
void saveConfigCallback() {
  Serial.println("Salvar a Configuracao");
  shouldSaveConfig = 1;
}


//----------------- Copiando parametros configurados via HTML ---------------------------
void copyHTMLPar(char mqtt_server[], char device_login[], char device_pass[], WiFiManagerParameter custom_mqtt_server, WiFiManagerParameter custom_device_login, WiFiManagerParameter custom_device_pass){
  strcpy(mqtt_server, custom_mqtt_server.getValue());
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

          if (json.containsKey("mqtt_server")) strcpy(mqtt_server, json["mqtt_server"]);
          if (json.containsKey("device_login")) strcpy(device_login, json["device_login"]);
          if (json.containsKey("device_pass")) strcpy(device_pass, json["device_pass"]);


          configured_mqtt_server=String(mqtt_server);
          strcpy(configured_device_login,device_login);
          strcpy(configured_device_pass, device_pass);


        } else {
          Serial.println("Falha em ler o Arquivo");
        }
      }
    }
  } else {
    Serial.println("Falha ao montar o sistema de arquivos");
  }

}



//----------------- Salvando arquivo de configuracao ---------------------------
void saveConfigtoFile(char mqtt_server[], char device_login[], char device_pass[]){

  Serial.println("Salvando Configuracao");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  configured_mqtt_server=String(mqtt_server);
  strcpy(configured_device_login,device_login);
  strcpy(configured_device_pass,device_pass);
  json["mqtt_server"] = mqtt_server;
  json["device_login"] = device_login;
  json["device_pass"] = device_pass;



  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Falha em abrir o arquivo com permissao de gravacao");
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
          Serial.println("Falha em ler o Arquivo");
        }
      }
    }
  } else {
    Serial.println("Falha ao montar o sistema de arquivos");
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
      Serial.println("Falha em abrir o arquivo com permissao de gravacao");
    }

    filejson.printTo(Serial);
    filejson.printTo(configFile);
    configFile.close();
  }
}




void startKonkerAP(String apNome, char mqtt_server[], char device_login[], char device_pass[], WiFiManagerParameter custom_mqtt_server, WiFiManagerParameter custom_device_login, WiFiManagerParameter custom_device_pass){
  if(wifiManager.autoConnect(blinkMyID(apNome))==0) {
    //Copiando parametros
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(device_login, custom_device_login.getValue());
    strcpy(device_pass, custom_device_pass.getValue());

    delay(3000);
    ESP.reset();
  }
}



//////////////////////////////
//função de inicialização
void inicializacao(){
  //------------------- Montando Sistema de arquivos e copiando as configuracoes  ----------------------

  spiffsMount();

  //Criando as variaveis dentro do WiFiManager
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 64);
  WiFiManagerParameter custom_device_login("login", "device login", device_login, 32);
  WiFiManagerParameter custom_device_pass("password", "device pass", device_pass, 32);

  //wifiManager.resetSettings();

  //wifiManager.resetSettings();SPIFFS.format();


  //------------------- Configuracao do WifiManager ----------------------
  wifiManager.setTimeout(800);
  wifiManager.setBreakAfterConfig(1);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_device_login);
  wifiManager.addParameter(&custom_device_pass);



  if (checkConnection(10, chipid)==0) {

    Serial.println("Não consegui conectar, vou resetar as configurações do Wifi");
    wifiManager.resetSettings();//SPIFFS.format();
    delay(3000);
    startKonkerAP(NOME, mqtt_server, device_login, device_pass, custom_mqtt_server, custom_device_login, custom_device_pass);
  }



  Serial.println("Saving config to file....");
  saveConfigtoFile(mqtt_server, device_login, device_pass);
  //------------------- Caso tudo mais falhe copie os dados para o FS ----------------------
  //Copiando parametros
  copyHTMLPar(mqtt_server, device_login, device_pass, custom_mqtt_server, custom_device_login, custom_device_pass);

  //Salvando Configuracao
  if (shouldSaveConfig) {
    Serial.println("Saving config to file....");
    saveConfigtoFile(mqtt_server,device_login,device_pass);
  }
  delay(200);


  Serial.println();
  startBlinkID(0);// pass zero to stop
  //keep LED on
  digitalWrite(_STATUS_LED, HIGH);

  MQTTSUB(subChanArr);

  Serial.println("Fim da inicialização");
}
