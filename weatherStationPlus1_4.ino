#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
//
// Last modified version 1.4  by G.Hagy 7/23/2014.
//
// I used this with an XBee series 1 to transmit the count to  
// another device such as a PC with another XBee on a USB port.
// With the XBee sheild a PIR output is connected to input pin 3.
// 5v & Gnd pins are available on the sheild for the PIR as well.
// An LED is connected to pin 12 for viual feedback.
// The Arduino counts motion events from the PIR while waiting for
// commands to be received from the XBEE radio.
// At startup a "G" is transmitted to indicate that the
// device is available (Good to Go) to receive commands.
//
// The character "c" is the command to view the current count total
// response is to transmit the total number of motion events 
// detected by the PIR up to that point in time. 
// 
// "a" is the command to sound alarm, beep 3 times.
// "b" is the command to enable/disable the beeper, toggles on or off.
// "c" "C" will return the motion senor count total.
// "d" return details of the BMP180 sensor.
// "f" faster, decrements the multiplier variable which will speed up 
//     information messages trasmitted in "chatty" mode.
// "h" "H" display current humidity.
// "l" toggle the led light on/off.
// "m" return the current value of the multiplier variable.
// "p" "P" display current barometric pressure.
// "Q" Query command, returns "G" for Good to GO.
// "r" restart with the count zero.
// "s" slower, increments the multiplier variable which will slow down 
//     information messages trasmitted in "chatty" mode.
// "S" much slower, doubles the current value of the multiplier 
//     variable which will slow down information messages trasmitted 
//     in "chatty" mode.
// "t" "T" display current temperature (F).
// "v" return the version number as a string.
// "x" toggle the information set transmit on/off.
// "z" reset the motion sensor count to zero.
// 
// Any other characters received will be ignored and the message 
// returned will be "?" to indicate the command was received but not
// understood.
//
//

/*-----( Declare objects )-----*/
// pressure sensor
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/*-----( Declare Constants, Pin Numbers )-----*/

#define DHTTYPE DHT22   // DHT 22 (AM2302) temperature and humidity sensor.
#define DHTPIN 4

char Strv[] = "1.4";    // Version of this code.
int pirPin = 3;         // output from the PIR input to Arduino.
int ledPin = 12;        // pin for the led.
//int ledPin = 12;      // pin for second led or light.
int beepPin = 13;       // pin for the beeper.
int count = 0;          // running total number of motions detected by the PIR.
int state = 0;          // state of PIR.
int beep = 0;           // beep enable state, 0 is off, 1 is on.
char cmdIn = 'U';       // command input, default to Unknown. 
int delayTimeL= 500;    // half second delay time, Long delay.
int delayTimeS = 250;   // quarter second delay time, Short delay.
int multiplier = 2;     // delay time multiplier.
int chk = 0;            // value from checking dht22. 
int i = 0;              // index count for loops in setup and alarm. 
int x = 0;              // enable transmit beacon "chatty" mode.
int w = 1;              // number of times to wait for one minute.
int passNum = 0;        // keep track of how many messages are transmitted.
int light = 0;          // light, 0 is off, 1 is on

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);      // declare led as output
  pinMode(beepPin, OUTPUT);     // declare beep as output
  dht.begin();
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    // Serial.print("Ooops, no BMP085 detected ... Check your wiring .....");
    // while(1);
    for(i=0;i<5;i++) {
          delay(delayTimeL);
    }
    // Serial.print("Enter r to restart.");    
  }
}

void loop(void)
{
    if(x == 1) {
       Serial.println("G");          // send a char to show running status is "G"ood
    }
    digitalWrite(ledPin, light);
    // look for any input commands
    while(Serial.available() < 1) {
        state = 0;
	digitalWrite(ledPin, state);
	// transmit something 
        // if enabled
        if(x == 1) {
           passNum += 1;
           Serial.print(passNum);
           Serial.print(", ");
           Serial.print(count);
           Serial.print(", ");
           Serial.print(multiplier);
           Serial.print(", ");
           float Far = dht.readTemperature(true);
           Serial.print(Far);
           Serial.print(", ");
           float Hum = dht.readHumidity();
           Serial.print(Hum);           
           Serial.print(", ");
           bmpSensorPressure();          
           for(w=1;w<multiplier;w++){   // allow a longer wait between messages
               minuteTO();              // wait a minute between sending 
           }
        }	
        // read from PIR
	if (digitalRead(pirPin)) {
            addToCount();            
        }
    }    	  
	
    // get input  
    cmdIn = Serial.read();
    // see what the command is
    switch(cmdIn) {
           
      case 'a':
            // Serial.print("Alarm: "); // sound an alarm
            for(i=3; i > 0; i--) {
               digitalWrite(beepPin, 1); // beeper on
               digitalWrite(ledPin, 1);
               delay(delayTimeS);
               digitalWrite(beepPin, 0); // beeper off
               digitalWrite(ledPin, 0);
               delay(delayTimeL);               
            }
            break;  
            
      case 'b':                // toggle beeper on or off
            if(beep) {
              beep = 0;
            } else {
              beep = 1;
            }
            break;
      
      case 'C':
            Serial.println(count);
            break;
            
      case 'c':
            Serial.println(count); 
            break;
            
      case 'f':                // faster beacon transmit
            if (multiplier > 1) {       // reduce the delay multiplier
              multiplier--;
            } 
            break;
            
      case 'H':{
            float Hum = dht.readHumidity();
            Serial.println(Hum);
            }
            break;
            
      case 'h':{
            float Hum = dht.readHumidity();
            Serial.println(Hum);
            }
            break;
                             
      case 'l':                // toggle light on or off
            if(light) {
              light = 0;
            } else {
              light = 1;
              digitalWrite(ledPin, 1);
              delay(delayTimeL);
              digitalWrite(ledPin, 0);
            }
 
            digitalWrite(ledPin, light);
            break;
            
      case 'm':                // return the multiplier
            Serial.println(multiplier);
            break;
            
      case 'P':     
            bmpSensorPressure();
            break;
                    
      case 'p':     
            bmpSensorPressure();
            break;
                       
      case 'Q':
            Serial.println("G"); // I'm good
            break;
            
      case 'r':        
            { 
               for(i=0;i<10;i++) {
                  delay(delayTimeL);
               }
               count = 0;      // reset count to zero
               x = 1;          // enable becon transmit
               multiplier = 2;          // default transmit delay
               setup();
            }  
            break;
                                    
      case 's':                // slow beacon transmit
            multiplier++;      // by increasing the delay multiplier 
            break;
                                                
      case 'S':       // slow beacon transmit
            if(multiplier < 60) {
               multiplier = multiplier*2;    // by increasing the delay multiplier 
            }
            break;            
           
      case 'T':                // send temperature
            {
            float Far = dht.readTemperature(true);
            Serial.println(Far);
            }
            break;                
           
      case 't':                // send temperature
            {
            float Far = dht.readTemperature(true);
            Serial.println(Far);
            }
            break; 
            
      case 'v':                // return the version number
            Serial.println(Strv);
            break;
            
      case 'x':                // toggle beacon transmit enable on or off
            if(x) {
              x = 0;
              Serial.println("G");          // send a char to show running status is "G"ood
            } else {
              passNum = 0;
              x = 1;
            }
            break;
      
      case 'z':
            count = 0;               // reset count to zero 
            break;
            
      default:
            Serial.println("?"); // unknown
            break;
    }
}
	
/*-----( Declare User Functions )-----*/ 
void addToCount() {
	count += 1;   // saw something add it to the total
        // the PIR will wait a bit before it looks for a new motion,
        //  wait for it to go back to normal.            
        while (digitalRead(pirPin) == 1)  {
              state = 1;
              digitalWrite(ledPin, state);
              if(beep) {
                 digitalWrite(beepPin, state);
              }
              delay(delayTimeS);      
        }
        state = 0;
        digitalWrite(beepPin, state);
}

void minuteTO() {
        for(i=0;i<24;i++) {
          delay(2000);
          // read from PIR every two seconds even while in this time out period.
	  if (digitalRead(pirPin)) {
             addToCount();            
          }
          digitalWrite(ledPin, 1);
          delay(delayTimeL);
          digitalWrite(ledPin, 0);
        }       
}
   
//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
        return 1.8 * celsius + 32;
}

//hPa to in conversion
double hpaToIn(double pressure)
{
        return pressure * 0.031018;
}

/* Display some basic information on this sensor */
void displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
//  Serial.println("------------------------------------");
//  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
//  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
//  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
//  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
//  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
//  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
//  Serial.println("------------------------------------");
//  Serial.println("");
  delay(500);
}

void bmpSensorPressure(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);
 
  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure) 
  {
      /* Display atmospheric pressue in hPa */
      // Serial.print("Pressure: ");
      Serial.println(hpaToIn(event.pressure), 2); 
      // Serial.println(" in");
  }
  else
  {
      //Serial.println("Sensor error");
  }
  delay(1000);
}

void bmpSensorTemperature(void) 
{
   /* Get a new sensor event */ 
   sensors_event_t event;
   bmp.getEvent(&event);
   if (event.temperature)
   {
       float temperature;
       bmp.getTemperature(&temperature);
       // Temperature from the DHT22 seems closer to actual room temperature
       // uncomment the following 3 lines to use the BMP sensor.
       // Serial.print("Temperature: ");
       // Serial.print(Fahrenheit(temperature), 2); 
       // Serial.println(" F");
    }
      else
    {
       //Serial.println("Sensor error");
    }
    // device is slow make sure we don't poll it too often
    delay(1000);
}
	

