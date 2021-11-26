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
  unsigned int i;
  if (relleno) {
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
  
  /// @@@@@: Implementar Catmull-Clark... lineamientos:
  ///  0) Los nodos originales van de 0 a Nn-1, los elementos orignales de 0 a Ne-1
  ///  1) Por cada elemento, agregar el centroide (nuevos nodos: Nn a Nn+Ne-1)
  
  int Ne = e.size(), // numero de elementos -> CARAS
    Nn = p.size(); // numero de nodos
  
  for (Elemento elemento:e){ // recorremos los elementos
    Nodo centroide; // tomamos el primer punto del elemento
    for (int nodo=0; nodo<elemento.nv; nodo++){
      centroide += p[elemento.n[nodo]]; // y sumamos el resto
    }
    centroide/=elemento.nv; // hacemos el promedio
    p.push_back(centroide); // agregamos al vector de nodos;
    /* 
    NOTA: 
    esto implica que el centroide del elemento e esta en su indice mas el Ne original,
    que usaremos mas adelante
    */
  }
  
  ///  2) Por cada arista de cada cara, agregar un pto en el medio que es
  ///      promedio de los vertices de la arista y los centroides de las caras 
  ///      adyacentes. Aca hay que usar los elementos vecinos.
  ///      En los bordes, cuando no hay vecinos, es simplemente el promedio de los 
  ///      vertices de la arista.
  ///      Hay que evitar procesar dos veces la misma arista (como?)
  ///      Mas adelante vamos a necesitar encontrar los puntos que agregamos para 
  ///      cada arista, y ya que no se puede relacionar el indice de los nodos nuevos
  ///      (la cantidad de aristas por nodo es variable), se sugiere usar Mapa como 
  ///      estructura auxiliar
  Mapa mapaAristas;
  int ie = 0;
  for (Elemento elem:e){    
    for (int nArista = 0; nArista< elem.nv; nArista++) {
      
      Arista arista(elem.n[nArista],elem.n[(nArista+1)%elem.nv]); // creo una nueva arista
      
      if (mapaAristas.find(arista) == mapaAristas.end()){ // si la arista no esta en el mapa
        Nodo centroide = (p[arista.n[0]]+p[arista.n[1]])/2; // promedio de los puntos;
        
        int n_vecinos = elem.v[nArista];
        if (n_vecinos != -1) { // si tiene vecinos
          // p[elem.v[nArista]+Nn] busca un centroide del elemento vecino
          // p[ie+Nn] busca otro centroide del este elemento
          // luego promedia el punto medio de la arista con el de los centroides
          centroide = (centroide+(p[elem.v[nArista]+Nn]+p[ie+Nn])/2)/2;
        }
        p.push_back(centroide);
        mapaAristas[arista] = p.size()-1;
      } // fin end arista no calculada      
    } // fin del recorrido de las arisrtas
    ie++;
  } // fin del for 
  
  ///  3) Armar los elementos nuevos
  ///      Los quads se dividen en 4, (uno reemplaza al original, los otros 3 se agregan)
  ///      Los triangulos se dividen en 3, (uno reemplaza al original, los otros 2 se agregan)
  ///      Para encontrar los nodos de las aristas usar el mapa que armaron en el paso 2
  ie = 0;
  for (Elemento elem:e) {
    
    vector<int> punto_medio;
    
    for (int i=0; i<elem.nv; i++) {
      Arista arista(elem.n[i],elem.n[(i+1)%elem.nv]);
      punto_medio.push_back( mapaAristas[arista] );
    }
    
    for (int i=0; i<elem.nv-2; i++){
      // ie+Nn -> centroide de la cara ie
      // punto_medio[i] -> el punto medio de la arista i
      // elem.n[i+1] -> nodo i+1
      // punto_medio[i+1] -> punto medio de la siguiente arista      
      AgregarElemento(ie+Nn,punto_medio[i],elem.n[i+1],punto_medio[i+1]);
    }
    AgregarElemento(ie+Nn,punto_medio[elem.nv-1],elem.n[0],punto_medio[0]);//medio
    ReemplazarElemento(ie,ie+Nn,punto_medio[elem.nv-2],elem.n[elem.nv-1],punto_medio[elem.nv-1]); //centroide   
    
    ie++;
  }
  
  MakeVecinos(); // recalcula los vecinos
  
  ///  4) Calcular las nuevas posiciones de los nodos originales
  ///      Para nodos interiores: (4r-f+(n-3)p)/n
  ///         f=promedio de nodos interiores de las caras (los agregados en el paso 1)
  ///         r=promedio de los pts medios de las aristas (los agregados en el paso 2)
  ///         p=posicion del nodo original
  ///         n=cantidad de elementos para ese nodo
  ///      Para nodos del borde: (r+p)/2
  ///         r=promedio de los dos pts medios de las aristas
  ///         p=posicion del nodo original
  ///      Ojo: en el paso 3 cambio toda la malla, analizar donde quedan en los nuevos 
  ///      elementos (¿de que tipo son?) los nodos de las caras y los de las aristas 
  ///      que se agregaron antes.
  
  // no = nodos originales
  for (int no=0; no<Nn; no++) {//Recorro los nodos
    //calculo de f,promedio de los centroides
    Punto f,r,p1;
    int cant=0;
    
    float n = p[no].e.size();
    
    if(p[no].es_frontera==true){//si el punto es frontera
      
      for (int i=0; i<n; i++){
        Nodo pto_1 = p[e[p[no].e[i]].n[1]];
        Nodo pto_2 = p[e[p[no].e[i]].n[3]];
        
        if (pto_1.es_frontera) r+=pto_1;
        if (pto_2.es_frontera) r+=pto_2;
      }
      r=r/2;
      p1=(r+p[no])/2; 
    }else{//si el punto no es frontera
      for (int i=0;i<n;i++){
        f += p[e[p[no].e[i]].n[0]];
        r += p[e[p[no].e[i]].n[1]];
      }
      f = f/n;
      r = r/n;
      p1 = (r*4-f+p[no]*(n-3))/n;
    }  
    p[no] = p1;
  }
  
  MakeNormales(); // recalculo las normales
  
  /// tips:
  ///   no es necesario cambiar ni agregar nada fuera de este método, (con Mapa como 
  ///     estructura auxiliar alcanza)
  ///   sugerencia: probar primero usando el cubo (es cerrado y solo tiene quads)
  ///               despues usando la piramide (tambien cerrada, y solo triangulos)
  ///               despues el ejemplo plano (para ver que pasa en los bordes)
  ///               finalmente el mono (tiene mezcla y elementos sin vecinos)
  ///   repaso de como usar un mapa:
  ///     para asociar un indice (i) de nodo a una arista (n1-n2): elmapa[Arista(n1,n2)]=i;
  ///     para saber si hay un indice asociado a una arista:  ¿elmapa.find(Arista(n1,n2))!=elmapa.end()?
  ///     para recuperar el indice (en j) asociado a una arista: int j=elmapa[Arista(n1,n2)];
  
}
