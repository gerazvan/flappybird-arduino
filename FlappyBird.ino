// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12

#include "LedControl.h"
LedControl lc = LedControl(17, 16, 15, 1); //

#include "SevSeg.h"
SevSeg sevseg; //Initiate a seven segment controller object

const short int button = 14;

//Variabile pentru debounce
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

//Variabile pt dificultate
unsigned long lastDifficultyChange;
short int intervalWallMove = 300;
short int intervalWallCreate = 1300;
short int level = 3;

//Variabila pt scor
int scor=0;
int maxScor=0;

//Variabile pentru LED-ul 'pasare'
int flap = 0;
unsigned long lastFlap = 0;
int iLed = 3;
// jLed este mereu 1; pasarea sta mereu pe coloana 1, pozitia ei schimbandu-se doar pe linii

//Variabile pentru pereti
struct
{
  int iHole; //linia de la care incepe gaura; gaura va fi formata mereu din 3 puncte consecutive, primul fiind iHole
  int j; //coloana pe care se afla peretele acum
} wall[4];
int noWall = 0; //numarul peretilor
unsigned long lastWallCreate;
unsigned long lastWallMove;


void setup()
{
  //Initiere Afisor
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 12, 11, 13};
  bool resistorsOnSegments = false; // 'false' = rezistenele sunt pe pinii digitilor
  byte hardwareConfig = COMMON_CATHODE;
  bool updateWithDelays = false;
  bool leadingZeros = true;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(90);
  
  pinMode (button, INPUT);

  //Initiere matrice
  lc.shutdown(0, false);// turn off power saving, enables display
  lc.setIntensity(0, 8);// sets brightness (0~15 possible values)

  //Inceput joc
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

//functie care este apelata la inceputul jocului si care asteapta input
void startGame()
{
  lc.clearDisplay(0);
  drawBird();
  while (checkButton() == 0)
  {
    sevseg.setNumber(maxScor);
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

//functie care este apelata la sfarsitul jocului si care asteapta input
void endGame()
{
  if (scor > maxScor)
    maxScor = scor;
  bool commutator = 0;
  unsigned long lastEndGameFlick = millis();
  while (checkButton() == 0)
  {
    sevseg.setNumber(scor, level);
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

//functie care verifica daca pasarea atinge peretii
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

//functie care misca peretii
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

//functioneaza ca o anexa la doWall(); aprinde sau stinge un perete(in functie de parametrii)
void switchWall(int iHole, int j, bool b)
{
  int i;
  for (i = 0; i <= 7; i++)
    if (i != iHole && i != iHole + 1 && i != iHole + 2)
      lc.setLed(0, j, i, b);
}

//functie care misca pasarea
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

//functie care verifica daca butonul este apasat; de asemenea, are debounce
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

//functie care ajusteaza dificultatea
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

//functia care deseneaza pe matrice pasarea in functia startGame()
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
