#ifndef pubsubRESTfunctions
#define pubsubRESTfunctions

#include "pubREST.h"

bool SUB(MsgTuple &returnDestiny, char channel[]){
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
    Serial.println("strPayload=" + strPayload);
    http.end();   //Close connection
    if (strPayload!="[]"){
      char receivedTopicMsg[1024];
      strPayload.toCharArray(receivedTopicMsg, 1024);

      setTuple(returnDestiny, channel, receivedTopicMsg);

      Serial.println("returnDestiny.msg received on channel[" + String(channel) + "] >");
      Serial.println( String(returnDestiny.msg));
      Serial.println("<");
      Serial.println();
    }

     return 1;
   }else{
     return 0;
   }
}



bool SUB(MsgTuple *returnDestinies, char *channels[]){
  bool success=1;
  clearTupleMsgs(returnDestinies);
  for (int i = 0; i < sizeof(channels)-1; i++){
    if(strlen(channels[i]) != 0){
      if(!SUB(returnDestinies[i], channels[i])){
        success=0;
        failedComm=1;
      }
    }
  }
  return success;
}




bool SUB(MsgTuple *returnDestinies, ChanTuple chanTuples[]){
  bool success=1;
  clearTupleMsgs(returnDestinies);
  for (int i = 0; i < sizeof(chanTuples)-1; i++){
    if(strlen(chanTuples[i].chan) != 0){
      if(!SUB(returnDestinies[i], chanTuples[i].chan)){
        success=0;
        failedComm=1;
      }
    }
  }
  return success;
}

#endif
