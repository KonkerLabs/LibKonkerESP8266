#ifndef pubsubRESTfunctionsTS
#define pubsubRESTfunctionsTS

#include "pubREST.h"

bool SUB(char channel[]){
  bool subCode=0;
  char topic[32];

  buildSUBTopic(device_login, channel, topic);

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(device_login, device_pass);
  http.begin(server + "/" + String(topic));  //Specify request destination
  int httpCode = http.GET();

  Serial.println("Subscribing to: " + String(topic) + "; httpcode:" + String(httpCode));
  Serial.print(">");

  subCode=interpretHTTPCode(httpCode);

  if (!subCode){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
  }else{
    Serial.println("sucess");
    Serial.println("");
  }



  if (httpCode > 0) { //Check the returning code
    String strPayload = http.getString();
    http.end();   //Close connection
    if (strPayload!="[]"){
      long long ts =char2LL(parse_JSON_timestamp(strPayload));
       if(setSUBchanNewTime(channel,ts)){
         callback(topic, strPayload);
       }
     }

     return 1;
   }else{
     return 0;
   }
}


bool SUB(ChanTuple chanArr[]){
  bool success=1;
  for (int i = 0; i < sizeof(chanArr)-1; i++){
    if(strlen(chanArr[i].chan) != 0){
      if(!SUB(chanArr[i].chan)){
        success=0;
        failedComm=1;
      }
    }
  }
  return success;
}
#endif
