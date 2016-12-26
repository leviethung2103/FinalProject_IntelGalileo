#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
#define led 13
#define LM35_Pin A0
#define REDPIN 9
#define GREENPIN 11
#define BLUEPIN 10
#define FADESPEED 5     // make this higher to slow down

//192.168.137.160


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0x98, 0x4F, 0xEE, 0x00, 0xCE, 0xD2 };

IPAddress ip(192,168,137,213);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(8081);


#define DELAY 500


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display
const byte DS1307 = 0x68;           // Address of DS1307
const byte NumberOfFields = 7;
 
int second, minute, hour, day, wday, month, year;        
String readString;


byte termometru[8] = //icon for termometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

byte picatura[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};
byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

Servo gara;       // Create gara object to control a gara
Servo door;       // Create door object to control a door

int pos = 0;    // variable to store the servo position


/*
void nodejs()
{
  system("node /home/root/finalproject/fp.js");
}
*/


void closeGara() {
  gara.write(0);
}
void openGara() {
  gara.write(30);
}
void closeDoor() {
  door.write(0);
}

void openDoor(){
  door.write(30);
}
 
void setup()
{
  pinMode(led, OUTPUT);
  Wire.begin();                                                     
  lcd.init();                                                       // Begin LCD 16x2
  lcd.backlight();                                                  // Turn on backlight
  Serial.begin(9600);                                               // Begin Serial
  //setTime(13, 12, 00, 4, 7,12, 16); // 12:30:45 CN 08-02-2015     // Set time for DS1307

  lcd.createChar(1,termometru);
  lcd.createChar(2,picatura);
  lcd.createChar(3, degree);
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
 // analogReadResolution(12);   // get the full 12 bit ADC

  // Servo 
 door.attach(5);  // attaches the servo on pin 9 to the servo object
 gara.attach(6);  // attaches the servo on pin 9 to the servo object 
 closeGara();
 closeDoor();
  // Off RGB
 offLED();

 // Run Ubidots 
 // system("python home/root/py_ubidots.py");
 //system("node home/root/finalproject/fp.js");
} 
 
void loop()
{
// door.write(30);
 // gara.write(30);

  // Play RGB LEDs 
  // disco();
  //red();
  // green();
  // blue();

  unsigned long time;

  float voltage = analogRead(LM35_Pin) * 5.0 / 1023.0;

  float tempC = voltage * 100.0;
  float tempF = tempC * 1.8 + 32.0;
  Serial.print("Temperature is: ");
  Serial.println(tempC);                                             // Debug
  int tempC_int = voltage*100;
  
  readDS1307();                                                     // Read the data of DS1307
//digitalClockDisplay();                                            // Debug Serial in Time
  lcd.setCursor(0,0);
  switch(wday){
  case 1:
    lcd.print("Sun");
    break;
  case 2:
    lcd.print("Mon");
    break;
  case 3:
    lcd.print("Tue");
    break;
  case 4:
    lcd.print("Wed");
    break;
  case 5:
    lcd.print("Thu");
    break;
  case 6:
    lcd.print("Fri");
    break;
  case 7:
    lcd.print("Sat");
    break;
  }
  lcd.print(" ");
  printDigits(hour);
  lcd.print(":");
  printDigits(minute);

  lcd.setCursor(11,0);
  // lcd.print("T:");
  //lcd.write(1);
  lcd.print("TEMP");


  lcd.setCursor(0,1);
  lcd.print(day);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);

  lcd.setCursor(11,1);
  lcd.print(tempC_int,1);
  lcd.setCursor(13,1);
  lcd.write(3);
  lcd.print("C");

  /******************************* Server ********************************/
   // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //Serial.write(c);

        /****************************** Just Added **********************/
        // read char by char HTTP request
        if (readString.length() <100) {
          readString +=c;
        }
        

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        // if HTTP request hs ended  
        if (c == '\n' && currentLineIsBlank) {
          /****************************** Just Added **********************/

          Serial.println(readString); // print to serial monitor for debugging
          /****************************** End **********************/
            
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // add a meta refresh tag, so the browser pulls again every 5 seconds:
         // client.println("<meta http-equiv=\"refresh\" content=\"3\">");
          
          /****************************** Just Added **********************/
          client.println("<HEAD>");
          client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
          client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
          client.println("<link rel='stylesheet' type='text/css' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css' />");
          client.println("<TITLE> Final Project </TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<div class='alert alert-success'> <h1 p class='text-center'>Intel Galileo IoT - <strong>FINAL PROJECT  </strong>- Team 1</h1 p></div>");
         // client.println("<H1 p class='alert alert-default'> Final Project - Intel Galileo with IoT - Team 1 Xin Chào </H1 p>");
          client.println("<div class='container'><div class='row'> <div class='col-md-3'><div class='thumbnail'><img src='https://scontent-sea1-1.xx.fbcdn.net/v/t1.0-9/14330067_683921891757882_3324804193772573738_n.jpg?oh=94fc43573e5426ed2a23f1590a6547f2&oe=58B312C8'/></div><h3>Le Viet Hung</h3></div> <div class='col-md-3'><div class='thumbnail'><img src='https://scontent-sea1-1.xx.fbcdn.net/v/t1.0-9/15107363_1230670243656566_3813672913647862599_n.jpg?oh=c98cb27e194314dcce2f19d1f849dd80&oe=58B51C9C'/></div><h3>Tran Long Hung</h3></div> <div class='col-md-3'><div class='thumbnail'><img src='https://scontent-sea1-1.xx.fbcdn.net/v/t1.0-1/13938365_1624684684510989_9087037297620010550_n.jpg?oh=dcbcb41c10d96b6ac88c76d5fd9c9cc3&oe=58E9DB40'/></div><h3>Phan Trong Hiep</h3></div> <div class='col-md-3'><div class='thumbnail'><img src='https://scontent-sea1-1.xx.fbcdn.net/v/t1.0-1/c0.0.958.958/15355564_962013143904100_20787735967670184_n.jpg?oh=984618eaab3e4cbffe6f999608ade49b&oe=58BB787B'/></div><h3>Do Ly Ly</h3></div> </div></div>");
          client.println("<hr />");         
          // client.println("<br />");
          /* Modify Here         
          client.println("<p class='text-center'><a href=\"/?button1on\"\"><button class='btn btn-success'>Turn On <span class='badge'>LED</span></button> </a></p>");
          client.println("<p class='text-center'><a href=\"/?button1off\"\"><button class='btn btn-default text-center'>Turn Off <span class='badge'>LED</span></button></a></p>");
          client.println("<p class='text-center'><a href=\"/?garaon\"\"><button class='btn btn-success'> Open <span class='badge'>Gara</span></button> </a></p>");
          client.println("<p class='text-center'><a href=\"/?garaoff\"\"><button class='btn btn-default text-center'> Close <span class='badge'>Gara</span></button></a></p>");
          client.println("<p class='text-center'><a href=\"/?dooron\"\"><button class='btn btn-success'> Open <span class='badge'>Door</span></button> </a></p>");
          client.println("<p class='text-center'><a href=\"/?dooroff\"\"><button class='btn btn-default text-center'> Close <span class='badge'>Door</span></button></a></p>");
          */
          // client.println("<p class='text-center'><a href=\"/?red\"\"><button class='btn btn-default text-center'> RED </button></a></p>");
          client.println("<p class='text-center'><a href=\"/?button1on\"\"><button class='btn btn-success'>Turn On <span class='badge'>LED</span></button> </a><a href=\"/?garaon\"\"><button class='btn btn-success'> Open <span class='badge'>Gara</span></button> </a><a href=\"/?dooron\"\"><button class='btn btn-success'> Open <span class='badge'>Door</span></button> </a></p>");
          client.println("<p class='text-center'><a href=\"/?button1off\"\"><button class='btn btn-default text-center'>Turn Off <span class='badge'>LED</span></button></a><a href=\"/?garaoff\"\"><button class='btn btn-default text-center'> Close <span class='badge'>Gara</span></button></a><a href=\"/?dooroff\"\"><button class='btn btn-default text-center'> Close <span class='badge'>Door</span></button></a></p>");
          
          client.println("<p class='text-center'><a href=\"/?RGBoff\"\"><button type='button' class='btn btn-lg btn-default'>RGB OFF</button></a><a href=\"/?red\"\"><button type='button' class='btn btn-lg btn-danger'>RED</button></a><a href=\"/?blue\"\"><button type='button' class='btn btn-lg btn-primary'>BLUE</button></a><a href=\"/?green\"\"><button type='button' class='btn btn-lg btn-success'>GREEN</button></a><a href=\"/?autoLED\"\"><button type='button' class='btn btn-lg btn-default'>AUTO</button></a></p>");
            //client.println("<a href=\"/?blue\"\"><button type='button' class='btn btn-lg btn-primary'>BLUE</button></a></p>");
          // client.println("<br />");     
          // client.println("<br />"); 


          client.print("<H3 p class='text-center'>");
          client.print("Temperature in Celcius is: ");
          client.print("<strong>");
          client.print(tempC);
          client.print(" C");
          client.print("</strong>");
          client.print("</H3 p>");
   //       client.println("<br />"); 

          client.print("<H3 p class='text-center'>");
          client.print("Temperature in Fahrenheit is: ");
          client.print("<strong>");
          client.print(tempF);
          client.print(" F");
          client.print("</strong>");
          client.print("</H3 p>");
          client.println("<br />");
               
          client.println("</BODY>");

          /*
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }

          */
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");

    if (readString.indexOf("?button1on") > 0 ) {
      digitalWrite(led, HIGH);
    }
    if (readString.indexOf("?button1off") > 0 ) {
      digitalWrite(led,LOW);
    }  

    if (readString.indexOf("?garaon")>0) {
      gara.write(70);
    }

    if (readString.indexOf("?garaoff")>0) {
      gara.write(0);
    }

    if (readString.indexOf("?dooron")>0) {
      door.write(50);
    }

    if (readString.indexOf("?dooroff")>0) {
      door.write(0);
    }
    if (readString.indexOf("?red")>0) {
      analogWrite(REDPIN,127);
      analogWrite(BLUEPIN,0);
      analogWrite(GREENPIN,0);

    }
   if (readString.indexOf("?blue")>0) {
      analogWrite(BLUEPIN,127);
      analogWrite(REDPIN,0);
      analogWrite(GREENPIN,0);

    }

   if (readString.indexOf("?green")>0) {
      analogWrite(GREENPIN,127);
      analogWrite(BLUEPIN,0);
      analogWrite(REDPIN,0);
    }
    if (readString.indexOf("?RGBoff")>0) {
      analogWrite(BLUEPIN,0);
      analogWrite(REDPIN,0);
      analogWrite(GREENPIN,0);
    }
    if (readString.indexOf("?autoLED")>0) {
      disco();
    }

    readString = "";


  }

  /******************************* End ********************************/


   
  time = millis();
  while(millis()-time < DELAY)
  {

  }
}
 
void readDS1307()
{
        Wire.beginTransmission(DS1307);
        Wire.write((byte)0x00);
        Wire.endTransmission();
        Wire.requestFrom(DS1307, NumberOfFields);
        
        second = bcd2dec(Wire.read() & 0x7f);
        minute = bcd2dec(Wire.read() );
        hour   = bcd2dec(Wire.read() & 0x3f); // chế độ 24h.
        wday   = bcd2dec(Wire.read() );
        day    = bcd2dec(Wire.read() );
        month  = bcd2dec(Wire.read() );
        year   = bcd2dec(Wire.read() );
        year += 2000;    
}
/* Chuyen tu format BCD (Binary-Coded Decimal) sang Decimal */
int bcd2dec(byte num)
{
        return ((num/16 * 10) + (num % 16));
}
/* Chuyen từ Decimal sang BCD */
int dec2bcd(byte num)
{
        return ((num/10 * 16) + (num % 10));
}
 
void digitalClockDisplay(){
    // digital clock display of the time
    Serial.print(hour);
    printDigits(minute);
    printDigits(second);
    Serial.print(" ");
    Serial.print(day);
    Serial.print(" ");
    Serial.print(month);
    Serial.print(" ");
    Serial.print(year); 
    Serial.println(); 
}
 
void printDigits(int digits){
    Serial.print(":");                               // Cac thanh phan thoi gian duoc ngan cach bang dau :

    if(digits < 10)
    {
        Serial.print('0');
        lcd.print('0');
        Serial.print(digits);
        lcd.print(digits);
    } else 
    {
      lcd.print(digits);
    }
}
 
/* Cai dat thoi gian cho DS1307 */
void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr)
{
        Wire.beginTransmission(DS1307);
        Wire.write(byte(0x00));                    // Dat lai pointer
        Wire.write(dec2bcd(sec));
        Wire.write(dec2bcd(min));
        Wire.write(dec2bcd(hr));
        Wire.write(dec2bcd(wd));                    // day of week: Sunday = 1, Saturday = 7
        Wire.write(dec2bcd(d)); 
        Wire.write(dec2bcd(mth));
        Wire.write(dec2bcd(yr));
        Wire.endTransmission();
}


void disco() {
  while(1) {
    int r,g,b;   
  // fade from blue to violet
  for (r = 0; r < 256; r++) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from violet to red
  for (b = 255; b > 0; b--) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from red to yellow
  for (g = 0; g < 256; g++) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  // fade from yellow to green
  for (r = 255; r > 0; r--) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from green to teal
  for (b = 0; b < 256; b++) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from teal to blue
  for (g = 255; g > 0; g--) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  } 
}

void red(){
  analogWrite(REDPIN,255);
}

void blue(){
  analogWrite(BLUEPIN,255);
}

void green() {
  analogWrite(GREENPIN,255);
}

void offLED(){
  analogWrite(REDPIN,0);
  analogWrite(BLUEPIN,0);
  analogWrite(GREENPIN,0);
}