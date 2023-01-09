#include <LedControl.h>
#define DIN 11 
#define CS 7
#define CLK 13
LedControl lc=LedControl(DIN, CLK, CS,0);
int SS_PINS[]={5,6,10,9,8,4,3};
void printSEVEN(byte inp){
  byte out=0;
  switch(inp){
    case 1:
      out=0x06;
      break;
    case 2:
      out=0x5b;
      break;
    case 3:
      out=0x4f;
      break;
    case 4:
      out=0x66;
      break;
    case 5:
      out=0x6d;
      break;
    case 6:
      out=0x7d;
      break;
    case 7:
      out=0x07;
      break;
    case 8:
      out=0x7f;
      break;
    case 9:
      out=0x6f;
      break;
    case 0:
      out=0x3f;
      break;
    default:
    	break;
  }
  for(int i=0;i<7;i++){
    if(out & 0x01 ==0x01){
      digitalWrite(SS_PINS[i],LOW);
    }
    else{
      digitalWrite(SS_PINS[i],HIGH);
    }
    out=out>>1;
  }
}
class dot{
    public:
    int x,y;
    int velx,vely;
    dot(int x_in,int y_in,int velx_in,int vely_in){
        x=x_in;
        y=y_in;
        velx=velx_in;
        vely=vely_in;
    }
    void refresh(){
      lc.setLed(0,y,x,1);
    }
    void move(){
      if((y>=7&& vely>0)||(y<=0&& vely<0)){
        vely*=-1;
      }
      if((x>=7&& velx>0)||(x<=0&& velx<0)){
        velx*=-1;
      }
      x+=velx;
      y+=vely;
      refresh();

    }
};
class player{
    public:
    int x;
    int velx;
    int vel;
    player(){
        x=0;
        velx=0;
        vel=1;
        pinMode(A0,INPUT);
    }
    void refresh(){
      for(int i=0;i<3;i++){
        lc.setLed(0,0,x+i,1);        
      }
    }
    void respond(){
      int rd=analogRead(A0);
      if(rd>600){
        velx=vel;
      }
      else if(rd<400){
        velx=-1*vel;
      }
      else{
        velx=0;
      }
      if((x>=5 && velx>0)||(x<=0&& velx<0)){
        refresh();
        return;
      }
      x+=velx;
      refresh();
    }
};
class game{
  private:
  dot *d;
  player* p;
  public:
  int bounce;
  byte score,lives;
  game(dot* dot_pntr,player* player_pntr){
    d=dot_pntr;
    p=player_pntr;
    d->refresh();
    p->refresh();
    score=0;
    bounce=0;
    lives=5;
  }
  void start(){
    p->refresh();
    d->refresh();
    p->respond();
    printSEVEN(lives);
    delay(500);
  }
  bool check_col_pl(){
    if(d->y==1 && (d->x>=p->x && d->x<=p->x+3)&& d->vely<=0){
      return true;
    }
    return false;
  }
  bool check_col_wl(){
    if(d->y==0 && d->vely<=0 && !(check_col_pl())){
      return true;
    }
    return false;
  }
  void scr(){
    if(bounce>=5){
      bounce=0;
      if(score!=9){
        score++;
        digitalWrite(2,HIGH);
        delay(50);
        digitalWrite(2,LOW);
      }
    }

  }
  void tick(){
    if(lives>0){
      if(check_col_pl()){
        d->vely*=-1;
        if(p->velx!=0){
          d->velx=p->velx;
        }
        bounce++;
      }
      if(check_col_wl()){
        if(lives>=0){
          lives--;
        }
        printSEVEN(lives);
        digitalWrite(2,HIGH);
        delay(25);
        digitalWrite(2,LOW);
        delay(25);
        digitalWrite(2,HIGH);
        delay(25);
        digitalWrite(2,LOW);
      }
      scr();
      d->move();
      p->respond();
      delay(500*pow(2,-1*0.6*score));
    }
    else{
      delay(500);
      printSEVEN(score);
    }
  }
};
dot d(0,1,1,1);
player p;
game g(&d,&p);
void setup() {
  Serial.begin(9600);
  for(int i=0;i<7;i++){
    pinMode(SS_PINS[i],OUTPUT);
  }
  pinMode(2,OUTPUT);
  lc.shutdown(0,false);
  lc.setIntensity(0,1);
  lc.clearDisplay(0);
  g.start();
}
void loop(){
  lc.clearDisplay(0);
  g.tick();

}