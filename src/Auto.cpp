#include "Auto.hpp"
#include <cmath>
#include "Curva.h"

Pista la_pista;

Auto el_auto;

Pez el_pez;

constexpr float G2R = M_PI/180.f;
constexpr float R2G = 180.f/M_PI; // radianes a grados

void Pez::Mover(Spline curva){
  
  // calculamos posicion
  t += t_paso;
  if (t>1) t=0;
  
  punto p,d;
  curva.Evaluar(t,p,d);
  
  x = p[0]; y = p[1]; z = p[2];
  
  // calculamos el angulo xy
  float ang = atan(d[1]/d[0]); // radianes
  if (d[0]<0 and d[1]>0)  ang=M_PI+ang;
  if (d[0]<0 and d[1]<=0) ang+=M_PI;
  ang_xy = ang*R2G;
  
  // calculamos el angulo xz
  ang = atan(d[2]/d[0]); // radianes
  if (d[0]<0 and d[1]>0)  ang=M_PI+ang;
  if (d[0]<0 and d[1]<=0) ang+=M_PI;
  ang_xz = ang*R2G;
}

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
