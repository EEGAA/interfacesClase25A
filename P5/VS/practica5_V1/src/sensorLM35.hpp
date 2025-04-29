#include <Arduino.h>
class LM35{
private:
    int pinADC, pinGND;
    float voltaje, temperatura;
public:
    LM35(int a, int b){
        this->pinADC = a;
        this->pinGND = b;
        pinMode(pinADC, INPUT);
        pinMode(pinGND, OUTPUT);
        digitalWrite(pinGND, LOW);
    }
    float getCentigrados(){
        voltaje = analogRead(pinADC) * (3.3/4095.0);
        //Serial.print("V = "); Serial.println(voltaje);
        temperatura = voltaje * 100;
        //Serial.print(temperatura); Serial.print(" C°"); 
        return temperatura;
    }
};