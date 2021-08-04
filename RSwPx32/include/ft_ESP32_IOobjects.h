/*
Ausgangstreiber für ESP32-Fischertechnik-Anbindung
Autor: Johannes Marquart

Festlegungen:
	pin = Physikalischer Anschluss auf dem ESP32-Wroom-Board
	port = Logischer Anschluss
*/
#ifndef FT_ESP32_IOOBJECTS_H
#define FT_ESP32_IOOBJECTS_H

#include <Arduino.h>

//Festlegen Anzahl Ports
constexpr size_t MOTOR_QTY = 4;	//Anzahl der Motoren
constexpr size_t LAMP_QTY = 4;	//Anzahl der Lampen
constexpr size_t DAIN_QTY = 7;	//Anzahl der Eingänge (digital/analog)

//Zuweisung Ports
const int PIN_M_DIR[MOTOR_QTY] = { 17, 4, 14, 12 };	//Output-Pins Motor-Richtung
const int PIN_M_PWM[MOTOR_QTY] = { 16, 2, 13, 15 };	//Output-Pins Motor-Drehzahl
const int PIN_M_INH = 27;	//Output-Pin Einschalten Motortreiber

const int PIN_L_PWM[LAMP_QTY] = { 16, 2, 13, 15 };	//Output-Pins Lampe, werden hier über den selben Treiber angesteuert
const int PIN_L_INH = 27;	//Output-Pin Einschalten Lampentreiber

const int PIN_IN[DAIN_QTY] = { 39, 34, 35, 32, 33, 25, 26 };	//Input-Pins Ditital/Analog


class CMotor
{
public:
	CMotor();	//Standardkonstruktor, setzt alles auf 0;
	CMotor(unsigned int motorNr);	//Konstruktor, Motor-Nr (0..3), weist zu: Pin-Nr für PWM, Pin-Nr für Richtung
	//Destructor is missing -> detach PWM-Generator from pin!
	
	void setValues(bool, unsigned int);	//neue Motorwerte setzen (Richtung, Drehzahl)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
	//evtl. eine Methode Stop einbauen
private:
	unsigned int mMotorNr;	//Motornummer 0..3, wird bei Erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mPortNrDir;	//PortNr für Richtung, wird bei Erstellung des Objekts zugewiesen
	bool mRechtslauf;	//Drehrichtung: rechts = ture, links = false
	unsigned int mDrehzahl;	//aktuelle Geschwindigkeit (von 0 bis 8)

};

class CServoMotor
{
public:
	CServoMotor();	//default constructor, puts servomotor into neutral position
	//Not neccessarily needed, as second constructor covers the same:
	//CServoMotor(unsigned int motorNr);	//constructor, motor-Nr(0..3), gives pin-nr for PWM, init position is neutral
	CServoMotor(unsigned int motorNr, unsigned int dutyCycle = 50, unsigned int pinNrOverride = 0);	//contsructor, motor-Nr(0..3) - gives pin-nr for PWM, allows init of position - default is neutral, allows override of PWM-pin - Use with care!
	//Destructor is missing -> detach PWM-Generator from pin!
	
	void setValue(unsigned int dutyCycle);	//new values to be set (servo-position between 0%..100%)
	void reRun();	//pins will be set with stored values
private:
	unsigned int mMotorNr;	//motor-nr 0..3, will be set in constructor
	unsigned int mPinNrPWM;	//pin-nr for PWM, will be set in constructor
	unsigned int mMinDuty, mMaxDuty;	//minimum and maximum duty-cycle of servo (normally 4%..11%)
	unsigned int mRelDuty;	//current relative duty-cycle of servo (0%..100% between duty-boarders)
};

class EncoderMotor
{
public:

private:

};

class Lampe
{
public:
	Lampe();
	Lampe(unsigned int);	//Konstruktor, Lampe-Nr (0..7), weist zu: Pin-Nr für PWM
	//Destructor is missing->detach PWM - Generator from pin!

	void setValues(unsigned int);	//neue Lampenwerte setzen (Aktiv, Helligkeit)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
private:
	unsigned int mLampeNr;	//LampenNr 0..7, wird bei Erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mHelligkeit;	//aktuelle Helligkeit (von 0 bis 8)
};

class Encoder
{
public:

private:

};

class DigitalAnalogIn
{
public:
	DigitalAnalogIn();
	DigitalAnalogIn(unsigned int);
	unsigned int getValueAnalog();
	unsigned int getValueDigital();
	void setValueDigital(bool);	//Digitalen Ausgang setzen (HIGH/LOW)
private:
	unsigned int mInputNummer;
	unsigned int mInputPortNr;
};

#endif