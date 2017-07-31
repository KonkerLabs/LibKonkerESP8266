#ifndef chanTuple
#define chanTuple

struct chantuple
{
   char chan[7]={'\0'};
   long long time=0;
};

typedef struct chantuple ChanTuple;



ChanTuple getChanTuple(char *chan, long long time){
  ChanTuple newTuple;
  strncpy(newTuple.chan,chan,7);
  newTuple.time=time;
  return newTuple;
}

void copyChanTuple(ChanTuple &destiny, ChanTuple &origin){
  strncpy(destiny.chan,origin.chan,7);
  destiny.time=origin.time;
}


void setChanTuple(ChanTuple &destiny, char *chan, long long time){
  strncpy(destiny.chan,chan,7);
  destiny.time=time;
}
#endif