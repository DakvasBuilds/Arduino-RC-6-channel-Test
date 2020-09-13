// This program is used to test the radio-control transmitter/receiver
// communication to the Arduino by mapping different channels from the
// RC signal to different colors on different RGB LEDs. This program
// is meant to handle a 6-channel RC communication system. RC_test_10ch
// will be developed to handle a 10-channel system including switches
// on channels 7-10.

//Input channel pin assignment
const int ch[6] = {2, 4, 7, 8, 12, 13};

//LED pin assignment
const int LEDAred = 3;
const int LEDAgreen = 5;
const int LEDAblue = 6;

const int LEDBred = 9;
const int LEDBgreen = 10;
const int LEDBblue = 11;

//RX value range
const int RXlow = 1000;
const int RXmid = 1500;
const int RXhigh = 2000;

//PWM value range
const int PWMlow = 0;
const int PWMhigh = 255;

//Initialize storage for channel signal value
int input[6] = {0, 0, 0, 0, 0, 0};
int PWMarr[6] = {0, 0, 0, 0, 0, 0};

//Indicator flag for RC transmitter active
boolean txOnline = false;

//Initial setup
void setup() {
  //Initialze serial communication at 9600 bits per second  
  Serial.begin(9600);

  //Set RX input pins
  pinMode(ch[0], INPUT);
  pinMode(ch[1], INPUT);
  pinMode(ch[2], INPUT);
  pinMode(ch[3], INPUT);
  pinMode(ch[4], INPUT);
  pinMode(ch[5], INPUT);

  //Set LED output pins
  pinMode(LEDAred, OUTPUT);
  pinMode(LEDAgreen, OUTPUT);
  pinMode(LEDAblue, OUTPUT);

  pinMode(LEDBred, OUTPUT);
  pinMode(LEDBgreen, OUTPUT);
  pinMode(LEDBblue, OUTPUT);
}

//Main control loop
void loop() {
  //Run for each RX channel
  for(int i = 0; i<=5; i++)
  {
    //Read in value from RX
    readChannel(i);

    //If transmitter is on (timeout RX value when TX is off is 0)
    if(input[i] != 0)
    {
      //Set TX active flag
      txOnline = true;

      //Write RX value to Serial connection to PC
      serialSignalWrite(i);

      //Condition signal by restricting range
      conditionSignal(i);

      //Route PWM mapping based on type of control
      //  1D: Throttle control and knobs
      //  2D: Center-neutral controls
      switch(i)
      {
        case 2:
        case 4:
        case 5:
        {
          map1DSignalToPWM(i);
          break;
        }
        default:
        {
          map2DSignalToPWM(i);
          break;
        }
      }

      //Write mapped PWM value out to Serial connection
      serialPWMWrite(i);
    }
    else
    {
      //Set indicator flag if TX is not active
      txOnline = false;
    }
  }

  if(txOnline)
  {
    //If TX is active, write out mapped PWM values to LEDs
    writeLEDs();
  }
  else
  {
    //If TX is not active, reset LEDs to OFF state
    Serial.print("No transmitter signal...");
    resetLEDs();
  }

  //Write out end of line to Serial for to signify the end of
  //this loop cycle
  Serial.println();
}

//Read in pulse width values from RX
void readChannel(int in)
{
  input[in] = pulseIn(ch[in],HIGH);
}

//Write out signal data to Serial connection
void serialSignalWrite(int in)
{
  Serial.print("Ch");
  Serial.print(in+1);
  Serial.print(": ");
  Serial.print(input[in]);
}

//Write out PWM data to Serial connection
void serialPWMWrite(int in)
{
  Serial.print("(");
  Serial.print(PWMarr[in]);
  Serial.print(") | ");
}

//Restrict RX data to specific range, and for center to the 
//midpoint value for center-neutral controls
void conditionSignal(int in)
{
  input[in] = constrain(input[in], RXlow, RXhigh);
  if(abs(RXmid - input[in]) < 20)
  {
    input[in] = RXmid;  
  }
}

//Map signal data from 1D controls to PWM
void map1DSignalToPWM(int in)
{
  int temp = 0;
  temp = input[in];
  PWMarr[in] = map(temp, RXlow, RXhigh, PWMlow, PWMhigh);
}

//Map signal data from 2D controls to PWM
void map2DSignalToPWM(int in)
{
  int temp = 0;
  //Set middle value as PWM 0 value and increase as control
  //moves away from center
  temp = abs(RXmid - input[in]); 
  PWMarr[in] = map(temp, 0, (RXhigh - RXlow) / 2, PWMlow, PWMhigh);
}

//Write out PWM values to LEDs
void writeLEDs(){
  analogWrite(LEDAred, PWMarr[1]);
  analogWrite(LEDAgreen, PWMarr[0]);
  analogWrite(LEDAblue, PWMarr[5]);

  analogWrite(LEDBred, PWMarr[2]);
  analogWrite(LEDBgreen, PWMarr[3]);
  analogWrite(LEDBblue, PWMarr[4]); 
}

//Reset LED PWM values to 0
void resetLEDs()
{
  analogWrite(LEDAred, 0);
  analogWrite(LEDAgreen, 0);
  analogWrite(LEDAblue, 0);

  analogWrite(LEDBred, 0);
  analogWrite(LEDBgreen, 0);
  analogWrite(LEDBblue, 0);  
}
