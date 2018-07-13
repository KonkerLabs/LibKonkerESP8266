#ifndef topicBuilder
#define topicBuilder

void buildSUBTopic(char const device_login[], char const channel[], char *topic){
  String SString;
  SString = String(prefix) + String("/") + String(device_login) + String("/") + String(sub_dev_modifier) + String("/") + String(channel); //sub
  SString.toCharArray(topic, SString.length()+1);
}

//to be deprecated
char *buildSUBTopic(char const channel[]){
  String SString;

  SString = String(prefix) + String("/") + String(device_login) + String("/") + String(sub_dev_modifier) + String("/") + String(channel); //pub
  char *topic;
  SString.toCharArray(topic, SString.length()+1);
  return topic;
}

void buildPUBTopic(char const  device_login[], char  const channel[], char *topic){
  String SString;
  SString = String(prefix) + String("/") + String(device_login) + String("/") + String(pub_dev_modifier) + String("/") + String(channel); //pub
  SString.toCharArray(topic, SString.length()+1);
}


//to be deprecated
char *buildPUBTopic(char const channel[]){
  String SString;

  SString = String(prefix) + String("/") + String(device_login) + String("/") + String(pub_dev_modifier) + String("/") + String(channel); //pub
  char *topic;
  SString.toCharArray(topic, SString.length()+1);
  return topic;
}
#endif
