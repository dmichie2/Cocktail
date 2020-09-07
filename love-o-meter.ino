// Volatile Variables, used in the interrupt service routine!
volatile int BPM1;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal1;                // holds the incoming raw data
volatile int IBI1 = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse1 = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS1 = false;        // becomes true when Arduoino finds a beat.

volatile int BPM2;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal2;                // holds the incoming raw data
volatile int IBI2 = 600;              // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse2 = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS2 = false;        // becomes true when Arduoino finds a beat.

volatile int rate1[10];                      // array to hold last ten IBI values
volatile unsigned long sampleCounter1 = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime1 = 0;           // used to find IBI
volatile int P1 = 512;                     // used to find peak in pulse wave, seeded
volatile int T1 = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh1 = 525;                // used to find instant moment of heart beat, seeded
volatile int amp1 = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat1 = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat1 = false;      // used to seed rate array so we startup with reasonable BPM

volatile int rate2[10];                      // array to hold last ten IBI values
volatile unsigned long sampleCounter2 = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime2 = 0;           // used to find IBI
volatile int P2 = 512;                     // used to find peak in pulse wave, seeded
volatile int T2 = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh2 = 525;                // used to find instant moment of heart beat, seeded
volatile int amp2 = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat2 = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat2 = false;      // used to seed rate array so we startup with reasonable BPM

int pulsePin1 = A0;                  // Pulse Sensor purple wire connected to analog pin A0
int pulsePin2 = A1;                  // Pulse Sensor purple wire connected to analog pin A1

// LED hearts
int redPin1 = 2;
int greenPin1;
int bluePin1 = 3;

int redPin2 = 4;
int greenPin2;
int bluePin2 = 5;

//uncomment this line if using a Common Anode LED
#define COMMON_ANODE

void setup()
{
  Serial.begin(115200);             // we agree to talk fast!

  pinMode(redPin1, OUTPUT);
  pinMode(greenPin1, OUTPUT);
  pinMode(bluePin1, OUTPUT);

  pinMode(redPin2, OUTPUT);
  pinMode(greenPin2, OUTPUT);
  pinMode(bluePin2, OUTPUT);

  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
                                    // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE, 
                                    // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
                                    // analogReference(EXTERNAL);
}

void loop()
{

  sendDataToSerial('S1', Signal1, 'S2', Signal2);

  if (QS1 == true) // a heartbeat was found
    {     
      // BPM and IBI have been Determined
      // Quantified Self "QS" true when arduino finds a heartbeat
      serialOutputWhenBeatHappens(); // A beat happened, output that to serial.     
      QS1 = false; // reset the Quantified Self flag for next time
    }

    if (QS2 == true) // a heartbeat was found
    {     

      serialOutputWhenBeatHappens(); // A beat happened, output that to serial.     
      QS2 = false; // reset the Quantified Self flag for next time
    }

  hearts(); //stand-by, calculating, and showing the results
  
     }

void hearts(){
  
  if(BPM1 > 0 && BPM2 > 0){
    for(int i=0; i<=33; i++){     // 33 loops of 150ms each = 5s total                         
            // both heart rate sensors sense a heartbeat, now the system is 'calculating' the love --> not really, just creating suspense
            setColor(255, 0, 0);  // red
            delay(75);
            setColor(0, 0, 255);  // blue
            delay(75);
      }

      // LOVE CALCULATION
      if((BPM1/BPM2) < 0,8 | (BPM1/BPM2) > 1,2){
         setColor(0, 0, 255);  // blue
         delay(5000);
        }
      else{
        setColor(255, 0, 0);  // red
        delay(5000);
        }
      
  }
  else{
            // stand-by
            setColor(255, 0, 0);  // red
            delay(500);
            setColor(0, 0, 255);  // blue
            delay(500);
  }
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  
  analogWrite(redPin1, red);
  analogWrite(greenPin1, green);
  analogWrite(bluePin1, blue);
  
  analogWrite(redPin2, red);
  analogWrite(greenPin2, green);
  analogWrite(bluePin2, blue);
}

void interruptSetup()
{
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
}



void serialOutputWhenBeatHappens()
{    
     Serial.print("BPM1: ");
     Serial.println(BPM1);
     delay(300);

     Serial.print("BPM2: ");
     Serial.println(BPM2);
     delay(300);
}

void sendDataToSerial(char symbol1, int data1, char symbol2, int data2)
{
   Serial.print(symbol1);
   Serial.println(data1);
   Serial.print(symbol2);
   Serial.println(data2);
}

ISR(TIMER2_COMPA_vect) //triggered when Timer2 counts to 124
{
  cli();                                      // disable interrupts while we do this
  Signal1 = analogRead(pulsePin1);            // read the Pulse Sensor 
  Signal2 = analogRead(pulsePin2);            // read the Pulse Sensor 
  sampleCounter1 += 2;                        // keep track of the time in mS with this variable
  sampleCounter2 += 2;
  int N1 = sampleCounter1 - lastBeatTime1;     // monitor the time since the last beat to avoid noise
  int N2 = sampleCounter2 - lastBeatTime2;     //  find the peak and trough of the pulse wave
  
  if(Signal1 < thresh1 && N1 > (IBI1/5)*3) // avoid dichrotic noise by waiting 3/5 of last IBI
    {      
      if (Signal1 < T1) // T is the trough
      {                        
        T1 = Signal1; // keep track of lowest point in pulse wave 
      }
    }
    
if(Signal2 < thresh2 && N2 > (IBI2/5)*3) // avoid dichrotic noise by waiting 3/5 of last IBI
    {      
      if (Signal2 < T2) // T is the trough
      {                        
        T2 = Signal2; // keep track of lowest point in pulse wave 
      }
    }

  if(Signal1 > thresh1 && Signal1 > P1)
    {                                           // thresh condition helps avoid noise
      P1 = Signal1;                             // P is the peak
    }                                           // keep track of highest point in pulse wave

  if(Signal2 > thresh2 && Signal2 > P2)
    {          
      P2 = Signal2;                           
    }
    
  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N1 > 250)
  {                                   // avoid high frequency noise
    if ( (Signal1 > thresh1) && (Pulse1 == false) && (N1 > (IBI1/5)*3) )
      {        
        Pulse1 = true;                                // set the Pulse flag when we think there is a pulse
        IBI1 = sampleCounter1 - lastBeatTime1;        // measure time between beats in mS
        lastBeatTime1 = sampleCounter1;               // keep track of time for next pulse
  
        if(secondBeat1)
        {                                             // if this is the second beat, if secondBeat == TRUE
          secondBeat1 = false;                        // clear secondBeat flag
          for(int i=0; i<=9; i++)                     // seed the running total to get a realisitic BPM at startup
          {             
            rate1[i] = IBI1;                      
          }
        }
  
        if(firstBeat1) // if it's the first time we found a beat, if firstBeat == TRUE
        {                         
          firstBeat1 = false;                   // clear firstBeat flag
          secondBeat1 = true;                   // set the second beat flag
          sei();                                // enable interrupts again
          return;                               // IBI value is unreliable so discard it
        }   
      // keep a running total of the last 10 IBI values
      word runningTotal1 = 0;                  // clear the runningTotal variable    

      for(int i=0; i<=8; i++)
        {                // shift data in the rate array
          rate1[i] = rate1[i+1];                  // and drop the oldest IBI value 
          runningTotal1 += rate1[i];              // add up the 9 oldest IBI values
        }

      rate1[9] = IBI1;                            // add the latest IBI to the rate array
      runningTotal1 += rate1[9];                  // add the latest IBI to runningTotal
      runningTotal1 /= 10;                        // average the last 10 IBI values 
      BPM1 = 60000/runningTotal1;                 // how many beats can fit into a minute? that's BPM!
      QS1 = true;                                 // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }
  }

  if (N2 > 250)
  {                                   // avoid high frequency noise
    if ( (Signal2 > thresh2) && (Pulse2 == false) && (N2 > (IBI2/5)*3) )
      {        
        Pulse2 = true;                                // set the Pulse flag when we think there is a pulse
        IBI2 = sampleCounter2 - lastBeatTime2;        // measure time between beats in mS
        lastBeatTime2 = sampleCounter2;               // keep track of time for next pulse
  
        if(secondBeat2)
        {                                             // if this is the second beat, if secondBeat == TRUE
          secondBeat2 = false;                        // clear secondBeat flag
          for(int i=0; i<=9; i++)                     // seed the running total to get a realisitic BPM at startup
          {             
            rate2[i] = IBI2;                      
          }
        }
  
        if(firstBeat2) // if it's the first time we found a beat, if firstBeat == TRUE
        {                         
          firstBeat2 = false;                   // clear firstBeat flag
          secondBeat2 = true;                   // set the second beat flag
          sei();                                // enable interrupts again
          return;                               // IBI value is unreliable so discard it
        }   
      // keep a running total of the last 10 IBI values
      word runningTotal2 = 0;                  // clear the runningTotal variable    

      for(int i=0; i<=8; i++)
        {                // shift data in the rate array
          rate2[i] = rate2[i+1];                  // and drop the oldest IBI value 
          runningTotal2 += rate2[i];              // add up the 9 oldest IBI values
        }

      rate2[9] = IBI2;                            // add the latest IBI to the rate array
      runningTotal2 += rate2[9];                  // add the latest IBI to runningTotal
      runningTotal2 /= 10;                        // average the last 10 IBI values 
      BPM2 = 60000/runningTotal2;                 // how many beats can fit into a minute? that's BPM!
      QS2 = true;                                 // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }
  }

  if (Signal1 < thresh1 && Pulse1 == true)
    {   // when the values are going down, the beat is over

      Pulse1 = false;                           // reset the Pulse flag so we can do it again
      amp1 = P1 - T1;                           // get amplitude of the pulse wave
      thresh1 = amp1/2 + T1;                    // set thresh at 50% of the amplitude
      P1 = thresh1;                             // reset these for next time
      T1 = thresh1;
    }

  if (Signal2 < thresh2 && Pulse2 == true)
    {   // when the values are going down, the beat is over
      Pulse2 = false;                           // reset the Pulse flag so we can do it again
      amp2 = P2 - T2;                           // get amplitude of the pulse wave
      thresh2 = amp2/2 + T2;                    // set thresh at 50% of the amplitude
      P2 = thresh2;                             // reset these for next time
      T2 = thresh2;
    }

  if (N1 > 2500)
    {                           // if 2.5 seconds go by without a beat
      thresh1 = 512;                            // set thresh default
      P1 = 512;                                 // set P default
      T1 = 512;                                 // set T default
      lastBeatTime1 = sampleCounter1;           // bring the lastBeatTime up to date        
      firstBeat1 = true;                        // set these to avoid noise
      secondBeat1 = false;                      // when we get the heartbeat back
    }

  if (N2 > 2500)
    {                           // if 2.5 seconds go by without a beat
      thresh2 = 512;                            // set thresh default
      P2 = 512;                                 // set P default
      T2 = 512;                                 // set T default
      lastBeatTime2 = sampleCounter2;           // bring the lastBeatTime up to date        
      firstBeat2 = true;                        // set these to avoid noise
      secondBeat2 = false;                      // when we get the heartbeat back
    }

  sei();                                        // enable interrupts when youre done!
} // end isr
