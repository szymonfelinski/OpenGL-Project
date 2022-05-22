#ifndef globject_H
#define globject_H

#include "common.h"
#include "texture.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------
// 								CLASS INTERFACE
//--------------------------------------------------------------------------------------------
class glObject
{

public:

	struct Material // material 
	{

		char Name[1024];
		glm::vec3 Kd; //kolor obiektu
		//glm::vec3 Kd; //kolor diffuse
		//glm::vec3 Ks; //kolor specular
		float d; //przezroczystość (odwrotność Tr)

	};
	//-------------------------------------------------------------
	struct Vertex // wierzcholek
	{
		float x;
		float y;
		float z;
	};
	//-------------------------------------------------------------
	struct Normal // normalna
	{
		float x;
		float y;
		float z;
	};
	//-------------------------------------------------------------
	struct Textu
	{
		float u;
		float v;
	};
public:

	glObject(); //  domyslny konstruktor
	~glObject(); // domyslny destruktor 

	// laduje geometrie z pliku obj
	void LoadFromFile(char *OBJName, char *MTLName, char* path);
	void LoadMaterial(char *MTLName, Material *M);

	void CleanUp();
	void SetColor(float r, float g, float b); // ustawia biezacy kolor dla grupy wierzcholkow
	void SetColor(glm::vec3 colour);

	void Reload(); // laduje ponownie z pliku obj 

	void SetNormal(glm::vec3 *N); // ustawia aktualna normalna 
	void SetNormal(float _nx, float _ny, float _nz); // ustawia aktualna normalna 
	void BeginObject(GLenum P, GLuint TextureId = 0); // rozpoczyna generowanie nowego ksztaltu 

	void EndObject(); // tworzy obiekt po wypelnieniu wierzcholkami 

	void AddVertex(float x, float y, float z, float u = 0.0, float v = 0.0); // dodaje wierzcholek do listy ze wsp. tekstury 
	void AddVertex(glm::vec3 *v, glm::vec2* tex); // dodaje wierzcholek do listy ze wsp. tekstury 
	void AddVertex(glm::vec3 *v); // dodaje wierzcholek do listy bez wsp. tekstury 

	void Draw(); // rysuje obiekt na scenie z u¿yciem zadanego prymitywu

	void Draw(int cVAO); // rysuje obiekt na scenie z u¿yciem zadanej tablicy VAO

public:

	char OBJFileName[1024];
	char MTLFileName[1024];
	char TextureFileName[1024];

	//-------------------------------------------------------------

	//-------------------------------------------------------------
	struct Face // faseta 
	{
		int Fv1;
		int Fv2;
		int Fv3;
		int Fn1;
		int Fn2;
		int Fn3;
		int Ft1;
		int Ft2;
		int Ft3;
	};

#define MAX_VAO 128 // maksymalna liczba tablic VAO w obiekcie 
#define ATTRIB_PER_VERTEX 4 // liczba atrybutow skojarzonych z wierzcholkiem 

	int lVAO; // liczba zdefiniowanych tablic VAO
	float alpha; //przezroczystość


	GLuint VAO[MAX_VAO]; // id tablic wierzcholkow obiektow 
	GLuint VBO[ATTRIB_PER_VERTEX*MAX_VAO]; // id buforow wierzcholkow

	GLenum Primitives[MAX_VAO]; // tablica rodzajow prymitywow skojarzonych z VAO 
	GLuint Textures[MAX_VAO]; // tablica identyfikatorow tekstur skojarzonych z VAO

	int lCoords[MAX_VAO]; // liczba wspolrzednych skojarzona z kazda VAO

	std::vector<glm::vec3> Coords; // wektor ze wspolrzednymi
	std::vector<glm::vec3> Cols; // wektor z kolorami 
	std::vector<glm::vec3> Normals; // wektor z normalnymi
	std::vector<glm::vec3> TexCoords; // tablica ze wspolrzednymi tekstur

	// ustawienia aktualnego koloru 
	glm::vec3 Colors;

	// ustawienia aktualnej normalnej 
	glm::vec3 CurNormal;

	//kolizje
	glm::vec3 corner1;
	glm::vec3 corner2;

	//pozycja na scenie
	glm::vec3 Position;
	glm::vec3 Scale;

	// komunikaty diagnostyczne 
	char _msg[1024];

};

#endif

