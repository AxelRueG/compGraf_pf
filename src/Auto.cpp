#include "Auto.hpp"
#include <cmath>

Pista la_pista;

Auto el_auto;

constexpr float G2R = M_PI/180.f;

void Auto::Mover(float acel, float dir) {
  // aplicar los controles
  if (dir)
    rang1 = (8*rang1+dir*0.0087*float(10*vel+60*(top_speed-vel))/top_speed)/9;
  else
    rang1=3*rang1/4;
  if (!acel) acel = -.2;
  // mover el auto
  vel += acel*.75-.25;
  if (vel<0) vel=0;
  else if (vel>top_speed) vel=top_speed;
  x += vel*std::cos(ang)/100;
  y += vel*std::sin(ang)/100;
  // la pista es ciclica
  if (x<-la_pista.ancho) x += la_pista.ancho*2;
  else if (x>la_pista.ancho) x -= la_pista.ancho*2;
  if (y<-la_pista.alto) y += la_pista.alto*2;
  else if (y>la_pista.alto) y -= la_pista.alto*2;
  ang += rang1*vel/150;
  rang2 += vel/10;
}
