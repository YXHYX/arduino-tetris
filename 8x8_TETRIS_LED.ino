#define XOR(A, B) !A != !B // XOR(1, 1) !1 != !1
#include "LedControl.h"


#define MAXROW 8 
#define MAXCOL 32 

const byte GAME[4][8] = {
    {B11111110, B10000000, B10000000, B11111100, B10000000, B10000000, B10000000, B11111110}, //E
    {B11000110, B11101110, B10111010, B10010010, B10000010, B10000010, B10000010, B10000010}, //M
    {B00010000, B00101000, B00101000, B01000100, B01000100, B01111100, B10000010, B10000010}, //A
    {B01111100, B10000010, B10000000, B10000000, B10001110, B10000010, B10000010, B01111100} //G
};

const byte OVER[4][8] = {
    {B11111000, B10000100, B10000100, B10000100, B11111000, B10010000, B10001000, B10000100}, //R
    {B11111110, B10000000, B10000000, B11111100, B10000000, B10000000, B10000000, B11111110}, //E
    {B10000010, B10000010, B10000010, B01000100, B01000100, B00101000, B00101000, B00010000}, //V
    {B01111100, B10000010, B10000010, B10000010, B10000010, B10000010, B10000010, B01111100} //O
};


enum blockTypes {L, Z, T, O, I, J};
blockTypes types;

/*
 ***********************************
 *        CIRCUIT DIAGRAM          *
 ***********************************
 *     
 *    --8x8x4 MATRIX LED--
 *  pin 12 -------> DIN(Data in)
 *  pin 11 -------> CLK(clock)
 *  pin 10 -------> LOAD
 *  +5V    -------> VCC
 *  GND    -------> GND
 *  
 *    --JOYSTICK--
 *  pin 7  -------> SW (joystick button pin)
 *  #         -     VRy (we dont use VRy pin of the joystick)
 *  pin A0 -------> VRx (joystick x axis pin)
 *  +5V    -------> VCC 
 *  GND    -------> GND
 *  
 *    --BUZZER--
 *  GND    -------> GND
 *  VCC    -------> A1
 */

/*
    TETRIS GAME: by YXHYX
    Discord: YXHYX#2266

    All the libraries included are not mine
    if you are having problems and issues with this code, you can contact me on my discord server:
        https://discord.gg/vVxW6sJ

    i will make projects like this in the future!
*/

LedControl lc = LedControl(12, 11, 10, 4);

//Arduino pins
int JX = A0;
int JB = 7;
int SW = 0;

int buzzerPin = A2;

// Game variables
bool gameOver = false;
int score = 0;

int x = 0, y = 0;
int prevx = x, prevy = y;

bool rowcol[MAXROW][MAXCOL - 8];

bool blockGenerated = false;
bool block[3][3] = {0};
int rotation = 0;

//
unsigned long currentMillis = 0;
unsigned long interval = 100;
unsigned long previousMillis = 0;



void setup()
{
    //debug
    Serial.begin(9600);
    
    //intialize the inputs
    pinMode(JX, INPUT);
    pinMode(JB, INPUT_PULLUP);

    pinMode(buzzerPin, OUTPUT);
    // for random()
    randomSeed(analogRead(1));

    /* initialize the displays and set their brightness and clear them */
    for (int i = 0; i < 4; i++)
    {
        lc.shutdown(i, false);
        lc.setIntensity(i, 15); //3 for power saving!
        lc.clearDisplay(i);
    }
    //initialize the variables
    y = 24;
    prevy = y;
    x = 4;
}

void loop()
{
    // Main game loop
    while(!gameOver)
    {
        currentMillis = millis();
        // update the game if 100 millisecond has passed
        if(currentMillis - previousMillis >= interval)
        {
            updatePlayer();
            previousMillis = currentMillis;
        }
        playMusic();
    }
    if(gameOver)
        drawGameOver();

    if(digitalRead(JB) == false)
    {
        resetGame();
        gameOver = false;
    }
}

void updateBlocks()
{

     if(digitalRead(JB) == false)
     {
          rotation++;
          if(rotation > 3)
               rotation = 0;
     }
     
     if(y == 0 && (rowcol[x][0] == false || rowcol[x+1][0] == false))
     {
          for(int i = 0; i < 3; i++)
               for(int j = 0; j < 3; j++)
                    if(block[j][i] == true)
                         rowcol[x+j][y+i] = true;
          blockGenerated = false;
     }
     else if (rowcol[x+1][y-1] == true){
          for(int i = 0; i < 3; i++)
               for(int j = 0; j < 3; j++)
                    if(block[j][i] == true)
                         rowcol[x+j][y+i] = true;
     }
     

     
     //check if the block is placed to generate a new one
     generateBlock();

    //if the blocks reached the top
    if(rowcol[x][23] == true)
        gameOver = true;
}

void checkInput(){
     //take the previous coordinates
     prevy = y;
     prevx = x;

     //update the block coordinates
     y--;
     //get input
     if (map(analogRead(JX), 0, 1023, -20, 100) > 50){
          x++;
     }
     if (map(analogRead(JX), 0, 1023, -100, 20) < -50){
          x--;
     }


               
     if(x < 0){
          x = 0;
     }
     
     if(x > 6){
          x = 6;
     }
     
     for(int i = 0; i <3; i++)
          if(block[i][0] == false && x < 0)
               x = 0;
     
     for(int i = 0; i <3; i++)
          if(block[i][2] == false && x > 6)
               x = 6;
     //if block is placed
     if(rowcol[x][y+1] == true || y < 0)
     {
          y = 23;
          blockGenerated = false;
     }
}

//update and draw the block(player)
void updatePlayer()
{
     updateBlocks(); //updates the blocks (collision)
     
     checkInput(); //check the joysticks input and updates coordinates
     renderBlock(); //renders the blocks
     
}


//Still in construction
//notes: use delay without sleeping the arduino
void playMusic()
{
    //tone(buzzerPin, 500);
}

//checks wether a row is full or not
bool checkRow(int j)
{
    for(int i = 0; i < MAXROW; i++)
        if(rowcol[i][j] != true)
            return false;
    return true;
}

//draw the blocks
void renderBlock()
{
     for(int i = 0; i < 3; i++){

          for(int j = 0; j < 3; j++){

               if(y > 11 ) //if y > 11 then draw the block
                    lc.setLed(1, i+prevx, j+prevy-16, false);
            
               if(y < 20 && y > 3)
                    lc.setLed(2, i+prevx, j+prevy-8, false);
            
               if(y < 12)
                    lc.setLed(3, i+prevx, j+prevy, false);
          }
     }
     for(int i = 0; i < 3; i++){

          for(int j = 0; j < 3; j++){
            
               if(y > 11 && block[j][i] != false)
                    lc.setLed(1, i+x, j+y-16, true);
            
               if((y < 20 && y > 3) && block[j][i] != false)
                    lc.setLed(2, i+x, j+y-8, true);
            
               if(y < 12 && block[j][i] != false)
                    lc.setLed(3, i+x, j+y, true);
          }
     }
}

void generateBlock()
{
     if(blockGenerated == false)
     {
          types = random(0, 5);
          blockGenerated = true;
     }
     //L, Z, T, O, I, J
     //0, 1, 2, 3, 4, 5
     switch(types)
     {
          case 0:
               switch(rotation)
               {
                    case 0:
                         block[0][0] = 1; block[0][1] = 0; block[0][2] = 0; // 1 0 0
                         block[1][0] = 1; block[1][1] = 0; block[1][2] = 0; // 1 0 0
                         block[2][0] = 1; block[2][1] = 1; block[2][2] = 0; // 1 1 0
                         break;
                    case 1:
                         block[0][0] = 0; block[0][1] = 0; block[0][2] = 0; // 0 0 0
                         block[1][0] = 0; block[1][1] = 0; block[1][2] = 1; // 0 0 1
                         block[2][0] = 1; block[2][1] = 1; block[2][2] = 1; // 1 1 1
                         break;
                    case 2:
                         block[0][0] = 0; block[0][1] = 1; block[0][2] = 1; // 0 1 1
                         block[1][0] = 0; block[1][1] = 0; block[1][2] = 1; // 0 0 1
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 1; // 0 0 1
                         break;
                    case 3:
                         block[0][0] = 1; block[0][1] = 1; block[0][2] = 1; // 1 1 1
                         block[1][0] = 1; block[1][1] = 0; block[1][2] = 0; // 1 0 0
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 0; // 0 0 0
                         break;      
               }
            break;
            
          case 1:
               switch(rotation)
               {
                    case 0:
                         block[0][0] = 1; block[0][1] = 0; block[0][2] = 0; // 1 0 0
                         block[1][0] = 1; block[1][1] = 1; block[1][2] = 0; // 1 1 0
                         block[2][0] = 0; block[2][1] = 1; block[2][2] = 0; // 0 1 0
                         break;
                    case 1:
                         block[0][0] = 0; block[0][1] = 1; block[0][2] = 1; // 0 1 1
                         block[1][0] = 1; block[1][1] = 1; block[1][2] = 0; // 1 1 0
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 0; // 0 0 0
                         break;
                    case 2:
                         block[0][0] = 0; block[0][1] = 1; block[0][2] = 0; // 0 1 0
                         block[1][0] = 0; block[1][1] = 1; block[1][2] = 1; // 0 1 1
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 1; // 0 0 1
                         break;
                    case 3:
                         block[0][0] = 0; block[0][1] = 0; block[0][2] = 0; // 0 0 0
                         block[1][0] = 0; block[1][1] = 1; block[1][2] = 1; // 0 1 1
                         block[2][0] = 1; block[2][1] = 1; block[2][2] = 0; // 1 1 0
                         break;      
               }
            break;

          case 2:
               switch(rotation)
               {
                    case 0:
                         block[0][0] = 1; block[0][1] = 0; block[0][2] = 0; // 1 0 0
                         block[1][0] = 1; block[1][1] = 1; block[1][2] = 0; // 1 1 0
                         block[2][0] = 1; block[2][1] = 0; block[2][2] = 0; // 1 0 0
                         break;
                    case 1:
                         block[0][0] = 0; block[0][1] = 0; block[0][2] = 0; // 0 0 0
                         block[1][0] = 0; block[1][1] = 1; block[1][2] = 0; // 0 1 0
                         block[2][0] = 1; block[2][1] = 1; block[2][2] = 1; // 1 1 1
                         break;
                    case 2:
                         block[0][0] = 0; block[0][1] = 0; block[0][2] = 1; // 0 0 1
                         block[1][0] = 0; block[1][1] = 1; block[1][2] = 1; // 0 1 1
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 1; // 0 0 1
                         break;
                    case 3:
                         block[0][0] = 1; block[0][1] = 1; block[0][2] = 1; // 1 1 1
                         block[1][0] = 0; block[1][1] = 1; block[1][2] = 0; // 0 1 0
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 0; // 0 0 0
                         break;      
               }
            break;

          case 3:
               //there is no difference in rotating a square 90 deg
               block[0][0] = 1; block[0][1] = 1; block[0][2] = 0; // 1 1 0
               block[1][0] = 1; block[1][1] = 1; block[0][2] = 0; // 1 1 0
               block[2][0] = 0; block[2][1] = 0; block[0][2] = 0; // 0 0 0
            break;

          case 4:
               switch(rotation)
               {
                    case 0:
                         block[0][0] = 0; block[0][1] = 1; block[0][2] = 0; // 0 1 0
                         block[1][0] = 0; block[1][1] = 1; block[1][2] = 0; // 0 1 0
                         block[2][0] = 0; block[2][1] = 1; block[2][2] = 0; // 0 1 0
                         break;
                    case 1:
                         block[0][0] = 0; block[0][1] = 0; block[0][2] = 0; // 0 0 0
                         block[1][0] = 1; block[1][1] = 1; block[1][2] = 1; // 1 1 1
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 0; // 0 0 0
                         break;
                    case 2:
                         block[0][0] = 0; block[0][1] = 1; block[0][2] = 0; // 0 1 0
                         block[1][0] = 0; block[1][1] = 1; block[1][2] = 0; // 0 1 0
                         block[2][0] = 0; block[2][1] = 1; block[2][2] = 0; // 0 1 0
                         break;
                    case 3:
                         block[0][0] = 0; block[0][1] = 0; block[0][2] = 0; // 0 0 0
                         block[1][0] = 1; block[1][1] = 1; block[1][2] = 1; // 1 1 1
                         block[2][0] = 0; block[2][1] = 0; block[2][2] = 0; // 0 0 0
                         break;      
               }
            break;
     }
}

//resets the game if wanted to play again
void resetGame()
{
     //reset player
     x = 4, y = 24;
     prevx = 0, prevy = y;
    
     //Clear displays
     for(int i = 0; i < lc.getDeviceCount(); i++)
          lc.clearDisplay(i);

     //clear the blocks
     for(int i = 0; i < MAXROW; i++)
          for(int j = 0; j < MAXCOL-8; j++)
               rowcol[i][j] = false;
}

//draws text "GAME" and "OVER"
void drawGameOver()
{
     for(int i = 0; i < 4; i++)
          lc.clearDisplay(i);
     delay(500);
     for(int i = 0; i < 4; i++)
          for(int j = 0; j < 8; j++)
               lc.setRow(i, j, GAME[i][j]);
     delay(500);

     for(int i = 0; i < 4; i++)
          lc.clearDisplay(i);
     delay(500);
     
     for(int i = 0; i < 4; i++)
          for(int j = 0; j < 8; j++)
               lc.setRow(i, j, OVER[i][j]);
     delay(500);
}
