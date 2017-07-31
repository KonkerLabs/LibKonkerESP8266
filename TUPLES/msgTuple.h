#ifndef msgTuple
#define msgTuple

struct msgtuple
{
   char chan[32]={'\0'};
   char msg[1024]={'\0'};
};

typedef struct msgtuple MsgTuple;



void copyTuple(MsgTuple &destiny, MsgTuple &origin){
  strncpy(destiny.chan,origin.chan,32);
  strncpy(destiny.msg,origin.msg,1024);
}


void setTuple(MsgTuple &destiny, char const chan[], char const msg[]){
  strncpy(destiny.chan,chan,32);
	strncpy(destiny.msg,msg,1024);
}


void clearTupleMsgs(MsgTuple *targets){
  for (int i = 0; i < subChanSize; i++){
    strncpy(targets[i].msg,{'\0'},1024);
  }
}

void clearTuples(MsgTuple *target){
  for (int i = 0; i < subChanSize; i++){
    strncpy(target[i].chan,{'\0'},32);
    strncpy(target[i].msg,{'\0'},1024);
  }
}

void clearTuple(MsgTuple  &target){
  strncpy(target.chan,{'\0'},32);
  strncpy(target.msg,{'\0'},1024);
}

#endif
