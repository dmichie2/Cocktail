// FastLED code by Mark Kriegsman (2014) www.fastled.io

#include <SPI.h>  
#include "RF24.h"
#include <FastLED.h>

             
                    /////          LED RING INFO          /////
                    /////         PIN 30 =  1   LED       /////
                    /////         PIN 31 =  24  LEDS      /////
                    /////         PIN 32 =  12  LEDS      /////
                    /////         PIN 33 =  8   LEDS      /////

                    
          //#define CLK_PIN            4
          #define LED_TYPE             WS2812
          #define COLOR_ORDER          RGB

          #define DATA_PIN_1           31
          #define NUM_LEDS_1           1
          CRGB leds_1[NUM_LEDS_1];

          #define DATA_PIN_24          33
          #define NUM_LEDS_24          24
          CRGB leds_24[NUM_LEDS_24];

          #define DATA_PIN_12          30
          #define NUM_LEDS_12          12
          CRGB leds_12[NUM_LEDS_12];

          #define DATA_PIN_8           32
          #define NUM_LEDS_8           8
          CRGB leds_8[NUM_LEDS_8];
          
          #define BRIGHTNESS          150
          #define FRAMES_PER_SECOND   120

          ///// RECEIVER  /////
          RF24 myRadio (7, 10);     //CE, CSN

          struct package
          {
            int drink;
          };
          
          typedef struct package Package;
          Package data;
          const byte address[6] = "00001";

          ///// DISTANCE SENSOR  /////
          #define trigPin 8
          #define echoPin 9
          
          long  duration;
          float distanceInch;
          int   glass;

          int redPin = 12;
          int greenPin;
          int bluePin = 11;
          
          ///// PUMPS  /////          Which pumps are working at a certain moment?
          #define   wodka                44
          #define   witte_rum            40
          #define   gin                  45
          #define   limoensap            39
          
          #define   sinaasappelsap       36
          #define   cola                 37
          #define   cranberry            38
          #define   tonic                41
          #define   canada               42
          #define   cola_zero            43

          ///// PUMPING TIME  /////          How long should each pump operate? Time in seconds.
          int wodka_time            = 0;
          int witte_rum_time        = 0;
          int gin_time              = 0;
          int limoensap_time        = 0;
          
          int sinaasappelsap_time   = 0;
          int cola_time             = 0;
          int cranberry_time        = 0;
          int tonic_time            = 0;
          int canada_time           = 0;
          int cola_zero_time        = 0;

void setup() 
{


cli();//stop interrupts
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);  
sei();//allow interrupts


          ///// DISTANCE SENSOR  /////
          pinMode(echoPin, INPUT);
          pinMode(trigPin, OUTPUT);

          ///// PUMPS /////
          pinMode(36, OUTPUT); 
          pinMode(37, OUTPUT); 
          pinMode(38, OUTPUT);
          pinMode(39, OUTPUT);
          pinMode(40, OUTPUT); 
          pinMode(41, OUTPUT); 
          pinMode(42, OUTPUT); 
          pinMode(43, OUTPUT);
          pinMode(44, OUTPUT); 
          pinMode(45, OUTPUT);

          // Pin is now LOW by default, so pumps are working --> due to assembly of relay
          // We want to set it to HIGH (not working) as default.
          digitalWrite(36, HIGH); 
          digitalWrite(37, HIGH); 
          digitalWrite(38, HIGH);
          digitalWrite(39, HIGH);
          digitalWrite(40, HIGH); 
          digitalWrite(41, HIGH); 
          digitalWrite(42, HIGH); 
          digitalWrite(43, HIGH);
          digitalWrite(44, HIGH); 
          digitalWrite(45, HIGH);          

          ///// LED RING  /////
          // tell FastLED about the LED strip configuration
          //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
          FastLED.addLeds<LED_TYPE,DATA_PIN_1,COLOR_ORDER>(leds_1, NUM_LEDS_1).setCorrection(TypicalLEDStrip);
          FastLED.addLeds<LED_TYPE,DATA_PIN_24,COLOR_ORDER>(leds_24, NUM_LEDS_24).setCorrection(TypicalLEDStrip);
          FastLED.addLeds<LED_TYPE,DATA_PIN_12,COLOR_ORDER>(leds_12, NUM_LEDS_12).setCorrection(TypicalLEDStrip);
          FastLED.addLeds<LED_TYPE,DATA_PIN_8,COLOR_ORDER>(leds_8, NUM_LEDS_8).setCorrection(TypicalLEDStrip);
          
            // set master brightness control
          FastLED.setBrightness(BRIGHTNESS);
        
          ///// RECEIVER  /////
          Serial.begin(115200);
          delay(100);
          myRadio.begin();
          myRadio.setChannel(115);
          myRadio.setPALevel(RF24_PA_MIN);                //power amplifier level
          myRadio.setDataRate(RF24_250KBPS); 
          myRadio.openReadingPipe(0, address);
          myRadio.startListening();

}

          ///// uC TIMER INTERRUPT  /////
          ISR(TIMER1_COMPA_vect){     //timer1 interrupt 1Hz toggles pump pins
            
          //generates pulse wave of frequency 1Hz/2 = 0.5Hz (takes two cycles for full wave- toggle high then toggle low)
            if (((wodka_time) > 0) && glass == 1){ digitalWrite(wodka,LOW); wodka_time--; } else{ digitalWrite(wodka,HIGH);}
            if (((witte_rum_time) > 0) && glass == 1){ digitalWrite(witte_rum,LOW); witte_rum_time--; } else{ digitalWrite(witte_rum,HIGH);}
            if (((gin_time) > 0) && glass == 1){ digitalWrite(gin,LOW); gin_time--; } else{ digitalWrite(gin,HIGH);}
            if (((limoensap_time) > 0) && glass == 1){ digitalWrite(limoensap,LOW); limoensap_time--; } else{ digitalWrite(limoensap,HIGH);}
            if (((sinaasappelsap_time) > 0) && glass == 1){ digitalWrite(sinaasappelsap,LOW); sinaasappelsap_time--; } else{ digitalWrite(sinaasappelsap,HIGH);}
            if (((cola_time) > 0) && glass == 1){ digitalWrite(cola,LOW); cola_time--; } else{ digitalWrite(cola,HIGH);}
            if (((cranberry_time) > 0) && glass == 1){ digitalWrite(cranberry,LOW); cranberry_time--; } else{ digitalWrite(cranberry,HIGH);}
            if (((tonic_time) > 0) && glass == 1){ digitalWrite(tonic,LOW); tonic_time--; } else{ digitalWrite(tonic,HIGH);}
            if (((canada_time) > 0) && glass == 1){ digitalWrite(canada,LOW); canada_time--; } else{ digitalWrite(canada,HIGH);}
            if (((cola_zero_time) > 0) && glass == 1){ digitalWrite(cola_zero,LOW); cola_zero_time--; } else{ digitalWrite(cola_zero,HIGH);}
            
          }

          ///// LED RING  /////
            // List of patterns to cycle through.  Each is defined as a separate function below.
          typedef void (*SimplePatternList[])();
          SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, juggle};
          
          uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
          uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
          /////  RECEIVER  /////
          if (myRadio.available()) 
          {
            while (myRadio.available())
            {
              myRadio.read( &data, sizeof(data) );
            }
            Serial.print("\nDrink number: ");
            Serial.println(data.drink);
          }
          else {
            Serial.println("No signal");
          }

          ///// DISTANCE SENSOR  /////
          digitalWrite(trigPin, LOW);
          delayMicroseconds(2);
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
          
          duration = pulseIn(echoPin, HIGH);
          
          distanceInch = duration * 0.0133 /2;
          Serial.println(distanceInch);

          if(distanceInch < 2.5){
          Serial.println("Glass present");  
          glass = 1;  
            }
            else{
          Serial.println("Glass absent");
          glass = 0;
              }

if(data.drink > 0 && glass == 1){           // the order will not be excuted if there is no glass but will also not be reset (but can be overwritten by a new order)
            Serial.println("There's an order and the glass is present: "); 
            Serial.println(data.drink);
            
            while(data.drink == 1){                 //////// MOJITO = 1 ////////
                                           
            // pump the drink
            witte_rum_time              = 0;
            tonic_time                  = 0;
            limoensap_time              = 0;

            // Clear existing leds
            FastLED.clear();
        
            // White light under three middle rings
            leds_1[0] = CRGB::White;
            FastLED.show();
            
            for( int i = 0; i < NUM_LEDS_8; i++) {
            leds_8[i] = CRGB::White;
            FastLED.show();             
                        }

            for( int i = 0; i < NUM_LEDS_12; i++) {
            leds_12[i] = CRGB::White;              
            FastLED.show();
                        }
            
            // Show progress in outer ring
            for( int i = 0; i < NUM_LEDS_24; i++) {
            leds_24[i] = CRGB::White;              
            FastLED.show();
            delay(3000/24);                           /// Total pumping time diveded by 24 leds 
                                                      /// --> Longer pumping = Longer delay = Slower completion of the circle
                        }

            // Clear after completion
            
            
            // return to standby position
            data.drink = 0;
}

            while(data.drink == 2){                 //////// CUBA LIBRE = 2 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            cola_time                   = 0;
            limoensap_time              = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 3){                  //////// DAIQUIRI = 3 ////////
                              
            // pump the drink
            witte_rum_time              = 0;
            limoensap_time              = 0;            

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 4){                 //////// TOM COLLINS (G&T) = 4 ////////
                                              
            // pump the drink
            gin_time                    = 0;
            tonic_time                  = 0;
            limoensap_time              = 0;
            
            // return to standby position
            data.drink = 0;
}
            while(data.drink == 5){                 //////// CAPE COD = 5 ////////
                                              
            // pump the drink
            wodka_time                  = 8;
            cranberry_time              = 11;
            limoensap_time              = 6;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 6){                 //////// RICHMOND GIMLET = 6 ////////
                                              
            // pump the drink
            gin_time                    = 0;
            limoensap_time              = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 7){                 //////// CAIPIRANHA = 7 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            limoensap_time              = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 8){                 //////// L. ISLAND ICED TEA = 8 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            limoensap_time              = 0;
            wodka_time                  = 0;
            gin_time                    = 0;
            cola_time                   = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 9){                 //////// LAZY RED CHEEKS = 9 ////////
                                              
            // pump the drink
            wodka_time                  = 0;
            limoensap_time              = 0;
            tonic_time                  = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 10){                //////// MADRAS = 10 ////////
                                              
            // pump the drink
            wodka_time                      = 0;
            sinaasappelsap_time             = 0;
            limoensap_time                  = 0;
            cranberry_time                  = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 11){                //////// WODKA+SINAASAPPELSAP = 11 ////////
                                              
            // pump the drink
            wodka_time                  = 0;
            sinaasappelsap_time         = 0;
            
            // return to standby position
            data.drink = 0;
}
            while(data.drink == 12){                //////// WODKA+TONIC = 12 ////////
                                              
            // pump the drink
            wodka_time         = 0;
            tonic_time         = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 13){                //////// WODKA+COCA-COLA = 13 ////////
                                              
            // pump the drink
            wodka_time         = 0;
            cola_time          = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 14){                //////// WODKA+COCA-COLA_ZERO = 14 ////////
                                              
            // pump the drink
            wodka_time             = 0;
            cola_zero_time         = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 15){                //////// WODKA+CANADA DRY = 15 ////////
                                              
            // pump the drink
            wodka_time         = 0;
            canada_time         = 0;
            
            // return to standby position
            data.drink = 0;
}
            while(data.drink == 16){                //////// WITTE RUM+SINAASAPPELSAP = 16 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            sinaasappelsap_time         = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 17){                //////// WITTE RUM+TONIC = 17 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            tonic_time                  = 0;       

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 18){                //////// WITTE RUM+COCA-COLA = 18 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            cola_time                   = 0;
            
            // return to standby position
            data.drink = 0;
}
            while(data.drink == 19){                //////// WITTE RUM+COCA-COLA_ZERO = 19 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            cola_zero_time              = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 20){                //////// WITTE RUM+CANADA DRY = 20 ////////
                                              
            // pump the drink
            witte_rum_time              = 0;
            canada_time                 = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 21){                //////// GIN+SINAASAPPELSAP = 21 ////////
                                              
            // pump the drink
            gin_time                    = 0;
            sinaasappelsap_time         = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 22){                //////// GIN+TONIC = 22 ////////
                                              
            // pump the drink
            gin_time                    = 0;
            tonic_time                  = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 23){                //////// GIN+COCA-COLA = 23 ////////
                                              
            // pump the drink
            gin_time                    = 0;
            cola_time                   = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 24){                //////// GIN+COCA-COLA_ZERO = 24 ////////
                                              
            // pump the drink
            gin_time                    = 0;
            cola_zero_time              = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 25){                //////// GIN+CANADA DRY = 25 ////////
                                              
            // pump the drink
            gin_time                    = 0;
            canada_time                 = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 26){                //////// WODKA_SINGLE = 26 ////////
                                              
            // pump the drink
            wodka_time                    = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 27){                //////// WODKA_DOUBLE = 27 ////////
                                              
            // pump the drink
            wodka_time                    = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 28){                //////// WITTE RUM_SINGLE = 28 ////////
                                              
            // pump the drink
            witte_rum_time                = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 29){                //////// WITTE RUM_DOUBLE = 29 ////////
                                              
            // pump the drink
            wodka_time                    = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 30){                //////// GIN_SINGLE = 30 ////////
                                              
            // pump the drink
            gin_time                      = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 31){                //////// GIN_DOUBLE = 31 ////////
                                              
            // pump the drink
            gin_time                      = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 32){                //////// MOSCOW MULE = 32 ////////
                                              
            // pump the drink
            wodka_time                    = 0;
            canada_time                   = 0;
            limoensap_time                = 0;
            
            // return to standby position
            data.drink = 0;
}
            while(data.drink == 33){                //////// GIN-GIN MULE = 33 ////////
                                              
            // pump the drink
            gin_time                      = 0;
            canada_time                   = 0;
            limoensap_time                = 0;

            // return to standby position
            data.drink = 0;
}
            while(data.drink == 34){                //////// DARK & STORMY = 34 ////////
                                              
            // pump the drink
            witte_rum_time                = 0;
            canada_time                   = 0;
            limoensap_time                = 0;

            // return to standby position
            data.drink = 0;
} 
            while(data.drink == 35){                //////// GINGER LIME RICKEY = 35 ////////
                                              
            // pump the drink
            wodka_time                    = 0;
            canada_time                   = 0;
            cranberry_time                = 0;
            limoensap_time                = 0;

            // return to standby position
            data.drink = 0;
}

            while(data.drink == 100){                //////// OPSTARTSEQUENTIE = 100 ////////
            
            // fill the pumps
            wodka_time                    = 3;
            witte_rum_time                = 3;
            gin_time                      = 3;
            limoensap_time                = 3;

            sinaasappelsap_time           = 0;
            cola_time                     = 0;
            cranberry_time                = 0;
            tonic_time                    = 0;
            canada_time                   = 0;
            cola_zero_time                = 0;
            
            // return to standby position
            data.drink = 0;
}

            while(data.drink == 101){                //////// SCHOONMAAKSEQUENTIE = 101 ////////
                                              
            // clean the pumps
            wodka_time                    = 7;
            witte_rum_time                = 7;
            gin_time                      = 7;
            limoensap_time                = 7;

            sinaasappelsap_time           = 0;
            cola_time                     = 0;
            cranberry_time                = 0;
            tonic_time                    = 0;
            canada_time                   = 0;
            cola_zero_time                = 0;
            
            // return to standby position
            data.drink = 0;
}
            
           
}
else{
  ///// LED RINGS  /////
            // Call the current pattern function once, updating the 'leds' array
            gPatterns[gCurrentPatternNumber]();
          
            // send the 'leds' array out to the actual LED strip
            FastLED.show();
            // insert a delay to keep the framerate modest
            FastLED.delay(1000/FRAMES_PER_SECOND);
          
            // do some periodic updates
            EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
            EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
          
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


                      ///// PATTERNS LED RINGS  /////          
                      void nextPattern()
                      {
                        // add one to the current pattern number, and wrap around at the end
                        gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
                      }
                      
                      void rainbow() 
                      {
                        // FastLED's built-in rainbow generator
                        fill_rainbow(leds_1, NUM_LEDS_1, gHue, 7);
                        fill_rainbow(leds_24, NUM_LEDS_24, gHue, 7);
                        fill_rainbow(leds_12, NUM_LEDS_12, gHue, 7);
                        fill_rainbow(leds_8, NUM_LEDS_8, gHue, 7);
                      }
                      
                      void rainbowWithGlitter() 
                      {
                        // built-in FastLED rainbow, plus some random sparkly glitter
                        rainbow();
                        addGlitter(80);
                      }
                      
                      void addGlitter(fract8 chanceOfGlitter) 
                      {
                        if( random8() < chanceOfGlitter) {
                          leds_1[ random16(NUM_LEDS_1) ] += CRGB::White;
                          leds_24[ random16(NUM_LEDS_24) ] += CRGB::White;
                          leds_12[ random16(NUM_LEDS_12) ] += CRGB::White;
                          leds_8[ random16(NUM_LEDS_8) ] += CRGB::White;
                        }
                      }
                      
                      void confetti() 
                      {
                        // random colored speckles that blink in and fade smoothly
                        fadeToBlackBy( leds_1, NUM_LEDS_1, 10);
                        fadeToBlackBy( leds_24, NUM_LEDS_24, 10);
                        fadeToBlackBy( leds_12, NUM_LEDS_12, 10);
                        fadeToBlackBy( leds_8, NUM_LEDS_8, 10);
                        int pos_1 = random16(NUM_LEDS_1);
                        int pos_24 = random16(NUM_LEDS_24);
                        int pos_12 = random16(NUM_LEDS_12);
                        int pos_8 = random16(NUM_LEDS_8);
                        leds_1[pos_1] += CHSV( gHue + random8(64), 200, 255);
                        leds_24[pos_24] += CHSV( gHue + random8(64), 200, 255);
                        leds_12[pos_12] += CHSV( gHue + random8(64), 200, 255);
                        leds_8[pos_8] += CHSV( gHue + random8(64), 200, 255);
                      }
                      
                      void juggle() {
                        // eight colored dots, weaving in and out of sync with each other
                        fadeToBlackBy( leds_1, NUM_LEDS_1, 20);
                        fadeToBlackBy( leds_24, NUM_LEDS_24, 20);
                        fadeToBlackBy( leds_12, NUM_LEDS_12, 20);
                        fadeToBlackBy( leds_8, NUM_LEDS_8, 20);
                        byte dothue = 0;
                        for( int i = 0; i < 8; i++) {
                          leds_1[beatsin16( i+7, 0, NUM_LEDS_1-1 )] |= CHSV(dothue, 200, 255);
                          dothue += 32;
                        }
                        for( int i = 0; i < 8; i++) {
                          leds_24[beatsin16( i+7, 0, NUM_LEDS_24-1 )] |= CHSV(dothue, 200, 255);
                          dothue += 32;
                        }
                        for( int i = 0; i < 8; i++) {
                          leds_12[beatsin16( i+7, 0, NUM_LEDS_12-1 )] |= CHSV(dothue, 200, 255);
                          dothue += 32;
                        }
                        for( int i = 0; i < 8; i++) {
                          leds_8[beatsin16( i+7, 0, NUM_LEDS_8-1 )] |= CHSV(dothue, 200, 255);
                          dothue += 32;
                        }
                      }
