#include <MenuBackend.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include<Servo.h>


//struct Account{
//  String user;
//  String pass;
//  };
//Account accounts[10];
const int maxUsers = 5;
//char* userns[5];
//char* passws[5];

String userns[6];
String passws[6];
int  accountIndex = 0 ;
  
//-------------------------------------------------------------------------------------------------------------------------------
const int buzzer = 22;
float temp;
int tempPin = 1 ;
float favTemp;
const int heaterPin = 24;
const int coolerPin = 26;
bool favTempUsed;

const byte interruptPin = 11;
const byte interruptPin2 = 12;

const int servoPin = 13;


const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 5; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
{'F','G','#','*'},
{'1','2','3','w'},
{'4','5','6','s'},
{'7','8','9','q'},
{'a','0','d','e'}
};
byte rowPins[ROWS] = {36,38,40,42,44};
byte colPins[COLS] = {52,50,48,46};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//-------------------------------------------------------------------------------------------------------------------------------

/*
  This is the structure of the modelled menu
  
  
*/
static void menuChangeEvent(MenuChangeEvent changed);
static void menuUseEvent(MenuUseEvent used);

//this controls the menu backend and the event generation
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);

MenuItem temperature = MenuItem("Temperature");
    MenuItem setTemperature = MenuItem("Set Temperature");
    MenuItem showTemperature = MenuItem("Show Temperature");
  MenuItem music = MenuItem("Music");
    MenuItem play = MenuItem("Play Music");
    MenuItem stopp = MenuItem("Stop Music");
  MenuItem account = MenuItem("Account Settings");
    MenuItem addUser = MenuItem("Add User");
    MenuItem deleteUser = MenuItem("Delete User");
  MenuItem humidity = MenuItem("Humidity");
    MenuItem showHumidity = MenuItem("Show Humidity");
  MenuItem enter = MenuItem("Enter");
    MenuItem openDoor = MenuItem("Open Door");

//-------------------------------------------------------------------------------------------------------------------------------

void menuSetup()
{
  Serial.println("Setting up menu...");
  //add the file menu to the menu root
  menu.getRoot().add(temperature); 
    //setup the settings menu item
    temperature.addRight(setTemperature);
      //we want looping both up and down
      setTemperature.addBefore(showTemperature);
      setTemperature.addAfter(showTemperature);
      showTemperature.addAfter(setTemperature);
      //we want a left movement to pint to settings from anywhere
      showTemperature.addLeft(temperature);
      setTemperature.addLeft(temperature);
  temperature.addBefore(enter);
  temperature.addAfter(music);
  music.addBefore(temperature);
      music.addRight(play);
      play.addAfter(stopp);
      stopp.addAfter(play);
      play.addLeft(music);
      stopp.addLeft(music);
  music.addAfter(account);
  account.addBefore(music);
      account.addRight(addUser);
      addUser.addAfter(deleteUser);
      deleteUser.addAfter(addUser);
      addUser.addLeft(account);
      deleteUser.addLeft(account);
  account.addAfter(humidity);
  humidity.addBefore(account);
      humidity.addRight(showHumidity);
      showHumidity.addAfter(showHumidity);
      showHumidity.addLeft(humidity);
  humidity.addAfter(enter);
  enter.addBefore(humidity);
      enter.addRight(openDoor);
      openDoor.addLeft(enter);
  enter.addAfter(temperature);
      
}
//-------------------------------------------------------------------------------------------------------------------------------
/*
  This is an important function
  Here all use events are handled
  
  This is where you define a behaviour for a menu item
*/
void menuUseEvent(MenuUseEvent used)
{
  Serial.print("Menu use ");
  Serial.println(used.item.getName());
  
 if (used.item == "Set Temperature")
 {
    setTemp();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Temperature");
    Serial.println("Set Temperature");
 }

  if (used.item == "Show Temperature")
 {
    showTemp();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Show Temperature");
    Serial.println("Show Temperature");
 }

 if (used.item == "Play Music")
 {
    playMusic();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Play Music");
    Serial.println("Play Music");
 }

 if (used.item == "Stop Music")
 {
    stopMusic();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Stop Music");
    Serial.println("Stop Music");
 }

 if (used.item == "Add User")
 {
    adduser();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Add User");
    Serial.println("Add User");
 }

 if (used.item == "Delete User")
 {
    deleteuser();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Delete User");
    Serial.println("Delete User");
 }

 if (used.item == "Show Humidity")
 {
    showwHumidity();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Show Humidity");
    Serial.println("Show Humidity");
 }

 if (used.item == "Open Door")
 {
    doorOpen();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Open Door");
    Serial.println("Open Door");
 }

 
 
}

/*
  This is an important function
  Here we get a notification whenever the user changes the menu
  That is, when the menu is navigated
*/
void menuChangeEvent(MenuChangeEvent changed)
{
//  Serial.print("Menu change ");
//  Serial.print(changed.from.getName());
//  Serial.print(" ");
  Serial.println(changed.to.getName());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(changed.to.getName());
}
//-------------------------------------------------------------------------------------------------------------------------------


Servo myservo;

int op = 0;
void setup()
{
  
  myservo.attach(9,1000,2000);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(interruptPin2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), fire, RISING);
  digitalWrite(interruptPin2, LOW);
  
  favTempUsed = false;
  Serial.begin(9600);
  lcd.begin(16, 2);
  menuSetup();
  Serial.println("Starting navigation:\r\nUp: w   Down: s   Left: a   Right: d   Use: e");
  pinMode(heaterPin, OUTPUT);
  pinMode(coolerPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  userns[0] = "1111";
  passws[0] = "123456";
  accountIndex = accountIndex + 1;
  for(int i = 1 ; i < maxUsers ; i++){
//    accounts[i].user = "";
//    accounts[i].pass = "";
     userns[i] = "";
     passws[i] = "";
  }

  Serial.print("username enter initial ");
  Serial.println(userns[0]);
  Serial.print("password enter initial ");
  Serial.println(passws[0]);
 

  
  
}

//-------------------------------------------------------------------------------------------------------------------------------
void loop()
{
//  if (Serial.available()) {
//    byte read = Serial.read();
//    switch (read) {
//      case 'w': menu.moveUp(); break;
//      case 's': menu.moveDown(); break;
//      case 'd': menu.moveRight(); break;
//      case 'a': menu.moveLeft(); break;
//      case 'e': menu.use(); break;
//    }
//  }

//  if(op == 0){
//  openDoorFunction(0);
//  op = 1;
//  delay(20);
//  }
//  
//  
//  else if(op == 1){
//  openDoorFunction(90);
//  op = 0;
//  delay(20);
//  }
//  delay(2000);
  
  temp = analogRead(tempPin);
  temp = temp * 0.48828125;

  if (favTemp > temp && favTempUsed){
    digitalWrite(heaterPin,HIGH);
    digitalWrite(coolerPin,LOW);

  }

  if (favTemp < temp && favTempUsed){
    digitalWrite(coolerPin,HIGH);
    digitalWrite(heaterPin,LOW);

  }
  if ( favTemp - temp < 2 && favTemp > temp ) {
    digitalWrite(heaterPin,LOW);
    Serial.println("off heat");
  }
  
  if ( temp - favTemp > 2 && favTemp < temp ) {
    digitalWrite(heaterPin,LOW);
    Serial.println("off cool");
  }

  


  if(temp > 800){
    digitalWrite(interruptPin2, HIGH);
  }
  if(temp < 800){
    digitalWrite(interruptPin2, LOW);
  }

  
  char key = keypad.getKey();
  switch (key) {
     case 'w': menu.moveUp(); break;
     case 's': menu.moveDown(); break;
     case 'd': menu.moveRight(); break;
     case 'a': menu.moveLeft(); break;
     case 'e': menu.use(); break;
   }

}

//-----------------------------------------------------------------1----------------------------------------------------------------------

void setTemp(){
  if ( !favTempUsed){
  favTemp = temp;
  }
  while(true){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(favTemp);
    Serial.println(favTemp);
    
    char keyy = keypad.waitForKey();
    
    if (keyy == 'w') {
       favTemp = favTemp + 1;
    }
    else if ( keyy == 's'){
      favTemp = favTemp - 1;
    }
    else if (keyy == 'e'){
      favTempUsed = true;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Temperature Set on");
      Serial.println("Temperature Set on");
      lcd.setCursor(0,1);
      lcd.print(favTemp);
      Serial.println(favTemp);
      delay(2000);
      break;
    }
    else if (keyy == 'q'){
      break;
    }
  }
}
//-----------------------------------------------------------------2----------------------------------------------------------------------
void showTemp(){
  while(true){
    temp = analogRead(tempPin);
    temp = temp * 0.48828125;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(temp);
    lcd.setCursor(4,0);
    lcd.print("C");
//    Serial.println(temp);
    char keyy = keypad.getKey();
    if (keyy == 'q'){
      break;
    }
    delay(800);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(temp);
  lcd.setCursor(5,0);
  lcd.print("C");
  
  
}
//-----------------------------------------------------------------3----------------------------------------------------------------------
void playMusic(){
  digitalWrite(buzzer, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Music Started");
  Serial.println("Music Started");
  delay(2000);
}
//-----------------------------------------------------------------4----------------------------------------------------------------------
void stopMusic(){
  digitalWrite(buzzer, LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Music Stopped");
  Serial.println("Music Stopped");
  delay(2000);
}
//-----------------------------------------------------------------5----------------------------------------------------------------------
void showwHumidity(){
}
//-----------------------------------------------------------------6----------------------------------------------------------------------
void adduser(){
  if ( accountIndex == 10 ){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("User List is Full");
    delay(2000);
    return;
  }
  int emptyIndex;
  char tempUser[5];
  
  
  for(int i = 0 ; i < maxUsers ; i++){
    if (userns[i] == "" ){
      emptyIndex = i;
      Serial.print("empty indexd found on ");
      Serial.println(emptyIndex);
      break;
    }
  }

  for ( int j = 0 ; j < 4 ; j++){
    if(j == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter 4 number");
      lcd.setCursor(0,1);
      lcd.print("for username");
    }
    char kk = keypad.waitForKey();
    if (kk == 'q'){
      return;
    }
    tempUser[j] = kk ;
    if(j == 0){
        lcd.clear();
    }
    lcd.setCursor(j , 0);
    lcd.print(kk);
  }
  tempUser[4] = '\0';
  Serial.print("salmamm user ");
  Serial.println(tempUser);
  String userrr = (String) tempUser;
  Serial.print("salllllaaaamm user ");
  Serial.println(userrr);
  delay(2000);

  
  char tempPass[7];
  for ( int v = 0 ; v < 6 ; v++){
    if(v == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter 6 number");
      lcd.setCursor(0,1);
      lcd.print("for password");
    }
    char kl = keypad.waitForKey();
    if (kl == 'q'){
      return;
    }
    tempPass[v] = kl ;
    if(v == 0){
        lcd.clear();
    }
    lcd.setCursor(v , 0);
    lcd.print(kl);
  }
  tempPass[6] = '\0';

  delay(2000);
  Serial.println("out if loop");

  
//  userns[emptyIndex] = userrr;
userns[emptyIndex] = tempUser;
  Serial.print("username enter add");
  Serial.println(userns[emptyIndex]);

  String passwordddd = (String) tempPass;
//  passws[emptyIndex] = passwordddd;
passws[emptyIndex] = tempPass;

  Serial.print("password enter add");
  Serial.println(passws[emptyIndex]);
  accountIndex = accountIndex + 1 ;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("user added");
  lcd.setCursor(0,1);
  lcd.print("successfully");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(userns[emptyIndex]);
  lcd.setCursor(0,1);
  lcd.print(passws[emptyIndex]);
  delay(2000);
  

  
}

//-----------------------------------------------------------------7----------------------------------------------------------------------
void deleteuser(){
  
  char tempUser[5];

  for ( int j = 0 ; j < 4 ; j++){
    if(j == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter 4 number");
      lcd.setCursor(0,1);
      lcd.print("for username");
    }
    char kk = keypad.waitForKey();
    if (kk == 'q'){
      return;
    }
    tempUser[j] = kk ;
    if(j == 0){
        lcd.clear();
    }
    lcd.setCursor(j , 0);
    lcd.print(kk);
  }
  tempUser[4] = '\0';
  Serial.print("s user ");
  Serial.println(tempUser);
  String userrr = (String) tempUser;
  Serial.print("ss user ");
  Serial.println(userrr);
  delay(2000);

  userns[5] = tempUser;
  
  int userIndex = 12;
  for(int i = 0 ; i < maxUsers ; i++){
    Serial.print(userns[5]);
    Serial.print(" vs ");
    Serial.print(userns[i]);
    Serial.println(".");
    if (userns[i] == userns[5] ){
      userIndex = i;
      Serial.print("found on ");
      Serial.println(i);
      break;
    }
  }

  if ( userIndex == 12 ){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No user with th");
    lcd.setCursor(0,1);
    lcd.print("is specs ");
    delay(2000);
    return;
  }
  
  char tempPass[7];
  for ( int v = 0 ; v < 6 ; v++){
    if(v == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter 6 number");
      lcd.setCursor(0,1);
      lcd.print("for password");
    }
    char kl = keypad.waitForKey();
    if (kl == 'q'){
      return;
    }
    tempPass[v] = kl ;
    if(v == 0){
        lcd.clear();
    }
    lcd.setCursor(v , 0);
    lcd.print(kl);
  }
  tempPass[6] = '\0';

  delay(2000);
  

  

  passws[5] = tempPass;


  
  
  if (passws[userIndex] == passws[5]){
      userns[userIndex] = "";
      passws[userIndex] = "";
      accountIndex = accountIndex - 1 ;
    
      Serial.print("username enter delete");
      Serial.println(userns[userIndex]);
      Serial.print("password enter delete");
      Serial.println(passws[userIndex]);
    
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("user removed");
      lcd.setCursor(0,1);
      lcd.print("successfully");
      delay(2000);
  }

  else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wrong User Or");
      lcd.setCursor(0,1);
      lcd.print("Password.Go first");
      Serial.println("wrong go first");
      delay(2000);
    }

  
   
  
}
//-----------------------------------------------------------------8----------------------------------------------------------------------
void fire() {
  
  digitalWrite(buzzer, HIGH);
  openDoorFunction(90);
  for(volatile int i = 0 ; i < 10000000 ; i++){}
  digitalWrite(buzzer, LOW);
}

void openDoorFunction(float degree){
  myservo.write(degree);
}
//-----------------------------------------------------------------9----------------------------------------------------------------------
void doorOpen(){


  char tempUser[5];

  for ( int j = 0 ; j < 4 ; j++){
    if(j == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter 4 number");
      lcd.setCursor(0,1);
      lcd.print("for username");
    }
    char kk = keypad.waitForKey();
    if (kk == 'q'){
      return;
    }
    tempUser[j] = kk ;
    if(j == 0){
        lcd.clear();
    }
    lcd.setCursor(j , 0);
    lcd.print(kk);
  }
  tempUser[4] = '\0';
  Serial.print("s user ");
  Serial.println(tempUser);
  String userrr = (String) tempUser;
  Serial.print("ss user ");
  Serial.println(userrr);
  delay(2000);
  char tempPass[7];
  for ( int v = 0 ; v < 6 ; v++){
    if(v == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter 6 number");
      lcd.setCursor(0,1);
      lcd.print("for password");
    }
    char kl = keypad.waitForKey();
    if (kl == 'q'){
      return;
    }
    tempPass[v] = kl ;
    if(v == 0){
        lcd.clear();
    }
    lcd.setCursor(v , 0);
    lcd.print(kl);
  }
  tempPass[6] = '\0';

  delay(2000);
  Serial.println("out of loop");

  Serial.print("username enter check");
  Serial.println(tempUser);


  Serial.print("password enter check");
  Serial.println(tempPass);

  userns[5] = tempUser;

  passws[5] = tempPass;


  int userIndex = 12;
  for(int i = 0 ; i < maxUsers ; i++){
    Serial.print(userns[5]);
    Serial.print(" vs ");
    Serial.print(userns[i]);
    Serial.println(".");
    if (userns[i] == userns[5] ){
      userIndex = i;
      Serial.print("found on ");
      Serial.println(i);
      break;
    }
  }
  if (userIndex != 12 && passws[userIndex] == passws[5]){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Correct");
      lcd.setCursor(0,1);
      lcd.print("Welcome");
      Serial.println("correct welcome");
//      openDoorFunction(90);
      myservo.write(90);
      delay(1000);
      myservo.write(-90);
      delay(1000);
      delay(1000);
  }

  else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wrong User Or");
      lcd.setCursor(0,1);
      lcd.print("Password.Go first");
      Serial.println("wrong go first");
      delay(2000);
    }
  
  
}
