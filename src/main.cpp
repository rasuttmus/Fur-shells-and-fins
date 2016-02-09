#include <sstream>
#include <istream>
#include <iostream>
#include <math.h>
#include <vector>

#include <AntTweakBar.h>

#include "../include/Scene.h"
#include "../include/Geometry.h"


// functions
int initializeOpenGL();
void initializeAntTweakBar();
void mouseButton(GLFWwindow *, int, int, int);
void mouseMotion(GLFWwindow *, double, double);
void mouseScroll(GLFWwindow *, double, double);
void keyboardInput(GLFWwindow *, int, int, int, int);
double calculateFPS(double, std::string);
void loadGeometryData();
void updateTweakBarVariables();


// AntTweakBar variables

// For meshes
typedef enum { SPHERE, TORUS, PLANE, MONKEY, BUNNY, TEAPOT } Meshes;
Meshes currentMesh = TORUS;
TwEnumVal MeshesEV[] = { {SPHERE, "Sphere"}, {TORUS, "Torus"}, {PLANE, "Plane"}, {MONKEY, "Monkey"}, {BUNNY, "Bunny"}, {TEAPOT, "Teapot"} };
TwType meshType;

// For noise types
NoiseType currentNoise = SIMPLEX;
TwEnumVal NoiseTypesEV[] = { { SIMPLEX, "Simplex" }, { WORLEY, "Worley" } };
TwType noiseType;

// Variables for the tweakBar
float furLength, specularity, transparency, shinyness, furNoiseLengthVariation, furNoiseSampleScale, furPatternScale;
glm::vec3 skinColor, ambientColor, diffuseColor, specularColor, furColor;



// Pointer objects
GLFWwindow * window = nullptr;

TwBar * tweakbar = nullptr;

Scene * scene = nullptr;

Geometry * mesh = nullptr;

Geometry * torus = nullptr;

Geometry * sphere = nullptr;

Geometry * plane = nullptr;

Geometry * monkey = nullptr;

Geometry * bunny = nullptr;

Geometry * teapot = nullptr;

// Global variables
std::string windowTitle = "Fur - Shells and Fins";

std::map<std::string, std::vector<std::string>> geometryData;


int main() {

    // Magic
    glewExperimental = GL_TRUE;

    // Init GLEW and GLFW
    if(initializeOpenGL() == -1) {
        return -1;
    }

    // Create scene here.
    scene = new Scene();

    loadGeometryData();

    std::cout << "\nPre-processing..." << std::endl;

    torus  = new Geometry(geometryData["torus"],  glm::vec3(0.5f, 0.4f, 0.3f), 24, 0.15f);
    sphere = new Geometry(geometryData["sphere"], glm::vec3(0.5f, 0.4f, 0.3f), 24, 0.15f, false);
    plane  = new Geometry(geometryData["plane"],  glm::vec3(0.5f, 0.4f, 0.3f), 24, 0.15f, false);
    monkey = new Geometry(geometryData["monkey"], glm::vec3(0.5f, 0.4f, 0.3f), 24, 0.15f, false);
    bunny  = new Geometry(geometryData["bunny"],  glm::vec3(0.5f, 0.4f, 0.3f), 24, 0.15f, false);
    teapot = new Geometry(geometryData["teapot"], glm::vec3(0.5f, 0.4f, 0.3f), 24, 0.15f, false);

    scene->addGeometry(torus);
    scene->addGeometry(sphere);
    scene->addGeometry(plane);
    scene->addGeometry(monkey);
    scene->addGeometry(bunny);
    scene->addGeometry(teapot);

    // Initialize scene
    scene->initialize();

    mesh = torus;

    // Initialze AntTweakBar
    initializeAntTweakBar();

    std::cout << "\nPre-processing complete!" << std::endl;

    // Bind mouse and keyboard callback functions
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, mouseMotion);
    glfwSetScrollCallback(window, mouseScroll);
    glfwSetKeyCallback(window, keyboardInput);

    float time, previousTime = 0;

    // Render-loop
    do {
        calculateFPS(1.0, windowTitle);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        time = static_cast<float>(glfwGetTime());

        scene->update(time - previousTime);
        scene->setCurrentTime(time);
        
        updateTweakBarVariables();

        previousTime = time;

        // Render scene
        scene->render();

        // Render AntTweakBar
        TwDraw();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while ( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );


    // Clean-up
    delete scene;

    // Uninitialize AntTweakBar
    TwTerminate();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return EXIT_SUCCESS;
}


int initializeOpenGL() {
    
    // Initialise GLFW
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( WIDTH, HEIGHT, windowTitle.c_str(), NULL, NULL);

    if( window == NULL ){

        fprintf(stderr, "Failed to open GLFW window. You might be having an old GPU\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {

        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    std::cout << "Running OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Ensure we can capture keyboard input
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    return 0;
}


void initializeAntTweakBar() {

    // Scale the font, since AntTweakBar doesn't like retina displays
    TwDefine(" GLOBAL fontscaling=2 ");

    TwInit(TW_OPENGL_CORE, NULL);

    // Another retina hack
    TwWindowSize(WIDTH * 1.96, HEIGHT * 1.96);

    // Create tweakbar and set its size
    tweakbar = TwNewBar("Properties");
    TwDefine("Properties size='400 700' ");

    meshType = TwDefineEnum("MeshType", MeshesEV, 6);

    noiseType = TwDefineEnum("NoiseType", NoiseTypesEV, 2);

    furLength               = mesh->getFurLength();
    skinColor               = mesh->getColor();
    ambientColor            = mesh->getAmbient();
    diffuseColor            = mesh->getDiffuse();
    specularColor           = mesh->getSpecular();
    furColor                = mesh->getFurColor();
    specularity             = mesh->getSpecularity();
    transparency            = mesh->getTransparency();
    shinyness               = mesh->getShinyness();
    furNoiseLengthVariation = mesh->getFurNoiseLengthVariation();
    furNoiseSampleScale     = mesh->getFurNoiseSampleScale();
    furPatternScale         = mesh->getFurPatternScale();


    // Mesh to be rendered

    TwAddVarRW(
            tweakbar,
            "Mesh",
            meshType,
            &currentMesh,
            NULL
        );

    // Light source power
    TwAddVarRW(
            tweakbar, 
            "Power", 
            TW_TYPE_FLOAT, 
            &scene->getLightSourcePower(),
            " group='Scene' label='Power' min=0 max=2 step=0.05 help='Light source power' "
        );

    // Wind velocity
    TwAddVarRW(
            tweakbar,
            "Wind Velocity",
            TW_TYPE_FLOAT,
            &scene->getWindVelocity(),
            " group='Scene' label='Wind velocity' min=0 max=1.5 step=0.05 help='Wind velocity' "
        );

    // Main color of material
    TwAddVarRW(tweakbar, 
            "Color", 
            TW_TYPE_COLOR3F, 
            &skinColor,
            " group='Material' label='Color' "
        );

    // Ambient component
    TwAddVarRW(tweakbar, 
            "Ambient", 
            TW_TYPE_COLOR3F, 
            &ambientColor, 
            " group='Material' label='Ambient' "
        );

    // Diffuse component
    TwAddVarRW(tweakbar, 
            "Diffuse", 
            TW_TYPE_COLOR3F, 
            &diffuseColor, 
            " group='Material' label='Diffuse' "
        );

    // Specular component
    TwAddVarRW(tweakbar, 
            "Specular", 
            TW_TYPE_COLOR3F, 
            &specularColor, 
            " group='Material' label='Specular' "
        );

    // Specularity of material
    TwAddVarRW(
            tweakbar, 
            "Specularity", 
            TW_TYPE_FLOAT, 
            &specularity,
            " group='Material' label='Specularity' min=1 max=50 step=1 help='Specularity of material' "
        );

    // Shinyness of material
    TwAddVarRW(
            tweakbar, 
            "Shinyness", 
            TW_TYPE_FLOAT, 
            &shinyness,
            " group='Material' label='Shinyness' min=0 max=1 step=0.01 help='Shinyness of material' "
        );

    // Transparency of material
    TwAddVarRW(
            tweakbar, 
            "Transparency", 
            TW_TYPE_FLOAT, 
            &transparency,
            " group='Material' label='Transparency' min=0 max=1 step=0.01 help='Transparency of material' "
        );

    // Fur main color
    TwAddVarRW(tweakbar, 
            "Fur Color", 
            TW_TYPE_COLOR3F, 
            &furColor,
            " group='Fur' label='Fur Color' "
        );

    // Fur length
    TwAddVarRW(
            tweakbar, 
            "Length", 
            TW_TYPE_FLOAT, 
            &furLength,
            " group='Fur' label='Length' min=0.1 max=0.5 step=0.005 help='Length of fur' "
        );

    // Fur length variation
    TwAddVarRW(
            tweakbar, 
            "Length Variation", 
            TW_TYPE_FLOAT, 
            &furNoiseLengthVariation,
            " group='Fur' label='Length Variation' min=0.0 max=0.5 step=0.01 help='Length variation of fur strands' "
        );

    // Fur length variation sample scale
    TwAddVarRW(
            tweakbar, 
            "Length Variation Sample Scale", 
            TW_TYPE_FLOAT, 
            &furNoiseSampleScale,
            " group='Fur' label='Length Variation Sample Scale' min=0.5 max=20.0 step=0.1 help='Sample scale of length variation noise' "
        );

    // Fur length variation sample scale
    TwAddVarRW(
            tweakbar, 
            "Scale of the fur pattern", 
            TW_TYPE_FLOAT, 
            &furPatternScale,
            " group='Fur' label='Pattern scale' min=0.0 max=20.0 step=0.1 help='Scale of the fur pattern' "
        );

    TwAddVarRW(
            tweakbar,
            "Noise type",
            noiseType,
            &currentNoise,
            " group='Fur' label='Noise type' help='Type of noise function' "
        );
}


void mouseButton(GLFWwindow * window, int button, int action, int mods) {
    
    if(!TwEventMouseButtonGLFW(button, action)) {

        if(button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        switch(action) {

            case GLFW_PRESS:

                double x, y;
                glfwGetCursorPos(window, &x, &y);
                scene->mousePress(x, y);
                break;

            case GLFW_RELEASE:

                scene->mouseRelease();
                
                break;

            default:
                
                break;
        }
    }
}


void mouseMotion(GLFWwindow * window, double x, double y) {
    
    if(!TwEventMousePosGLFW(x * 2, y * 2)) {
        scene->updateCameraPosition(x, y);
    }
}


void mouseScroll(GLFWwindow * window, double x, double y) {
    
    scene->updateCameraZoom(x, y);
}


void keyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if(action == GLFW_PRESS) {
        
        switch(key) {
            
            case GLFW_KEY_SPACE:
                
                scene->resetCamera();
                
                break;

            default:
                
                break;
        }
    }
}


double calculateFPS(double timeInterval = 1.0, std::string windowTitle = "NONE") {

    // Static values which only get initialised the first time the function runs
    static double startTime  =  glfwGetTime(); // Set the initial time to now
    static double fps        =  0.0;           // Set the initial FPS value to 0.0
 
    // Set the initial frame count to -1.0 (it gets set to 0.0 on the next line). Because
    // we don't have a start time we simply cannot get an accurate FPS value on our very
    // first read if the time interval is zero, so we'll settle for an FPS value of zero instead.
    static double frameCount =  -1.0;
 
    frameCount++;
 
    // Ensure the time interval between FPS checks is sane (low cap = 0.0 i.e. every frame, high cap = 10.0s)
    if (timeInterval < 0.0)
        timeInterval = 0.0;

    else if (timeInterval > 10.0)
        timeInterval = 10.0;
 
    // Get the duration in seconds since the last FPS reporting interval elapsed
    // as the current time minus the interval start time
    double duration = glfwGetTime() - startTime;
 
    // If the time interval has elapsed...
    if (duration > timeInterval) {

        // Calculate the FPS as the number of frames divided by the duration in seconds
        fps = round(frameCount / duration);
 
        // If the user specified a window title to append the FPS value to...
        if (windowTitle != "NONE") {

            // Convert the fps value into a string using an output stringstream
            std::ostringstream stream;
            stream << fps;
            std::string fpsString = stream.str();
 
            // Append the FPS value to the window title details
            windowTitle += " | FPS: " + fpsString;
 
            // Convert the new window title to a c_str and set it
            const char* pszConstString = windowTitle.c_str();
            glfwSetWindowTitle(window, pszConstString);
        }
        // If the user didn't specify a window to append the FPS to then output the FPS to the console
        else {
            
            std::cout << "FPS: " << fps << std::endl;
        }
 
        // Reset the frame count to zero and set the initial time to be now
        frameCount  = 0.0;
        startTime   = glfwGetTime();
    }
 
    // Return the current FPS - doesn't have to be used if you don't want it!
    return fps;
}


void loadGeometryData() {

    std::vector<std::string> data;
    data.resize(4);

    data[I_FILENAME] = "blender_monkey";
    data[I_TEXTURE]  = "monkey_tex";
    data[I_HAIRMAP]  = "monkey_hairmap";
    data[I_TEXSIZE]  = "1024";

    geometryData.insert(std::pair< std::string, std::vector<std::string> >("monkey", data));

    data[I_FILENAME] = "bunny";
    data[I_TEXTURE]  = "bunny_tex";
    data[I_HAIRMAP]  = "bunny_hairmap";
    data[I_TEXSIZE]  = "1024";

    geometryData.insert(std::pair< std::string, std::vector<std::string> >("bunny", data));

    data[I_FILENAME] = "sphere";
    data[I_TEXTURE]  = "bunny_tex";
    data[I_HAIRMAP]  = "bunny_hairmap";
    data[I_TEXSIZE]  = "512";

    geometryData.insert(std::pair< std::string, std::vector<std::string> >("sphere", data));

    data[I_FILENAME] = "torus";
    data[I_TEXTURE]  = "bunny_tex";
    data[I_HAIRMAP]  = "bunny_hairmap";
    data[I_TEXSIZE]  = "1024";

    geometryData.insert(std::pair< std::string, std::vector<std::string> >("torus", data));

    data[I_FILENAME] = "plane";
    data[I_TEXTURE]  = "bunny_tex";
    data[I_HAIRMAP]  = "bunny_hairmap";
    data[I_TEXSIZE]  = "512";

    geometryData.insert(std::pair< std::string, std::vector<std::string> >("plane", data));

    data[I_FILENAME] = "teapot";
    data[I_TEXTURE]  = "bunny_tex";
    data[I_HAIRMAP]  = "bunny_hairmap";
    data[I_TEXSIZE]  = "256";

    geometryData.insert(std::pair< std::string, std::vector<std::string> >("teapot", data));
}


void updateTweakBarVariables() {

    switch(currentMesh) {

        case TORUS:
            
            if(mesh != torus) {
                torus->setShallRender(true);
                sphere->setShallRender(false);
                plane->setShallRender(false);
                monkey->setShallRender(false);
                bunny->setShallRender(false);
                teapot->setShallRender(false);
                mesh = torus;
            }

            break;

        case SPHERE:
            
            if(mesh != sphere) {
                sphere->setShallRender(true);
                torus->setShallRender(false);
                plane->setShallRender(false);
                monkey->setShallRender(false);
                bunny->setShallRender(false);
                teapot->setShallRender(false);
                mesh = sphere;
            }

            break;

        case PLANE:
            
            if(mesh != plane) {
                plane->setShallRender(true);
                torus->setShallRender(false);
                sphere->setShallRender(false);
                monkey->setShallRender(false);
                bunny->setShallRender(false);
                teapot->setShallRender(false);
                mesh = plane;
            }

            break;

        case MONKEY:
            
            if(mesh != monkey) {
                monkey->setShallRender(true);
                torus->setShallRender(false);
                sphere->setShallRender(false);
                plane->setShallRender(false);
                bunny->setShallRender(false);
                teapot->setShallRender(false);
                mesh = monkey;
            }

            break;

        case BUNNY:
            
            if(mesh != bunny) {
                bunny->setShallRender(true);
                torus->setShallRender(false);
                sphere->setShallRender(false);
                plane->setShallRender(false);
                monkey->setShallRender(false);
                teapot->setShallRender(false);
                mesh = bunny;
            }

            break;

        case TEAPOT:
            
            if(mesh != teapot) {
                teapot->setShallRender(true);
                torus->setShallRender(false);
                sphere->setShallRender(false);
                plane->setShallRender(false);
                monkey->setShallRender(false);
                bunny->setShallRender(false);
                mesh = teapot;
            }

            break;

        default: 

            break;
    }


    switch(currentNoise) {

        case SIMPLEX:

            if(mesh->getNoiseType() != SIMPLEX) {
                mesh->setNoiseType(SIMPLEX);
            }

            break;

        case WORLEY:

            if(mesh->getNoiseType() != WORLEY) {
                mesh->setNoiseType(WORLEY);
            }

            break;

        default:

            break;
    }

    mesh->setFurLength(furLength);
    mesh->setColor(skinColor);
    mesh->setAmbient(ambientColor);
    mesh->setDiffuse(diffuseColor);
    mesh->setSpecular(specularColor);
    mesh->setFurColor(furColor);
    mesh->setSpecularity(specularity);
    mesh->setTransparency(transparency);
    mesh->setShinyness(shinyness);
    mesh->setFurNoiseLengthVariation(furNoiseLengthVariation);
    mesh->setFurNoiseSampleScale(furNoiseSampleScale);
    mesh->setFurPatternScale(furPatternScale);
}

