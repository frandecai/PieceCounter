#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/*******************/
/* Global constant */
/*******************/
#define LIMIT 70
#define TIMEALARM 5

/********************/
/* Global variables */
/********************/
boolean detection = false;
unsigned long totalSeconds = 0;
int seconds = 0;
int secondsAlarm = 0;
int state = 0;
int pieces = 0;

// LCD definition
LiquidCrystal_I2C lcd(0x3F, 16, 2);

/******************/
/* Setup function */
/******************/
void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(2, 0);
  lcd.print("Piece counter");
  lcd.setCursor(5, 1);
  lcd.print("AGROTOMY");

  // PIN for read from start button (input)
  pinMode(7, INPUT);
  // PIN for read from sensor (input)
  pinMode(8, INPUT);
  // PIN for alarm (output)
  pinMode(9, OUTPUT); // set pin as input
  digitalWrite(9, LOW);

  delay(2000);

  lcd.clear();
}


/**********************/
/* Main loop function */
/**********************/
void loop()
{
  manageState();

  if ((millis() / 1000) > (totalSeconds)) {
    totalSeconds++;

    if (state == 2) {
      seconds++;

      // Add a second if the alarm is on
      if (digitalRead(9) == HIGH) {
        secondsAlarm++;
      }
    }
  }
    
  if (state == 2) {  
    // Detects a "thing"
    if ((digitalRead(8) == LOW) && (!detection)){
      pieces++;
      detection = true;
    }
    else if ((digitalRead(8) == HIGH) && (detection)) {
      detection = false;
    }

    // Check number of things per minute
    // And reset seconds when reaches a minute
    if (seconds >= 60) {
      // Alarm if number of pieces detected is below LIMIT
      if (pieces < LIMIT) {
        digitalWrite(9, HIGH);
      }
   
      seconds = 0;
      pieces = 0;
    }

    // Switch off the alarm after 5 seconds
    if (secondsAlarm > TIMEALARM) {
      digitalWrite(9, LOW);
    }
  }
  else if (state == 0){
    seconds = 0;
    secondsAlarm = 0;
    pieces = 0;
    digitalWrite(9, LOW);
  }

  manageLCD();
  
  delay(50);
}


/******************/
/* States machine */
/******************/
void manageState()
{
  // Press button for the first time
  if (digitalRead(7) && (state == 0)) {
    state = 1;
  }
  // Start running when stop pressing the button (RUNNING)
  else if (!digitalRead(7) && (state == 1)) {
    state = 2;
  }
  // Press button when running
  else if (digitalRead(7) && (state == 2)) {
    state = 3;
  }
  // Stop running when stop pressing the button (STOPPPED) 
  else if (!digitalRead(7) && (state == 3)) {
    state = 0;
  }
}


/*************************/
/* Print messages to LCD */
/*************************/
void manageLCD()
{
  // Prints seconds
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  if (seconds < 10) {
    lcd.print("0");
    lcd.print(seconds);
  }
  else {
    lcd.print(seconds);
  }

  lcd.setCursor(0, 1);
  lcd.print("Pieces:");
  if (pieces < 10) {
    lcd.print("0");
    lcd.print(pieces);
  }
  else {
    lcd.print(pieces);
  }
    
  // Prints state
  if (state == 2) {
    lcd.setCursor(9, 0);
    lcd.print("Running");
  }
  else if (state == 0) {
    lcd.setCursor(9, 0);
    lcd.print("Stopped");
  }

  // Prints ALARM in case of alarm
  if (digitalRead(9) == HIGH) {
    lcd.setCursor(11, 1);
    lcd.print("ALARM");
  }
  else {
    lcd.setCursor(11, 1);
    lcd.print("     ");    
  }
}
