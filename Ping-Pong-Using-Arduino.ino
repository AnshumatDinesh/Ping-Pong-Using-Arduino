#include <LedControl.h>//Importing the ledcontrol library for easy interface
//with the 8x8 led matrix
//--------Declaring the pin connection of 8x8 Led Matrix----------------
#define DIN 11 
#define CS 7
#define CLK 13
LedControl lc=LedControl(DIN, CLK, CS,0);//creating a ledcontrol object
//connection of seven segment pins a to g
int SS_PINS[]={5,6,10,9,8,4,3};
//The bit map for a frown face that will be displayed on game over
byte over[8]={B11000111,
B01100111,
B00110111,
B00110000,
B00110000,
B00110111,
B01100111,
B11000111};
/*This funtion prints a byte to a sevensegment display
for this SS_PINS is declared prior as an interger array with pins of a to g
*/
void printSEVEN(byte inp){
  byte out=0;
  //switch statement to get the seven segment code from byte
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
  //writing the hexcode to the a to g pin
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
/*This class defines a dot with a position x,y and velocity in x,y
*/
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
    /*This method refreshes the dot position*/
    void refresh(){
      lc.setLed(0,y,x,logic);
    }
    /*This method moves the dot according to the velocity*/
    void move(){
      //after collision with y axis the vely is reversed in y axis
      if((y>=7&& vely>0)||(y<=0&& vely<0)){
        vely*=-1;
      }
      //after collision with x axis the velx is reversed in x axis
      if((x>=7&& velx>0)||(x<=0&& velx<0)){
        velx*=-1;
      }
      //else the position is incremented
      x+=velx;
      y+=vely;
      //led is refreshed
      refresh();

    }
};
/*This class defines the player charecter that would be manupilated by
the user using a joystick. the player is 3 dots in yaxis with a x corditate
and velx*/
class player{
    public:
    int x;
    int velx;
    int vel;
    player(){
        x=0;
        velx=0;
        vel=1;
        pinMode(A0,INPUT);//joystick
    }
    //this method refreshes the player chareter
    void refresh(){
      for(int i=0;i<3;i++){
        lc.setLed(0,0,x+i,logic);        
      }
    }
    //this method reponds to the joystick movement in y axis
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
      //defining the bounds for the player
      if((x>=5 && velx>0)||(x<=0&& velx<0)){
        refresh();
        return;
      }
      x+=velx;
      refresh();
    }
};
/*this class defines an instance of the game
*/
class game{
  private:
  dot *d;
  player* p;
  public:
  int bounce;//no of times the player touched bot
  byte score,lives;//count of lives and score
  game(dot* dot_pntr,player* player_pntr){
    d=dot_pntr;
    p=player_pntr;
    d->refresh();
    p->refresh();
    score=0;
    bounce=0;
    lives=5;
  }
  //initial state of game
  void start(){
    lc.clearDisplay(0);
    p->refresh();
    d->refresh();
    p->respond();
    printSEVEN(lives);
    delay(500);
  }
  //collision check with player
  bool check_col_pl(){
    if(d->y==1 && (d->x>=p->x && d->x<=p->x+3)&& d->vely<=0){
      return true;
    }
    return false;
  }
  //collision chec with wall
  bool check_col_wl(){
    if(d->y==0 && d->vely<=0 && !(check_col_pl())){
      return true;
    }
    return false;
  }
  //updation of  score after 5 bounces
  void scr(){
    if(bounce>=5){
      bounce=0;
      if(score!=9){
        score++;
        //single beep sound
        digitalWrite(2,HIGH);
        delay(50);
        digitalWrite(2,LOW);
      }
    }

  }
  //this method defines one tick of the game
  void tick(){
    if(lives>0){//chacking for game over
      //checking for collusions
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
        //2 beep sound
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
    else{//if game over draw the frown face
      for(int i=0;i<8;i++){
        lc.setRow(0, i, over[i]);
      }
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