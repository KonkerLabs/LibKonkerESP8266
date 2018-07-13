#ifndef httpHelper
#define httpHelper

bool interpretHTTPCode(int httpCode){
  if (httpCode > 0 && httpCode<300) { //Check the returning code
    return 1;

  }else{
    return 0;
  }
}


bool getRemoteString(String URL, String &outputString){
	bool subCode=0;
  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.begin(URL);  //Specify request destination
  int httpCode = http.GET();

  Serial.println("Http.get, URL: " + URL);
	Serial.println("Result code: " + String(httpCode));
  subCode=interpretHTTPCode(httpCode);

  if (!subCode){
    Serial.println("Failed");
    Serial.println("");
    return 0;
  }else{
    Serial.println("Success");
    Serial.println("");		
  }


  if (httpCode > 0) { //Check the returning code
    outputString= http.getString();
		Serial.println("Payload=" +outputString);
		return 1;
	}
}

#endif