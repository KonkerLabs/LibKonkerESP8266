#include <stdlib.h>
#define subChanSize 5
ChanTuple subChanArr[subChanSize];

void addSUBchan(char chan[]){
  for (int i = 0; i < subChanSize; i++){
    if(strlen(subChanArr[i].chan) == 0){
      setChanTuple(subChanArr[i],chan,0);
      return;
    }
  }
}

void addSUBchan(char chan[], int time){
  for (int i = 0; i < subChanSize; i++){
    if(strlen(subChanArr[i].chan) == 0){
      setChanTuple(subChanArr[i],chan,time);
      return;
    }
  }
}

int addSUBchanArr(ChanTuple chanArr[]){
  for (int i = 0; i < subChanSize; i++){
    if(strlen(chanArr[i].chan) != 0){
      addSUBchan(chanArr[i].chan, chanArr[i].time);
    }
  }
}

void removeSUBchan(char chan[]){
  for (int i = 0; i < subChanSize; i++){
    if(strcmp(subChanArr[i].chan, chan)==0){
      memset(subChanArr[i].chan, 0, sizeof(subChanArr[i].chan));
      subChanArr[i].time=0;
      return;
    }
  }
}



void setSUBchan(char chan[], int time){
  for (int i = 0; i < subChanSize; i++){
    if(strcmp(subChanArr[i].chan, chan)==0){
      strcpy(subChanArr[i].chan, chan);
      subChanArr[i].time=time;
      return;
    }
  }
}

#ifdef REST_IGNORE_SAME_TIMESTAMP
int setSUBchanNewTime(char chan[], long long time){
  for (int i = 0; i < subChanSize; i++){
    if(strlen(subChanArr[i].chan) != 0){
      if(strcmp(subChanArr[i].chan, chan)==0 && subChanArr[i].time!=time){
        if(subChanArr[i].time==0){//isso serve para 1a interação ao ligar o esp, primeiro atualiza com o último valor disponível
          subChanArr[i].time=time;
          Serial.println("Set first ts for " + String(subChanArr[i].chan));
        }else{
          Serial.println("Changing ts!!...");
          subChanArr[i].time=time;
          return 1;
        }
      }
    }else{
      return 0;
    }
  }
  return 0;
}
#endif
