//--------------------------------------------------------------------------------------------
//
// File: 	scene.cpp
// Author:	P. Katarzynski (CCE)
//
// Description: Implementacja klasy sceny OpenGL
//
//--------------------------------------------------------------------------------------------
//									ZALEZNOSI 
//--------------------------------------------------------------------------------------------
#include "scene.h"

//--------------------------------------------------------------------------------------------
// zglasza wyjatek z komunikatem do debuggowania 

//--------------------------------------------------------------------------------------------
Scene::Scene(int new_width,int new_height)
{	
	width = new_width;
	height = new_height;	
	rot_x = 0.0f;
	rot_y = 0.0f;
	rot_z = 0.0f;
	Axes = NULL;	
	Obj = NULL;
	House = NULL;
	LightAmbient = 0.1f;	
	err = 1;	
	przesuniecie = 0.0f;
	frame = 120;

	FogDensity = 0.05f;
	Alpha = 1.0f;

	//z lab 3
	R = 5.0f; //odleglosc od punktu
	alfa = PI / 2.0f; // poczatkowy kat obrotu
	y_angle = PI / 2.0f; // poczatkowy kat obrotu
	height_beta = PI / 2.0f;

	xe = 0.0f; //poczatkowe wsp. kamery
	ye = ye_start;
	ze = 0.0f;
	yt = ye_start;

	xt = xe + R * cos(alfa); //wsp. celu kamery
	zt = ze + R * sin(alfa);

	d = 5.0f; //dlugosc przesuniecia
	dx = 0.0f; //ostatnie x
	dz = 0.0f; //ostatnie z

	rise = false;
	crouch_handle = false;
}
//--------------------------------------------------------------------------------------------
// Domyslny destruktor 
Scene::~Scene()
{
	// usun program przetwarzania 
	if (glIsProgram(program)) glDeleteProgram(program);
	if (Axes) delete Axes;	
	if (Obj) delete Obj;
	if (House) delete House;
}
//--------------------------------------------------------------------------------------------
// przygotowuje programy cienionwania 
void Scene::PreparePrograms()
{
	err = 0; // ustaw flage bledu
	program  = glCreateProgram();
	if (!glIsProgram(program)) {err = 1; ThrowException("Nie udalo sie utworzyc programu");}
	
	vertex_shader = LoadShader(GL_VERTEX_SHADER,"vs.glsl");
	glAttachShader(program,vertex_shader);

	fragment_shader = LoadShader(GL_FRAGMENT_SHADER,"fs.glsl");
	glAttachShader(program,fragment_shader);
	
	// linkowanie programu 
	glLinkProgram(program);

	GLint link_status; 
    glGetProgramiv( program, GL_LINK_STATUS, &link_status); 
    if( link_status == GL_FALSE ) 
    { 
        // pobranie i wyœwietlenie komunikatu b³êdu 
        GLint logLength; 
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength ); 
        char *log = new char[logLength]; 
        glGetProgramInfoLog( program, logLength, NULL, log ); 
        PrintLog(log);
        delete[] log; 
		err = 1;
		ThrowException("Blad linkowania programu");
    }
	else
		PrintLog("Program zlinkowany");

	// walidowanie programu 
	glValidateProgram(program); 
	GLint validate_status;
    // sprawdzenie poprawnoœci walidacji obiektu programu 
    glGetProgramiv( program, GL_VALIDATE_STATUS, &validate_status ); 
    if( validate_status == GL_FALSE ) 
    { 
        // pobranie i wyświetlenie komunikatu błędu 
        GLint logLength; 
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength ); 
        char *log = new char[logLength]; 
        glGetProgramInfoLog( program, logLength, NULL, log ); 
        PrintLog(log);
        delete[] log; 
		err = 1;
        ThrowException("Blad walidacji programu");
    } 
	else
		PrintLog("Program prawidlowy");
	
	srand(69);

	glUseProgram(program);
}
//--------------------------------------------------------------------------------------------
void Scene::PrepareObjects()
{  
	char* notex = "textures\\white.bmp";
	Projectile = new glObject();
	Projectile->LoadFromFile("models\\Ball.obj", "models\\Ball.mtl", "textures\\Ball.bmp");

	Obj = new glObject();
	Obj->Position = glm::vec3(-5.0f, -0.5f, 0.0f);
	Obj->LoadFromFile("models\\LowPolyBall.obj", "models\\LowPolyBall.mtl", notex);

	Suzanne = new glObject();
	Suzanne->Position = glm::vec3(5.0f, 1.0f, 5.0f);
	Suzanne->LoadFromFile("models\\Suzanne.obj", "models\\Suzanne.mtl", "textures\\triangles.bmp");
	
	ConeTorus = new glObject();
	ConeTorus->Position = glm::vec3(-5.0f, 1.0f, 5.0f);
	ConeTorus->LoadFromFile("models\\ConeTorus.obj", "models\\ConeTorus.mtl","textures\\moon.bmp");

	House = new glObject();
	House->Position = glm::vec3(5.0f, 0.0f, -10.0f);
	House->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
	House->LoadFromFile("models\\cottage.obj", "models\\cottage.mtl", "textures\\cottage.bmp");

	Sculpt = new glObject();
	Sculpt->Position = glm::vec3(5.0f, 1.0f, 0.0f);
	Sculpt->LoadFromFile("models\\sculpture.obj", "models\\sculpture.mtl", "textures\\kora.bmp");
	
	Panther = new glObject();
	Panther->Position = glm::vec3(10.0f, 0.0f, 3.0f);
	Panther->Scale = glm::vec3(0.1f, 0.1f, 0.1f);
	Panther->LoadFromFile("models\\panther.obj", "models\\panther.mtl", notex);

	Snowman = new glObject();
	Snowman->Position = glm::vec3(9.0f, 1.0f, -16.0f);
	Snowman->Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	Snowman->LoadFromFile("models\\balwan.obj", "models\\balwan.mtl", "textures\\balwan.bmp");
	
	Cybertruck = new glObject();
	Cybertruck->Position = glm::vec3(19.0f, 1.0f, 6.0f);
	Cybertruck->Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	Cybertruck->LoadFromFile("models\\cybertruck.obj", "models\\cybertruck.mtl", "textures\\cybertruck.bmp");

	Axes = new glObject();	
	Axes->BeginObject(GL_LINES);
	Axes->SetColor(1.0f,0.0f,0.0f); // os X w kolorze czerwonym
	Axes->AddVertex(0.0f,0.0f,0.0f);
	Axes->AddVertex(10.0f,0.0f,0.0f);
	Axes->SetColor(0.0f,1.0f,0.0f); // os Y w kolorze zielonym 
	Axes->AddVertex(0.0f,0.0f,0.0f);
	Axes->AddVertex(0.0f,10.0f,0.0f);
	Axes->SetColor(0.0f,0.0f,1.0f); // os Z w kolorze niebieskim 
	Axes->AddVertex(0.0f,0.0f,0.0f);
	Axes->AddVertex(0.0f,0.0f,10.0f);
	Axes->EndObject();

	float mul = 10.0f;

	Cube = new glObject();
	Cube->SetColor(0.5f,0.0f,0.0f);
	Cube->BeginObject(GL_TRIANGLE_STRIP);
	Cube->AddVertex(mul,mul,mul);
	Cube->AddVertex(mul,-mul,mul);
	Cube->AddVertex(mul,mul,-mul);
	Cube->AddVertex(mul,-mul,-mul);
	Cube->EndObject();
	Cube->SetColor(0.0f,0.3f,0.0f);
	Cube->BeginObject(GL_TRIANGLE_STRIP);
	Cube->AddVertex(-mul,mul,mul);
	Cube->AddVertex(-mul,-mul,mul);
	Cube->AddVertex(-mul,mul,-mul);
	Cube->AddVertex(-mul,-mul,-mul);
	Cube->EndObject();

	Plane = new glObject();
	glTexture* Floor = new glTexture("textures\\grass.bmp");
	Plane->SetColor(1.0f, 1.0f, 1.0f);
	Plane->BeginObject(GL_QUADS, Floor->Bind());
	float size = 2.5f;
	int floor = 100;
	for (int x = -floor/2; x < floor/2; x++)
	{
		for (int z = -floor / 2; z < floor/2; z++)
		{
			Plane->AddVertex(x * size, 0.0f, z * size, 0.0f, 0.0f);
			Plane->AddVertex((x + 1) * size, 0.0f, z * size, 0.0f, 1.0f);
			Plane->AddVertex((x + 1) * size, 0.0f, (z - 1)* size, 1.0f, 1.0f);
			Plane->AddVertex(x * size, 0.0f, (z - 1) * size, 1.0f, 0.0f);
		}
	}

	Prn = new glPrinter("tahoma.ttf", 64);

	/*
	Plane->AddVertex(-1000.0f, 0.0f, -1000.0f, 0.0f, 0.0f);
	Plane->AddVertex(1000.0f, 0.0f, -1000.0f, 0.0f, 500.0f);
	Plane->AddVertex(1000.0f, 0.0f, 1000.0f, 500.0f, 500.0f);
	Plane->AddVertex(-1000.0f, 0.0f, 1000.0f, 500.0f, 0.0f);
	*/
	Plane->EndObject();
}
//--------------------------------------------------------------------------------------------
// Odpowiada za skalowanie sceny przy zmianach rozmiaru okna
void Scene::Resize(int new_width, int new_height)
{
	// przypisz nowe gabaryty do pol klasy 
	width = new_width;
	// uwzgledniaj obecnosc kontrolki wizualnej 
	height = new_height; 	
  	// rozszerz obszar renderowania do obszaru o wymiarach 'width' x 'height'
	glViewport(0, 0, width, height);	
	
	mProjection = glm::perspective(60.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);
}
//--------------------------------------------------------------------------------------------
void Scene::SaveAsBmp(char* filename)
{
	int _width = this->width;      // bitmap height 
	int _height = this->height;    // bitmap width
	while (_width * 3 % 4) _width--;  // adjust bmp width to meet 4B row padding rule
	int img_size = _width * _height * 3; // image size (each pixel is coded by 3Bytes)

	int storage_4B; // four byte storage used for saving integer bytes into bmp file 	
	FILE* fil; // prepare file handle
	fopen_s(&fil, filename, "wb"); // open the file in binary mode
	rewind(fil);
	//*********************** START OF THE HEADER **********************************
	// BM signature
	storage_4B = 0x00004d42; fwrite((char*)&storage_4B, 1, 2, fil);
	// file size
	storage_4B = 54 + img_size; fwrite((char*)&storage_4B, 1, 4, fil);
	// four empty bytes 
	storage_4B = 0x00000000;  fwrite((char*)&storage_4B, 1, 4, fil);
	// pixeltable address //Ah
	storage_4B = 0x00000036; fwrite((char*)&storage_4B, 1, 4, fil);
	//Eh
	storage_4B = 0x00000028; fwrite((char*)&storage_4B, 1, 4, fil);
	//12h
	storage_4B = _width; fwrite((char*)&storage_4B, 1, 4, fil);
	//16h
	storage_4B = _height; fwrite((char*)&storage_4B, 1, 4, fil);
	//1Ah
	storage_4B = 0x00000001; fwrite((char*)&storage_4B, 1, 2, fil);
	//1Ch
	storage_4B = 0x00000018; fwrite((char*)&storage_4B, 1, 2, fil);
	//1Eh
	storage_4B = 0x00000000; fwrite((char*)&storage_4B, 1, 4, fil);
	//22h
	storage_4B = img_size; fwrite((char*)&storage_4B, 1, 4, fil);
	//26h
	storage_4B = 0x00000b13; fwrite((char*)&storage_4B, 1, 4, fil);
	//2Ah
	storage_4B = 0x00000b13; fwrite((char*)&storage_4B, 1, 4, fil);
	//2Eh
	storage_4B = 0x00000000; fwrite((char*)&storage_4B, 1, 4, fil);
	//32h
	storage_4B = 0x00000000; fwrite((char*)&storage_4B, 1, 4, fil);

	//*********************** END OF THE HEADER **********************************

	//TODO: Place your code here 
	unsigned char* pixels; // room for pixeltable
	pixels = (unsigned char*)malloc(img_size * sizeof(unsigned char));
	// read pixels from colorbuffer
	glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	// store pixels in the file
	fwrite(pixels, 1, img_size, fil);
	fflush(fil); // clear file cache
	fclose(fil); // close the file
	free(pixels); // release memory


}
//--------------------------------------------------------------------------------------------
// laduje program shadera z zewnetrznego pliku 
GLuint Scene::LoadShader(GLenum type,const char *file_name)
{  
  // zmienna plikowa 
  FILE *fil = NULL; 
  // sproboj otworzyc plik 
  fil = fopen(file_name,"rb");
  // sprawdz, czy plik sie otworzyl  
  sprintf(_msg,"Nie mozna otworzyc %s",file_name);
  //if (fil == NULL)  ThrowException(_msg);
  if (fil == NULL)  ThrowException("Nie mozna otworzyc shader");

  // okresl rozmiar pliku
  fseek( fil, 0, SEEK_END );
  long int file_size = ftell(fil); 
  // przewin na poczatek 
  rewind(fil); 
  // utworzenie bufora na kod zrodlowy programu shadera
  GLchar *srcBuf = new GLchar[(file_size+1)*sizeof(GLchar)];

  // przeczytanie kodu shadera z pliku 
  fread(srcBuf,1,file_size,fil);

  // zamknij plik 
  fclose(fil);

  // tekst programu MUSI miec NULL na koncu
  srcBuf[file_size] = 0x00;

  // utworzenie obiektu shadera
  GLuint shader = glCreateShader(type);

  // przypisanie zrodla do shadera 
  glShaderSource(shader,1,const_cast<const GLchar**>(&srcBuf),NULL);

  // sprzatanie 
  delete[] srcBuf;

  // proba skompilowania programu shadera 
  glCompileShader(shader);

  // sprawdzenie czy sie udalo 
  GLint compile_status;
  glGetShaderiv(shader,GL_COMPILE_STATUS,&compile_status);

  if (compile_status != GL_TRUE) // nie udalo sie 
  {
	    GLint logLength; 
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength ); 
        char *log = new char[logLength]; 
        glGetShaderInfoLog( shader, logLength, NULL, log ); 
		sprintf(_msg,"Blad kompilacji pliku shadera %s",file_name);
		PrintLog(_msg);
        PrintLog(log);
		ThrowException("Blad kompilacji shadera");
	    delete []log;
  }
  else
  {
	  sprintf(_msg,"Plik shadera %s skompilowany",file_name);
	  //PrintLog(_msg);
	  PrintLog("Plik shadera %s skompilowany");
  }

  return shader; // zwroc id shadera 
}
//--------------------------------------------------------------------------------------------
// inicjuje proces renderowania OpenGL
void Scene::Init()
{
	// inicjalizacja modu³u glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		sprintf(_msg, "GLew error: %s\n", glewGetErrorString(err));	
		err = 1;
		ThrowException("GLew error: ");
	}
	
	// pobierz informacje o wersji openGL 
	sprintf(_msg,"OpenGL vendor: ");
	strcat(_msg,(const char*)glGetString( GL_VENDOR ));
	PrintLog(_msg);

	sprintf(_msg,"OpenGL renderer: ");
	strcat(_msg,(const char*)glGetString( GL_RENDERER));
	PrintLog(_msg);

	sprintf(_msg,"OpenGL version: ");
	strcat(_msg,(const char*)glGetString( GL_VERSION));
	PrintLog(_msg);

	//  ustaw kolor tla sceny (RGB Z=1.0)
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	
	// przygotuj programy shaderow
	PreparePrograms();
	
	if (err) return; 

	// przygotuj obiekty do wyswietlenia 
	PrepareObjects();
	
	glClearDepth(1.0);
	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	
	
	/*//light0
	float ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diffuseLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float spotlightPosition[] = { 6.0f, 0.5f, 0.0f, 1.0f };
	float spotlightDirection[] = { -1.0f, 0.0f, -1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ambientLight);
	glLightfv(GL_LIGHT0, GL_POSITION, spotlightPosition);
	GLfloat cutoff = 40.0f;
	GLfloat exponent = 30.0f;
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutoff);
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, &exponent);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotlightDirection);

	glEnable(GL_LIGHT0);*/


}
//--------------------------------------------------------------------------------------------
// przeprowadza animacje sceny 
void Scene::Animate()
{
	if (rot_y < 360.0)
		rot_y += 1.0;
	else
		rot_y = 0;
}
//--------------------------------------------------------------------------------------------
// obsluga klikania mysza
void Scene::MouseDown(int x, int y)
{
	frame = 0;

	clickPos = glm::vec3(xe, ye, ze);
	clickDir = glm::vec3(xt, yt, zt);

	/*// Get mouse position
	int xpos, ypos;
	//ObjectSelected = 0;
	//Draw(1);
	BYTE bArray[4];
	int _x = x;
	int _y = height - y + 100;
	glReadPixels(_x, _y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, bArray);

	//ObjectSelected = bArray[0];

	//sprintf(_msg, "Selected: %d", ObjectSelected);
	PrintLog(_msg);*/
}

//obsluga ruszania mysza
void Scene::CameraMove(float x, float y) //ile kamera ma sie ruszyc, podaje piksele
{
	float sensitivity = 0.01f;
	x *= sensitivity;
	y *= sensitivity;

	alfa -= x;
	y_angle -= y;

	if (y_angle > PI-0.1f)
		y_angle = PI-0.1f;
	if (y_angle < 0.1f)
		y_angle = 0.1f;

	
}
// obsluga kolka myszki
void Scene::MouseWheel(int x, int y)
{
	// Get mouse position
}
//--------------------------------------------------------------------------------------------
// kontrola naciskania klawiszy klawiatury
void Scene::KeyDown(unsigned char key) 
{            
	
	if (key == ESCAPE) System::Windows::Forms::Application::Exit();
	if (key == 37) { rot_y -= 5.0f; }
	if (key == 38) { rot_x -= 5.0f; }
	if (key == 39) { rot_y += 5.0f; }
	if (key == 40) { rot_x += 5.0f; }
	if (key == 113) { LightAmbient += 0.05f; if (LightAmbient >= 1.0f) LightAmbient = 1.0f; } // F1		
	if (key == 112) { LightAmbient -= 0.05f; if (LightAmbient <= 0.0f) LightAmbient = 0.0f; } //F2		

	if (key == 114) {  } //F3		
	if (key == 115) {  } //F4		

	if (key == 116) { House->Reload();  } //F5		
	if (key == 117) {  } //F6		

	if (key == 107) {  } //num+
	if (key == 109) {  } //num-
	if (key == 87) {
		fw = true;
		} //W
	if (key == 83) {
		bw = true;
		} //S		
	if (key == 65) {
		lt = true;
		} //A
	if (key == 68) {
		rt = true;
		} //D
	if (key == 16)
	{
		fast = true;
	}
	if (key == 32)
	{
		jump = true;
	}
	if (key == 17)
	{
		crouch = true;
	}
	if (key == 8)
	{
		SaveAsBmp("screen.bmp"); //backspace
	}
	if (key == 'I')
	{
		cone = 1;
	}
}

void Scene::KeyUp(unsigned char key)
{
	if (key == 87) {
		fw = false;
	} //W
	if (key == 83) {
		bw = false;
	} //S		
	if (key == 65) {
		lt = false;
	} //A
	if (key == 68) {
		rt = false;
	} //D
	if (key == 16)
	{
		fast = false;
	}
	if (key == 17)
	{
		crouch = false;
	}
}

void Scene::CheckBorder(glm::vec2 corner, glm::vec2 corner2) 
{
	if (xe <= corner.x || xe >= corner2.x)
	{
		xe = dx;
	}
	if (ze <= corner.y || ze >= corner2.y)
	{
		ze = dz;
	}
}

void Scene::CheckCollision(glObject* Object)
{
	glm::vec3 Col = Object->corner1 * Object->Scale + Object->Position;
	glm::vec3 Col2 = Object->corner2 * Object->Scale + Object->Position;
	
	//Col = glm::rotate(Col, rot_y, glm::vec3(0.0f, 1.0f, 0.0f)) + Object->Position;
	//Col2 = glm::rotate(Col2, rot_y, glm::vec3(0.0f, 1.0f, 0.0f)) + Object->Position;
	if (xe >= Col.x && xe <=Col2.x && ye>=Col.y && ye-ye_start<=Col2.y && ze >= Col.z && ze<=Col2.z)
	{
		xe = dx;
		ye = dy;
		ze = dz;
	}
}

void Scene::Move()
{
	if (crouch && !crouch_handle)
	{
		ye_start /= 2;
		crouch_handle = true;
		freeze = true;
	}
	if (!crouch && crouch_handle)
	{
		ye_start *= 2;
		crouch_handle = false;
		freeze = false;;
	}
	if (!freeze)
	{
		if (fast) d = 0.35f; //prędkość kamery gdy sprint
		else d = 0.15f; // prędkość kamery normalnie
		if (jump && ye_start > PLAYERHEIGHT && rise == false)
		{
			ye_start -= d / 3;
			if (ye_start <= 2.0f)
			{
				ye_start = 2.0f;
				jump = false;
			}
		}
		else if (jump && ye_start < PLAYERHEIGHT + 1.5f)
		{
			rise = true;
			ye_start += d / 2;
		}
		else if (jump && ye_start >= PLAYERHEIGHT + 1.5f)
		{
			rise = false;
			ye_start += d / 3;
		}

		if (fw) //W
		{
			height_beta += (PI / 16.0f);
			xe += d * cos(alfa);
			ze += d * sin(alfa);
			xt += d * cos(alfa); //cel kamery
			zt += d * sin(alfa);
		}
		if (bw) //S
		{
			height_beta -= (PI / 16.0f);
			xe += -d * cos(alfa);
			ze += -d * sin(alfa);
			xt += -d * cos(alfa); //cel kamery
			zt += -d * sin(alfa);
		}

		const float sideSpeed = 3 * d / 4;
		if (lt) //A
		{
			xe += sideSpeed * sin(alfa);
			ze -= sideSpeed * cos(alfa);
			xt += sideSpeed * sin(alfa); //cel kamery
			zt -= sideSpeed * cos(alfa);
		}
		if (rt) //D
		{
			xe += -sideSpeed * sin(alfa);
			ze -= -sideSpeed * cos(alfa);
			xt += -sideSpeed * sin(alfa); //cel kamery
			zt -= -sideSpeed * cos(alfa);
		}
	}
}

//--------------------------------------------------------------------------------------------
// rysuje scene OpenGL 
void Scene::Draw()
{	
	if (err) return; // sprawdz flage bledu (np. kompilacja shadera)

	// czyscimy bufor kolorow
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	
	/*GLfloat white[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat cyan[] = { 0.f, .8f, .8f, 1.f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);*/

	int _ModelView = glGetUniformLocation(program, "modelViewMatrix");
	int _Projection = glGetUniformLocation(program, "projectionMatrix");
	int _LightColor = glGetUniformLocation(program, "LightColor");
	int _LightAmbient = glGetUniformLocation(program, "LightAmbient");
	int _NormalMatrix = glGetUniformLocation(program, "normalMatrix");
	int _Sampler = glGetUniformLocation(program, "gSampler");
	int _ShadingMode = glGetUniformLocation(program, "ShadingMode");
	int _Alpha = glGetUniformLocation(program, "Alpha");
	int _FogDensity = glGetUniformLocation(program, "FogDensity");

	glUniform1i(_ShadingMode, 1);
	glUniform1f(_Alpha, Alpha);	
	glUniform1f(_FogDensity, FogDensity);

	// ustaw i wyslij kolor swiatla 
	glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glUniform3fv(_LightColor, 1, glm::value_ptr(LightColor));

	//glDisable(GL_LIGHTING);

	// ustaw sampler tekstury 
	glUniform1i(_Sampler, 0);

	// ustaw macierz projekcji na perspektywiczna
	glUniformMatrix4fv(_Projection, 1, GL_FALSE, glm::value_ptr(mProjection));

	dx = xe;
	dy = ye;
	dz = ze; //poprzednie współrzędne kamery

	Move(); //odśwież kamerę
	
	xt = xe + R * cos(alfa) * sin(y_angle);
	zt = ze + R * sin(alfa) * sin(y_angle);
	ye = ye_start + sin(height_beta) / 4;
	yt = ye + R * cos(y_angle);

	CheckBorder(glm::vec2(-25.0, -25.0), glm::vec2(25.0, 25.0)); //sprawdza, czy player wystaje poza obręb dostępnej mapy
	CheckCollision(House); //sprawdza, czy player wchodzi w obiekt
	CheckCollision(Obj); //sprawdza, czy player wchodzi w obiekt
	CheckCollision(Sculpt); //sprawdza, czy player wchodzi w obiekt
	CheckCollision(Cybertruck); //sprawdza, czy player wchodzi w obiekt

	// inicjuj macierz MV z polozeniem obserwatora
	glm::mat4 mModelView = glm::lookAt(glm::vec3(xe, ye, ze), 
				glm::vec3(xt, yt, zt), 
				glm::vec3(0.0f, 1.0f, 0.0f));

	// wyslij MV do shadera
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView));

	glm::vec3 CamPos = glm::vec3(xe, ye, ze); // pozycja kamery
	int _CamPos = glGetUniformLocation(program, "CamPos");
	glUniform3fv(_CamPos, 1, glm::value_ptr(CamPos));
	glm::vec3 CamDir = glm::vec3(xt, yt, zt); // kierunek kamery
	int _CamDir = glGetUniformLocation(program, "CamDir");
	glUniform3fv(_CamPos, 1, glm::value_ptr(CamDir));

	// wyslij macierz normalnych 	
	glUniformMatrix4fv(_NormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(mModelView))));

	// wyslij parametr swiatla ambient
	glUniform1f(_LightAmbient, LightAmbient);
	
	//------------------------------------------------------------------------------------------------------
	// Rysowanie w trybie perspektywicznym 
	//------------------------------------------------------------------------------------------------------
	Plane->Draw();
	//Axes->Draw();
	glm::mat4 refModelView = mModelView;

	glm::mat4 ProjectileModelView = mModelView;
	if (frame < 120) //rysowanie pocisku przez 120 klatek
	{
		ProjectileModelView = glm::translate(ProjectileModelView, clickPos);
		glm::vec3 pos = clickDir - clickPos;
		pos /= 9;
		pos *= frame;
		if (pos.y <= 0) pos.y = 0;
		ProjectileModelView = glm::translate(ProjectileModelView,
			pos);
		glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(ProjectileModelView));
		glUniform1f(_Alpha, Projectile->alpha);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE); 
		Projectile->Draw();
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		frame++;
	}

	glm::mat4 mTransform = glm::mat4(1.0);
	mTransform = glm::rotate(mTransform, rot_x, glm::vec3(1.0f, 0.0f, 0.0f));
	mTransform = glm::rotate(mTransform, rot_y, glm::vec3(0.0f, 1.0f, 0.0f));
	mTransform = glm::rotate(mTransform, rot_z, glm::vec3(0.0f, 0.0f, 1.0f));
	
	//glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView*mTransform));

	if (cone)
	{
		//dziwne cos v2
		mModelView = glm::translate(mModelView,
			glm::vec3(xe,ye,ze));
		mModelView = glm::scale(mModelView, ConeTorus->Scale);
		glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(glm::rotate(mModelView * mTransform, rot_y, glm::vec3(0.0f, 0.0f, 1.0f))));
		glUniform1f(_Alpha, ConeTorus->alpha);
		ConeTorus->Draw();
	}

	//suzanne
	mModelView = glm::translate(refModelView,
		Suzanne->Position);
	mModelView = glm::scale(mModelView, Suzanne->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
	glUniform1f(_Alpha, Suzanne->alpha);
	Suzanne->Draw();

	//dziwne cos
	mModelView = glm::translate(refModelView,
		ConeTorus->Position);
	mModelView = glm::scale(mModelView, ConeTorus->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
	glUniform1f(_Alpha, ConeTorus->alpha);
	ConeTorus->Draw();

	//dziwne cos v2
	mModelView = glm::translate(refModelView,
		ConeTorus->Position + glm::vec3(5.0f, 0.0f, 0.0f));
	mModelView = glm::scale(mModelView, ConeTorus->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(glm::rotate(mModelView * mTransform,rot_y, glm::vec3(0.0f, 0.0f, 1.0f))));
	glUniform1f(_Alpha, ConeTorus->alpha);
	ConeTorus->Draw();

	//kulka
	mModelView = glm::translate(refModelView,
		Obj->Position);
	mModelView = glm::scale(mModelView, Obj->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
	glUniform1f(_Alpha, Obj->alpha);
	Obj->Draw();

	//pantera
	mModelView = glm::translate(refModelView,
		Panther->Position);
	mModelView = glm::scale(mModelView, Panther->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView));
	glUniform1f(_Alpha, Panther->alpha);
	Panther->Draw();

	//Cybertruck
	mModelView = glm::translate(refModelView,
		Cybertruck->Position);
	mModelView = glm::scale(mModelView, Cybertruck->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView));
	glUniform1f(_Alpha, Cybertruck->alpha);
	Cybertruck->Draw();
	
	//balwan
	mModelView = glm::translate(refModelView,
		Snowman->Position);
	mModelView = glm::scale(mModelView, Snowman->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView));
	glUniform1f(_Alpha, Snowman->alpha);
	Snowman->Draw();

	//rzeźba
	mModelView = glm::translate(refModelView,
		Sculpt->Position);
	mModelView = glm::scale(mModelView, Sculpt->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
	glUniform1f(_Alpha, Sculpt->alpha);
	Sculpt->Draw();

	//domek
	mModelView = glm::translate(refModelView,
		House->Position);
	mModelView = glm::scale(mModelView, House->Scale);
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView));
	glUniform1f(_Alpha, House->alpha);
	House->Draw();

	//TEKST PRZY DOMU
	mTransform = glm::mat4(1.0);
	mTransform = glm::scale(mTransform, glm::vec3(0.5 / float(Prn->CharWidth), 0.5 / float(Prn->CharHeight), 1.0f));
	mModelView = glm::translate(mModelView, glm::vec3(1.0f, 3.0f,5.0f)+House->corner1);
	mModelView = glm::rotate(mModelView, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	glUniform1i(_ShadingMode, 0);
	char str[10];
	for (int i = 0; i < 10; i++)
	{
		str[i] = ' ';
	}
	sprintf(str, "%d", (int)rot_y);
	for (int i = 0; i < 10; i++)
	{
		mModelView = glm::translate(mModelView, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-')
		{
			Prn->Draw(str[i]);
		}
	}
	char text[] = "red";
	for (int i = 0; i < sizeof(text) - 1; i++)
	{
		Prn->Draw(text[i]);
		mModelView = glm::translate(mModelView, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
	}

	//rzutowanie ortogonalne

	glm::mat4 mOrto = glm::ortho(-10.0f, float(width),
		-10.0f, float(height));
	mModelView = glm::mat4(1.0);
	mTransform = glm::mat4(1.0);
	// ustaw macierz projekcji na ortogonalna
	glUniformMatrix4fv(_Projection, 1, GL_FALSE,
		glm::value_ptr(mOrto));
	// ustaw przeksztalcenia macierzowe
	glUniformMatrix4fv(_ModelView, 1, GL_FALSE,
		glm::value_ptr(mModelView * mTransform));	

	char imie[] = " ";
	imie[sizeof(imie)-1] = ' ';
	mModelView = glm::translate(mModelView, glm::vec3(0.0f, height-50.0f, 0.0f));
	mModelView = glm::scale(mModelView, glm::vec3(0.5f, 0.5f, 0.5f));
	for (int i = 0; i < sizeof(imie)-1; i++)
	{
		Prn->Draw(imie[i]);
		mModelView = glm::translate(mModelView, glm::vec3(64.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
	}

	for (int i = 0; i < 10; i++)
	{
		str[i] = ' ';
	}
	sprintf(str, "%d", (int)rot_y);
	for (int i = 0; i < 10; i++)
	{
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-')
			Prn->Draw(str[i]);
		mModelView = glm::translate(mModelView, glm::vec3(64.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(_ModelView, 1, GL_FALSE, glm::value_ptr(mModelView * mTransform));
	}
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}	
//------------------------------- KONIEC PLIKU -----------------------------------------------