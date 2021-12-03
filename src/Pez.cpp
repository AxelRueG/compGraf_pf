#include "Pez.hpp"
#include <cmath>
#include "Curva.h"
using namespace std;

Pez el_pez;
//float M_PI = 3.141516;

constexpr float G2R =3.141516/180.f;
constexpr float R2G = 180.f/3.141516; // radianes a grados

float calcAnglee (float x, float y) {
	/// calculamos el angulo en grados
	float ang = atan(y/x); // radianes
	if (x<0 and y>0)  ang=M_PI+ang;
	if (x<0 and y<=0) ang+=M_PI;
	if (ang<0) ang += 2*M_PI;
	return ang*R2G;
}

punto vectorRotado (punto p, float angle) {
	angle *= G2R;
	float x = p[0]*cos(angle)+p[1]*sin(angle);
	float y = -p[0]*sin(angle)+p[1]*cos(angle);
	float z = p[2];
	return punto(x,y,z,0);
}

void Pez::Mover(Spline curva){
	
	//	calculamos posicion
	t += t_paso;
	if (t>1) t=0;
	
	punto p,d;
	curva.Evaluar(t,p,d);
	
	//	actualizo la ubicacion del pez
	x = p[0]; y = p[1]; z = p[2];
	
	//	calculo el angulo de la matriz de rotatacion z
	ang_xy = calcAnglee(d[0],d[1]);  
	
	//	creamos un vector unitario en direccion x
	punto unitario(1,0,0,0);
	punto u = vectorRotado(unitario,-ang_xy); // lo rotamos para que quede en un plano vertical con la derivada
	float m1 = d.mod();
	float productoPunto = (d*u)/m1; 
	
	ang_zx = acos(productoPunto) * R2G;   	
	if ( (u[0]*d[2]) > 0 ) ang_zx = -1*ang_zx;
	if (d[2] == 0) ang_zx = 0;
	
}
