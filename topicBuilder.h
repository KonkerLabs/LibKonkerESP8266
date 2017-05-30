
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
