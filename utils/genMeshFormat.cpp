#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
using namespace std;

/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Cargar malla <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
struct Vertex{
	float x,y,z;
	void showAsString () {
		cout<<x<<" "<<y<<" "<<z<<endl;
	}
};

// datos que necesitamos para armar el .dat
vector<Vertex> vertices;
vector<vector<int>> elementos;

// datos temporales que necesito
vector<Vertex> temp_vertices;

void LoadObjWithTexture (const char* filename) {
	FILE *file = fopen(filename, "r");
	if( file == NULL ){
		printf("Impossible to open the file !\n");
		return;
	}
	// recorremos el archivo
	while( 1 ){
		char lineHeader[128];
		// Lee la primera palabra de la l�nea
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File, es decir, el final del archivo. Se finaliza el ciclo.

		// else : analizar el lineHeader
		if ( strcmp( lineHeader, "v" ) == 0 ){ // que sea un nodo
			Vertex vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){ // relaciones entre nodo, normal
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			
			/// si se que hay coordenadas de textura
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2],
				&vertexIndex[3], &uvIndex[3], &normalIndex[3] );
			
			cout<<"matches"<<matches<<endl;
			
			vector<int> elemento;
			if (matches == 12){ // trapesoide
				elemento.push_back(4);
				for (int i=0; i<4; i++) elemento.push_back(vertexIndex[i]-1);
				elementos.push_back(elemento);
			}
			else if (matches == 9) { // triangulo
				elemento.push_back(3);
				for (int i=0; i<3; i++) elemento.push_back(vertexIndex[i]-1);
				elementos.push_back(elemento);
			}
			else { // forma desconocida
				printf("File can't be read by our simple parser :( Try exporting with other options\n");
				return;
			}
		}
	}
}
void LoadObjWithoutTexture(const char* filename) {
	FILE *file = fopen(filename, "r");
	if( file == NULL ){
		printf("Impossible to open the file !\n");
		return;
	}
	// recorremos el archivo
	while( 1 ){
		char lineHeader[128];
		// Lee la primera palabra de la l�nea
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File, es decir, el final del archivo. Se finaliza el ciclo.

		// else : analizar el lineHeader
		if ( strcmp( lineHeader, "v" ) == 0 ){ // que sea un nodo
			Vertex vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){ // relaciones entre nodo, normal
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[4], normalIndex[4];
			
			/// si solo hay vertices y normales
			int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n",
				&vertexIndex[0], &normalIndex[0],
				&vertexIndex[1], &normalIndex[1],
				&vertexIndex[2], &normalIndex[2],
				&vertexIndex[3], &normalIndex[3] );

			vector<int> elemento;
			if (matches == 8){ // trapesoide
				elemento.push_back(4);
				for (int i=0; i<4; i++) elemento.push_back(vertexIndex[i]-1);
				elementos.push_back(elemento);
			}
			else if (matches == 6) { // triangulo
				elemento.push_back(3);
				for (int i=0; i<3; i++) elemento.push_back(vertexIndex[i]-1);
				elementos.push_back(elemento);
			}
			else { // forma desconocida
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return;
			}
		}
	}
}

void showVertex () {
	cout<<vertices.size()<<endl;
	for (Vertex vec: vertices)
		vec.showAsString();
	cout<<elementos.size()<<endl;
	for (vector<int> vec : elementos){
		for (int i: vec)
			cout<<i<<" ";
		cout<<endl;
	}
}

/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> funcion Main <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int main (int argc, char **argv) {

//	LoadObjWithTexture("cubo.obj");
  LoadObjWithoutTexture("Fish3.obj");
	showVertex();
	
	return 0;
}
