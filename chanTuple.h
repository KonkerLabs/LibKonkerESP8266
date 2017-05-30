struct chantuple
{
   char chan[7]={'\0'};
   long long time=0;
};

typedef struct chantuple ChanTuple;



ChanTuple getChanTuple(char chan[], long long time){
  ChanTuple newTuple;
  strcpy(newTuple.chan, chan);
  newTuple.time=time;
  return newTuple;
}

void copyChanTuple(ChanTuple destiny, ChanTuple origin){
  strcpy(destiny.chan, origin.chan);
  destiny.time=origin.time;
}


void setChanTuple(ChanTuple &destiny, char chan[7], long long time){
  strcpy(destiny.chan, chan);
  destiny.time=time;
}
