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
