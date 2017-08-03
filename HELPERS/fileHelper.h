#ifndef fileHelper
#define fileHelper

bool spiffsMounted=0;

bool spiffsMount(){
	if (spiffsMounted==0){
	  if (SPIFFS.begin()) {
	  	Serial.println("File system mounted");
			spiffsMounted=1;
			return 1;
		}else{
			Serial.println("Failed to mount file system!");
			return 0;
		}		
	}else{
  	Serial.println("File system already mounted");
		return 1;
	}
}

void formatFileSystem(){
	if(spiffsMount()){
		SPIFFS.format();
	}
}


int openFile(String filePath, char *output){
	if(spiffsMount()){
		if (SPIFFS.exists(filePath)) {
			Serial.println("File found: " + filePath);
			File foundFile = SPIFFS.open(filePath, "r");
			if (foundFile) {
				Serial.println("File opened: " + filePath);
				String contents = foundFile.readString();
				contents.toCharArray(output, contents.length()+1);
				Serial.println("Contents: "  + (String)output);
				foundFile.close();
				return 1;
			}else{
				Serial.println("Error writing file: "  + filePath);
				return -1;
			}
		}else{
			Serial.println("File not found: " + filePath);
			return 0;
		}
	}
}


bool saveFile(String filePath, char *dataToSave){
	if(spiffsMount()){
		File myFile = SPIFFS.open(filePath, "w");
		if (!myFile) {
			Serial.println("Failed to open file to write");
			return 0;
		}else{

			Serial.println("Saving content: "+ (String)dataToSave);

			myFile.print(dataToSave);
			myFile.close();
			Serial.println("File created!");
			return 1;
		}
	}else{
		Serial.println("Failed to mount file system");
		return 0;
	}
}


bool writeFile(String path, String contents){
  Serial.print("writeFile:");
  Serial.println(path);
  Serial.print("contents:");
  Serial.println(contents);
  if(SPIFFS.exists(path)){
    Serial.println("FILE EXISTS");
  }
  File file = SPIFFS.open(path, "w");
  file.print(contents);
  file.close();
	return 1;
}


bool replaceFile(String filePath, String dataToSave){
	if(spiffsMount()){
		if (SPIFFS.exists(filePath)) {
			SPIFFS.remove(filePath);
		}
		Serial.println("Creating new file: " + filePath);
		File myFile = SPIFFS.open(filePath, "w");
		if (!myFile) {
			Serial.println("Failed to open file to write");
			return 0;
		}else{

			Serial.println("Saving content: "+ dataToSave);

			myFile.print(dataToSave);
			myFile.close();
			Serial.println("File created!");
			return 1;
		}
	}
}


#endif