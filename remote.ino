/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


// RFID CHECK //


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

/* LET OP: De beveiliging van deze kaarten is zwak. Niet gebruiken voor beveiliging. De
 * kaartnummers hoeven niet per se uniek te zijn. Sommige Chinese kaarten staan toe dat
 * het nummer kan worden veranderd.
 * 
 * Pin layout:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>                        // SPI bibliotheek
#include <MFRC522.h>                    // MFRC522 bibliotheek
#include "RF24.h"                       //

      int tonePin     =  36;
const int pinRST      =  40;                // Reset pin RFID
const int pinSS       =  53;                // Serial data pin RFID
      
      int rfid        = 0;                  // ACCESS DENIED = 0; ACCESS GRANTED = 1; MASTER ACCESS GRANTED = 2;

const String cardIdValid1 = "E785C43B";  // Toegestane kaart IDs
const String cardIdValid2 = "4A47B615";  // THIS THE MASTER KEY

MFRC522 mfrc522(pinSS, pinRST);         // Instantieer MFRC522 op pinSS en pinRST

// Lees het kaart ID uit
void getCardID() {
  // Als geen nieuwe kaart is gevonden EN
  // Als geen kaart data wordt gelezen
  // PICC = Proximity Integrated Circuit Card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {

    String cardIdRead = "";

    // Er zijn kaarten met 4 of met 7 bytes. De bibliotheek ondersteund nog geen 7 bit adressen
    // dus we hoeven maar 4 bytes te doorlopen
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      // cardID[i] = mfrc522.uid.uidByte[i];
      cardIdRead.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      cardIdRead.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    cardIdRead.toUpperCase();

    Serial.print("Gelezen kaart ID: ");
    Serial.println(cardIdRead);

    // Als het cardID is toegestaan
    if (cardIdRead == cardIdValid1|cardIdRead == cardIdValid2) {
      Serial.println("Kaart toegestaan");
      // Stop het lezen
      // mfrc522.PICC_HaltA();
      
      // Green light on for 750 ms
      digitalWrite(14,HIGH);
      digitalWrite(41,HIGH);
        kort();
        delay(200);
      digitalWrite(41,LOW);
      digitalWrite(14,LOW);
     
        // MASTERKEY
        if(cardIdRead == cardIdValid2){
      rfid = 2;
        }
        else{
      rfid = 1;
          }
      
      
    } else {
      Serial.println("Kaart niet toegestaan");

       //Red light on
      digitalWrite(16,HIGH);
      digitalWrite(41,HIGH);
        kort();
        delay(450);
      digitalWrite(41,LOW);
      digitalWrite(16,LOW);

      //Avoid quick repetition of vibration
      delay(2000);

       rfid = 0;
       
    }
    
  }
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


// TFT DISPLAY //


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


#include <TouchScreen.h> //touch library
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

//if the IC model is known or the modules is unreadable,you can use this constructed function
//LCDWIKI_KBV my_lcd(ILI9341,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//if the IC model is not known and the modules is readable,you can use this constructed function
LCDWIKI_KBV my_lcd(240,320,A3,A2,A1,A0,A4);//width,height,CS,RS,wr,rd,reset

//colors
#define BLUE         0x001F
#define RED          0xF800
#define WHITE        0xFFFF
#define BLACK        0x0000
#define GREEN        0x07E0
#define CYAN         0x07FF
#define MAGENTA      0xF81F

#define PIXEL_NUMBER  (my_lcd.Get_Display_Width()/4)

//UI

#define YP A3  // must be an analog pin, use "An" notation! //CS
#define XM A2  // must be an analog pin, use "An" notation! //RS
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//touch sensitivity for X
#define TS_MINX 911
#define TS_MAXX 117

//touch sensitivity for Y
#define TS_MINY 87
#define TS_MAXY 906

//touch sensitivity for press
#define MINPRESSURE 10
#define MAXPRESSURE 1000

#include <SD.h>
#include <SPI.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

uint32_t bmp_offset = 0;

//input
int order             = 0;
int booze             = 0;

//which beverage did the user choose? i.e. what input do we send to the machine? = order
int command           = 0;
int confirmation      = 0;
int confirmation_page = 0;

//paginanummers
int page              = 0;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//display string
void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//Check whether to press or not
boolean is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py)
{
    if((px > x1 && px < x2) && (py > y1 && py < y2))
    {
        return true;  
    } 
    else
    {
        return false;  
    }
 }

//display the main menu
void show_menu(void)
{
      //reset rfid (also master key, if applicable) after getting access
      rfid = 0;
      
      my_lcd.Fill_Screen(BLUE);
      show_string("COCKTAILS",  my_lcd.Get_Display_Width()/2-strlen("COCKTAILS")*3*6/2+1,  my_lcd.Get_Display_Height()/6-3*8/2+1,  3,   WHITE,   BLACK,   1);
      show_string("LONGDRINKS",  my_lcd.Get_Display_Width()/2-strlen("LONGDRINKS")*3*6/2+1,  my_lcd.Get_Display_Height()/2-3*8/2+1,  3,   WHITE,   BLACK,   1);
      show_string("SHOTS",  my_lcd.Get_Display_Width()/2-strlen("SHOTS")*3*6/2+1,  5*my_lcd.Get_Display_Height()/6-3*8/2+1,  3,   WHITE,   BLACK,   1);

   my_lcd.Set_Draw_color(WHITE);
   my_lcd.Fill_Rectangle(0, 106, 240, 106);
   my_lcd.Fill_Rectangle(0, 212, 240, 212);
   page = 1;

}

void show_master_menu(void)
{

              //reset rfid (also master key, if applicable) after getting access
              rfid = 0;
               
              //clear screen
              my_lcd.Fill_Screen(BLUE);

              //text on screen
              show_string("OPSTARTEN",  my_lcd.Get_Display_Width()/2-strlen("OPSTARTEN")*3*6/2+1,  my_lcd.Get_Display_Height()/6-3*8/2+1,  3,   WHITE,   BLACK,   1);
              show_string("SCHOONMAKEN",  my_lcd.Get_Display_Width()/2-strlen("SCHOONMAKEN")*3*6/2+1,  my_lcd.Get_Display_Height()/2-3*8/2+1,  3,   WHITE,   BLACK,   1);
              show_string("DRINKS",  my_lcd.Get_Display_Width()/2-strlen("DRINKS")*3*6/2+1,  5*my_lcd.Get_Display_Height()/6-3*8/2+1,  3,   WHITE,   BLACK,   1);

              //lines between text
              my_lcd.Set_Draw_color(WHITE);
              my_lcd.Fill_Rectangle(0, 106, 240, 106);
              my_lcd.Fill_Rectangle(0, 212, 240, 212);

              page = 9999;
              return;
  
}


void get_access(void)
{
  my_lcd.Fill_Screen(BLUE);
  show_string("Welkom",             25,                 1*my_lcd.Get_Display_Height()/8,   5,   CYAN,      BLACK,   1);
  show_string("SCAN",               25,                 4*my_lcd.Get_Display_Height()/11,  4,   MAGENTA,   BLACK,   1);
  show_string("RFID TAG",           25,                 6*my_lcd.Get_Display_Height()/11,  4,   MAGENTA,   BLACK,   1);
}


void kort() {

    tone(tonePin, 1975, 12.8530625198);
    delay(15.8679784195);
    tone(tonePin, 1318, 19.2795937798);
    delay(23.8019676293);

}

void lang() {

    tone(tonePin, 622, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 659, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 698, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 739, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 783, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 830, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 880, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 932, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 987, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 1046, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 1108, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 1174, 17.4418604651);
    delay(19.3798449612);
    delay(9.68992248062);
    tone(tonePin, 1318, 26.1627906977);
    delay(29.0697674419);
    
}


////////// TRANSMITTER //////////

// PINS
RF24 myRadio (38, 37); //CE, CSN

const byte address[6] = "00001";



                            
void setup(void) 
{
  
  Serial.begin(115200);                         // Stel de seriële monitor in
  delay(1000);
  myRadio.begin();  
  myRadio.setChannel(115);
  myRadio.setPALevel(RF24_PA_MIN);              //power amplifier level
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openWritingPipe(address);
  myRadio.stopListening();
  delay(1000);
  
   my_lcd.Init_LCD();
   Serial.println(my_lcd.Read_ID(), HEX);
   my_lcd.Fill_Screen(BLUE);

   // speaker
   pinMode(36, OUTPUT);

   // lcd
   pinMode(A0, INPUT);        //READ (RD)
   pinMode(A1, OUTPUT);       //WRITE (WR)

   // leds
   pinMode(14, OUTPUT); //LED1
   pinMode(16, OUTPUT); //LED2
   pinMode(41, OUTPUT); //TRILLINGSMOTOR
   
  SPI.begin();          // Initialiseer de SPI bus
  mfrc522.PCD_Init();   // Initialiseer de MFRC522 lezer

  // Print MFRC522 Card Reader details naar seriële monitor
  mfrc522.PCD_DumpVersionToSerial();

  get_access();
  Serial.println("System ready...");
       
}


uint16_t text_x=10,text_y=6,text_x_add = 6*3,text_y_add = 8*3;
uint16_t n=0;

void loop(void)
{  

  uint16_t i;

  //here it reads the touchscreen input
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    //p.x = my_lcd.Get_Display_Width()-map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
    //p.y = my_lcd.Get_Display_Height()-map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(), 0);
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, my_lcd.Get_Display_Width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, my_lcd.Get_Display_Height());
     
  }


  if(page == 0 && rfid == 0)
      {
        getCardID();
      }
    
    if(page == 0 && rfid == 1)
      {
        show_menu();
      }

    if((page == 0 && rfid == 2) | (is_pressed(0, 280,  40,  321,  p.x,  p.y) && page == 6000))
      {
        show_master_menu();
        return;
      }


////////////// MASTER OPTIONS //////////////
if(is_pressed(1, 1,  240,  106,  p.x,  p.y) && page == 9999){
  
  order = 100;      // START-UP SEQUENCE (filling pumps)
  confirmation_page = 1;
  page = 6000;

  return;
  
  }


if(is_pressed(1, 107,  240,  212,  p.x,  p.y) && page == 9999){
  
  order = 101;      // CLEANING (rinsing pumps)
  confirmation_page = 1;
  page = 6000;

  return;
  
  }

if(is_pressed(1, 213,  240,  320,  p.x,  p.y) && page == 9999){

  show_menu();      // TO THE MAIN MENU
  
  return;
  
  }
////////////////////////////////////////////

   
         //if cocktails via menu or BACK (= RED button on page 3 of cocktails)
         if((is_pressed(1, 1,  240,  106,  p.x,  p.y) && page == 1) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 33) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 44) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 1000))
         {
              
              //clear screen
              my_lcd.Fill_Screen(BLUE);
              
              //create 'home' button
              my_lcd.Set_Draw_color(RED);
              my_lcd.Fill_Rectangle(0, 321, 40, 280);
              show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

              //create 'next page' button
              my_lcd.Set_Draw_color(GREEN);
              my_lcd.Fill_Rectangle(200, 321, 240, 280);
              show_string(">>",    203,                 290,  3,   WHITE,   BLACK,   1);
              
              show_string("MOJITO",  my_lcd.Get_Display_Width()/2-strlen("MOJITO")*2*6/2+1,  1*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("CUBA LIBRE",  my_lcd.Get_Display_Width()/2-strlen("CUBA LIBRE")*2*6/2+1,  3*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("DAIQUIRI",  my_lcd.Get_Display_Width()/2-strlen("DAIQUIRI")*2*6/2+1,  5*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("TOM COLLINS (G&T)",  my_lcd.Get_Display_Width()/2-strlen("TOM COLLINS (G&T)")*2*6/2+1,  7*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("CAPE COD",  my_lcd.Get_Display_Width()/2-strlen("CAPE COD")*2*6/2+1,  9*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);

              //??
              my_lcd.Set_Draw_color(CYAN);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
        
        my_lcd.Set_Draw_color(WHITE);
        my_lcd.Fill_Rectangle(0, 1*(320/5), 240, 1*(320/5));
        my_lcd.Fill_Rectangle(0, 2*(320/5), 240, 2*(320/5));
        my_lcd.Fill_Rectangle(0, 3*(320/5), 240, 3*(320/5));
        my_lcd.Fill_Rectangle(0, 4*(320/5), 240, 4*(320/5));

                  page = 22; //we use '22' to distinguish page two of cocktails from page two of longdrinks and shots bc we need to add a 'next page' button

        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;                  
         
         }


         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 22){
         
         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Witte rum",  my_lcd.Get_Display_Width()/2-strlen("Witte rum")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Tonic",  my_lcd.Get_Display_Width()/2-strlen("Tonic")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Muntblaadjes",  my_lcd.Get_Display_Width()/2-strlen("Muntblaadjes")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Rietsuiker",  my_lcd.Get_Display_Width()/2-strlen("Rietsuiker")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  11*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 44;

         return;
         
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 22){
         
         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Witte rum",  my_lcd.Get_Display_Width()/2-strlen("Witte rum")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Coca-Cola",  my_lcd.Get_Display_Width()/2-strlen("Coca-Cola")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 44;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 22){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Witte rum",  my_lcd.Get_Display_Width()/2-strlen("Witte rum")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Suiker",  my_lcd.Get_Display_Width()/2-strlen("Suiker")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 44;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 22){
         
         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Gin",  my_lcd.Get_Display_Width()/2-strlen("Gin")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Tonic",  my_lcd.Get_Display_Width()/2-strlen("Tonic")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("(Poeder)suiker",  my_lcd.Get_Display_Width()/2-strlen("(Poeder)suiker")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Botanicals",  my_lcd.Get_Display_Width()/2-strlen("Botanicals")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Komkommer",  my_lcd.Get_Display_Width()/2-strlen("Komkommer")*2*6/2+1,  11*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  13*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 44;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 22){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Wodka",  my_lcd.Get_Display_Width()/2-strlen("Wodka")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Cranberrysap",  my_lcd.Get_Display_Width()/2-strlen("Cranberrysap")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 44;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 33){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Gin",  my_lcd.Get_Display_Width()/2-strlen("Gin")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Suiker",  my_lcd.Get_Display_Width()/2-strlen("Suiker")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Muntblaadjes",  my_lcd.Get_Display_Width()/2-strlen("Muntblaadjes")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 55;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 33){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Witte rum",  my_lcd.Get_Display_Width()/2-strlen("Witte rum")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Rietsuiker",  my_lcd.Get_Display_Width()/2-strlen("Rietsuiker")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 55;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 33){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         
         // INGREDIENTS
         show_string("Wodka",  my_lcd.Get_Display_Width()/2-strlen("Wodka")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Witte rum",  my_lcd.Get_Display_Width()/2-strlen("Witte rum")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Gin",  my_lcd.Get_Display_Width()/2-strlen("Gin")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Coca-Cola",  my_lcd.Get_Display_Width()/2-strlen("Coca-Cola")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("(Tequilla)",  my_lcd.Get_Display_Width()/2-strlen("(Tequilla)")*2*6/2+1,  11*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("(Triple Sec)",  my_lcd.Get_Display_Width()/2-strlen("(Triple Sec)")*2*6/2+1,  13*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 55;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 33){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Wodka",  my_lcd.Get_Display_Width()/2-strlen("Wodka")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Tonic",  my_lcd.Get_Display_Width()/2-strlen("Tonic")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Violetsuiker",  my_lcd.Get_Display_Width()/2-strlen("Violetsuiker")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Frambozen",  my_lcd.Get_Display_Width()/2-strlen("Frambozen")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  11*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 55;

         return;

          
         }

         
         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 33){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Wodka",  my_lcd.Get_Display_Width()/2-strlen("Wodka")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Appelsiensap",  my_lcd.Get_Display_Width()/2-strlen("Appelsiensap")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Cranberrysap",  my_lcd.Get_Display_Width()/2-strlen("Cranberrysap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         
         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 55;

         return;

          
         }


         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 66){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Wodka",  my_lcd.Get_Display_Width()/2-strlen("Wodka")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Gemberbier",  my_lcd.Get_Display_Width()/2-strlen("Gemberbier")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 77;

         return;

          
         }


         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 66){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Gin",  my_lcd.Get_Display_Width()/2-strlen("Gin")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Gemberbier",  my_lcd.Get_Display_Width()/2-strlen("Gemberbier")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Rietsuiker",  my_lcd.Get_Display_Width()/2-strlen("Rietsuiker")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Verse munt",  my_lcd.Get_Display_Width()/2-strlen("Verse munt")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 77;

         return;

          
         }


         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 66){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Witte rum",  my_lcd.Get_Display_Width()/2-strlen("Witte rum")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Gemberbier",  my_lcd.Get_Display_Width()/2-strlen("Gemberbier")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);

         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 77;

         return;

          
         }


         if(is_pressed(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16,  p.x,  p.y) && page == 66){

         // clear screen
         my_lcd.Fill_Screen(BLUE);

         // INGREDIENTS
         show_string("Wodka",  my_lcd.Get_Display_Width()/2-strlen("Wodka")*2*6/2+1,  1*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Gemberbier",  my_lcd.Get_Display_Width()/2-strlen("Gemberbier")*2*6/2+1,  3*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Cranberrysap",  my_lcd.Get_Display_Width()/2-strlen("Cranberrysap")*2*6/2+1,  5*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Limoensap",  my_lcd.Get_Display_Width()/2-strlen("Limoensap")*2*6/2+1,  7*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         show_string("Ijs",  my_lcd.Get_Display_Width()/2-strlen("Ijs")*2*6/2+1,  9*my_lcd.Get_Display_Height()/16-2*8/2+1+20,  2,  WHITE,BLACK,1);
         
         // create 'home' button
         my_lcd.Set_Draw_color(RED);
         my_lcd.Fill_Rectangle(0, 321, 40, 280);
         show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

         page = 77;

         return;

          
         }

         
         //if longdrinks page 222
        if((is_pressed(1, 107,  240,  212,  p.x,  p.y) && page == 1) | (is_pressed(1, 280,  40,  320,  p.x,  p.y) && page == 333) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 2000))
         {
              
              //clear screen
              my_lcd.Fill_Screen(BLUE);
              
              //create 'home' button
              my_lcd.Set_Draw_color(RED);
              my_lcd.Fill_Rectangle(0, 321, 40, 280);
              show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

              show_string("WODKA",  my_lcd.Get_Display_Width()/2-strlen("WODKA")*3*6/2+1,   my_lcd.Get_Display_Height()/6-3*8/2+1,  3,  WHITE,BLACK,1);
              show_string("WITTE RUM",  my_lcd.Get_Display_Width()/2-strlen("WITTE RUM")*3*6/2+1,   my_lcd.Get_Display_Height()/2-3*8/2+1,  3,  WHITE,BLACK,1);
              show_string("GIN",  my_lcd.Get_Display_Width()/2-strlen("GIN")*3*6/2+1,  5*my_lcd.Get_Display_Height()/6-3*8/2+1,  3,  WHITE,BLACK,1);
        
        my_lcd.Set_Draw_color(WHITE);
        my_lcd.Fill_Rectangle(0, 106, 240, 106);
        my_lcd.Fill_Rectangle(0, 212, 240, 212);

                  page = 222;

         // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
         return;
         
         }

         
        //if longdrinks page 333
        if(is_pressed(41, 1,  240,  279,  p.x,  p.y) && page == 222)
         {

              //what did you choose?
              //// BOOZE ////

              if(is_pressed(0, 0, 240, 106,  p.x,  p.y)){
              booze = 1;                                                     //////// WODKA ////////
          }


              if(is_pressed(0, 107, 240, 212,  p.x,  p.y)){
              booze = 2;                                                     //////// WITTE RUM ////////
          }

              if(is_pressed(0, 213, 240, 320,  p.x,  p.y)){
              booze = 3;                                                     //////// GIN ////////
          }
              
              //clear screen
              my_lcd.Fill_Screen(BLUE);
              
              //create 'home' button
              my_lcd.Set_Draw_color(RED);
              my_lcd.Fill_Rectangle(0, 321, 40, 280);
              show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);
        
        my_lcd.Set_Draw_color(WHITE);
        my_lcd.Fill_Rectangle(0, 1*(320/5), 240, 1*(320/5));
        my_lcd.Fill_Rectangle(0, 2*(320/5), 240, 2*(320/5));
        my_lcd.Fill_Rectangle(0, 3*(320/5), 240, 3*(320/5));
        my_lcd.Fill_Rectangle(0, 4*(320/5), 240, 4*(320/5));

              show_string("SINAASAPPELSAP",  my_lcd.Get_Display_Width()/2-strlen("SINAASAPPELSAP")*2*6/2+1,  1*my_lcd.Get_Display_Height()/10-2*8/2+1,  2,  WHITE,BLACK,1);
              show_string("TONIC",  my_lcd.Get_Display_Width()/2-strlen("TONIC")*2*6/2+1,  3*my_lcd.Get_Display_Height()/10-2*8/2+1,  2,  WHITE,BLACK,1);
              show_string("COCA-COLA",  my_lcd.Get_Display_Width()/2-strlen("COCA-COLA")*2*6/2+1,  5*my_lcd.Get_Display_Height()/10-2*8/2+1,  2,  WHITE,BLACK,1);
              show_string("COCA-COLA ZERO",  my_lcd.Get_Display_Width()/2-strlen("COCA-COLA ZERO")*2*6/2+1,  7*my_lcd.Get_Display_Height()/10-2*8/2+1,  2,  WHITE,BLACK,1);
              show_string("CANADA DRY",  my_lcd.Get_Display_Width()/2-strlen("CANADA DRY")*2*6/2+1,  9*my_lcd.Get_Display_Height()/10-2*8/2+1,  2,  WHITE,BLACK,1);

                  page = 333;

        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;                  

         }
                  

         //if SHOTS
             if((is_pressed(1, 213,  240,  320,  p.x,  p.y) && page == 1) | (is_pressed(0, 280,  40,  321,  p.x,  p.y) && page == 3) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 3000))
         {
              
              //clear screen
              my_lcd.Fill_Screen(BLUE);
              
              //create 'home' button
              my_lcd.Set_Draw_color(RED);
              my_lcd.Fill_Rectangle(0, 321, 40, 280);
              show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

              show_string("WODKA",  my_lcd.Get_Display_Width()/2-strlen("WODKA")*3*6/2+1,   my_lcd.Get_Display_Height()/6-3*8/2+1,  3,  WHITE,BLACK,1);
              show_string("WITTE RUM",  my_lcd.Get_Display_Width()/2-strlen("WITTE RUM")*3*6/2+1,   my_lcd.Get_Display_Height()/2-3*8/2+1,  3,  WHITE,BLACK,1);
              show_string("GIN",  my_lcd.Get_Display_Width()/2-strlen("GIN")*3*6/2+1,  5*my_lcd.Get_Display_Height()/6-3*8/2+1,  3,  WHITE,BLACK,1);
         
         my_lcd.Set_Draw_color(WHITE);
         my_lcd.Fill_Rectangle(0, 106, 240, 106);
         my_lcd.Fill_Rectangle(0, 212, 240, 212);

                  page = 2;

        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;                           
          
          }


          //if HOME
             if(is_pressed(1, 280, 40, 320,  p.x,  p.y) && (page == 2 | page == 22 | page == 222))
         {
             
                my_lcd.Fill_Screen(BLUE);
             
             show_string("COCKTAILS",  my_lcd.Get_Display_Width()/2-strlen("COCKTAILS")*3*6/2+1,  my_lcd.Get_Display_Height()/6-3*8/2+1,  3,   WHITE,   BLACK,   1);
             show_string("LONGDRINKS",  my_lcd.Get_Display_Width()/2-strlen("LONGDRINKS")*3*6/2+1,  my_lcd.Get_Display_Height()/2-3*8/2+1,  3,   WHITE,   BLACK,   1);
             show_string("SHOTS",  my_lcd.Get_Display_Width()/2-strlen("SHOTS")*3*6/2+1,  5*my_lcd.Get_Display_Height()/6-3*8/2+1,  3,   WHITE,   BLACK,   1);

          my_lcd.Set_Draw_color(WHITE);
          my_lcd.Fill_Rectangle(0, 106, 240, 106);
          my_lcd.Fill_Rectangle(0, 212, 240, 212);

                  page = 1;

        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;                  
     
         }


          //if NEXT PAGE or back from '?' (= page 2 of cocktails)
             if((is_pressed(200, 280, 240, 320,  p.x,  p.y) && page == 22) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 55) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 66) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 4000))
         {
          

              //clear screen
              my_lcd.Fill_Screen(BLUE);

              //create 'back' button
              my_lcd.Set_Draw_color(RED);
              my_lcd.Fill_Rectangle(0, 321, 40, 280);
              show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);

              //create 'next page' button
              my_lcd.Set_Draw_color(GREEN);
              my_lcd.Fill_Rectangle(200, 321, 240, 280);
              show_string(">>",    203,                 290,  3,   WHITE,   BLACK,   1);
              
              show_string("RICHMOND GIMLET",  my_lcd.Get_Display_Width()/2-strlen("RICHMOND GIMLET")*2*6/2+1,  1*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("CAIPIRANHA",  my_lcd.Get_Display_Width()/2-strlen("CAIPIRANHA")*2*6/2+1,  3*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("L. ISLAND ICED TEA",  my_lcd.Get_Display_Width()/2-strlen("L. ISLAND ICED TEA")*2*6/2+1,  5*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("LAZY RED CHEEKS",  my_lcd.Get_Display_Width()/2-strlen("LAZY RED CHEEKS")*2*6/2+1,  7*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("MADRAS",  my_lcd.Get_Display_Width()/2-strlen("MADRAS")*2*6/2+1,  9*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
        
        my_lcd.Set_Draw_color(WHITE);
        my_lcd.Fill_Rectangle(0, 1*(320/5), 240, 1*(320/5));
        my_lcd.Fill_Rectangle(0, 2*(320/5), 240, 2*(320/5));
        my_lcd.Fill_Rectangle(0, 3*(320/5), 240, 3*(320/5));
        my_lcd.Fill_Rectangle(0, 4*(320/5), 240, 4*(320/5));

              //??
              my_lcd.Set_Draw_color(CYAN);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);

              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  9*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);

                  page = 33;

        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
     
         }



//if NEXT PAGE or back from '?' (= page 3 of cocktails)
             if((is_pressed(200, 280, 240, 320,  p.x,  p.y) && page == 33) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 77) | (is_pressed(1, 280, 40, 320,  p.x,  p.y) && page == 5000))
         {
          

              //clear screen
              my_lcd.Fill_Screen(BLUE);

              //create 'back' button
              my_lcd.Set_Draw_color(RED);
              my_lcd.Fill_Rectangle(0, 321, 40, 280);
              show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);
              
              show_string("MOSCOW MULE",  my_lcd.Get_Display_Width()/2-strlen("MOSCOW MULE")*2*6/2+1,  1*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("GIN-GIN MULE",  my_lcd.Get_Display_Width()/2-strlen("GIN-GIN MULE")*2*6/2+1,  3*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("DARK & STORMY",  my_lcd.Get_Display_Width()/2-strlen("DARK & STORMY")*2*6/2+1,  5*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
              show_string("GINGER LIME RICKEY",  my_lcd.Get_Display_Width()/2-strlen("GINGER LIME RICKEY")*2*6/2+1,  7*my_lcd.Get_Display_Height()/10-2*8/2+1-7,  2,  WHITE,BLACK,1);
        
        my_lcd.Set_Draw_color(WHITE);
        my_lcd.Fill_Rectangle(0, 1*(320/5), 240, 1*(320/5));
        my_lcd.Fill_Rectangle(0, 2*(320/5), 240, 2*(320/5));
        my_lcd.Fill_Rectangle(0, 3*(320/5), 240, 3*(320/5));
        my_lcd.Fill_Rectangle(0, 4*(320/5), 240, 4*(320/5));

              //??
              my_lcd.Set_Draw_color(CYAN);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);
              my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1-25,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20-3, my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1+35,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20+16);

              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  1*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  3*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  5*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);
              show_string("?",  my_lcd.Get_Display_Width()/2-strlen("?")*2*6/2+1,  7*my_lcd.Get_Display_Height()/10-2*8/2+1+20,  2,  BLACK,BLACK,1);

                  page = 66;

        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
     
         }





         //if SHOTS page 3
             if(is_pressed(41, 1, 240, 279,  p.x,  p.y) && page == 2)
         {

              //what did you choose?
              //// BOOZE ////

              if(is_pressed(0, 0, 240, 106,  p.x,  p.y) && page == 2){
              booze = 1;                                                     //////// WODKA ////////
          }


              if(is_pressed(0, 107, 240, 212,  p.x,  p.y) && page == 2){
              booze = 2;                                                     //////// WITTE RUM ////////
          }

              if(is_pressed(0, 213, 240, 320,  p.x,  p.y) && page == 2){
              booze = 3;                                                     //////// GIN ////////
          }

              //clear screen
              my_lcd.Fill_Screen(BLUE);

              //create 'back' button
              my_lcd.Set_Draw_color(RED);
              my_lcd.Fill_Rectangle(0, 321, 40, 280);
              show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);
              
              show_string("SINGLE",  my_lcd.Get_Display_Width()/2-strlen("SINGLE")*4*6/2+1,  1*my_lcd.Get_Display_Height()/6-4*8/2+1,  4,  WHITE,BLACK,1);
              show_string("40 ML",  my_lcd.Get_Display_Width()/2-strlen("40 ml")*2*6/2+1,  2*my_lcd.Get_Display_Height()/6-2*8/2+1,  2,  WHITE,BLACK,1);
              show_string("DOUBLE",  my_lcd.Get_Display_Width()/2-strlen("DOUBLE")*4*6/2+1,  4*my_lcd.Get_Display_Height()/6-4*8/2+1,  4,  WHITE,BLACK,1);
              show_string("80 ML",  my_lcd.Get_Display_Width()/2-strlen("80 ml")*2*6/2+1,  5*my_lcd.Get_Display_Height()/6-2*8/2+1,  2,  WHITE,BLACK,1);
        
        my_lcd.Set_Draw_color(WHITE);
        my_lcd.Fill_Rectangle(0, 320/2, 240, 320/2);

                  page = 3;

        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the touch does not 'stay on the screen'        
        return;
     
         }






/// Determine ORDER

//// COCKTAILS ////

if(is_pressed(0, 0, 240, 64,  p.x,  p.y) && page == 22){
        order = 1;                                              //////// MOJITO = 1 ////////
        page = 1000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 65, 240, 128,  p.x,  p.y) && page == 22){
        order = 2;                                              //////// CUBA LIBRE = 2 ////////
        page = 1000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 129, 240, 192,  p.x,  p.y) && page == 22){
        order = 3;                                              //////// DAIQUIRI = 3 ////////
        page = 1000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 193, 240, 256,  p.x,  p.y) && page == 22){
        order = 4;                                              //////// TOM COLLINS (G&T) = 4 ////////
        page = 1000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 257, 240, 320,  p.x,  p.y) && page == 22){
        order = 5;                                              //////// CAPE COD = 5 ////////
        page = 1000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 0, 240, 64,  p.x,  p.y) && page == 33){
        order = 6;                                              //////// RICHMOND GIMLET = 6 ////////
        page = 4000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 65, 240, 128,  p.x,  p.y) && page == 33){
        order = 7;                                              //////// CAIPIRANHA = 7 ////////
        page = 4000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 129, 240, 192,  p.x,  p.y) && page == 33){
        order = 8;                                              //////// L. ISLAND ICED TEA = 8 ////////
        page = 4000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }
  

if(is_pressed(0, 193, 240, 256,  p.x,  p.y) && page == 33){
        order = 9;                                              //////// LAZY RED CHEEKS = 9 ////////
        page = 4000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if(is_pressed(0, 257, 240, 320,  p.x,  p.y) && page == 33){
        order = 10;                                              //////// MADRAS = 10 ////////
        page = 4000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
  }


if(is_pressed(0, 0, 240, 64,  p.x,  p.y) && page == 66){
        order = 32;                                              //////// MOSCOW MULE = 32 ////////
        page = 5000;
        confirmation_page = 1;

        return;

  }

if(is_pressed(0, 65, 240, 128,  p.x,  p.y) && page == 66){
        order = 33;                                              //////// GIN-GIN MULE = 33 ////////
        page = 5000;
        confirmation_page = 1;

        return;
  }

if(is_pressed(0, 129, 240, 192,  p.x,  p.y) && page == 66){
        order = 34;                                              //////// DARK & STORMY = 34 ////////
        page = 5000;
        confirmation_page = 1;

        return;
  }

if(is_pressed(0, 193, 240, 256,  p.x,  p.y) && page == 66){
        order = 35;                                              //////// GINGER LIME RICKEY = 35 ////////
        page = 5000;
        confirmation_page = 1;

        return;
  }


//// LONGDRINKS ////

if((is_pressed(0, 0, 240, 64,  p.x,  p.y) && page == 333) && booze == 1){
        order = 11;                                              //////// WODKA+SINAASAPPELSAP = 11 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 65, 240, 128,  p.x,  p.y) && page == 333) && booze == 1){
        order = 12;                                              //////// WODKA+TONIC = 12 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 129, 240, 192,  p.x,  p.y) && page == 333) && booze == 1){
        order = 13;                                              //////// WODKA+COCA-COLA = 13 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 193, 240, 256,  p.x,  p.y) && page == 333) && booze == 1){
        order = 14;                                              //////// WODKA+COCA-COLA_ZERO = 14 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 257, 240, 320,  p.x,  p.y) && page == 333) && booze == 1){
        order = 15;                                              //////// WODKA+CANADA DRY = 15 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 0, 240, 64, p.x, p.y) && page == 333) && booze == 2){
        order = 16;                                              //////// WITTE RUM+SINAASAPPELSAP = 16 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 65, 240, 128, p.x, p.y) && page == 333) && booze == 2){
        order = 17;                                              //////// WITTE RUM+TONIC = 17 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 129, 240, 192, p.x, p.y) && page == 333) && booze == 2){
        order = 18;                                              //////// WITTE RUM+COCA-COLA = 18 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 193, 240, 256, p.x, p.y) && page == 333) && booze == 2){
        order = 19;                                              //////// WITTE RUM+COCA-COLA_ZERO = 19 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 257, 240, 320, p.x, p.y) && page == 333) && booze == 2){
        order = 20;                                              //////// WITTE RUM+CANADA DRY = 20 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 0, 240, 64, p.x, p.y) && page == 333) && booze == 2){
        order = 21;                                              //////// GIN+SINAASAPPELSAP = 21 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 65, 240, 128, p.x, p.y) && page == 333) && booze == 3){
        order = 22;                                              //////// GIN+TONIC = 22 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 129, 240, 192, p.x, p.y) && page == 333) && booze == 3){
        order = 23;                                              //////// GIN+COCA-COLA = 23 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 193, 240, 256, p.x, p.y) && page == 333) && booze == 3){
        order = 24;                                              //////// GIN+COCA-COLA_ZERO = 24 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 257, 240, 320, p.x, p.y) && page == 333) && booze == 3){
        order = 25;                                              //////// GIN+CANADA DRY = 25 ////////
        page = 2000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }


//// SHOTS ////


if((is_pressed(0, 0, 240, 160,  p.x,  p.y) && page == 3) && booze == 1){
        order = 26;                                              //////// WODKA_SINGLE = 26 ////////
        page = 3000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 161, 240, 320,  p.x,  p.y) && page == 3) && booze == 1){
        order = 27;                                              //////// WODKA_DOUBLE = 27 ////////
        page = 3000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 0, 240, 160,  p.x,  p.y) && page == 3) && booze == 2){
        order = 28;                                              //////// WITTE RUM_SINGLE = 28 ////////
        page = 3000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 161, 240, 320,  p.x,  p.y) && page == 3) && booze == 2){
        order = 29;                                              //////// WITTE RUM_DOUBLE = 29 ////////
        page = 3000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 0, 240, 160,  p.x,  p.y) && page == 3) && booze == 3){
        order = 30;                                              //////// GIN_SINGLE = 30 ////////
        page = 3000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }

if((is_pressed(0, 161, 240, 320,  p.x,  p.y) && page == 3) && booze == 3){
        order = 31;                                              //////// GIN_DOUBLE = 31 ////////
        page = 3000;
        confirmation_page = 1;
                  
                  
        // VERY IMPORTANT !! // returns to the beginning of the loop // this ensures that the   touch does not 'stay on the screen'        
        return;
        
  }
  

if (confirmation_page == 1){                                                       
      
      //clear screen
      my_lcd.Fill_Screen(BLUE);
      
      //show drink and ask for confirmation
      show_string("Ready?",    my_lcd.Get_Display_Width()/2-strlen("Ready?")*3*6/2+3,                 my_lcd.Get_Display_Height()/5-13,  3,   WHITE,   BLACK,   1);

      //create 'home' button
      my_lcd.Set_Draw_color(RED);
      my_lcd.Fill_Rectangle(0, 321, 40, 280);
      show_string("<<",    1,                 290,  3,   WHITE,   BLACK,   1);


///// COCKTAILS /////
if(order == 1){
show_string("MOJITO", my_lcd.Get_Display_Width()/2-strlen("MOJITO")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 2){
show_string("CUBA LIBRE", my_lcd.Get_Display_Width()/2-strlen("CUBA LIBRE")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 3){
show_string("DAIQUIRI", my_lcd.Get_Display_Width()/2-strlen("DAIQUIRI")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 4){
show_string("TOM COLLINS (G&T)", my_lcd.Get_Display_Width()/2-strlen("TOM COLLINS (G&T)")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 5){
show_string("CAPE COD", my_lcd.Get_Display_Width()/2-strlen("CAPE COD")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 6){
show_string("RICHMOND GIMLET", my_lcd.Get_Display_Width()/2-strlen("RICHMOND GIMLET")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 7){
show_string("CAIPIRANHA", my_lcd.Get_Display_Width()/2-strlen("CAIPIRANHA")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 8){
show_string("L. ISLAND ICED TEA", my_lcd.Get_Display_Width()/2-strlen("L. ISLAND ICED TEA")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 9){
show_string("LAZY RED CHEEKS", my_lcd.Get_Display_Width()/2-strlen("LAZY RED CHEEKS")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 10){
show_string("MADRAS", my_lcd.Get_Display_Width()/2-strlen("MADRAS")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 32){
show_string("MOSCOW MULE", my_lcd.Get_Display_Width()/2-strlen("MOSCOW MULE")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 33){
show_string("GIN-GIN MULE", my_lcd.Get_Display_Width()/2-strlen("GIN-GIN MULE")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 34){
show_string("DARK & STORMY", my_lcd.Get_Display_Width()/2-strlen("DARK & STORMY")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(order == 35){
show_string("GINGER LIME RICKEY", my_lcd.Get_Display_Width()/2-strlen("GINGER LIME RICKEY")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}



///// BOOZE /////
if((order > 10)  && (order < 32)){

if(booze == 1){
show_string("WODKA", my_lcd.Get_Display_Width()/2-strlen("WODKA")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(booze == 2){
show_string("WITTE RUM", my_lcd.Get_Display_Width()/2-strlen("WITTE RUM")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

if(booze == 3){
show_string("GIN", my_lcd.Get_Display_Width()/2-strlen("GIN")*2*6/2, 100,  2,   GREEN,   BLACK,   1);
}

}


///// SODA /////

if(order == 11 | order == 16 | order == 21){
  show_string("SINAASAPPELSAP", my_lcd.Get_Display_Width()/2-strlen("SINAASAPPELSAP")*2*6/2 , 125,  2,   GREEN,   BLACK,   1);
}


if(order == 12 | order == 17 | order == 22){
  show_string("TONIC", my_lcd.Get_Display_Width()/2-strlen("TONIC")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}

if(order == 13 | order == 18 | order == 23){
  show_string("COCA-COLA", my_lcd.Get_Display_Width()/2-strlen("COCA-COLA")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}

if(order == 14 | order == 19 | order == 24){
  show_string("COCA-COLA ZERO", my_lcd.Get_Display_Width()/2-strlen("COCA-COLA ZERO")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}

if(order == 15 | order == 20 | order == 25){
  show_string("CANADA DRY", my_lcd.Get_Display_Width()/2-strlen("CANADA DRY")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}



///// SINGLE/DOUBLE /////
if(order == 26 | order == 28 | order == 30){
  show_string("SINGLE", my_lcd.Get_Display_Width()/2-strlen("SINGLE")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}

if(order == 27 | order == 29 | order == 31){
  show_string("DOUBLE", my_lcd.Get_Display_Width()/2-strlen("DOUBLE")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}



///// MASTER KEY OPTIONS ////
if(order == 100){
  show_string("OPSTARTEN", my_lcd.Get_Display_Width()/2-strlen("OPSTARTEN")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}

if(order == 101){
  show_string("SCHOONMAKEN", my_lcd.Get_Display_Width()/2-strlen("SCHOONMAKEN")*2*6/2, 125,  2,   GREEN,   BLACK,   1);
}


      //create 'Let's roll!' button
      my_lcd.Set_Draw_color(GREEN);
      my_lcd.Fill_Rectangle(0, 250, 240, 180);
      show_string("LET'S ROLL!", 25, 204,  3,   WHITE,   BLACK,   1);

      //prevent looping
      confirmation_page = 2;

      return;
        
}



if(is_pressed(0, 180,  240,  250,  p.x,  p.y) && confirmation_page == 2){

          confirmation = 1;
          
          // Vibration
          digitalWrite(41,HIGH);
          lang();
          delay(200);
          digitalWrite(41,LOW);

          return;
        
      }

if(confirmation == 1){


    struct package
        {
        int drink = order;
         };
 
    typedef struct package Package;
    Package data; 

         
         myRadio.write(&data, sizeof(data)); 
         delay(1000);
        Serial.print("drankje: "); Serial.println(order);

        // reset all markers for new input
        confirmation        = 0;
        confirmation_page   = 0;
        page                = 0;
        order               = 0;
        booze               = 0;

        // go back to square one // security (rifd)
        get_access();
          
        }
  
}
