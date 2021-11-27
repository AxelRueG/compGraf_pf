#ifndef AUTO_H
#define AUTO_H
#include <iostream>
#include "Curva.h"
using namespace std;

struct Pista {
  int ancho = 200;
  int alto = 200;
};

extern Pista la_pista;

struct Pez {
  float t = 0; // tiempo en el que se encuentra el pez
  float t_paso = 0.001;
  float ang_xy = 0; // orientacion en el plano xy
  float ang_xz = 0; // orientacion en el plano xz
  float x = 0, 
        y = 0,
        z = 0; // posicion en la pista
  void toString(){
    cout<<"fish position: "<<x<<" "<<y<<" "<<z<<endl;
  }
  void Mover(Spline curva);
  //  float vel = 0; // velocidad actual
  //  const float top_speed = 60; // velociad máxima
};

extern Pez el_pez;

struct Auto {
  float ang = 4.53; // orientacion
  float x = -66, y = 35; // posicion en la pista
  float vel = 0; // velocidad actual
  float rang1 = 0; // direccion de las ruedas delanteras respecto al auto (eje x del mouse) 
  float rang2 = 0; // giro de las ruedas sobre su eje, cuando el auto avanza 
  const float top_speed = 60; // velociad máxima
  void Mover(float acel, float dir); // función que aplica la "física" y actualiza el estado
  void toString(){
    cout<<"car position: "<<x<<" "<<y<<endl;
  }
};

extern Auto el_auto;

#endif
