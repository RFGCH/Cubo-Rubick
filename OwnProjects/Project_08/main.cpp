/*

	Autores:
		- Rubén Guzmán
		- Luis Arroyo

	Teclas:
	
		- 1 - 9 : Rotaciones
		- q - o : Rotaciones inversa
		- m : Solver
		- j : Shuffle
		
		- z s x c : Desplazamiento de la camara

*/

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <stdlib.h>
#include <time.h>

//Librerias de los tutos y nuestra (matrix_l.h)
#include "linmath.h"
#include "camera.h"
#include "matrix_l.h"

//Librerias para el solver
#include "Cube.hpp"
#include "Solver.hpp"
#include "Rubik.hpp"

//Libreria para la imagen
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
int keyboard_callback(GLFWwindow* window);

using namespace std;

// Cubo virtual para el solver
Cube cubo_vitrual_solver;
string camino = "";
string solver_path = "";
map<int,string> Movimiento;
map<string,int> Movimiento_Solver;


// settings
int success;
char infoLog[512];
GLFWwindow* window;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//Camera camera(glm::vec3(1.2f, 0.9f, 9.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//Multiplicador
float MaximoTraslaccion = 3.0f;
matriz<float> R(Matriz_R2<float>(0));
// ------------ Valores para el cuadrado ------
//GL_RGBA means png
void init(){
	Movimiento[0] = "L";
	//Movimiento[1] = "L'R";
	Movimiento[2] = "R'";
	Movimiento[3] = "D";
	//Movimiento[4] = "UD'";
	Movimiento[5] = "U'";
	Movimiento[6] = "F";
	//Movimiento[7] = "F'B";
	Movimiento[8] = "B'";
	Movimiento[9] = "L'";
	//Movimiento[10] = "LR'";
	Movimiento[11] = "R";
	Movimiento[12] = "D'";
	//Movimiento[13] = "DU'";
	Movimiento[14] = "U";
	Movimiento[15] = "F'";
	//Movimiento[16] = "FB'";
	Movimiento[17] = "B";
	
	Movimiento_Solver["L1"]  = 0;
	Movimiento_Solver["L2"]  = 0;
	Movimiento_Solver["R3"]  = 2;
	Movimiento_Solver["D1"]  = 3;
	Movimiento_Solver["D2"]  = 3;
	Movimiento_Solver["U3"]  = 5;
	Movimiento_Solver["F1"]  = 6;
	Movimiento_Solver["F2"]  = 6;
	Movimiento_Solver["B3"]  = 8;
	Movimiento_Solver["L3"]  = 9;
	Movimiento_Solver["R1"]  = 11;
	Movimiento_Solver["R2"]  = 11;
	Movimiento_Solver["D3"]  = 12;
	Movimiento_Solver["U1"]  = 14;
	Movimiento_Solver["U2"]  = 14;
	Movimiento_Solver["F3"]  = 15;
	Movimiento_Solver["B1"]  = 17;
	Movimiento_Solver["B2"]  = 17;
	
	// glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
bool error_window(){
    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// glad: load all OpenGL function pointers
    // ---------------------------------------
	if (!gladLoadGL(glfwGetProcAddress)) return 1;
	return 0;
}
	
struct Respirar{
	int operator()(int Limite){
		if(Flag_Respirar==Limite || Flag_Respirar==1) Flag2_Respirar = -Flag2_Respirar;
		Flag_Respirar += Flag2_Respirar;
		return (Flag2_Respirar==1)?1:-1;
	}
	int Flag_Respirar = 2;
	int Flag2_Respirar = 1;
};
const int iteradores = 8;
Respirar iter[iteradores];

class Cubo
{
private:
	//Variables
	unsigned int VAO,VBO,EBO,EBO_Lineas;
	int Num_Puntos_Cuadrado = 24;
	float x_,y_,z_;
	float vertices[24*11] = { 
	//Coordenadas(x,y,z) - Normal(x,y,z) - Color(r,g,b) - Textura(x,y)
	    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	//Izquierda izqArriba, izqAbajo, derArriba, derAbajo
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	
		0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,1.0f, 0.5f, 0.0f, 0.0f, 0.0f,		//Derecha
		0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,1.0f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,1.0f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,1.0f, 0.5f, 0.0f, 1.0f, 0.0f,

		0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,1.0f, 1.0f, 1.0f, 0.0f, 0.0f,	//Abajo
	    0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,1.0f, 1.0f, 1.0f,0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f,  0.0f,1.0f, 1.0f, 1.0f,1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	
		0.5f, 0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f,0.0f, 0.0f,	//Arriba
		0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f,  1.0f,  0.0f,1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	
		0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,	//Atras
		0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	
		0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,0.0f, 0.0f, 1.0f,1.0f, 1.0f,	//Adelante derArriba, derAbajo, izqArriba y izqAbajo
		0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,0.0f, 0.0f, 1.0f,1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,0.0f, 0.0f, 1.0f,0.0f, 1.0f};
	unsigned int indices[36] = {
		0, 1, 3,
		1, 2, 3,
		
		4, 7, 5,
		5, 7, 6,
		
		8, 11 , 9,
		10, 11 , 9,
		
		12, 15 , 13,
		14, 15 , 13,
		
		16, 19 , 17,
		18, 19 , 17,

		20, 23 , 21,
		22, 23 , 21
	};
	unsigned int indices2[16] = {
		0,1,2,3,7,6,5,4,0,6,2,4,7,1,5,3
	};
	void AttribPointer()
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float) ));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (6 * sizeof(float) ));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (9 * sizeof(float) ));
		glEnableVertexAttribArray(3);
	}
	void AttribPointer2()
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	void puntos_centrales()
	{
		x_ = y_ = z_ = 0;
		for(int i = 0; i < 24; i++){
			x_ += vertices[i*11];
			y_ += vertices[i*11+1];
			z_ += vertices[i*11+2];
		}
		x_ = int(x_);
		y_ = int(y_);
		z_ = int(z_);
	}

public:
	void ImprimirVertices()
	{
			cout<<vertices[0]<<vertices[1]<<vertices[2]<<endl;
	}
	void set_S(float Escala){
		Transformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_S<float>(Escala,Escala,Escala));
		Transformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_S<float>(Escala,Escala,Escala));
	}
	void set_Tras_Des(float x,float y,float z)
	{
		Destransformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_T<float>(x,y,z));
		Destransformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_T<float>(x,y,z));
	}
	void set_Tras(float x,float y,float z)
	{
		Transformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_T<float>(x,y,z));
		Transformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_T<float>(x,y,z));
	}
	void Draw()
	{		
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES,36, GL_UNSIGNED_INT, 0);
		
		DrawLineas();

	}
	void DrawLineas(){
		float colores[8];
		for(int i = 0; i < 8; i++){
			colores[i] = vertices[i*11+3];
			vertices[i*11+3] = 0.0f;
			vertices[i*11+4] = 0.0f;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Lineas);
		glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawElements(GL_LINE_LOOP,36, GL_UNSIGNED_INT, 0);
		for(int i = 0; i < 8; i++){
			vertices[i*11+3] = 1.0f;
			vertices[i*11+4] = colores[i];
		}
	}
	void Rotar(int Lado, float angulo)
	{
		Destransformar_Vertices(vertices,Num_Puntos_Cuadrado,R.valores);
		int inversa = Lado/9;
		int signo = Lado%3;
		Lado = Lado/3;
		angulo = 90 / angulo;
		puntos_centrales();
	
		if (inversa == 0){
			if (Lado == 0){
				if(z_ < 0 && signo == 0||z_ == 0 && signo == 1 || z_ > 0 && signo == 2)
				{
					Transformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_R3_Z<float>(angulo));
					Transformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_R3_Z<float>(angulo));
				}
			}
			if (Lado == 1)
				if(y_ < 0 && signo == 0||y_ == 0 && signo == 1 || y_ > 0 && signo == 2)
				{
					Transformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_R3_Y<float>(angulo));
					Transformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_R3_Y<float>(angulo));
				}
			if (Lado == 2)
				if(x_ < 0 && signo == 0||x_ == 0 && signo == 1 || x_ > 0 && signo == 2)
				{
					Transformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_R3_X<float>(angulo));	
					Transformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_R3_X<float>(angulo));
				}					
		}
		else{
			if (Lado == 3)
				if(z_ < 0 && signo == 0||z_ == 0 && signo == 1 || z_ > 0 && signo == 2)
				{
					Destransformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_R3_Z<float>(angulo));
					Destransformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_R3_Z<float>(angulo));
				}
			if (Lado == 4)
				if(y_ < 0 && signo == 0||y_ == 0 && signo == 1 || y_ > 0 && signo == 2)
				{
					Destransformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_R3_Y<float>(angulo));
					Destransformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_R3_Y<float>(angulo));
				}
			if (Lado == 5)
				if(x_ < 0 && signo == 0||x_ == 0 && signo == 1 || x_ > 0 && signo == 2)
				{
					Destransformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_R3_X<float>(angulo));	
					Destransformar_Normal(vertices,Num_Puntos_Cuadrado,Matriz_R3_X<float>(angulo));
				}
		}

		Transformar_Vertices(vertices,Num_Puntos_Cuadrado,R.valores);
	}
	void Init(float x = 0.0f,float y = 0.0f,float z = 0.0f)
	{
		//	Transformaciones de inicialización
		Transformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_S<float>(0.1,0.1,0.1));
		Transformar_Vertices(vertices,Num_Puntos_Cuadrado,Matriz_T<float>(x,y,z));
		matriz<float>Tras(Matriz_T<float>(3.0f,3.0f,3.0f));
		//matriz<float>RotarX(Matriz_R3_X<float>(30));
		//matriz<float>RotarY(Matriz_R3_Y<float>(30));
		//matriz<float>RotarZ(Matriz_R3_Z<float>(30));
		//R = Tras*RotarZ*RotarY*RotarX;
		R = Tras;
		Transformar_Vertices(vertices,Num_Puntos_Cuadrado,R.valores);
		//	Inicializacion de los buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &EBO_Lineas);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Lineas);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices2), indices2, GL_STATIC_DRAW);
		AttribPointer();
	}
	void InitLuz()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &EBO_Lineas);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Lineas);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices2), indices2, GL_STATIC_DRAW);
		AttribPointer2();
	}
};

class CuboRubick
{
private:
//Nota->Si escalo primero, tengo q trabajar con unidades mas pequeñas de traslaciones.
	//float Radio = 1.5f; //Usar si primero traslado y luego escalo.
	float Radio = 0.105f; //Usar si primero escalo y luego traslado.
	float PosCAnimacion = 0.105f*MaximoTraslaccion;
	const static int Numero_Cubos = 27;
	Cubo cubos[Numero_Cubos];
	float Pos_Cubos[Numero_Cubos*3] = {
			-Radio, 0.0f, 0.0f,	//A->0  //Parte del centro 0->7
			 0.0f, 0.0f, 0.0f,
			 Radio, 0.0f, 0.0f,	//B->1
			 
			-Radio, -Radio, 0.0f,	//C->2
			 0.0f , -Radio, 0.0f, //D->3
			 Radio, -Radio, 0.0f, //E->4
			 
			-Radio, Radio, 0.0f,	//F->5
			 0.0f,  Radio, 0.0f,	//G->6
			 Radio, Radio, 0.0f,  //H->7


			-Radio, 0.0f, -Radio,	//I->8   //Parte de abajo 8->16
			 0.0f , 0.0f, -Radio,	//J->9
			 Radio, 0.0f, -Radio,	//K->10
			 
			-Radio, -Radio, -Radio, //L->11
			 0.0f , -Radio, -Radio, //M->12
			 Radio, -Radio, -Radio, //N->13
			 
			-Radio, Radio, -Radio,	//O->14
			 0.0f , Radio, -Radio,	//P->15
			 Radio, Radio, -Radio, //Q->16
				
				
			-Radio, 0.0f, Radio,	//R->17    //Parte de arriba 17->25
			 0.0f , 0.0f, Radio,	//S->18
			 Radio, 0.0f, Radio,	//T->19
			 
			-Radio, -Radio, Radio, //U->20
			 0.0f , -Radio, Radio, //V->21
			 Radio, -Radio, Radio, //W->22
			
			-Radio, Radio, Radio,	//X->23
			 0.0f , Radio, Radio,	//Y->24
			 Radio, Radio, Radio //Z->25
	};
	float Pos_Cubos_Animacion[Numero_Cubos*3] = {
			-PosCAnimacion, 0.0f, 0.0f,	//A->0  //Parte del centro 0->7
			 0.0f, 0.0f, 0.0f,
			 PosCAnimacion, 0.0f, 0.0f,	//B->1
			 
			-PosCAnimacion, -PosCAnimacion, 0.0f,	//C->2
			 0.0f , -PosCAnimacion, 0.0f, //D->3
			 PosCAnimacion, -PosCAnimacion, 0.0f, //E->4
			 
			-PosCAnimacion, PosCAnimacion, 0.0f,	//F->5
			 0.0f,  PosCAnimacion, 0.0f,	//G->6
			 PosCAnimacion, PosCAnimacion, 0.0f,  //H->7


			-PosCAnimacion, 0.0f, -PosCAnimacion,	//I->8   //Parte de abajo 8->16
			 0.0f , 0.0f, -PosCAnimacion,	//J->9
			 PosCAnimacion, 0.0f, -PosCAnimacion,	//K->10
			 
			-PosCAnimacion, -PosCAnimacion, -PosCAnimacion, //L->11
			 0.0f , -PosCAnimacion, -PosCAnimacion, //M->12
			 PosCAnimacion, -PosCAnimacion, -PosCAnimacion, //N->13
			 
			-PosCAnimacion, PosCAnimacion, -PosCAnimacion,	//O->14
			 0.0f , PosCAnimacion, -PosCAnimacion,	//P->15
			 PosCAnimacion, PosCAnimacion, -PosCAnimacion, //Q->16
				
				
			-PosCAnimacion, 0.0f, PosCAnimacion,	//R->17    //Parte de arriba 17->25
			 0.0f , 0.0f, PosCAnimacion,	//S->18
			 PosCAnimacion, 0.0f, PosCAnimacion,	//T->19
			 
			-PosCAnimacion, -PosCAnimacion, PosCAnimacion, //U->20
			 0.0f , -PosCAnimacion, PosCAnimacion, //V->21
			 PosCAnimacion, -PosCAnimacion, PosCAnimacion, //W->22
			
			-PosCAnimacion, PosCAnimacion, PosCAnimacion,	//X->23
			 0.0f , PosCAnimacion, PosCAnimacion,	//Y->24
			 PosCAnimacion, PosCAnimacion, PosCAnimacion //Z->25
	};

public:
	CuboRubick ()
	{
		for(int i = 0; i < Numero_Cubos;i++)
		{
			cubos[i].Init(
				Pos_Cubos[i*3],
				Pos_Cubos[i*3+1],
				Pos_Cubos[i*3+2]);
		}
	}
	void Draw()
	{
		for(int i = 0; i < Numero_Cubos;i++)
			cubos[i].Draw();
	}
	void MoverCamada(int Lado,int Frame)
	{
		for(int i=0; i < Numero_Cubos; i++)
			cubos[i].Rotar(Lado,Frame);
		Draw();
	}
	void AnimacionTraslacion(bool flag,float trasla)
	{ 
		if(flag)
		{
			float x(0.0f),y(0.0f),z(0.0f);
			for(int i=0;i < Numero_Cubos;i++)
			{
				if(Pos_Cubos[i*3] < 0.0f)
					x = -trasla;
				if(Pos_Cubos[i*3] > 0.0f)
					x = trasla;
				if(Pos_Cubos[i*3+1] < 0.0f)
					y = -trasla;
				if(Pos_Cubos[i*3+1] > 0.0f)
					y = trasla;
				if(Pos_Cubos[i*3+2] < 0.0f)
					z = -trasla;
				if(Pos_Cubos[i*3+2] > 0.0f)
					z = trasla;
				cubos[i].set_Tras(x,y,z);
			}
		}
		else
		{
			float x(0.0f),y(0.0f),z(0.0f);
			for(int i=0;i < Numero_Cubos;i++)
			{
				if(Pos_Cubos[i*3] < 0.0f)
					x = -trasla;
				if(Pos_Cubos[i*3] > 0.0f)
					x = trasla;
				if(Pos_Cubos[i*3+1] < 0.0f)
					y = -trasla;
				if(Pos_Cubos[i*3+1] > 0.0f)
					y = trasla;
				if(Pos_Cubos[i*3+2] < 0.0f)
					z = -trasla;
				if(Pos_Cubos[i*3+2] > 0.0f)
					z = trasla;
				cubos[i].set_Tras_Des(x,y,z);
			}
		}
		Draw();
	}
};
void ColocarTextura()
{
		// texture 1
	// ---------
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); 
		 // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
		// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load("D:/GraficaLab2/glfw-master/OwnProjects/Project_08/pelos.jpeg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB , GL_UNSIGNED_BYTE, data); //Si se usa jpg
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA , GL_UNSIGNED_BYTE, data); //Si se usa png
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	
	// either set it manually like so:
	//glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	// or set it via the texture class
	//ourShader.setInt("texture1", 0);
}
//Funciones del solver
string numero_simbolo(string path){
	string ret;

	for (int i = 0; path[i]; i++)
	{
		if (path[i] == '1')
			ret += " ";
		else if (path[i] == '3')
			ret += "\' ";
		else if (path[i] == '2')
			ret += "2 ";
		else
			ret += path[i];
	}
	return ret;
}
string simbolo_numero(string path){
	string ret;

	for (int i = 0; path[i]; i++)
	{
		if (path[i] == ' ')
			ret += "1";
		else if (path[i] == '\'')
			ret += "3";
		else if (path[i] == '2')
			ret += "2";
		else
			ret += path[i];
	}
	return ret;
}
string shufle(int Pasos){
	string resultado;
	string auxiliar_movimiento;
	for(int i = 0; i < Pasos; i++){
		auxiliar_movimiento = Movimiento[rand()%18];
		while (auxiliar_movimiento == "")
			auxiliar_movimiento = Movimiento[rand()%18];
		resultado += auxiliar_movimiento;
		if (auxiliar_movimiento[1] != '\'' && auxiliar_movimiento[1] != '2') resultado += " ";
	}
	return resultado;
}

void init_solver(string camino){
	for (int i = 0; camino[i]; i++)
		if (camino[i] == 'F' || camino[i] == 'R' || camino[i] == 'U' ||
			camino[i] == 'B' || camino[i] == 'L' || camino[i] == 'D')
		{
			int num = 1;
			if (camino[i + 1] == '\'')
			num = 3;
			else if (camino[i + 1] == '2')
				num = 2;
			cubo_vitrual_solver.rotCube(camino[i], num);
		}
		cubo_vitrual_solver.getColor();
}
void bfsSolve(Cube *solverCube, Solver *s, string *output)
{
	for (int phase = 1; phase <= 4; phase++)
	{
		queue<Cube> queue;
		queue.push(*solverCube);
		*solverCube = s->BFS(0, queue);
		solverCube->getColor();
		output->append(solverCube->path);
		solverCube->path = "";
	}
}
string Path_Solver(){
	string output;
	init_solver(camino);
	Solver s(cubo_vitrual_solver);
	bfsSolve(&cubo_vitrual_solver, &s, &output);
	camino = "";
	return output;
}

// lighting
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool flagcamera = false;
int main()
{
	
	//Probando
	srand(time(NULL));
	init();
	if(error_window())return -1;
	glEnable(GL_DEPTH_TEST);
	
	
	CuboRubick veintiseislados;
	Cubo Luz;
	//Cubo Sala;
	
	//Textura
	Shader ourShader("D:/GraficaLab2/glfw-master/OwnProjects/Project_08/VertexShader.vs", "D:/GraficaLab2/glfw-master/OwnProjects/Project_08/FragmentShader.fs");
	Shader sol("D:/GraficaLab2/glfw-master/OwnProjects/Project_08/LuzVertexShader.vs","D:/GraficaLab2/glfw-master/OwnProjects/Project_08/LuzFragmentShader.fs");
	ColocarTextura();
	ourShader.use();
	//Luz
	ourShader.setVec3("lightColor",  1.0f, 1.0f, 0.5f);
    ourShader.setVec3("lightPos", lightPos);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);	
    ourShader.setMat4("projection", projection);
	        // camera/view transformation

	//view->lookat
	//projection->perspective
	
	//Sol
	sol.use();
	Luz.InitLuz();
	Luz.set_S(0.2f);
	sol.setMat4("projection", projection);
		
	//Cubo
	glLineWidth(3.0);
	int key = -1;
	int frame = 100;
	int animacion_en_curso = -1;
	int animacionExplota = 1;
	int animacionRegreso = 1;
	float animacionTrasla = 0.05;
	bool traslafinal = false;
    while (!glfwWindowShouldClose(window))
    { // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		
        processInput(window);
		
        //glClearColor(1.0f, 0.7f, 0.4f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);
		if(flagcamera == true)
		{	
			glm::mat4 view = glm::mat4(1.0f);
			float radius = 7.0f;
			float camX = static_cast<float>(sin(glfwGetTime()) * radius);
			float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
			float camY = static_cast<float>(cos(glfwGetTime()) * radius);
			view = glm::lookAt(glm::vec3(camX,camY, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("view", view);
		}
		ourShader.setVec3("viewPos", camera.Position);
		if(animacion_en_curso == -1){
			veintiseislados.Draw();
			key = keyboard_callback(window);
			if(key!=-1)	
				if (key == 18)				// Llamamos a Solver
					if(camino == ""){		// No hay movimientos
						key = -1;
					}
					else {
						solver_path = Path_Solver();
						animacion_en_curso = 0;
					}
				else{
					camino += Movimiento[key];
					animacion_en_curso = 0;
				}
		}
		else {
			if(key == 19){
				solver_path = simbolo_numero(shufle(10));
				camino += numero_simbolo(solver_path);
				key = 18;
			}
			if(key == 18)
			{				
				if(!solver_path.empty())
				{
					key = Movimiento_Solver[solver_path.substr(0,2)];
					if(solver_path[1] == '2'){
						solver_path = solver_path.substr(2);
						solver_path = Movimiento[key] + "1" + solver_path;	// Revisar luego
					}
					else{
						solver_path = solver_path.substr(2);
					}
				}
			}
			if(key == 20)
			{
				_sleep(10);
				if(animacionExplota != 100)
				{
					veintiseislados.AnimacionTraslacion(true,animacionTrasla);
					animacionExplota++;
				}
				if(animacionExplota == 100 && animacionRegreso != 100)
				{
					veintiseislados.AnimacionTraslacion(false,animacionTrasla);
					animacionRegreso++;
				}
			}
			veintiseislados.MoverCamada(key,frame);
			if (++animacion_en_curso == frame)
			{
				animacion_en_curso = -1;
				if (solver_path.size()!=0){
					key = 18;
					animacion_en_curso = 0;
				}
			}
		}
		sol.use();
		sol.setMat4("view", view);
		Luz.Draw();
		glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
//Funciones para mover el cuadrado
int keyboard_callback(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		return 0;
	//if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		//return 1;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		return 2;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		return 3;
	//if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		//return 4;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		return 5;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		return 6;
	//if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		//return 7;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		return 8;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		return 9;
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		//return 10;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		return 11;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		return 12;
    //if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		//return 13;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		return 14;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		return 15;
   // if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		//return 16;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		return 17;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		return 18;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		return 19;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		return 20;
	return -1;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
if(flagcamera == false)
{
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
	if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		flagcamera = true;
	if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		flagcamera = false;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if(flagcamera == false)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

/*void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}*/