#include <Arduino.h>
class senUltra{
private:
    int pinEcho, pinTrig;
    long duration;
    float distance;
public:
    senUltra(int echo, int trig){
        this->pinEcho = echo;
        this->pinTrig = trig;
        pinMode(pinTrig, OUTPUT);
        pinMode(pinEcho, INPUT);
    }
    float getDistancia(){
        // Enviar un pulso corto al pin de trigger
        digitalWrite(pinTrig, LOW);
        delayMicroseconds(2);
        digitalWrite(pinTrig, HIGH);
        delayMicroseconds(10);
        digitalWrite(pinTrig, LOW);

        // Medir la duración del pulso en el pin de echo
        duration = pulseIn(pinEcho, HIGH);

        // Calcular la distancia en centímetros
        // La velocidad del sonido es 343 m/s o 0.0343 cm/μs
        // La distancia es ida y vuelta, por lo que se divide entre 2
        distance = duration * 0.0343 / 2;

        return distance;
    }
};