

//https://www.youtube.com/watch?v=J9cDEef0IbQ is the example
//this code was created to measure the time between interrupts from a shaft encoder.
//main intent was to see if an arduino uno or due can keep up with the count.
//encoder used has 5000 PPR, used electric drill to spin shaft.
//I was measuring 9microseconds between interrupts.
//I had to play games with serial.print, as the time taken to push this data back to the serial monitor was loosing interrupts
//at this point I think I will need to learn how to write the ISR in assembly.

// Used for generating interrupts using encoder A signal
const int PinA = 3; 

// Used for reading encoder B signal
const int PinB = 4;  

// Keep track of rotary encoder value
volatile int long lastCount;
volatile int long modulolastCount;
volatile int count;
volatile int int_count;

// Updated by the ISR (Interrupt Service Routine)
volatile int long virtualPosition;
volatile unsigned long lastInterruptTime;
volatile unsigned long InterruptTime_avg;
volatile unsigned long InterruptTime_dif;
volatile unsigned long interruptTime;

// ------------------------------------------------------------------
// INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT 
// ------------------------------------------------------------------
void isr ()  
{
  interruptTime = micros(); //time since powered up or reset
  if (digitalRead(PinB) == LOW) //
  {
    virtualPosition-- ; // anti clockwise
  }
  else 
  { 
     virtualPosition++ ; // clockwise 
  }
}

// ------------------------------------------------------------------
// SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    
// ------------------------------------------------------------------
void setup() 
{
  // Just whilst we debug, view output on serial monitor
  Serial.begin(115200); //found with China copy of DUE, could not run this faster

  // Rotary pulses are INPUTs
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);

  // Switch is floating so use the in-built PULLUP so we don't need a resistor
  //pinMode(PinSW, INPUT_PULLUP);

  // Ready to go!
  Serial.println("Start");
  
  // Attach the routine to service the interrupts using encoder A, on logic low
  attachInterrupt(digitalPinToInterrupt(PinA), isr, FALLING); //options LOW CHANGE RISING FALLING 
}

// ------------------------------------------------------------------
// MAIN LOOP     MAIN LOOP     MAIN LOOP     MAIN LOOP     MAIN LOOP
// ------------------------------------------------------------------
void loop() 
{
  // If the current rotary switch position has changed then update everything
  if (virtualPosition != lastCount)  //is the counter moving?
  {
    // Keep track of this new value
    lastCount = virtualPosition;
    InterruptTime_dif = interruptTime - lastInterruptTime; // compute the interrupt difference from the new time snap shot takein in the interrupt routine
    InterruptTime_avg = InterruptTime_dif + InterruptTime_avg;  //add up 100 interruptTime_dif so an average of 100 can be calculated.
    lastInterruptTime = interruptTime;  //save the current interrupt time so the difference of the next interrupt can be done
    int_count ++; // increment the average counter
    if (int_count == 1000) // if we have 100 summed samples, then compute the average and print
    {
      InterruptTime_avg = InterruptTime_avg / 1000; //average computation of 100 summed samples
      Serial.print("I"); // put out an I for interrupt
      Serial.println(InterruptTime_avg); //and print the average last 100 interrupts
      int_count = 0;  //zero the counter 
      InterruptTime_avg = 0; //zero the average
    }
    count ++; //as the RS232 data was excessive, this only prints after 1000 increments of the encoder
    if (count == 50000) 
    {
      // Write out to serial monitor the value and direction
      //Serial.print(virtualPosition > lastCount ? " Up  :" : " Down:");
      Serial.print("\t");
      Serial.println(virtualPosition);
      count = 0;
    }
  }
  //else
 // {
 // Serial.println(virtualPosition);
 // }
}

