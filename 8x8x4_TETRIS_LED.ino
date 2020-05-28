//We always have to include the library
#include "LedControl.h"


#define MAXROW 8 //x
#define MAXCOL 32 //y

/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have 4 MAX72XX.
*/

/*
    TETRIS GAME: by YXHYX
    Discord: YXHYX#2266

    All the libraries included are not mine
    if are having problems and issues you can ask me about all this join my discord server:
        https://discord.gg/vVxW6sJ

    i will make projects like this in the future!
*/

LedControl lc = LedControl(12, 11, 10, 4);

int JX = A0;
int JY = A1;
int JB = 7;
int SW = 0;

bool gameOver = false;

int x = 0, y = 0;
int prevx = x, prevy = y;
bool rowcol[MAXROW][MAXCOL]; //y x

unsigned long currentMillis = 0;
unsigned long interval = 100;
unsigned long previousMillis = 0;

void setup() {
    /*
        The MAX72XX is in power-saving mode on startup,
        we have to do a wakeup call
    */
    Serial.begin(9600);
    pinMode(JX, INPUT);
    pinMode(JY, INPUT);
    pinMode(JB, INPUT_PULLUP);
    /*
    for(int i = 0; i < MAXROW; i++)
        for(int j = 0; j < MAXCOL; j++)
            full[j][i] = 0;*/
    for (int i = 0; i < 4; i++)
    {
        lc.shutdown(i, false);
        lc.setIntensity(i, 8);
        lc.clearDisplay(i);
    }
    /* Set the brightness to a medium values */
    /* and clear the display */
    y = 31;
    prevy = y;
    x = 4;
}

void loop()
{
    //lc.setLed(0, 4, 4, true);
    while(gameOver != true)
    {
        if(gameOver == true)
            break;
        currentMillis = millis();
           
        if(currentMillis - previousMillis >= interval)
        {
            checkBlocks();
            checkDirection();
            updatePlayer();
            previousMillis = currentMillis;
        }
    }
    if(digitalRead(JB) == false)
    {
        gameOver = false;
        resetGame();
        Serial.println("RESETED!");
    }
}

void checkBlocks()
{

    //debug
    for(int i = 0; i < MAXCOL - 5; i++)
        rowcol[x][i] = true;
    /*for(int i = 0; i < MAXROW; i++)
    {
        if()
        lc.setLed();
    } 
    */
    if(y == 0)
        rowcol[x][y] = true;

    if(y != 0)
    {
        if(rowcol[x][y-1] == true)
        {
            rowcol[x][y] = true;
            if(y < 8)
                lc.setLed(0, x, y, rowcol[x][y]);
            if(y > 7 && y < 16)
                lc.setLed(1, x, y, rowcol[y][x]);
            if(y > 15 && y < 24)
                lc.setLed(2, x, y, rowcol[y][x]);
            if(y > 23)
                lc.setLed(3, x, y, rowcol[y][x]);
        }
    }
    if(rowcol[x][31] == true)
    {
        gameOver = true;
    }
}

void checkDirection()
{
    prevy = y;
    prevx = x;

    y--;
    if (map(analogRead(JX), 0, 1023, 0, 50) > 20)
        x++;

    if (map(analogRead(JX), 0, 1023, -50, 0) < -20)
        x--;

    if(x < 0)
        x = 0;

    if(x > 7)
        x = 7;

    if(rowcol[x][y+1] == true)
        y = 31;
}

void updatePlayer()
{
    if(prevy > 23) //31 is max
    {
        lc.setLed(0, x, y-24, true);
        lc.setLed(0, prevx, prevy-24, false);
    }    
    if(prevy > 15 && y < 24) //24 is max
    {
        lc.setLed(1, x, y-16, true);
        lc.setLed(1, prevx, prevy-16, false);
    }    
    if(prevy > 7 && y < 16) //15 is max
    {
        lc.setLed(2, x, y-8, true);
        lc.setLed(2, prevx, prevy-8, false);
    }   
    if(prevy > -1 && y < 8) //7 is max
    {
        lc.setLed(3, x, y, true);
        lc.setLed(3, prevx, prevy, false);
    }
}
void playMusic();

void resetGame()
{
    //reset player
    x = 4, y = 31;
    prevx = 0, prevy = y;
    
    //Clear displays
    for(int i = 0; i < lc.getDeviceCount(); i++)
        lc.clearDisplay(i);

    //clear the blocks
    for(int i = 0; i < MAXROW; i++)
        for(int j = 0; j < MAXCOL; j++)
            rowcol[i][j] = false;
}