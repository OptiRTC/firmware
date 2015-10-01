#include "main.h"
#include "application.h"

void setup(){
	pinMode(D7, OUTPUT);
}

void loop(){
	digitalWrite(D7,HIGH);
	delay(200);
	digitalWrite(D7,LOW);
	delay(200);

}
