/*
Ausgangstreiber fuer ESP32-Fischertechnik-Anbindung
Autor: Johannes Marquart
*/

// #define DEBUGGING	//comment out if not debugging

#include "ft_ESP32_IOobjects.h"

CMotor::CMotor()
{
	//Abschalten des Motortreibers, welcher von diesem Objekt versorgt wird.
	//Evtl. noch undefinierte Pins koennen so kein falsches Signal an den Motortreiber geben
	pinMode(PIN_M_INH, OUTPUT);
	digitalWrite(PIN_M_INH, LOW);
	
	mMotorNr = 0;
	mPortNrPWM = 0;
	mPortNrDir = 0;
	mRechtslauf = true;
	mDrehzahl = 0;
}

CMotor::CMotor(unsigned int motorNr)
{
	//Abschalten des Motortreibers, evtl. noch undefinierte Pins koennen so kein falsches Signal an den Motortreiber geben
	pinMode(PIN_M_INH, OUTPUT);
	digitalWrite(PIN_M_INH, LOW);
	
	//Initialisieren des Motorobjektes
	mMotorNr = motorNr;
	
	mPortNrPWM = PIN_M_PWM[mMotorNr];
	mPortNrDir = PIN_M_DIR[mMotorNr];
	mRechtslauf = true;
	mDrehzahl = 0;

	//Zuweisen PWM-Generator zu Pin. Generator 0,2,4,6 fuer Drehzahl
	ledcAttachPin(mPortNrPWM, mMotorNr*2);	//Pin-Nr fuer Drehzahl, PWM-Generator Nr
	ledcSetup(mMotorNr*2, 21700, 8);	//PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
	//Frequency of 21700Hz is optimal for this driver, according to datasheet

	//pinMode(mPortNrDir, OUTPUT);	//Pin-Nr fuer Richtungsangabe, Ausgang
	//digitalWrite(mPortNrDir, HIGH);	//fruehzeitiges Definieren des Dir-Pins	//Umwandeln in PWM-Signal, bessere "Kooperation" mit Lampen (gleiche ansteuerung)!!!!!!!!
	//Zuweisen PWM-Generator zu Pin. Generator 1,3,5,7 fuer Richtung
	ledcAttachPin(mPortNrDir, (mMotorNr*2)+1);	//Pin-Nr fuer Richtungsangabe, PWM-Generator Nr
	ledcSetup(mMotorNr*2+1, 21700, 8);
	
	ledcWrite(mMotorNr*2, 0);	//fruehzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
	ledcWrite(mMotorNr*2+1,255);	//fruehzeitiges Definieren des Dir-Pins
	
}

void CMotor::setValues(bool rechtslauf, unsigned int drehzahl)
{
	mRechtslauf = rechtslauf;
	mDrehzahl = drehzahl;
	
	//Serial.begin(9600); -> sollte in der aufrufenden Instanz schon initialisiert sein
#ifdef DEBUGGING
	Serial.print("Motor ");
	Serial.print(mMotorNr);
	Serial.print(" dreht in Richtung ");
	Serial.print(mRechtslauf);
	Serial.print(" mit Drehzahl ");
	Serial.print(mDrehzahl);
#endif // DEBUGGING

	//Berechnen der PWM-Werte
	int drehzahl_pwm;
	if (mDrehzahl < 1)
	{
		drehzahl_pwm = 0;
	}
	else if (mDrehzahl >7)
	{
		drehzahl_pwm = 255;
	}
	else
	{
		drehzahl_pwm = mDrehzahl * 256 / 8;
	}

	//Zuweisen der Richtung an den richtigen Pin entsprechend der Motornr.
	if (mRechtslauf)
	{
		//digitalWrite(mPortNrDir, HIGH);
		ledcWrite(mMotorNr*2+1, 255);	//Generator fuer Richtung wird auf max. (255) gesetzt
	}
	else
	{
		//digitalWrite(mPortNrDir, LOW);
		ledcWrite(mMotorNr*2+1, 0);	//Generator fuer Richtung wird auf 0 gesetzt
		//!!! Unbedingt im Datenblatt des Motortreibers nachsehen, wie PWM und Richtung zusammenhaengen !!!
		drehzahl_pwm = 255 - drehzahl_pwm;	//wenn der Motor rueckwaerts laeuft, ist die PWM invertiert (255 = min, 0 = max)
	}

	//Zuweisen des PWM-Werts an den richtigen Generator entsprechend der Motornr.
	ledcWrite(mMotorNr*2, drehzahl_pwm);
	
	digitalWrite(PIN_M_INH, HIGH);	//Einschalten Motortreiber
	
#ifdef DEBUGGING
	Serial.print("  rawVal Dir: ");
	Serial.print(mPortNrDir);
	Serial.print(" PWM: ");
	Serial.print(mPortNrPWM);
	Serial.print(" Val: ");
	Serial.println(drehzahl_pwm);
#endif
}

void CMotor::reRun()
{
	if(mDrehzahl > 0)
	{
		setValues(mRechtslauf, mDrehzahl);
	}
}

CServoMotor::CServoMotor()
{
	mMotorNr = 0;
	mPinNrPWM = 0;
	mMinDuty = 4;
	mMaxDuty = 11;
	mRelDuty = 0;
}
/*
CServoMotor::CServoMotor(unsigned int motorNr)
{
	//init of servomotor-object
	mMotorNr = motorNr;
	mPinNrPWM = PIN_M_PWM[motorNr];
	mMinDuty = 4;
	mMaxDuty = 11;
	mRelDuty = 50;
	//attach PWM-generator to pin
	ledcAttachPin(mPinNrPWM, mMotorNr*2);	//pin-nr for position, pwm-generator nr
	ledcSetup(mMotorNr*2, 50, 12);	//pwm-generator nr, 50Hz pwm, 12-bit resolution (0..4095)
		//high resolution as only a small range is used (between 163 and 450)
	ledcWrite(mMotorNr*2, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);	//early set of pin -> servo moves to neutral
}
*/
CServoMotor::CServoMotor(unsigned int motorNr, unsigned int dutyCycle, unsigned int pinNrOverride)
{
	//init of servomotor-object
	mMotorNr = motorNr;
	if (0 == pinNrOverride)
	{
		mPinNrPWM = PIN_M_PWM[motorNr];
	}
	else
	{
		mPinNrPWM = pinNrOverride;
	}
	mMinDuty = 4;	//minimal real DutyCycle in %
	mMaxDuty = 11;	//maximal real DutyCycle in %
	mRelDuty = dutyCycle < 100 ? dutyCycle : 100;	//limiting maximum value to 100(%)
	//attach PWM-generator to pin
	ledcAttachPin(mPinNrPWM, mMotorNr*2);	//pin-nr for position, pwm-generator nr
	ledcSetup(mMotorNr*2, 50, 12);	//pwm-generator nr, 50Hz pwm, 12-bit resolution (0..4095)
		//high resolution as only a small range is used (between 163 and 450)
	ledcWrite(mMotorNr*2, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);	//early set of pin -> servo moves to given duty-cycle
}

void CServoMotor::setValue(unsigned int dutyCycle)
{
	mRelDuty = dutyCycle < 100 ? dutyCycle : 100;	//limiting maximum value to 100(%)

#ifdef DEBUGGING
	Serial.print("Servo ");
	Serial.print(mMotorNr);
	Serial.print(" geht auf Position ");
	Serial.print(dutyCycle);
#endif // DEBUGGING

	ledcWrite(mMotorNr*2, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);	//calculate real duty-cycle for servos: 0%rel = 4%, 100%rel = 11%

#ifdef DEBUGGING
	Serial.print("  raw Duty-Cycle Val: ");
	Serial.println((4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);
#endif // DEBUGGING

}

void CServoMotor::reRun()
{
	ledcWrite(mMotorNr*2, (4095 * (100 * mMinDuty + (mMaxDuty - mMinDuty) * mRelDuty)) / 10000);
}

Lampe::Lampe()
{
	//Abschalten des Lampentreibers, welcher von diesem Objekt versorgt wird.
	//Evtl. noch undefinierte Pins koennen so kein falsches Signal an den Lampentreiber geben
	pinMode(PIN_L_INH, OUTPUT);
	digitalWrite(PIN_L_INH, LOW);
	
	mLampeNr = 0;
	mPortNrPWM = 0;
	mHelligkeit = 0;
}

Lampe::Lampe(unsigned int lampeNr)
{
	//Abschalten des Motortreibers, evtl. noch undefinierte Pins koennen so kein falsches Signal an den Motortreiber geben
	pinMode(PIN_L_INH, OUTPUT);
	digitalWrite(PIN_L_INH, LOW);
	
	//Initialisieren des Lampenobjektes
	mLampeNr = lampeNr;
	
	//Folgender Abschnitt erlaubt es pro 'pololu a4990 dual-motor-driver' 4 Lampen unabhaengig voneinander anzusteuern
	if(mLampeNr % 2 == 0)	//Lampen 0,2,4,... werden durch Pins der PWM-Reihe angesteuert
	{
		mPortNrPWM = PIN_L_PWM[mLampeNr/2];
	}
	else	//Lampen 1,3,5,... werden durch Pins der DIR-Reihe angesteuert
	{
		mPortNrPWM = PIN_M_DIR[mLampeNr/2];
	}
	mHelligkeit = 0;

	//Zuweisen des PWM-Generators an einen Port entsprechend der Lampennummer...
	ledcAttachPin(mPortNrPWM, mLampeNr);	//Pin-Nr, PWM-Generator Nr
	ledcSetup(mLampeNr, 21700, 8);	//PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
	//pinMode(PORT_M_DIR[mLampeNr], OUTPUT);	//Pin-Nr fuer "Richtungsangabe", Ausgang
	/*da hier Lampen am selben Treiber angeschlossen sind wie die Motoren, benoetigen sie eine "Richtung"
	fuer eine korrekte Interpretation der PWM-Signale
	*/
	
	//digitalWrite(PORT_M_DIR[mLampeNr], HIGH);	//fruehzeitiges Definieren des Dir-Pins
	ledcWrite(mLampeNr, 0);	//fruehzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
	
}

void Lampe::setValues(unsigned int helligkeit)
{
	//Serial.begin(9600);
#ifdef DEBUGGING
	mHelligkeit = helligkeit;
	Serial.print("Lampe ");
	Serial.print(mLampeNr);
	Serial.print(" leuchtet mit Helligkeit ");
	Serial.println(mHelligkeit);
#endif

	//Berechnen der PWM - Werte
	int helligkeit_pwm;
	if (mHelligkeit > 7)
	{
		helligkeit_pwm = 255;
	}
	else if (mHelligkeit < 1)
	{
		helligkeit_pwm = 0;
	}
	else
	{
		helligkeit_pwm = mHelligkeit * 256 / 8;
	}

	if(mLampeNr%2 == 1)	//bei den Lampen 1,3,5,7 sind laut Datenblatt die PWM invertiert (255 = min, 0 = max)
	{
		helligkeit_pwm = 255 - helligkeit_pwm;
	}
	
	//Zuweisen des PWM-Werts an den richtigen Port entsprechend der Lampennummer
	ledcWrite(mLampeNr, helligkeit_pwm);
	//digitalWrite(PORT_M_DIR[mLampeNr], HIGH);	//Richtungsangabe, siehe Beschreibung im Konstruktor
	
	digitalWrite(PIN_L_INH, HIGH);	//Einschalten Lampentreiber
	
#ifdef DEBUGGING
	Serial.print("PWM: ");
	Serial.print(mPortNrPWM);
	Serial.print(" Val: ");
	Serial.println(helligkeit_pwm);
#endif
}

void Lampe::reRun()
{
	if(mHelligkeit > 0)
	{
		setValues(mHelligkeit);
	}
}

DigitalAnalogIn::DigitalAnalogIn()
{
	mInputNummer = 0;
}

DigitalAnalogIn::DigitalAnalogIn(unsigned int inputNummer)
{
	mInputNummer = inputNummer;
	mInputPortNr = PIN_IN[mInputNummer];
}

unsigned int DigitalAnalogIn::getValueDigital()
{
	pinMode(mInputPortNr, INPUT_PULLUP);	//Pin-Modus einrichten: Input mit Pull-Up Widerstand
	bool eingabe = !digitalRead(mInputPortNr);	//Inverse Logik: Schalter gedrueckt = 1 (= Port liegt auf Masse)
	return (unsigned int) eingabe;
}

unsigned int DigitalAnalogIn::getValueAnalog()
{
	pinMode(mInputPortNr, INPUT);	//Pin-Modus einrichten: Input ohne Pull-Up Widerstand
	unsigned int eingabe = analogRead(mInputPortNr);
	return eingabe;
}

void DigitalAnalogIn::setValueDigital(bool ledLevel)
{
	pinMode(mInputPortNr, OUTPUT);	//Pin_Modus einrichten: Output
#ifdef DEBUGGING
	Serial.print("Setze LED ");
	Serial.print(mInputNummer);
	Serial.print(" auf ");
#endif
	if (ledLevel)
	{
		digitalWrite(mInputPortNr, HIGH);	//Pin auf HIGH setzen
#ifdef DEBUGGING
		Serial.println(" HIGH");
#endif
	}
	else
	{
		digitalWrite(mInputPortNr, LOW);	//Pin auf LOW setzen
#ifdef DEBUGGING
		Serial.println(" LOW");
#endif
	}
}
