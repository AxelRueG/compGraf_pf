#include <iostream>
#include <fstream>
#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
#endif
#include "Malla.hpp"
using namespace std;
void Malla::Load(const char *fname) {
  e.clear(); p.clear();
  ifstream f(fname);
  if (!f.is_open()) {
    cout<< "no hay archivo"<<endl;
    return;
  }
  int i,nv;
  f>>nv;
  float x,y,z;
  for (i=0;i<nv;i++) {
    f>>x>>y>>z;
    p.push_back(Nodo(x,y,z));
    pIdentity.push_back(Punto(x,y,z));
  }
  int ne;
  f>>ne;
  int v0,v1,v2,v3;
  for (i=0;i<ne;i++) {
    f>>nv>>v0>>v1>>v2;
    if (nv==3) { AgregarElemento(v0,v1,v2); }
    else { f>>v3; AgregarElemento(v0,v1,v2,v3); }
  }
  f.close();
  MakeVecinos();
  MakeNormales();
}

Malla::Malla(const char *fname) {
  if (fname) Load(fname);
}

void Malla::Save(const char *fname) {
  ofstream f(fname);
  f<<p.size()<<endl;
  unsigned int i,j;
  for (i=0;i<p.size();i++)
    f<<p[i].x[0]<<' '<<p[i].x[1]<<' '<<p[i].x[2]<<endl;
  f<<e.size()<<endl;
  for (i=0;i<e.size();i++) {
    f<<e[i].nv;
    for (j=0;j<e[i].nv;j++)
      f<<' '<<e[i][j];
      f<<endl;
  }
  f.close();
}

void Malla::AgregarElemento(int n0, int n1, int n2, int n3) {
  int ie=e.size(); e.push_back(Elemento(n0,n1,n2,n3)); // agrega el Elemento
  // avisa a cada nodo que ahora es vertice de este elemento
  p[n0].e.push_back(ie); p[n1].e.push_back(ie);
  p[n2].e.push_back(ie); if (n3>=0) p[n3].e.push_back(ie);
  
}

void Malla::ReemplazarElemento(int ie, int n0, int n1, int n2, int n3) {
  Elemento &ei=e[ie];
  // estos nodos ya no seran vertices de este elemento
  for (unsigned int i=0;i<ei.nv;i++) {
    vector<int> &ve=p[ei[i]].e;
    ve.erase(find(ve.begin(),ve.end(),ie));
  }
  ei.SetNodos(n0,n1,n2,n3);
  // estos nodos ahora son vertices
  p[n0].e.push_back(ie); p[n1].e.push_back(ie); p[n2].e.push_back(ie); 
  if (n3>=0) p[n3].e.push_back(ie); 
}

void Malla::MakeNormales() {
  normal.resize(p.size());
  for (unsigned int i=0;i<p.size();i++) {// "promedio" de normales de cara
    vector<int> &en=p[i].e;
    Punto n(0,0,0); int k;
    for (unsigned int j=0;j<en.size();j++) {
      Elemento &ej=e[en[j]];
      k=ej.Indice(i);
      n+=(p[ej[k]]-p[ej[k-1]])%(p[ej[k+1]]-p[ej[k]]);
    }
    float m=n.mod(); if (m>1e-10) n/=m; else n.zero();
    normal[i]=n;
  }
}

void Malla::Draw(bool relleno) {
  // dibuja los Elementos
//  glColor4f(1.f,1.f,1.f,0.f);
  unsigned int i;
  if (relleno) {	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  } else {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  }
  for (i=0;i<e.size();i++) {
    if (e[i].nv==4) {
      glBegin(GL_QUADS);
      glNormal3fv(normal[e[i][0]].x); glVertex3fv(p[e[i][0]].x); 
      glNormal3fv(normal[e[i][1]].x); glVertex3fv(p[e[i][1]].x); 
      glNormal3fv(normal[e[i][2]].x); glVertex3fv(p[e[i][2]].x); 
      glNormal3fv(normal[e[i][3]].x); glVertex3fv(p[e[i][3]].x); 
    } else {
      glBegin(GL_TRIANGLES);
      glNormal3fv(normal[e[i][0]].x); glVertex3fv(p[e[i][0]].x);
      glNormal3fv(normal[e[i][1]].x); glVertex3fv(p[e[i][1]].x);
      glNormal3fv(normal[e[i][2]].x); glVertex3fv(p[e[i][2]].x);
    }
    glEnd();
  }
  // dibuja solo los nodos sueltos
  glDisable(GL_LIGHTING);
  glBegin(GL_POINTS);
  for (i=0;i<p.size();i++) {
    if (p[i].e.size()==0) glVertex3fv(p[i].x);
  }
  glEnd();
}

// Identifica los pares de elementos vecinos y las aristas de frontera
// Actualiza el atributo v (lista de vecinos) de cada elemento y el atributo es_frontera de cada nodo
void Malla::MakeVecinos() {
  unsigned int i,j,k,ie,iev;
  int ix; // puede dar -1
  // inicializa
  for (i=0;i<p.size();i++) p[i].es_frontera=false; // le dice a todos los nodos que no son frontera
  for (i=0;i<e.size();i++) e[i].v[0]=e[i].v[1]=e[i].v[2]=e[i].v[3]=-1; // le dice a todos los elementos que no tienen vecinos
  // identificacion de vecinos
  for (ie=0;ie<e.size();ie++) { // por cada elemento
    for (j=0;j<e[ie].nv;j++) { // por cada arista
      if (e[ie].v[j]>=0) continue; // ya se hizo
      int in0=e[ie][j], in1=e[ie][j+1]; // 1er y 2do nodo de la arista
      for (k=0;k<p[in0].e.size();k++) { // recorro los elementos del primer nodo
        iev=p[in0].e[k];
        if (iev==ie) continue; // es este mismo
        // se fija si tiene a in1 (el 2do nodo)
        ix=e[iev].Indice(in1);
        if (ix<0) continue; 
        // tiene al 2do
        e[ie].v[j]=p[in0].e[k]; // ese es el vecino
        e[iev].v[ix]=ie;
        break; // solo dos posibles vecinos para una arista
      }
      if (k==p[in0].e.size()) // no encontro vecino ==> frontera
        p[in0].es_frontera=p[in1].es_frontera=true;
    }
  }
}

void Malla::Subdivide() {
	cout<<"funcion inutilizada"<<endl;
}
/// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Definimos el nado del pez <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
float parametrizeZ(float z) {
	// valores z maximo y minimo de la malla original
	float valMax = 3.10011;
	float valMin = -4.75;
	// para llevar un numero a el initervalo [0,1] calculamos:
	//   val = (x-min)/l 
	// donde l=max-min
	float val01 = 1-((z-valMin)/(valMax-valMin)); // aca le restamos uno porque el pez va hacia arriva
	// queremos que la senoidal este entre [0;3pi/4] (valor puesto a ojo)
	return val01*((3.f/2.f)*M_PI);
}

void Malla::MoveMalla() {
	int np = p.size();
	for (int i=0; i<np; i++){
		// cargamos los puntos originales
		p[i].x[0] = pIdentity[i].x[0];
		p[i].x[1] = pIdentity[i].x[1];
		p[i].x[2] = pIdentity[i].x[2];
	}
	
	for (int i=0; i<np; i++){
		// calculamos el t de la funcion seno
		float t = parametrizeZ(p[i].x[2]);
		float dir = amplitud*sin(t);
		p[i].x[0] += dir;
		if (p[i].x[2]< -1.5){ //para que la cola se mueva un 20% mas rapido
			float t = parametrizeZ(p[i].x[2]);
			float dir = amplitud*sin(1.2*t);
			p[i].x[0] += dir;	
		}
		
	}
	
	// modificamos la amplitud del seno
	amplitud += paso;
	if (abs(amplitud)>=amplitudMax) paso *=-1;  
	MakeNormales();
}
