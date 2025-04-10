#include <Arduino.h>
class puenteH{
private:
  int a1, a2, b1, b2;
  public:
  puenteH(int a, int b, int c, int d){
    this->a1 = a; this->a2 = b;
    this->b1 = c; this->b2 = d;
    pinMode(a1, OUTPUT);
    pinMode(a2, OUTPUT);
    pinMode(b1, OUTPUT);
    pinMode(b2, OUTPUT);
   }
  void mtrA(bool dir){
    digitalWrite(a1, dir);
    digitalWrite(a2, !dir);
  }
  void mtrB(bool dir){
    digitalWrite(b1, dir);
    digitalWrite(b2, !dir);
  }
  void mtrStop(){
    digitalWrite(a1, 0);
    digitalWrite(a2, 0);
    digitalWrite(b1, 0);
    digitalWrite(b2, 0);
  }
  void adelante(){
    mtrA(0);
    mtrB(1);
  }
  void atras(){
    mtrA(1);
    mtrB(0);
  }
  void derecha(){
    mtrA(0);
    mtrB(0);
  }
  void izquierda(){
    mtrA(1);
    mtrB(1);
  }

};
