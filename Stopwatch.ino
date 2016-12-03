/*
*  Stopwatch - 2 targets, which will stop counting
*  
*/

#include <TimerFive.h>
#include <Adafruit_CharacterOLED.h>  // Oled display used http://www.tme.eu/cz/details/rec001602abpp5n0/alfanumericke-oled-displeje/raystar-optronics/rec001602abpp5n00000/
// #include <LiquidCrystal.h> // Uncomment for LCD 
#include "PinChangeInterrupt.h"


#define ledPin 13					//Led on the board
const int buzzer = A0;				//buzzer to arduino pin A0


// Choose a valid PinChangeInterrupt pin of your Arduino board
#define Start A8					// Input for the Start of the Stopwatch
#define Reset A14					// Input for the Reset of the Stopwatch
#define Left_Target A9				// Input for the Left Target of the Stopwatch
#define Right_Target A12				// Input for the Right Target of the Stopwatch

// State of the stopwatch
volatile int stat = 2;				//State 1= Ready for Start, 2= Running, 3= Left Target, 4 Right Target


volatile int clock_millisecond = 0;
volatile int clock_hundreths = 0;
volatile int clock_tenths = 0;
volatile int clock_second = 0;
volatile int clock_minute = 0;
volatile int clock_hour = 0;

volatile int left_clock_millisecond = 0;
volatile int left_clock_hundreths = 0;
volatile int left_clock_tenths = 0;
volatile int left_clock_second = 0;
volatile int left_clock_minute = 0;
volatile int left_clock_hour = 0;

volatile int right_clock_millisecond = 0;
volatile int right_clock_hundreths = 0;
volatile int right_clock_tenths = 0;
volatile int right_clock_second = 0;
volatile int right_clock_minute = 0;
volatile int right_clock_hour = 0;

volatile bool zmenaCasu = false;
int pocitadlo = 0;
bool LevyTerc = true;				// False= shozen� True=naho�e
bool PravyTerc = true;				// False= shozen� True=naho�e
bool Beep = false;					// False - Off, True- On
bool stopped = true;				// Stopped???


/*************************
Initialize display
*************************/
Adafruit_CharacterOLED lcd(OLED_V2, 8, 10, 9, 4, 5, 6, 7);
//LiquidCrystal lcd(8,10, 9, 4, 5, 6, 7); // RS,RW,E,D4,D5,D6,D7 on display


void setup()
{
	// Initialize Timer, Serial Communication, LCD
	Timer5.initialize(1000);         // initialize timer5, and set a 1 000 us interrupt
	Timer5.attachInterrupt(Pricitej);  // attaches Pricitej() as a timer overflow interrupt
	Serial.begin(115200);
	lcd.begin(16, 2);
	lcd.clear(); // clear the display
	lcd.setCursor(0, 0); // set the cursor to first character on line 1

	// Set pin to input with a pullup, led to output
	pinMode(Start, INPUT_PULLUP);
	pinMode(Reset, INPUT_PULLUP);
	pinMode(Left_Target, INPUT_PULLUP);
	pinMode(Right_Target, INPUT_PULLUP);
	pinMode(ledPin, OUTPUT);

	// Attach the new PinChangeInterrupt and enable event function below
	attachPCINT(digitalPinToPCINT(Start), Start_Stopek, CHANGE);
	attachPCINT(digitalPinToPCINT(Reset), Reset_Stopek, CHANGE);
	attachPCINT(digitalPinToPCINT(Left_Target), Stop_Left_Target, CHANGE);
	attachPCINT(digitalPinToPCINT(Right_Target), Stop_Right_Target, CHANGE);

}


/*
**
**	Here are interrupts routines for the buttons - Start, Stop, Laft and Right Targets and Timer interrupt routine
**
*/

void Start_Stopek(void) {
	// Switch Led state
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	stat = 1;
	
}

void Reset_Stopek(void) {
	// Switch Led state
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	stat = 2;
}

void Stop_Left_Target(void) {
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	stat = 3;
}

void Stop_Right_Target(void) {
	// Switch Led state
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	stat = 4;
}


// Interrupt Timer5 - pricita cas
void Pricitej()
{	
	cli();                // disable global interrupts
	
	clock_millisecond++;
	if (clock_millisecond == 10)
	{	
		zmenaCasu = true;
		clock_hundreths++;
		clock_millisecond = 0;
	}
		
	if (clock_hundreths == 10)
	{
		clock_tenths++;
		clock_hundreths = 0;
		digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
	}
	
	if (clock_tenths == 10)
	{
		clock_second++;
		clock_tenths = 0;
	}

	if (clock_second == 60)
	{
		clock_minute++;
		clock_second = 0;
	}
	if (clock_minute == 60)
	{
		clock_hour++;
		clock_minute = 0;
	}
	sei();                // enable interrupts
}


void loop()
{
	// your program here...
	switch (stat) 
	{
	case 1: 
		{
			
			Timer5.start();
			pocitadlo = 0;
			stopped = false;
		}
		break;
	case 2: {
			clock_millisecond = 0;
			clock_hundreths = 0;
			clock_tenths = 0;
			clock_second = 0;
			clock_minute = 0;
			clock_hour = 0;
			LevyTerc = true;
			PravyTerc = true;
			stopped = false;
			zmenaCasu = true;
		//	Timer5.restart();
			if (pocitadlo == 0)
				{
					lcd.clear();
					pocitadlo = pocitadlo + 1;
					Beep = true;
				}
			lcd.setCursor(0, 1); }
			break;
	case 3: {
			if (LevyTerc == true) {
				LevyTerc = false;
				if (PravyTerc == false)
					{
						Timer5.stop();
						stopped = true;
					}	
				lcd.setCursor(0, 1);
				lcd.print("L");
				
				left_clock_millisecond=clock_millisecond;
				left_clock_hundreths=clock_hundreths;
				left_clock_tenths= clock_tenths;
				left_clock_second= clock_second;
				left_clock_minute= clock_minute;
				left_clock_hour=clock_hour;
						
				char timeL[6];
				sprintf(timeL, "%02i:%01i%01i%01i", left_clock_second, left_clock_tenths, left_clock_hundreths, left_clock_millisecond);
				lcd.print(timeL);
				Serial.print("Levy terc:");
				Serial.println(timeL);
				Beep = true;
				
				}
			}
			break;
	case 4: {
			if (PravyTerc == true) {
				PravyTerc = false;
				if (LevyTerc == false)
					{
					Timer5.stop();
					stopped = true;
					}
				//Pravy_cas = Stopky.elapsed();
				lcd.setCursor(9, 1);
				lcd.print("P");
				
				right_clock_millisecond = clock_millisecond;
				right_clock_hundreths = clock_hundreths;
				right_clock_tenths = clock_tenths;
				right_clock_second = clock_second;
				right_clock_minute = clock_minute;
				right_clock_hour = clock_hour;

				char timeL[6];
				sprintf(timeL, "%02i:%01i%01i%01i", right_clock_second, right_clock_tenths, right_clock_hundreths, right_clock_millisecond);
				lcd.print(timeL);
				Serial.print("Levy terc:");
				Serial.println(timeL);
				Beep = true;
				

				}
			}	
			break;
	default:
		break;
	}
	
	
	
	
	// There was some change in the time???
	if (zmenaCasu == true) {
		zmenaCasu = false;
		char Time[12];
		
		sprintf(Time, "%02i:%02i.%01i%01i", clock_minute, clock_second, clock_tenths, clock_hundreths);
		
		Serial.println(Time);
		lcd.setCursor(0, 0);
		lcd.print(Time);
	}
	
	if (stopped == true)
	{
		stopped = false;
		char Time[12];
		sprintf(Time, "%02i:%02i.%01i%01i%01i", clock_minute, clock_second, clock_tenths, clock_hundreths, clock_millisecond);
		Serial.println(Time);
		lcd.setCursor(0, 0);
		lcd.print(Time);
	}

	if (Beep == true)
	{
		Beep = false;		// Reset Beep
		tone(buzzer, 2000, 50); // Send 2KHz sound signal, time 0,05 s ...
	}
}
