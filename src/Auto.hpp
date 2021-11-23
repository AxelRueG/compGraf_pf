#ifndef AUTO_H
#define AUTO_H

struct Pista {
  int ancho = 200;
  int alto = 200;
};

extern Pista la_pista;

struct Auto {
  float ang = 4.53; // orientacion
  float x = -66, y = 35; // posicion en la pista
  float vel = 0; // velocidad actual
  float rang1 = 0; // direccion de las ruedas delanteras respecto al auto (eje x del mouse) 
  float rang2 = 0; // giro de las ruedas sobre su eje, cuando el auto avanza 
  const float top_speed = 60; // velociad máxima
  void Mover(float acel, float dir); // función que aplica la "física" y actualiza el estado
};

extern Auto el_auto;

#endif
