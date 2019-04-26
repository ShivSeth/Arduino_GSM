#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

SoftwareSerial SIM900A(10,11);//RX,TX
const int rs = 12, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int line_1=8;
int line_2=9;
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
String text="";
String line_state="";
char key;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte spark[8] = {
  B00011,
  B00110,
  B01100,
  B11111,
  B11111,
  B00110,
  B01100,
  B11000
};

byte rowPins[ROWS] = {47, 45, 43, 41}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {39, 37, 35}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup()
{
  lcd.begin(16, 2);// Print a message to the LCD.
  lcd.createChar(1, spark);
  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.print("SETUP IS READY");
  lcd.write(byte(1));
  
  SIM900A.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println ("SIM900A Ready");
  delay(100);
  Serial.println ("Send Message to GSM Module");
  delay(100);
  SIM900A.print("AT+CMGF=1\r");
  delay(1000);
  SIM900A.println("AT+CNMI=2,2,0,0,0\r"); // AT Command to receive a live SMS
  delay(1000);
  
  pinMode(line_1,OUTPUT);
  pinMode(line_2,OUTPUT);
  digitalWrite(line_1,LOW);
  digitalWrite(line_2,LOW);  
}
void loop()
{
  if (SIM900A.available()>0){
    text=SIM900A.readString();
    Serial.print(text);
    delay(10);
  }
  
  key=keypad.getKey();
  
  if(text.indexOf("LINE 1:ON")>=0 || key=='1'){
   digitalWrite(line_1, HIGH);
   digitalWrite(line_2, LOW);
   lcd.clear();
   lcd.print("LINE 1:ON");  
   lcd.setCursor(0, 1);
   lcd.print("LINE 2:OFF");
   text = "";
   line_state="LINE 1: ON || LINE 2: OFF";   
  }
  
  if(text.indexOf("LINE 1:OFF")>=0){
   digitalWrite(line_1, LOW);
   lcd.clear();
   lcd.print("LINE 1:OFF");
   lcd.setCursor(0, 1);  
   lcd.print("LINE 2:OFF");
   text = ""; 
   line_state="LINE 1: OFF || LINE 2: OFF";     
  }
  
  if(text.indexOf("LINE 2:ON")>=0 || key=='2'){
   digitalWrite(line_2, HIGH);
   digitalWrite(line_1, LOW);
   lcd.clear();
   lcd.print("LINE 1:OFF");
   lcd.setCursor(0, 1);
   lcd.print("LINE 2:ON"); 
   line_state="LINE 2: ON || LINE 1: OFF";
   text = "";  
  }
  
  if(text.indexOf("LINE 2:OFF")>=0){
   digitalWrite(line_2, LOW);
   lcd.clear();
   lcd.print("LINE 1:OFF");
   lcd.setCursor(0, 1);// print the number of seconds since reset:
   lcd.print("LINE 2:OFF");
   text = "";
   line_state="LINE 1: OFF || LINE 2: OFF";   
  }
  if(key=='0'){
   digitalWrite(line_1, LOW);
   digitalWrite(line_2, LOW);
   lcd.clear();
   lcd.write(byte(1));
   lcd.print("ALL LINE RESET");
   lcd.write(byte(1));
   line_state="ALL LINE RESET!";
   lcd.setCursor(0, 1);
   for(int i=0;i<16;i++)
    lcd.print("*");   
   text = "";   
  }
  if(key=='*'){
   digitalWrite(line_1, HIGH);
   digitalWrite(line_2, HIGH);
   lcd.clear();
   lcd.print("$ALL LINES LIVE$");
   line_state="ALL LINES LIVE";
   lcd.setCursor(0,1);
   for(int i=0;i<16;i++)
    lcd.write(byte(1)); 
   text = "";   
  }
  if(text.indexOf("STATE")>=0){
    sendSMS(line_state);
    Serial.println("Line state resquest");
    text = "";
  }
 }
//function that send SMS
void sendSMS(String message){
  Serial.println("Sending State");
  // AT command to set SIM900 to SMS mode
  SIM900A.println("AT+CMGF=1"); 
  delay(1000);
  // change to your SIM900A's to your phone number with ISD code
  SIM900A.println("AT+CMGS=\"+91XXXXXXXXXX\"\r"); 
  delay(1000);
  // Send the SMS
  SIM900A.println(message); 
  delay(100);
  // End AT command with ASCII code of CTRL+Z
  SIM900A.println((char)26); 
  delay(1000);
  SIM900A.println("State Sent");
}
