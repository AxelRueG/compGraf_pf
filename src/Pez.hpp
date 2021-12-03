#ifndef PEZ_H
#define PEZ_H
#include <iostream>
#include "Curva.h"
using namespace std;

struct Pez {
  bool pezNada = true;
  bool pezZigZaguea = true;
	
  float t = 0; // tiempo en el que se encuentra el pez
  float t_paso = 0.001;
  float ang_xy = 0; // orientacion en el plano xy
  float ang_zx = 0; // orientacion en el plano xz
  float x = 0, 
        y = 0,
        z = 0; // posicion en el agua
  void toString(){
    cout<<"fish position: "<<x<<" "<<y<<" "<<z<<endl;
  }
  void Mover(Spline curva);
  //  float vel = 0; // velocidad actual
  //  const float top_speed = 60; // velociad máxima
};

extern Pez el_pez;

#endif
