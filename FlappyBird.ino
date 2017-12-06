#include "LedControl.h"
LedControl lc = LedControl(17, 16, 15, 1); //

#include "SevSeg.h"
SevSeg sevseg; //Initiate a seven segment controller object

const short int button = 14;

//Variables for debouce
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

//Variables for difficulty
unsigned long lastDifficultyChange;
short int intervalWallMove = 300;
short int intervalWallCreate = 1300;
short int level = 3;

//Variables for the score
int scor=0;
int maxScor=0;

//Variables for the "Bird" LED
int flap = 0;
unsigned long lastFlap = 0;
int iLed = 3;
// jLed is always 1; the bird is bound to column 1, as its position changes on lines

//Variables for the wall
struct
{
  int iHole; //the line at which the hole starts; the hole is always made of 3 consecutive LEDs, the first one being iHole
  int j; //the column that the wall is at a given time
} wall[4];
int noWall = 0; //counts the number of the walls
unsigned long lastWallCreate;
unsigned long lastWallMove;


void setup()
{
  //4 Digit 7 Segment initialization; used for the score
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 12, 11, 13};
  bool resistorsOnSegments = false; // 'false' = rezistenele sunt pe pinii digitilor
  byte hardwareConfig = COMMON_CATHODE;
  bool updateWithDelays = false;
  bool leadingZeros = true;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(90);
  
  //Button initialization
  pinMode (button, INPUT);

  //Matrix initialization
  lc.shutdown(0, false);// turn off power saving, enables display
  lc.setIntensity(0, 8);// sets brightness (0~15 possible values)

  startGame();
}


void loop()
{
  if (checkButton())
    flap = flap + 2;
  doFlap();
  doWall();
  adjustGameDifficulty();
  sevseg.setNumber(scor, level);
  sevseg.refreshDisplay(); 
  if (checkCollision())
  {
    endGame();
    startGame();
  }  
}

//function used at the start of the game; waits for input
void startGame()
{
  lc.clearDisplay(0);
  drawBird();
  while (checkButton() == 0)
  {
    sevseg.setNumber(maxScor); //shows the highest score registered since the arduino was powered on
    sevseg.refreshDisplay(); 
  }
  lc.clearDisplay(0);
  noWall = 0;
  iLed = 1;
  flap = 0;
  lastDifficultyChange = millis();
  lastWallCreate = millis();
  lc.setLed(0, 1, iLed, true);
}

//function used at the end of the game; waits for input
void endGame()
{
  if (scor > maxScor)
    maxScor = scor;
  bool commutator = 0;
  unsigned long lastEndGameFlick = millis();
  while (checkButton() == 0)
  {
    sevseg.setNumber(scor, level); //shows the score from the last round
    sevseg.refreshDisplay();
    if (millis() - lastEndGameFlick >= 300)
    {
      lastEndGameFlick = millis();
      for (int p = 0; p < noWall; p++)
        switchWall(wall[p].iHole, wall[p].j, commutator);
      commutator = !commutator; 
    }
  }
  scor = 0;
  intervalWallMove = 300;
  intervalWallCreate = 1300;
  level = 3;
}

//function that checks collision between the bird and the walls
bool checkCollision()
{
  bool end = 0;
  if (iLed > 7) end = 1;
  for (int p = 0; p < noWall; p++)
    if(wall[p].j == 1)
      if (iLed != wall[p].iHole && iLed != (wall[p].iHole + 1) && iLed != (wall[p].iHole + 2))
        end = 1;
  return end;
}

//function that moves the walls
void doWall()
{
  if (millis() - lastWallCreate >= intervalWallCreate)
  {
    lastWallCreate = millis();
    wall[noWall].iHole = random(1, 5);
    wall[noWall].j = 7;
    switchWall(wall[noWall].iHole, wall[noWall].j, 1);
    noWall++;
  }
  if (millis() - lastWallMove >= intervalWallMove)
  {
    lastWallMove = millis();
    for (int p = 0; p < noWall; p++)
    {
      if (wall[p].j == 0)
      {
        switchWall(wall[p].iHole, wall[p].j, 0);
        wall[p] = wall[noWall - 1];
        noWall--;
        p--;
        scor++;
      }
      else
      {
        switchWall(wall[p].iHole, wall[p].j, 0);
        wall[p].j--;
        switchWall(wall[p].iHole, wall[p].j, 1);
      }
    }
  }
}

//function used in doWall(); turns ON or OFF a wall (based on the boolean b)
void switchWall(int iHole, int j, bool b)
{
  int i;
  for (i = 0; i <= 7; i++)
    if (i != iHole && i != iHole + 1 && i != iHole + 2)
      lc.setLed(0, j, i, b);
}

//function that moves the bird
void doFlap()
{
  if (!flap)
  {
    if (millis() - lastFlap >= 200)
    {
      lastFlap = millis();
      lc.setLed(0, 1, iLed, false);
      iLed++;
      if (iLed <= 7)
        lc.setLed(0, 1, iLed, true);
    }
  }
  else
  {
    if (millis() - lastFlap >= 130)
    {
      lastFlap = millis();
      lc.setLed(0, 1, iLed, false);
      if (iLed > 0)
        iLed--;
      lc.setLed(0, 1, iLed, true);
      flap--;
    }
  }
}

//function that debounces the input from the button
bool checkButton()
{
  bool press = 0;
  int reading = digitalRead(button);
  if (reading != lastButtonState)
    lastDebounceTime = millis();
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;
      if (buttonState == HIGH)
      {
        press = 1;
      }
    }
  }
  lastButtonState = reading;
  return press;
}

//function that adjusts the game's difficulty
void adjustGameDifficulty()
{
  if(level > 0)
    if(millis() - lastDifficultyChange >= 15000)
    {
      intervalWallMove -= 50;
      intervalWallCreate -= 150;
      level--;
      lastDifficultyChange = millis();   
    }   
  //Conditie pentru stop joc
  if(scor >= 9000)
  {
    intervalWallCreate = 100;
  }  
}

//a function that 'tries' to draw a bird at the start of the game while waiting for input
void drawBird()
{
  lc.setLed(0, 2, 0, true);
  lc.setLed(0, 3, 0, true);

  lc.setLed(0, 1, 1, true);
  lc.setLed(0, 2, 1, true);
  lc.setLed(0, 3, 1, true);
  lc.setLed(0, 4, 1, true);

  lc.setLed(0, 0, 2, true);
  lc.setLed(0, 1, 2, true);
  lc.setLed(0, 2, 2, true);
  lc.setLed(0, 3, 2, true);
  lc.setLed(0, 4, 2, true);
  lc.setLed(0, 7, 2, true);

  lc.setLed(0, 1, 3, true);
  lc.setLed(0, 2, 3, true);
  lc.setLed(0, 3, 3, true);
  lc.setLed(0, 4, 3, true);
  lc.setLed(0, 5, 3, true);
  lc.setLed(0, 6, 3, true);
  lc.setLed(0, 7, 3, true);

  lc.setLed(0, 1, 4, true);
  lc.setLed(0, 2, 4, true);
  lc.setLed(0, 3, 4, true);
  lc.setLed(0, 4, 4, true);
  lc.setLed(0, 5, 4, true);
  lc.setLed(0, 6, 4, true);
  lc.setLed(0, 7, 4, true);

  lc.setLed(0, 2, 5, true);
  lc.setLed(0, 3, 5, true);
  lc.setLed(0, 4, 5, true);
  lc.setLed(0, 5, 5, true);
  lc.setLed(0, 6, 5, true);

  lc.setLed(0, 2, 6, true);
  lc.setLed(0, 3, 6, true);
  lc.setLed(0, 4, 6, true);

  lc.setLed(0, 2, 7, true);
  lc.setLed(0, 4, 7, true);
}
