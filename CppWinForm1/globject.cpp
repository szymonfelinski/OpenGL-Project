#include "globject.h"

int CalcNormal(float A[], float B[], float C[], float *N)
{
	const int x = 0;
	const int y = 1;
	const int z = 2;
	float U[3];
	float V[3];
	// oblicz wspolrzedne wektorow U oraz V
	U[x] = A[x] - B[x];
	U[y] = A[y] - B[y];
	U[z] = A[z] - B[z];
	V[x] = B[x] - C[x];
	V[y] = B[y] - C[y];
	V[z] = B[z] - C[z];
	// wyznacz wspolrzedne normalnej
	N[x] = U[y] * V[z] - U[z] * V[y];
	N[y] = U[z] * V[x] - U[x] * V[z];
	N[z] = U[x] * V[y] - U[y] * V[x];
	return 1;
}

int Normalize(float *N)
{
	const int x = 0;
	const int y = 1;
	const int z = 2;
	// oblicz dlugosc wektora
	float L = (float)sqrt(N[x] * N[x] + N[y] * N[y] + N[z] * N[z]);
	if (L < 0.01) L = 0.01f;
	// wyznacz wspolrzedne normalnej
	N[x] /= L;
	N[y] /= L;
	N[z] /= L;
	return 1;
}
//--------------------------------------------------------------------------------------------
// domyslny konstruktor 
glObject::glObject()
{
	CurNormal = glm::vec3(1.0f, 0.0f, 0.0f);
	Colors = glm::vec3(0.2f, 0.2f, 0.2f);
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	alpha = 1.0f;
	lVAO = 0;
	OBJFileName[0] = 0x00;
	TextureFileName[0] = 0x00;
	MTLFileName[0] = 0x00;
}
//--------------------------------------------------------------------------------------------
// domyslny destruktor 
glObject::~glObject()
{
	Coords.clear();
	Cols.clear();
	Normals.clear();
	TexCoords.clear();
}
//--------------------------------------------------------------------------------------------
// ustawia aktualny kolor rysowania prymitywu 
void glObject::SetColor(glm::vec3 colour)
{
	SetColor(colour.x, colour.y, colour.z);
}
void glObject::SetColor(float r, float g, float b)
{
	Colors = glm::vec3(r, g, b);
}
//--------------------------------------------------------------------------------------------
 // ustawia aktualna normalna 
void glObject::SetNormal(glm::vec3 *N)
{
	CurNormal = *N;
}
//--------------------------------------------------------------------------------------------
// ustawia aktualna normalna 
void glObject::SetNormal(float _nx, float _ny, float _nz)
{
	SetNormal(&glm::vec3(_nx, _ny, _nz));
}
//--------------------------------------------------------------------------------------------
// usuwa alokowane atrybuty
void glObject::CleanUp()
{
	lVAO = 0;
}
//--------------------------------------------------------------------------------------------
// laduje ponownie obiekt z pliku obj 
void glObject::Reload()
{
	CleanUp();
	LoadFromFile(OBJFileName, MTLFileName, TextureFileName);
}
//--------------------------------------------------------------------------------------------
// laduje definicje materialu z pliku 
void glObject::LoadMaterial(char *MTLName, Material *M)
{
	FILE *fil = NULL;
	fil = fopen(MTLName, "r");
	char line[1024];

	int found = 0;

	while (!feof(fil))
	{
		fgets(line, 1024, fil);

		if ((line[0] == 'n') && (line[1] == 'e') && (line[2] == 'w'))
		{
			char tmp[100];
			char tmp2[100];
			sscanf(line, "%s %s", tmp, tmp2);
			if (strstr(tmp2, M->Name))
				found = 1;
			else
				found = 0;
		} // condition for newmtl found 

		if (found == 0) continue;

		if ((line[0] == 'K') && (line[1] == 'a')) sscanf(line, "%*s %f %f %f", &M->Kd.x, &M->Kd.y, &M->Kd.z);
		if ((line[0] == 'd') && (line[1] == ' ')) sscanf(line, "%*s %f", &alpha);
		if ((line[0] == 'T') && (line[1] == 'r'))
		{
			sscanf(line, "%*s %f", &alpha);
			alpha = 1 / alpha;
		}

	}
	//PrintLog("material found");
	if (fil) fclose(fil);
}
//--------------------------------------------------------------------------------------------
// laduje geometrie obiektu z pliku obj
void glObject::LoadFromFile(char *OBJName, char *MTLName, char* path)
{
	strcpy(OBJFileName, OBJName);
	strcpy(MTLFileName, MTLName);
	
	// definicja materialu dla obiektu
	Material MAT;
	Face faseta;

	if (path != 0) {
		glTexture* Tex = new glTexture(path);
		BeginObject(GL_TRIANGLES, Tex->Bind());
		strcpy(TextureFileName, path);
	}
	else {
		BeginObject(GL_TRIANGLES);
	}
	
	//MODYFIKACJA-------------------------------------------------------------------

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textures;
	std::vector<GLushort> elements;

	int lV = 0; // liczba wierzcholkow
	int lF = 0; // liczba faset
	int lN = 0; // liczba normalnych 

	float tmpx = 0.0f;
	float tmpy = 0.0f;
	float tmpz = 0.0f;
	glm::vec3 v;
	glm::vec3 n;
	glm::vec2 t;

	FILE* fil = NULL; //otwieranie pliku
	fil = fopen(OBJName, "r");
	if (fil == NULL)
	{
		sprintf(_msg, "Nie udalo siê otworzyc pliku %s\n", OBJName);
		//ThrowException(_msg);
		ThrowException("Nie udalo siê otworzyc pliku");
	}

	char line[1024]; // pojedyncza linia z pliku
	char tmp[200];

	while (fgets(line, 1024, fil))
	{
		if (line[0] == 'v' && line[1] == ' ')
		{
			sscanf(line, "%*s %f %f %f", &tmpx, &tmpy, &tmpz);
			v.x = tmpx;
			v.y = tmpy;
			v.z = tmpz;
			vertices.push_back(v);
			if (vertices.size() >= 1)
			{
				if (v.x < corner1.x) corner1.x = v.x;
				if (v.y < corner1.y) corner1.y = v.y;
				if (v.z < corner1.z) corner1.z = v.z;
				if (v.x > corner2.x) corner2.x = v.x;
				if (v.y > corner2.y) corner2.y = v.y;
				if (v.z > corner2.z) corner2.z = v.z;
			}
			else
			{
				corner1.x = v.x;
				corner1.y = v.y;
				corner1.z = v.z;
				corner2.x = v.x;
				corner2.y = v.y;
				corner2.z = v.z;
			}
		}
		// new normal
		else if ((line[0] == 'v') && (line[1] == 'n') && (line[2] == ' '))
		{
			sscanf(line, "%*s %f %f %f", &tmpx, &tmpy, &tmpz);
			n.x = tmpx;
			n.y = tmpy;
			n.z = tmpz;
			normals.push_back(n);
		}
		// new texture
		else if ((line[0] == 'v') && (line[1] == 't') && (line[2] == ' '))
		{
			sscanf(line, "%*s %f %f", &t.x, &t.y);
			textures.push_back(t);
		}
		//new face
		else if ((line[0] == 'f') && (line[1] == ' ') && textures.size()>0)
		{
			sscanf(line, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &faseta.Fv1, &faseta.Ft1, &faseta.Fn1, &faseta.Fv2, &faseta.Ft2, &faseta.Fn2, &faseta.Fv3, &faseta.Ft3, &faseta.Fn3);
			SetNormal(&normals[faseta.Fn1 - 1]);
			AddVertex(&vertices[faseta.Fv1 - 1], &textures[faseta.Ft1 - 1]);
			SetNormal(&normals[faseta.Fn2 - 1]);
			AddVertex(&vertices[faseta.Fv2 - 1], &textures[faseta.Ft2 - 1]);
			SetNormal(&normals[faseta.Fn3 - 1]);
			AddVertex(&vertices[faseta.Fv3 - 1], &textures[faseta.Ft3 - 1]);
		}
		else if ((line[0] == 'f') && (line[1] == ' ') && textures.size() == 0)
		{
			sscanf(line, "%*s %d//%d %d//%d %d//%d", &faseta.Fv1, &faseta.Fn1, &faseta.Fv2, &faseta.Fn2, &faseta.Fv3, &faseta.Fn3);
			SetNormal(&normals[faseta.Fn1 - 1]);
			AddVertex(&vertices[faseta.Fv1 - 1]);
			SetNormal(&normals[faseta.Fn2 - 1]);
			AddVertex(&vertices[faseta.Fv2 - 1]);
			SetNormal(&normals[faseta.Fn3 - 1]);
			AddVertex(&vertices[faseta.Fv3 - 1]);
		}
		else if (line[0] == 'u' && line[1] == 's' && line[2] == 'e')
		{
			sscanf(line, "%*s %s", &tmp);
			strcpy(MAT.Name, tmp);
			LoadMaterial(MTLName, &MAT);
			SetColor(MAT.Kd);
		}
		else
		{
			//nic nie robi
		}
	}
	//LoadMaterial(MTLName, MAT);
	//SetColor(0.8f, 0.8f, 0.8f);
	
	if (fil) fclose(fil);

	EndObject();


}

//--------------------------------------------------------------------------------------------
// rozpoczyna tworzenie tablicy VAO dla danego prymitywu
void glObject::BeginObject(GLenum P, GLuint TextureId)
{

	lVAO++;
	// przypisz rodzaj prymitywu do narysowania VAO
	Primitives[lVAO - 1] = P;

	// przypisz Id tekstury do narysowania VAO
	Textures[lVAO - 1] = TextureId;

	// wyzeruj licznik wspolrzednych 
	lCoords[lVAO - 1] = 0;
	//Coords = (float *)malloc(sizeof(float));
	//Cols = (float *)malloc(sizeof(float));
	//Normals = (float *)malloc(sizeof(float));

	if (lVAO > MAX_VAO) ThrowException("Przekroczono maksymalna liczbe VAO w glObject");

	GLuint VAO_id[1];
	// przygotuj tablice VAO
	glGenVertexArrays(1, VAO_id);
	VAO[lVAO - 1] = VAO_id[0];

	glBindVertexArray(VAO[lVAO - 1]);

	GLuint VBO_id[4];
	// przygotuj bufory VBO
	glGenBuffers(4, VBO_id);

	VBO[4 * lVAO - 4] = VBO_id[0];
	VBO[4 * lVAO - 3] = VBO_id[1];
	VBO[4 * lVAO - 2] = VBO_id[2];
	VBO[4 * lVAO - 1] = VBO_id[3];

}
//--------------------------------------------------------------------------------------------
// dodaje wierzcholek do listy ze wsp. tekstury 
// wrapper dla argumentu
void glObject::AddVertex(glm::vec3* v)
{
	AddVertex(v, &glm::vec2(0.0f, 0.0f));
}
void glObject::AddVertex(float x, float y, float z, float u, float v)

{
	AddVertex(&glm::vec3(x, y, z), &glm::vec2(u, v));
}
//--------------------------------------------------------------------------------------------
// dodaje wierzcholek do listy ze wsp. tekstury 
void glObject::AddVertex(glm::vec3* v, glm::vec2* tex)
{
	lCoords[lVAO - 1] += 3;
	Coords.push_back(*v);
	Cols.push_back(Colors);
	Normals.push_back(CurNormal);
	TexCoords.push_back(glm::vec3(*tex, 0.0f));
}

//--------------------------------------------------------------------------------------------
void glObject::EndObject()
{
	// podlacz pierwszy obiekt z VAOs
	glBindVertexArray(VAO[lVAO - 1]);
	// podlacz pierwszy bufor VBOs
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4 * lVAO - 4]);
	// wypelnij bufor wspolrzednymi wierzcholka 

	glBufferData(GL_ARRAY_BUFFER, lCoords[lVAO - 1] * sizeof(float), &Coords.front(), GL_STATIC_DRAW);
	// wybierz atrybut indeksie 0 (wskazany w shaderze)
	glEnableVertexAttribArray(0);
	// powiaz dane z bufora ze wskazanym atrybutem
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// podlacz drugi bufor VBOs
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4 * lVAO - 3]);
	// wypelnij bufor kolorami wierzcholka
	glBufferData(GL_ARRAY_BUFFER, lCoords[lVAO - 1] * sizeof(float), &Cols.front(), GL_STATIC_DRAW);
	// wybierz atrybut indeksie 1 (wskazany w shaderze)
	glEnableVertexAttribArray(1);
	// powiaz dane z bufora ze wskazanym atrybutem
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// podlacz trzeci bufor VBOs
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4 * lVAO - 2]);
	// wypelnij bufor kolorami wierzcholka
	glBufferData(GL_ARRAY_BUFFER, lCoords[lVAO - 1] * sizeof(float), &Normals.front(), GL_STATIC_DRAW);
	// wybierz atrybut indeksie 2 (wskazany w shaderze)
	glEnableVertexAttribArray(2);
	// powiaz dane z bufora ze wskazanym atrybutem
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// podlacz czwarty bufor VBOs
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4 * lVAO - 1]);
	// wypelnij bufor kolorami wierzcholka
	glBufferData(GL_ARRAY_BUFFER, lCoords[lVAO - 1] * sizeof(float), &TexCoords.front(), GL_STATIC_DRAW);
	// wybierz atrybut indeksie 3 (wskazany w shaderze)
	glEnableVertexAttribArray(3);
	// powiaz dane z bufora ze wskazanym atrybutem
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}
//--------------------------------------------------------------------------------------------
void glObject::Draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < lVAO; i++)
	{
		if (Textures[i] == 0)
		{
			glDisable(GL_TEXTURE_2D);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, Textures[i]);
			glEnable(GL_TEXTURE_2D);
		}
		glBindVertexArray(VAO[i]);
		glDrawArrays(Primitives[i], 0, lCoords[i] / 3);
		glBindVertexArray(0);
	}
	glDisable(GL_BLEND);
}
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// the end