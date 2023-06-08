/*--------------------------------------------------------------------------------------------*/
/* ---------------------------------- PROYECTO FINAL CGeIHC ----------------------------------*/
/*-------------------------------------------2023-2-------------------------------------------*/
/*----------------------------------   Zoologíco ---------------------------------------------*/
/*-------------------------  Alumno: Guzmán Ramírez Aldo Yael   ------------------------------*/
/*------------- Codigo Base proporcionado por ING. Luis Sergio Valencia Castro ---------------*/
/*--------------------------------------------------------------------------------------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

// posiciones
float	movAuto_x = 0.0f,
		movAuto_z = 0.0f,
		orienta = 0.0f;
bool	animacion = false,
		recorrido1 = true,
		recorrido2 = false,
		recorrido3 = false,
		recorrido4 = false;



//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
rotRodDer = 0.0f,
giroMonito = 0.0f,
movBrazoDer = 0.0f,
movBrazoIzq = 0.0f,
movCabeza = 0.0f;

// Variables para calculo del incremento
float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
incRodIzq = 0.0f,
incRodDer = 0.0f,
giroMonitoInc = 0.0f,
incBrazoDer = 0.0f,
incBrazoIzq = 0.0f,
incCabeza = 0.0f;

// Bisonte
float	movX = 0.0f,
movY = 0.0f,
movZ = 0.0f,
rotPDelanDer = 0.0f,
rotPDelanIzq = 0.0f,
rotPTrasDer = 0.0f,
rotPTrasIzq = 0.0f,
colaBisonte = 0.0f,
giroBisonte = 0.0f;

// Variables para calculo del incremento
float	inX = 0.0f,
inY = 0.0f,
inZ = 0.0f,
incPDelanDer = 0.0f,
incPDelanIzq = 0.0f,
incPTrasDer = 0.0f,
incPTrasIzq = 0.0f,
incColaBisonte = 0.0f,
incGiroBisonte = 0.0f;

#define MAX_FRAMES 30
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float rotRodDer;
	float giroMonito;
	float movBrazoDer;
	float movBrazoIzq;
	float movCabeza;

	// Bisonte
	float	movX;
	float	movY;
	float	movZ;
	float	rotPDelanDer;
	float	rotPDelanIzq;
	float	rotPTrasDer;
	float	rotPTrasIzq;
	float	colaBisonte;
	float	giroBisonte;

}FRAME_BISONT;

FRAME_BISONT KeyFrame[MAX_FRAMES];
int FrameIndexBisont = 17;			//introducir número en caso de tener Key guardados
bool play = true;
int playIndexBisont = 0;


/* Se manda llamar con la tecla L, es la que guarda los frames clave, y se guardan siempre y cuando
   aun haya espacio en el arreglo
   Se guarda la variable de dibujo en el arreglo */
void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndexBisont << std::endl;

	/* Del lado derecho de la asignacion estan las variables de dibujo */

	// Bisonte
	KeyFrame[FrameIndexBisont].movX = movX;
	KeyFrame[FrameIndexBisont].movY = movY;
	KeyFrame[FrameIndexBisont].movZ = movZ;
	KeyFrame[FrameIndexBisont].rotPDelanDer = rotPDelanDer;
	KeyFrame[FrameIndexBisont].rotPDelanIzq = rotPDelanIzq;
	KeyFrame[FrameIndexBisont].rotPTrasDer = rotPTrasDer;
	KeyFrame[FrameIndexBisont].rotPTrasIzq = rotPTrasIzq;
	KeyFrame[FrameIndexBisont].giroBisonte = giroBisonte;
	KeyFrame[FrameIndexBisont].colaBisonte = colaBisonte;

	FrameIndexBisont++;
}

/* La animacion comienza desde el cuadro clave 0
   Tomando la informacion de ese cuadro clave y se manda a la variable de dibujo*/
void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	rotRodDer = KeyFrame[0].rotRodDer;
	giroMonito = KeyFrame[0].giroMonito;

	movBrazoDer = KeyFrame[0].movBrazoDer;
	movBrazoIzq = KeyFrame[0].movBrazoIzq;
	movCabeza = KeyFrame[0].movCabeza;

	// Bisonte
	movX = KeyFrame[0].movX;
	movY = KeyFrame[0].movY;
	movZ = KeyFrame[0].movZ;
	rotPDelanDer = KeyFrame[0].rotPDelanDer;
	rotPDelanIzq = KeyFrame[0].rotPDelanIzq;
	rotPTrasDer = KeyFrame[0].rotPTrasDer;
	rotPTrasIzq = KeyFrame[0].rotPTrasIzq;
	giroBisonte = KeyFrame[0].giroBisonte;
	colaBisonte = KeyFrame[0].colaBisonte;
}
void interpolation(void)
{
	
	// Bisonte
	inX = (KeyFrame[playIndexBisont + 1].movX - KeyFrame[playIndexBisont].movX) / i_max_steps;
	inY = (KeyFrame[playIndexBisont + 1].movY - KeyFrame[playIndexBisont].movY) / i_max_steps;
	inZ = (KeyFrame[playIndexBisont + 1].movZ - KeyFrame[playIndexBisont].movZ) / i_max_steps;
	incPDelanDer = (KeyFrame[playIndexBisont + 1].rotPDelanDer - KeyFrame[playIndexBisont].rotPDelanDer) / i_max_steps;
	incPDelanIzq = (KeyFrame[playIndexBisont + 1].rotPDelanIzq - KeyFrame[playIndexBisont].rotPDelanIzq) / i_max_steps;
	incPTrasDer = (KeyFrame[playIndexBisont + 1].rotPTrasDer - KeyFrame[playIndexBisont].rotPTrasDer) / i_max_steps;
	incPTrasIzq = (KeyFrame[playIndexBisont + 1].rotPTrasIzq - KeyFrame[playIndexBisont].rotPTrasIzq) / i_max_steps;
	incGiroBisonte = (KeyFrame[playIndexBisont + 1].giroBisonte - KeyFrame[playIndexBisont].giroBisonte) / i_max_steps;
	incColaBisonte = (KeyFrame[playIndexBisont + 1].colaBisonte - KeyFrame[playIndexBisont].colaBisonte) / i_max_steps;

}

void animate(void)
{


	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndexBisont++;
			if (playIndexBisont > FrameIndexBisont - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndexBisont = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			/* El incremento calculado se suma a la variable de dibujo
			   Se pone del lado derecho la variable de dibujo y en el izquierdo la de incremento */
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += incRodIzq;
			rotRodDer += incRodDer;
			giroMonito += giroMonitoInc;

			movBrazoDer += incBrazoDer;
			movBrazoIzq += incBrazoIzq;
			movCabeza += incCabeza;

			// Bisonte
			movX += inX;
			movY += inY;
			movZ += inZ;
			rotPDelanDer += incPDelanDer;
			rotPDelanIzq += incPDelanIzq;
			rotPTrasDer += incPTrasDer;
			rotPTrasIzq += incPTrasIzq;
			giroBisonte += incGiroBisonte;
			colaBisonte += incColaBisonte;

			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{
		movAuto_z += 3.0f;
	}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	//********************************************************************************************************************
	//||												Carga de modelos												||
	//********************************************************************************************************************
	// 
	// ------------------------------------------------- Carga de Bufalo-------------------------------------------------
	// 
	// Bufalo
	Model cuerpo_Bisonte("resources/objects/HabitadSabana/BuffaloKey/cuerpo_Bisonte.obj");
	Model pata_DelanDer("resources/objects/HabitadSabana/BuffaloKey/pata_DelanDer.obj");
	Model pata_DelanIzq("resources/objects/HabitadSabana/BuffaloKey/pata_DelanIzq.obj");
	Model pata_TrasDer("resources/objects/HabitadSabana/BuffaloKey/pata_TrasDer.obj");
	Model pata_TrasIzq("resources/objects/HabitadSabana/BuffaloKey/pata_TrasIzq.obj");
	Model cola_Bisonte("resources/objects/HabitadSabana/BuffaloKey/cola_Bisonte.obj");

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotRodDer = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].movBrazoDer = 0;
		KeyFrame[i].movBrazoIzq = 0;
		KeyFrame[i].movCabeza = 0;

		// Bisonte
		KeyFrame[i].movX = 0;
		KeyFrame[i].movY = 0;
		KeyFrame[i].movZ = 0;
		KeyFrame[i].rotPDelanDer = 0;
		KeyFrame[i].rotPDelanIzq = 0;
		KeyFrame[i].rotPTrasDer = 0;
		KeyFrame[i].rotPTrasIzq = 0;
		KeyFrame[i].giroBisonte = 0;
		KeyFrame[i].colaBisonte = 0;
	}


	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

	// ---------------------------------------------- Carga de Modelos General ----------------------------------------------
		Model piso("resources/objects/piso/piso.obj");
		Model bardas("resources/objects/Bardas/bardas.obj");
	
	// ---------------------------------------------- Carga de Habitad Desierto ----------------------------------------------
		Model desierto("resources/objects/HabitadDesierto/Desierto/desierto.obj");
		Model vallasdes("resources/objects/HabitadDesierto/valla/vallasdes.obj");
		Model cactus("resources/objects/HabitadDesierto/Cactus/cactus.obj");
		Model arboldes("resources/objects/HabitadDesierto/Arbol_Deser/Arbolesdes.obj");
		Model craneo("resources/objects/HabitadDesierto/Craneo/Craneo.obj");
		Model nopal("resources/objects/HabitadDesierto/nopal/Nopal.obj");
		Model estanque("resources/objects/HabitadDesierto/Estanque/estanque.obj");
		Model cang("resources/objects/HabitadDesierto/Canguro/kanguro.obj");

	// ---------------------------------------------- Carga de Habitad Polar ----------------------------------------------
	   Model Polar("resources/objects/HabitadPolar/Polar.obj");
	
	// ---------------------------------------------- Carga de Habitad Bosque ----------------------------------------------
	   Model bosque("resources/objects/HabitadBosque/forest/Bosque.obj");

	// ---------------------------------------------- Carga de Habitad Selva ----------------------------------------------
		Model selva("resources/objects/HabitadSelva/selva/S.obj");

	// ---------------------------------------------- Carga de Habitad Sabana ----------------------------------------------
		Model Sabana("resources/objects/HabitadSabana/Sabana/sabana.obj");
	
	// ---------------------------------------------- Carga de Area Central ----------------------------------------------
		Model fuente("resources/objects/Centro/Fuente/centrofuente.obj");
		Model estatuaelef("resources/objects/Centro/Estatuaelef/estatuaelef.obj");
		Model entradabardas("resources/objects/Centro/entradabardas/entradabardas.obj");
		Model entrada("resources/objects/Centro/Entrada/entrada.obj");
		Model banca("resources/objects/Centro/banca/banca.obj");
		Model bonsai("resources/objects/Centro/bonsai/bonsai.obj");
		Model estatuaserp("resources/objects/Centro/estatuaserp/estatuaserp.obj");
	// ======================================================= Animales =======================================================
	    


	    ModelAnim serp("resources/objects/HabitadDesierto/Serpiente/serp.fbx");
		serp.initShaders(animShader.ID);

		ModelAnim camel("resources/objects/HabitadDesierto/Camello/camel.fbx");
		camel.initShaders(animShader.ID);

		ModelAnim ping("resources/objects/HabitadPolar/Pinguino/pingui.fbx");
		ping.initShaders(animShader.ID);

		ModelAnim oso("resources/objects/HabitadPolar/Oso/osito.fbx");
		oso.initShaders(animShader.ID);

		ModelAnim lobo("resources/objects/HabitadBosque/Lobo/lobo.fbx");
		lobo.initShaders(animShader.ID);

		ModelAnim leon("resources/objects/HabitadSabana/leo/Low_Lion_animation.fbx");
		leon.initShaders(animShader.ID);
	
	//********************************************************************************************************************
	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


		// -------------------------------------------------------------------------------------------------------------------------
		//  Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);
		
		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, -170.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.1f));
		animShader.setMat4("model", model);
		serp.Draw(animShader);


		model = glm::translate(glm::mat4(1.0f), glm::vec3(-210.0f, 7.0f, -210.0f)); //
		model = glm::scale(model, glm::vec3(0.012f));
		animShader.setMat4("model", model);
		camel.Draw(animShader);

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, -30.0f));
		model = glm::scale(model, glm::vec3(0.007f));
		animShader.setMat4("model", model);
		ping.Draw(animShader);

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
		model = glm::scale(model, glm::vec3(0.01f));
		animShader.setMat4("model", model);
		oso.Draw(animShader);
		
		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f));
		animShader.setMat4("model", model);
		lobo.Draw(animShader);

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 38.0f, 5.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		animShader.setMat4("model", model);
		leon.Draw(animShader);
		

		// -------------------------------------------------------------------------------------------------------------------------
		// Animacion Bisonte
		// -------------------------------------------------------------------------------------------------------------------------

		KeyFrame[0].movX = 0.000000;
		KeyFrame[0].movY = 0.000000;
		KeyFrame[0].movZ = 0.000000;
		KeyFrame[0].rotPDelanDer = 0.000000;
		KeyFrame[0].rotPDelanIzq = 0.000000;
		KeyFrame[0].rotPTrasDer = 0.000000;
		KeyFrame[0].rotPTrasIzq = 0.000000;
		KeyFrame[0].giroBisonte = 0.000000;
		KeyFrame[0].colaBisonte = 0.000000;

		KeyFrame[1].movX = 0.000000;
		KeyFrame[1].movY = 0.000000;
		KeyFrame[1].movZ = 0.000000;
		KeyFrame[1].rotPDelanDer = 21.000000;
		KeyFrame[1].rotPDelanIzq = -21.000000;
		KeyFrame[1].rotPTrasDer = 21.000000;
		KeyFrame[1].rotPTrasIzq = -21.000000;
		KeyFrame[1].giroBisonte = 7.000000;
		KeyFrame[1].colaBisonte = -21.000000;

		KeyFrame[2].movX = 0.000000;
		KeyFrame[2].movY = 0.000000;
		KeyFrame[2].movZ = -10.000000;
		KeyFrame[2].rotPDelanDer = 0.000000;
		KeyFrame[2].rotPDelanIzq = 0.000000;
		KeyFrame[2].rotPTrasDer = 0.000000;
		KeyFrame[2].rotPTrasIzq = 0.000000;
		KeyFrame[2].giroBisonte = 17.000000;
		KeyFrame[2].colaBisonte = -42.000000;

		KeyFrame[3].movX = -3.000000;
		KeyFrame[3].movY = 0.000000;
		KeyFrame[3].movZ = -14.000000;
		KeyFrame[3].rotPDelanDer = -21.000000;
		KeyFrame[3].rotPDelanIzq = 21.000000;
		KeyFrame[3].rotPTrasDer = -21.000000;
		KeyFrame[3].rotPTrasIzq = 21.000000;
		KeyFrame[3].giroBisonte = 31.000000;
		KeyFrame[3].colaBisonte = -42.000000;

		KeyFrame[4].movX = -8.000000;
		KeyFrame[4].movY = 0.000000;
		KeyFrame[4].movZ = -21.000000;
		KeyFrame[4].rotPDelanDer = 0.000000;
		KeyFrame[4].rotPDelanIzq = 0.000000;
		KeyFrame[4].rotPTrasDer = 0.000000;
		KeyFrame[4].rotPTrasIzq = 0.000000;
		KeyFrame[4].giroBisonte = 48.000000;
		KeyFrame[4].colaBisonte = -63.000000;

		KeyFrame[5].movX = -18.000000;
		KeyFrame[5].movY = 0.000000;
		KeyFrame[5].movZ = -25.000000;
		KeyFrame[5].rotPDelanDer = 21.000000;
		KeyFrame[5].rotPDelanIzq = -21.000000;
		KeyFrame[5].rotPTrasDer = 21.000000;
		KeyFrame[5].rotPTrasIzq = -21.000000;
		KeyFrame[5].giroBisonte = 58.000000;
		KeyFrame[5].colaBisonte = -42.000000;

		KeyFrame[6].movX = -28.000000;
		KeyFrame[6].movY = 0.000000;
		KeyFrame[6].movZ = -25.000000;
		KeyFrame[6].rotPDelanDer = 0.000000;
		KeyFrame[6].rotPDelanIzq = 0.000000;
		KeyFrame[6].rotPTrasDer = 0.000000;
		KeyFrame[6].rotPTrasIzq = 0.000000;
		KeyFrame[6].giroBisonte = 68.000000;
		KeyFrame[6].colaBisonte = -21.000000;

		KeyFrame[7].movX = -38.000000;
		KeyFrame[7].movY = 0.000000;
		KeyFrame[7].movZ = -25.000000;
		KeyFrame[7].rotPDelanDer = -21.000000;
		KeyFrame[7].rotPDelanIzq = 21.000000;
		KeyFrame[7].rotPTrasDer = -21.000000;
		KeyFrame[7].rotPTrasIzq = 21.000000;
		KeyFrame[7].giroBisonte = 80.000000;
		KeyFrame[7].colaBisonte = -42.000000;

		KeyFrame[8].movX = -42.000000;
		KeyFrame[8].movY = 0.000000;
		KeyFrame[8].movZ = -22.000000;
		KeyFrame[8].rotPDelanDer = 0.000000;
		KeyFrame[8].rotPDelanIzq = 0.000000;
		KeyFrame[8].rotPTrasDer = 0.000000;
		KeyFrame[8].rotPTrasIzq = 0.000000;
		KeyFrame[8].giroBisonte = 97.000000;
		KeyFrame[8].colaBisonte = -21.000000;

		KeyFrame[9].movX = -47.000000;
		KeyFrame[9].movY = 0.000000;
		KeyFrame[9].movZ = -15.000000;
		KeyFrame[9].rotPDelanDer = 21.000000;
		KeyFrame[9].rotPDelanIzq = -21.000000;
		KeyFrame[9].rotPTrasDer = 21.000000;
		KeyFrame[9].rotPTrasIzq = -21.000000;
		KeyFrame[9].giroBisonte = 128.000000;
		KeyFrame[9].colaBisonte = -51.000000;

		KeyFrame[10].movX = -48.000000;
		KeyFrame[10].movY = 0.000000;
		KeyFrame[10].movZ = -1.000000;
		KeyFrame[10].rotPDelanDer = 0.000000;
		KeyFrame[10].rotPDelanIzq = 0.000000;
		KeyFrame[10].rotPTrasDer = 0.000000;
		KeyFrame[10].rotPTrasIzq = 0.000000;
		KeyFrame[10].giroBisonte = 144.000000;
		KeyFrame[10].colaBisonte = -81.000000;

		KeyFrame[11].movX = -48.000000;
		KeyFrame[11].movY = 0.000000;
		KeyFrame[11].movZ = 9.000000;
		KeyFrame[11].rotPDelanDer = -21.000000;
		KeyFrame[11].rotPDelanIzq = 21.000000;
		KeyFrame[11].rotPTrasDer = -21.000000;
		KeyFrame[11].rotPTrasIzq = 21.000000;
		KeyFrame[11].giroBisonte = 155.000000;
		KeyFrame[11].colaBisonte = -45.000000;

		KeyFrame[12].movX = -48.000000;
		KeyFrame[12].movY = 0.000000;
		KeyFrame[12].movZ = 18.000000;
		KeyFrame[12].rotPDelanDer = 0.000000;
		KeyFrame[12].rotPDelanIzq = 0.000000;
		KeyFrame[12].rotPTrasDer = 0.000000;
		KeyFrame[12].rotPTrasIzq = 0.000000;
		KeyFrame[12].giroBisonte = 170.000000;
		KeyFrame[12].colaBisonte = -12.000000;

		KeyFrame[13].movX = -48.000000;
		KeyFrame[13].movY = 0.000000;
		KeyFrame[13].movZ = 18.000000;
		KeyFrame[13].rotPDelanDer = 21.000000;
		KeyFrame[13].rotPDelanIzq = -21.000000;
		KeyFrame[13].rotPTrasDer = 21.000000;
		KeyFrame[13].rotPTrasIzq = -21.000000;
		KeyFrame[13].giroBisonte = 205.000000;
		KeyFrame[13].colaBisonte = -12.000000;

		KeyFrame[14].movX = -43.000000;
		KeyFrame[14].movY = 0.000000;
		KeyFrame[14].movZ = 20.000000;
		KeyFrame[14].rotPDelanDer = 0.000000;
		KeyFrame[14].rotPDelanIzq = 0.000000;
		KeyFrame[14].rotPTrasDer = 0.000000;
		KeyFrame[14].rotPTrasIzq = 0.000000;
		KeyFrame[14].giroBisonte = 205.000000;
		KeyFrame[14].colaBisonte = -36.000000;

		KeyFrame[15].movX = -29.000000;
		KeyFrame[15].movY = 0.000000;
		KeyFrame[15].movZ = 21.000000;
		KeyFrame[15].rotPDelanDer = -21.000000;
		KeyFrame[15].rotPDelanIzq = 21.000000;
		KeyFrame[15].rotPTrasDer = -21.000000;
		KeyFrame[15].rotPTrasIzq = 21.000000;
		KeyFrame[15].giroBisonte = 233.000000;
		KeyFrame[15].colaBisonte = -36.000000;

		KeyFrame[16].movX = 0.000000;
		KeyFrame[16].movY = 0.000000;
		KeyFrame[16].movZ = 0.000000;
		KeyFrame[16].rotPDelanDer = 0.000000;
		KeyFrame[16].rotPDelanIzq = 0.000000;
		KeyFrame[16].rotPTrasDer = 0.000000;
		KeyFrame[16].rotPTrasIzq = 0.000000;
		KeyFrame[16].giroBisonte = -1.000000;
		KeyFrame[16].colaBisonte = 0.000000;

		//Animales
		//********************************************************************************************************************
		//||											Construccion de Escenario											||
		//********************************************************************************************************************
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		// ======================================================= Piso =======================================================
		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -100.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		// ====================================================== Bardas ======================================================
		model = glm::mat4(1.0f);
		staticShader.setMat4("model", model);
		bardas.Draw(staticShader);
		
		// ======================================================= Valla ======================================================
		model = glm::mat4(1.0f);
		staticShader.setMat4("model", model);
		vallasdes.Draw(staticShader);
		
		// ===================================================== Desierto =====================================================
		//PISO
		tmp = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setMat4("model", model);
		desierto.Draw(staticShader);
		//CACTUS1
		tmp = model = glm::translate(tmp, glm::vec3(-170.0f, 0.0f, -170.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//CACTUS2
		tmp = model = glm::translate(tmp, glm::vec3(-50.0f, 0.0f, -50.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//CACTUS3
		tmp = model = glm::translate(tmp, glm::vec3(-70.0f, 0.0f, -40.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.5f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//CACTUS4
		tmp = model = glm::translate(tmp, glm::vec3(30.0f, 0.0f, 20.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.7f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//CACTUS5
		tmp = model = glm::translate(tmp, glm::vec3(50.0f, 0.0f, 10.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//CACTUS6
		tmp = model = glm::translate(tmp, glm::vec3(50.0f, 0.0f, 10.0f));
		model = glm::rotate(model, glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//CACTUS7
		tmp = model = glm::translate(tmp, glm::vec3(-80.0f, 0.0f, 30.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.6f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//CACTUS8
		tmp = model = glm::translate(tmp, glm::vec3(-35.0f, 0.0f, -30.0f));
		model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.5f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Arboles del Desierto
		model = glm::mat4(1.0f);
		tmp = model = glm::translate(model, glm::vec3(-210.0f, 0.0f, -280.0f));
		model = glm::scale(model, glm::vec3(28.0f));
		staticShader.setMat4("model", model);
		arboldes.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-70.0f, 0.0f, 90.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(30.0f));
		staticShader.setMat4("model", model);
		arboldes.Draw(staticShader);

		//CactusCraneo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-155.0f, 0.0f, -155.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Craneo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, -150.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		craneo.Draw(staticShader);

		//Nopal
		model = glm::mat4(1.0f);
		tmp = model = glm::translate(model, glm::vec3(-190.0f, 0.0f, -150.0f));
		model = glm::scale(model, glm::vec3(2.5f));
		staticShader.setMat4("model", model);
		nopal.Draw(staticShader);

		tmp = model = glm::translate(tmp, glm::vec3(-55.0f, 0.0f, -50.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		nopal.Draw(staticShader);

		tmp = model = glm::translate(tmp, glm::vec3(10.0f, 0.0f, -30.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.8f));
		staticShader.setMat4("model", model);
		nopal.Draw(staticShader);

		tmp = model = glm::translate(tmp, glm::vec3(40.0f, 0.0f, 50.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.8f));
		staticShader.setMat4("model", model);
		nopal.Draw(staticShader);

		//Estanque
		model = glm::mat4(1.0f);
		tmp = model = glm::translate(model, glm::vec3(-160.0f, 0.1f, -250.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		estanque.Draw(staticShader);

		//Canguro
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-220.0f, 0.0f, -150.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		cang.Draw(staticShader);
		

		// ====================================================== Polar ======================================================
		tmp = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.1f, 0.0f));
		staticShader.setMat4("model", model);
		Polar.Draw(staticShader);

		//valla
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		vallasdes.Draw(staticShader);
		
		
		// ====================================================== Centro ======================================================
		//fuente
		tmp = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.15f, 230.0f));
		model = glm::scale(model, glm::vec3(8.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		fuente.Draw(staticShader);

		//estatuaelef
		tmp = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-120.0f, 0.15f, 105.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		estatuaelef.Draw(staticShader);

		tmp = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(120.0f, 0.15f, 105.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		estatuaelef.Draw(staticShader);

		//entradabardas
		tmp = model = glm::mat4(1.0f);
		staticShader.setMat4("model", model);
		entradabardas.Draw(staticShader);

		//entrada
		tmp = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 420.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		entrada.Draw(staticShader);

		//bancas
		tmp = model = glm::mat4(1.0f);
		tmp=model = glm::translate(model, glm::vec3(-185.0f, 0.0f, 180.0f));
		model = glm::scale(model, glm::vec3(0.6f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		banca.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 130.0f));
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.6f));
		staticShader.setMat4("model", model);
		banca.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(370.0f, 0.0f, 130.0f));
		model = glm::rotate(model, glm::radians(125.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.6f));
		staticShader.setMat4("model", model);
		banca.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(370.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(215.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.6f));
		staticShader.setMat4("model", model);
		banca.Draw(staticShader);

		//bonsai
		tmp = model = glm::mat4(1.0f);
		tmp= model = glm::translate(tmp, glm::vec3(-250.0f, 6.0f, 160.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		bonsai.Draw(staticShader);

		tmp= model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 170.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		bonsai.Draw(staticShader);

		tmp= model = glm::translate(tmp, glm::vec3(500.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		bonsai.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, -170.0f));
		model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		bonsai.Draw(staticShader);

		//estatuaserp
		tmp = model = glm::mat4(1.0f);
		tmp =model =glm::translate(model, glm::vec3(-255.0f, 0.0f, 240.0f));
		model = glm::rotate(model, glm::radians(190.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(4.0f));
		staticShader.setMat4("model", model);
		estatuaserp.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(510.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(80.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(4.0f));
		staticShader.setMat4("model", model);
		estatuaserp.Draw(staticShader);
	
		
		// ====================================================== Sabana ======================================================
		tmp = model = glm::mat4(1.0f);
		staticShader.setMat4("model", model);
		Sabana.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -210.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		vallasdes.Draw(staticShader);
		
		// ====================================================== Bosque ======================================================
		tmp = model = glm::mat4(1.0f);
		staticShader.setMat4("model", model);
		bosque.Draw(staticShader);
		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -210.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		vallasdes.Draw(staticShader);

		// ======================================================= Selva =======================================================
		tmp = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		selva.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-205.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		vallasdes.Draw(staticShader); 
		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -426.0f));
		model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		vallasdes.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Bisonte
		// -------------------------------------------------------------------------------------------------------------------------

		// Cuerpo
		model = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 10.0f, 20.0f));
		model = glm::translate(model, glm::vec3(movX, movY, movZ));
		tmp = model = glm::rotate(model, glm::radians(giroBisonte), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(25.0f));
		staticShader.setMat4("model", model);
		cuerpo_Bisonte.Draw(staticShader);

		// Pata Delantera Derecha
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		model = glm::translate(model, glm::vec3(-1.138f, -0.139f, -0.629f));
		model = glm::rotate(model, glm::radians(rotPDelanDer), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		pata_DelanDer.Draw(staticShader);

		// Pata Delantera Izquierda
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		model = glm::translate(model, glm::vec3(-1.345f, -0.137f, -0.635f));
		model = glm::rotate(model, glm::radians(rotPDelanIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		pata_DelanIzq.Draw(staticShader);

		// Pata Trasera Derecha
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		model = glm::translate(model, glm::vec3(-1.157f, -0.117f, -0.182f));
		model = glm::rotate(model, glm::radians(rotPTrasDer), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		pata_TrasDer.Draw(staticShader);

		// Pata Trasera Izquierda
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		model = glm::translate(model, glm::vec3(-1.31f, -0.128f, -0.181f));
		model = glm::rotate(model, glm::radians(rotPTrasIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		pata_TrasIzq.Draw(staticShader);

		// Cola
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		model = glm::translate(model, glm::vec3(-1.238f, 0.003f, -0.047f));
		model = glm::rotate(model, glm::radians(colaBisonte), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		cola_Bisonte.Draw(staticShader);
		


		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------

		
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	// Bisonte
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		movX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		movX++;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		movY++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		movY--;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		movZ++;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		movZ--;

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroBisonte--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroBisonte++;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotPDelanDer -= 3.0f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotPDelanDer += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotPDelanIzq += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotPDelanIzq -= 3.0f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotPTrasDer += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotPTrasDer -= 3.0f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotPTrasIzq -= 3.0f;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotPTrasIzq += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		colaBisonte += 3.0f;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		colaBisonte -= 3.0f;



	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndexBisont < MAX_FRAMES)
		{
			saveFrame();
		}
	}
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}