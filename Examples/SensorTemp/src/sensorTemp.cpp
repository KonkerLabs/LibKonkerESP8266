#include <FS.h>
#include <Arduino.h>
#include <NTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <konkerMQTT.h>

///variáveis de aplicação
char firmware[7]="FW2.2";

#define ONE_WIRE_BUS 2// Dados do sensor de temperatura no pino 2


int pinoHALL=4;


int estadoPorta; // LOW=fechada - HIGH =aberta
int ultimoEstadoPorta;//guarda o último valor do estado da porta

char channelTemperatura[7] = "data";

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

// Armazena temperaturas minima e maxima
float tempMin = 999;
float tempMax = 0;

WiFiUDP ntpUDP;
int16_t utc = -3; //UTC -3:00 Brazil

NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);


char *mensagemjson;
int lasttimeCheck=0;


void mostra_endereco_sensor(DeviceAddress deviceAddress){
 for (uint8_t i = 0; i < 8; i++) {
   // Adiciona zeros se necessário
   if (deviceAddress[i] < 16) Serial.print("0");
   Serial.print(deviceAddress[i], HEX);
 }
}



///^^^^^^^^^^^^^^^^^^APLICAÇÃO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111





void enviaTemperatura(){
  if ((millis()-lasttimeCheck) > 120000){//checa a meio segundo
      //Serial.println("NOVO FW!!!");
      // Le a informacao do sensor
      sensors.requestTemperatures();
      float tempC = sensors.getTempC(sensor1);
      // Atualiza temperaturas minima e maxima
      if (tempC < tempMin)
      {
        tempMin = tempC;
      }
      if (tempC > tempMax)
      {
        tempMax = tempC;
      }


      int epochtime=timeClient.getEpochTime();
      StaticJsonBuffer<220> jsonBuffer;
      JsonObject& jsonMSG = jsonBuffer.createObject();


      delay(10);

      jsonMSG["deviceId"] = chipid;
      jsonMSG["Temp"] = tempC;
      jsonMSG["Min"] = tempMin;
      jsonMSG["Max"] = tempMax;

      mensagemjson = buildJSONmsg(jsonMSG);
      Serial.println("Publicando no canal:" + (String)channelTemperatura);
      Serial.println("A mensagem:");
      Serial.println(mensagemjson);

      //lib konker
      PUB(channelTemperatura, mensagemjson);



    lasttimeCheck = millis();
  }
}




// Setup do Microcontrolador
void setup(){

  Serial.begin(115200);
  Serial.println("Setup");
  Serial.println(firmware);


  //lib konker
  NAME="SensorTemperatura";
  char subChanConfig[]="config";
  //restSubscribePeriord=3000;
  addSUBchan(subChanConfig);

  initialize(firmware);


  Serial.print("SSID:");
  Serial.println(WiFi.SSID());
  Serial.print("Local ip:");
  Serial.println(WiFi.localIP());

  //pega a hora atual da internet
  timeClient.begin();
  delay(500);
  timeClient.update();
  delay(500);



  // Start up the library
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0))
     Serial.println("Sensores nao encontrados !");
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor: ");
  mostra_endereco_sensor(sensor1);

  pinMode(pinoHALL, INPUT_PULLUP);

  Serial.println("Fim do setup");
}

// Loop com o programa principal
void loop(){

  if (checkConnection(10, chipid)==0) {
    Serial.println("Não consegui conectar");
    //wifiManager.resetSettings();//SPIFFS.format();
    delay(3000);
    ESP.restart();
  }


  //checa temperatura
  enviaTemperatura();

  //lib konker
  MsgTuple inTuple;
  CheckMSGs(inTuple);




  delay(100);
}
