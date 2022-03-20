// C++ Libraries
#include <iostream>
#include <vector>

// GLEW
#include <GL/glew.h>

// FREE GLUT
#include <GL/freeglut.h>

// CYCODE
#include "cyGL.h"
#include "cyTriMesh.h"

// GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>

// lodepng
#include "lodepng.h"

// Screen Variables
int window_width = 1920;
int window_height = 1080;

// CyCode Variables
cy::TriMesh tea_mesh; // global variable for storing the scene data
cy::TriMesh cube_mesh;
cy::GLSLProgram teaProgram;  // global variable for compiling and running shader program
cy::GLSLProgram quadProgram; // global variable for compiling and running shader program
cy::GLSLProgram cubeProgram;

// Vertex Array Objects
GLuint teaVertexArrayObject; // global variable for storing vertex array data for teapot
GLuint quadVertexArrayObject;
GLuint cubeVertexArrayObject;

// Vertex Buffer Objects
GLuint vertexBufferObject; // global variable for storing vertex buffer data
GLuint quadVertexBufferObject;
GLuint cubeVertexBufferObject;

// Vertex Normal Objects
GLuint normalBufferObject; // global variable for storing normal buffer data

// Texture Buffer Objects
GLuint diffuseTextureBufferObject; // global variable for storing diffuse texture buffer data
GLuint cubeTextureBufferObject;

// Textures
GLuint diffuseTexture; // global variable for storing diffuse texture data
GLuint cubeTexture;

// Render to Texture
GLuint frameBuffer;
GLuint depthBuffer;
GLuint renderedTexture;

// Speed Variable - for controlling refresh/rendering rate
float speed = 0.2f;

// TEAPOT VARIABLES
// Camera Transform Variables
glm::mat4 modelMatrix;      // global variable for representing the model space with respect to the camera
glm::mat4 viewMatrix;       // global variable for representing the view space with respect to the camera
glm::mat4 projectionMatrix; // global variable for representing the real world space with respect to the camera
glm::mat4 mvMatrix;         // global variable for representing the model-view space
glm::mat4 mvpMatrix;        // global variable for representing the model-view-projection space
glm::mat4 mvnMatrix;        // global variable for representing the model-view-normal space

// Light Transform Variables
glm::mat4 lightModelMatrix; // global variable for representing the model space with respect to the light
glm::mat4 lightViewMatrix;  // global variable for representing the view space with respect to the light
glm::mat4 lightMVMatrix;    // global variable for representing the model-view space with respect to the light

// Camera Vectors
glm::vec3 cameraPosition; // global variable for representing the camera location
glm::vec3 cameraTarget;   // global variable for representing where the camera is pointing at
glm::vec3 cameraUp;       // global variable for representing whether the camera is pointing up or down

// Camera Previous State Variables
float anglePrevX;
float anglePrevY;
float camDistancePrev;

// Camera Current State Variables
float currentAngleX;
float currentAngleY;
float currentCameraDistance;

// Camera Default Settings
float yaw = 0.0f;   // global variable for controlling horiziontal camera movement
float pitch = 0.0f; // global variable for controlling vertical camera movement
float FOV = 45.0f;  // global variable for controlling field of view (zoom)

// LIGHT VARIABLES
// Light Vectors
glm::vec4 lightPosition;     // global variable for representing the light position for light movement
glm::vec3 lightViewPosition; // global variable for representing the light location
glm::vec3 lightTarget;       // global variable for representing
glm::vec3 lightUp;           // global variable for representing where whether the light is pointing up or down

// Light Previous State Variables
float lightAnglePrevX;
float lightAnglePrevY;

// Light Current State Variables
float lightCurrentAngleX;
float lightCurrentAngleY;

// Light Default Settings
float lightYaw = 0.0f;   // global variabe for controlling horizontal light movement
float lightPitch = 0.0f; // global variable for controlling vertical light movement

// Mouse Event Variables (User Interaction Controls)
bool leftDown = false;
bool rightDown = false;
bool ctrlDown = false;

// Scene Variables
std::vector<cy::Vec3f> triangles; // global variable for storing triangle faces from the scene
std::vector<cy::Vec3f> normals;   // global variable for storing normal faces from the scene
std::vector<cy::Vec3f> cubes;

// Texture Coordinates
// Diffuse Texture Coordinates
std::vector<cy::Vec2f> diffuseTextureCoords;

// Texture Data
// Diffuse Texture
std::vector<unsigned char> diffuseTextureData;
unsigned diffuseWidth, diffuseHeight;

// Environment Texture
std::vector<unsigned char> cube1;
std::vector<unsigned char> cube2;
std::vector<unsigned char> cube3;
std::vector<unsigned char> cube4;
std::vector<unsigned char> cube5;
std::vector<unsigned char> cube6;
unsigned cubeWidth, cubeHeight;

// PLANE VARIABLES
glm::mat4 planeModelMatrix;      // global variable for representing the model space with respect to the camera
glm::mat4 planeViewMatrix;       // global variable for representing the view space with respect to the camera
glm::mat4 planeProjectionMatrix; // global variable for representing the real world space with respect to the camera
glm::mat4 planemvMatrix;         // global variable for representing the model-view space
glm::mat4 planemvpMatrix;        // global variable for representing the model-view-projection space

// Camera Vectors
glm::vec3 planeCameraPosition; // global variable for representing the camera location
glm::vec3 planeCameraTarget;   // global variable for representing where the camera is pointing at
glm::vec3 planeCameraUp;       // global variable for representing whether the camera is pointing up or down

// Camera Previous State Variables
float planePrevAngleX;
float planeAnglePrevY;
float planeCameraDistancePrev;

// Camera Current State Variables
float planeCurrentAngleX;
float planeCurrentAngleY;
float planeCurrentCameraDistance;

// Camera Default Settings
float planeYaw = 0.0f;   // global variable for controlling horiziontal camera movement
float planePitch = 0.0f; // global variable for controlling vertical camera movement
float planeFOV = 45.0f;  // global variable for controlling field of view (zoom)

// Mouse Event Variables (User Interaction Controls)
bool altDown = false;

// Initialization Functions
/// <summary>
/// Initialize the tea camera
/// </summary>
void teaCameraInit()
{
    modelMatrix = glm::mat4(1.0f);

    cameraPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    viewMatrix = glm::lookAt(
        cameraPosition, // CAMERA POSITION
        cameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        cameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    projectionMatrix = glm::perspective(
        glm::radians(FOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);
}

/// <summary>
/// Initialize the scene lighting
/// </summary>
void lightInit()
{
    lightModelMatrix = glm::mat4(1.0f);

    lightViewPosition = glm::vec3(70.0f, 50.0f, 0.0f);
    lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    lightUp = glm::vec3(0.0f, 1.0f, 0.0f);
    lightViewMatrix = glm::lookAt(
        lightViewPosition, // LIGHT POSITION
        lightTarget,       // and looks at the origin (camera location) LIGHT DIRECTION
        lightUp            // Head is up (set to 0,-1,0 to look upside-down) // LIGHT RIGHT
    );
}

/// <summary>
/// Initialize quad camera
/// </summary>
void quadCameraInit()
{
    planeModelMatrix = glm::mat4(1.0f);

    planeCameraPosition = glm::vec3(0.0f, -1.0f, 1.0f);
    planeCameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    planeCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    planeViewMatrix = glm::lookAt(
        planeCameraPosition, // CAMERA POSITION
        planeCameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        planeCameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    planeProjectionMatrix = glm::perspective(
        glm::radians(planeFOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);
}

/// <summary>
/// Camera motion in the horizontal and vertical directions
/// </summary>
void tea_transformCamera()
{
    projectionMatrix = glm::perspective(
        glm::radians(FOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);

    viewMatrix = glm::lookAt(
        cameraPosition, // CAMERA POSITION
        cameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        cameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    viewMatrix = glm::translate(viewMatrix, cameraTarget);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(yaw), glm::vec3(0, 1, 0));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(pitch), glm::vec3(1, 0, 0));
}

/// <summary>
/// Camera motion in the horizontal and vertical directions
/// </summary>
void quad_transformCamera()
{
    planeProjectionMatrix = glm::perspective(
        glm::radians(planeFOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);

    planeViewMatrix = glm::lookAt(
        planeCameraPosition, // CAMERA POSITION
        planeCameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        planeCameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    planeViewMatrix = glm::translate(planeViewMatrix, planeCameraTarget);
    planeViewMatrix = glm::rotate(planeViewMatrix, glm::radians(planeYaw), glm::vec3(0, 1, 0));
    planeViewMatrix = glm::rotate(planeViewMatrix, glm::radians(planePitch), glm::vec3(1, 0, 0));
}

/// <summary>
/// Light motion in the horizontal and vertical directions
/// </summary>
void light_Transform()
{
    lightViewMatrix = glm::lookAt(
        lightViewPosition, // CAMERA POSITION
        lightTarget,       // and looks at the origin (camera location) CAMERA DIRECTION
        lightUp            // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    lightViewMatrix = glm::translate(lightViewMatrix, lightTarget);
    lightViewMatrix = glm::rotate(lightViewMatrix, glm::radians(-lightPitch), glm::vec3(0, 0, 1));
    lightViewMatrix = glm::rotate(lightViewMatrix, glm::radians(lightYaw), glm::vec3(0, 1, 0));
}

/// <summary>
/// Define Bounding Box for rotation about center for scene
/// </summary>
void boundingBox()
{
    tea_mesh.ComputeBoundingBox();
    cy::Vec3f boundBoxMin = tea_mesh.GetBoundMin();
    cy::Vec3f boundBoxMax = tea_mesh.GetBoundMax();
    float yMidpoint = -0.05 * (boundBoxMax.y + boundBoxMin.y) / 2;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, yMidpoint));
}

// Callback Functions
/// <summary>
/// Function for handling non-special FreeGlut key events
/// </summary>
/// <param name="key">Key selected by user</param>
/// <param name="x">Screen x-coordinate when key is pressed by user</param>
/// <param name="y">Screen y-coordinate when key is pressed by user</param>
void myKeyboard(unsigned char key, int x, int y)
{
    // GLUT uses ascii for key events
    switch (key)
    {
    case 27:                 // escape key
        glutLeaveMainLoop(); // Exit main loop and clean up resources
        break;
    default:
        break;
    }
    return;
}

/// <summary>
/// Function for handling function key events with FreeGlut
/// Supports recompiling shaders with f-6
/// </summary>
/// <param name="key">FreeGlut Special Key pressed by user</param>
/// <param name="x">Screen x-coordinate when key is pressed by user</param>
/// <param name="y">Screen y-coordinate when key is pressed by user</param>
void mySpecialKeyboard(int key, int x, int y)
{
    // GLUT uses ascii for key events
    switch (key)
    {
    case GLUT_KEY_F6: // Recompile Shaders
        bool shader_recompile = teaProgram.BuildFiles("tea.vert", "tea.frag");
        break;
    }
    return;
}

/// <summary>
/// Mouse Control definitions for camera angle, light angle and zoom (Field-Of-View)
/// </summary>
/// <param name="button">button selected by user</param>
/// <param name="state">flag indicating whether a button has been pressed down or lifted</param>
/// <param name="x">Screen x-coordinate of mouse location when a button is clicked</param>
/// <param name="y">Screen y-coordinate of mouse location when a button is clicked</param>
void myMouse(int button, int state, int x, int y)
{
    int mod;
    mod = glutGetModifiers();
    if (mod == GLUT_ACTIVE_ALT)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            planePrevAngleX = (float)x;
            planeAnglePrevY = (float)y;

            leftDown = true;
            rightDown = false;
            ctrlDown = false;
            altDown = true;
        }

        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            leftDown = false;
            rightDown = false;
            ctrlDown = false;
            altDown = false;
        }

        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            planeCameraDistancePrev = (float)y;

            leftDown = false;
            rightDown = true;
            ctrlDown = false;
            altDown = true;
        }

        if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
        {
            leftDown = false;
            rightDown = false;
            ctrlDown = false;
            altDown = false;
        }
    }

    else if (mod == GLUT_ACTIVE_CTRL)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            lightAnglePrevX = (float)x;
            lightAnglePrevY = (float)y;

            leftDown = true;
            rightDown = false;
            ctrlDown = true;
            altDown = false;
        }

        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            leftDown = false;
            rightDown = false;
            ctrlDown = false;
            altDown = false;
        }
    }

    else
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            anglePrevX = (float)x;
            anglePrevY = (float)y;

            leftDown = true;
            rightDown = false;
            ctrlDown = false;
            altDown = false;
        }

        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            leftDown = false;
            rightDown = false;
            ctrlDown = true;
            altDown = false;
        }

        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            camDistancePrev = (float)y;

            leftDown = false;
            rightDown = true;
            ctrlDown = false;
            altDown = false;
        }

        if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
        {
            leftDown = false;
            rightDown = false;
            ctrlDown = false;
            altDown = false;
        }
    }
}

/// <summary>
/// Camera Angle, Light Angle and Field-Of-View calculations based on mouse motion
/// </summary>
/// <param name="x">Screen x-coordinate for where mouse is located</param>
/// <param name="y">Screen y-coordinate for where mouse is located</param>
void myMouseMotion(int x, int y)
{
    // Variables for storing the current camera angle and current distance
    currentAngleX = (float)x;
    currentAngleY = (float)y;
    currentCameraDistance = (float)y;

    lightCurrentAngleX = (float)x;
    lightCurrentAngleY = (float)y;

    planeCurrentAngleX = (float)x;
    planeCurrentAngleY = (float)y;
    planeCurrentCameraDistance = (float)y;

    // Update Camera angle
    if (leftDown == true && ctrlDown == false && altDown == false)
    {
        // Calculate the differential x and y when the mouse moves and store the previous coordinate
        float dx = currentAngleX - anglePrevX;
        float dy = currentAngleY - anglePrevY;
        anglePrevX = currentAngleX;
        anglePrevY = currentAngleY;

        // Scale the differential for a smooth transition
        dx *= speed;
        dy *= speed;

        // Set yaw and pitch
        yaw += dx;
        pitch += dy;

        // Update Camera View
        tea_transformCamera();
    }

    // Update Light Angle
    if (leftDown == true && ctrlDown == true && altDown == false)
    {
        float dx = lightCurrentAngleX - lightAnglePrevX;
        float dy = lightCurrentAngleY - lightAnglePrevY;
        lightAnglePrevX = lightCurrentAngleX;
        lightAnglePrevY = lightCurrentAngleY;

        // Scale the differential for a smooth transition
        dx *= speed;
        dy *= speed;

        // Set yaw and pitch
        lightYaw += dx;
        lightPitch += dy;

        // Update Scene Lighting
        light_Transform();
    }

    // Update Field of View
    if (rightDown == true && ctrlDown == false && altDown == false)
    {
        float dy = 0.0f;
        dy += currentCameraDistance - camDistancePrev;
        camDistancePrev = currentCameraDistance;
        dy *= speed;
        FOV += dy;
        if (FOV < 10.0f)
        {
            FOV = 10.0f;
        }
        if (FOV > 90.0f)
        {

            FOV = 90.0f;
        }
        // Update Camera View
        tea_transformCamera();
    }

    // Update Camera angle
    if (leftDown == true && altDown == true && ctrlDown == false)
    {
        // Calculate the differential x and y when the mouse moves and store the previous coordinate
        float dx = planeCurrentAngleX - planePrevAngleX;
        float dy = planeCurrentAngleY - planeAnglePrevY;
        planePrevAngleX = planeCurrentAngleX;
        planeAnglePrevY = planeCurrentAngleY;

        // Scale the differential for a smooth transition
        dx *= speed;
        dy *= speed;

        // Set yaw and pitch
        planeYaw += dx;
        planePitch += dy;

        // Update Camera View
        quad_transformCamera();
    }

    // Update Field of View
    if (rightDown == true && altDown == true && ctrlDown == false)
    {
        float dy = 0.0f;
        dy += planeCurrentCameraDistance - planeCameraDistancePrev;
        planeCameraDistancePrev = planeCurrentCameraDistance;
        dy *= speed;
        planeFOV += dy;
        if (planeFOV < 10.0f)
        {
            planeFOV = 10.0f;
        }
        if (planeFOV > 90.0f)
        {

            planeFOV = 90.0f;
        }
        // Update Camera View
        quad_transformCamera();
    }
}

/// <summary>
/// Animation and refresh rate for rendering
/// </summary>
void myIdle()
{
    // Tell GLUT to redraw in the main loop
    glutPostRedisplay();
}

/// <summary>
/// Load data for triangles, normals and textures
/// </summary>
void loadTeapotData()
{
    // Load Triangles
    for (int i = 0; i < tea_mesh.NF(); i++)
    {
        cy::Vec3f vertex1 = tea_mesh.V(tea_mesh.F(i).v[0]);
        cy::Vec3f vertex2 = tea_mesh.V(tea_mesh.F(i).v[1]);
        cy::Vec3f vertex3 = tea_mesh.V(tea_mesh.F(i).v[2]);
        triangles.push_back(vertex1);
        triangles.push_back(vertex2);
        triangles.push_back(vertex3);
    }

    // Load Normals
    for (int j = 0; j < tea_mesh.NF(); j++)
    {
        cy::Vec3f normals1 = tea_mesh.VN(tea_mesh.FN(j).v[0]);
        cy::Vec3f normals2 = tea_mesh.VN(tea_mesh.FN(j).v[1]);
        cy::Vec3f normals3 = tea_mesh.VN(tea_mesh.FN(j).v[2]);
        normals.push_back(normals1);
        normals.push_back(normals2);
        normals.push_back(normals3);
    }
}

/// <summary>
/// Load and build the buffers for the teapot scene
/// </summary>
void loadTeapotBuffers()
{

    // Vertex Array Object
    glGenVertexArrays(1, &teaVertexArrayObject);
    glBindVertexArray(teaVertexArrayObject);

    // Create Buffers
    // Vertex Buffer
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec3f>) * triangles.size(),
        &triangles[0],
        GL_STATIC_DRAW);

    // Normal Buffer
    glGenBuffers(1, &normalBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec3f>) * normals.size(),
        &normals[0],
        GL_STATIC_DRAW);
}

/// <summary>
/// Bind Buffers to Vertices for Teapot
/// </summary>
void teapotBindBuffersToVertices()
{
    // Assign Vertex Buffer Objects to Vertex Attributes
    GLuint pos = glGetAttribLocation(teaProgram.GetID(), "pos");
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glVertexAttribPointer(
        pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // Assign Normal Buffer Objects to Vertex Attributes
    GLuint normalPos = glGetAttribLocation(teaProgram.GetID(), "normalPos");
    glEnableVertexAttribArray(normalPos);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
    glVertexAttribPointer(
        normalPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
}

/// <summary>
/// Load Cube Environment texture
/// </summary>
GLuint environmentTexture()
{
    // Load textures
    unsigned error1 = lodepng::decode(cube1, cubeWidth, cubeHeight, "../scene/cubemap_posx.PNG");
    unsigned error2 = lodepng::decode(cube2, cubeWidth, cubeHeight, "../scene/cubemap_negx.PNG");
    unsigned error3 = lodepng::decode(cube3, cubeWidth, cubeHeight, "../scene/cubemap_posy.PNG");
    unsigned error4 = lodepng::decode(cube4, cubeWidth, cubeHeight, "../scene/cubemap_negy.PNG");
    unsigned error5 = lodepng::decode(cube5, cubeWidth, cubeHeight, "../scene/cubemap_posz.PNG");
    unsigned error6 = lodepng::decode(cube6, cubeWidth, cubeHeight, "../scene/cubemap_negz.PNG");

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, cubeWidth, cubeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cube1[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, cubeWidth, cubeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cube2[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, cubeWidth, cubeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cube3[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, cubeWidth, cubeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cube4[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, cubeWidth, cubeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cube5[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, cubeWidth, cubeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cube6[0]);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return texID;
}

/// <summary>
/// Load Cube Data
/// </summary>
void loadCubeData()
{
    // Load cubes
    for (int i = 0; i < cube_mesh.NF(); i++)
    {
        cy::Vec3f vertex1 = cube_mesh.V(cube_mesh.F(i).v[0]);
        cy::Vec3f vertex2 = cube_mesh.V(cube_mesh.F(i).v[1]);
        cy::Vec3f vertex3 = cube_mesh.V(cube_mesh.F(i).v[2]);
        cubes.push_back(vertex1);
        cubes.push_back(vertex2);
        cubes.push_back(vertex3);
    }
}

/// <summary>
/// Load  Cube Buffer Objects
/// </summary>
void loadCubeBuffers()
{
    // Vertex Array Object
    glGenVertexArrays(1, &cubeVertexArrayObject);
    glBindVertexArray(cubeVertexArrayObject);

    // Create Buffers
    // Vertex Buffer
    glGenBuffers(1, &cubeVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec3f>) * cubes.size(),
        &cubes[0],
        GL_STATIC_DRAW);
}

/// <summary>
/// Bind Cube Vertices to Object
/// </summary>
void cubeBindBuffers()
{
    GLuint cubePos = glGetAttribLocation(cubeProgram.GetID(), "cubePos");
    glEnableVertexAttribArray(cubePos);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferObject);
    glVertexAttribPointer(
        cubePos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
}

/// <summary>
/// Load and render the teapot
/// </summary>
void teaInit()
{
    loadTeapotData();                                                    // load teapot scene data
    loadTeapotBuffers();                                                 // load the buffers for the teapot
    bool shader_compile = teaProgram.BuildFiles("tea.vert", "tea.frag"); // compile the shaders
    teapotBindBuffersToVertices();                                       // bind the buffers to the vertices
    boundingBox();                                                       // bounding box
}

/// <summary>
/// Load and render the cube
/// </summary>
void cubesInit()
{
    cubeTexture = environmentTexture();
    loadCubeData();
    loadCubeBuffers();
    bool shader_compile = cubeProgram.BuildFiles("cubes.vert", "cubes.frag");
    cubeBindBuffers();
    // boundingBox();
}

/// <summary>
/// Load and render the plane
/// </summary>
void quadInit()
{
    static const GLfloat quad_vertex_buffer_data[] = {
        -1.0f,
        -1.0f,
        0.0f,
        1.0f,
        -1.0f,
        0.0f,
        -1.0f,
        1.0f,
        0.0f,
        -1.0f,
        1.0f,
        0.0f,
        1.0f,
        -1.0f,
        0.0f,
        1.0f,
        1.0f,
        0.0f,
    };

    // Vertex Array Object
    glGenVertexArrays(1, &quadVertexArrayObject);
    glBindVertexArray(quadVertexArrayObject);

    // Vertex Buffer Object
    glGenBuffers(1, &quadVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_buffer_data), quad_vertex_buffer_data, GL_STATIC_DRAW);

    // Compile shaders
    bool quad_compile_shaders = quadProgram.BuildFiles("plane.vert", "plane.frag");

    // Assign Vertex Buffer Objects to Vertex Attributes
    GLuint plane_pos = glGetAttribLocation(quadProgram.GetID(), "plane_pos");
    glEnableVertexAttribArray(plane_pos);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBufferObject);
    glVertexAttribPointer(
        plane_pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
}

/// <summary>
/// Configure the frame buffer to render to texture
/// </summary>
void renderBufferInit()
{
    // FRAME BUFFER
    frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // TEXTURE
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // DEPTH BUFFER
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // CONFIGURE FRAME BUFFER
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "THERE WAS AN ERROR WITH THE FRAME BUFFER! GET SHREKT!" << std::endl;
    }
}

/// <summary>
/// Render Scene data to screen
/// </summary>
void render()
{

    // Specify OpenGL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LESS);
    glEnable(GL_TEXTURE_2D);

    // Bind Frame Buffer
    glViewport(0, 0, window_width, window_height);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind tea shader program
    teaProgram.Bind();

    // Compute the different matrices and vectors for rendering the scene
    mvMatrix = viewMatrix * modelMatrix;
    lightPosition = glm::vec4(107.780487f, 83.130577f, 0.0f, 1.0f);
    lightMVMatrix = lightViewMatrix * lightModelMatrix;
    lightPosition = lightMVMatrix * lightPosition;
    mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    mvnMatrix = glm::inverseTranspose(mvMatrix);

    //// Set Uniform ID Variables for the Shader Program
    GLuint mvpID = glGetUniformLocation(teaProgram.GetID(), "mvp");
    GLuint mvID = glGetUniformLocation(teaProgram.GetID(), "mv");
    GLuint lightPosID = glGetUniformLocation(teaProgram.GetID(), "lightPos");
    GLuint mvnID = glGetUniformLocation(teaProgram.GetID(), "mvn");

    //// Set Uniform Data Variables for the Shader Program
    glUniformMatrix4fv(mvID, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(mvnID, 1, GL_FALSE, &mvnMatrix[0][0]);
    glUniform3f(lightPosID, lightPosition.x, lightPosition.y, lightPosition.z);

    //// Bind vertex data to vertices of scene
    glBindVertexArray(teaVertexArrayObject);

    //// Render Data to the Screen
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());
    // glGenerateTextureMipmap(renderedTexture);

    //////// RENDER TO TEXTURE SCREEN
    /*  glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, window_width, window_height);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);*/
    /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

    //////// Bind Quad Program
    quadProgram.Bind();

    //////// Bind our texture in Texture Unit 0
    // GLuint teaTextureID = glGetUniformLocation(quadProgram.GetID(), "renderTexture");
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, renderedTexture);
    // glUniform1i(teaTextureID, 0);

    //////// Compute the different matrices and vectors for rendering the scene
    planemvMatrix = planeViewMatrix * planeModelMatrix;
    planemvpMatrix = planeProjectionMatrix * planeViewMatrix * planeModelMatrix;

    //////// Set Uniform ID Variables for the Shader Program
    GLuint plane_mvpID = glGetUniformLocation(quadProgram.GetID(), "plane_mvp");

    //////// Set Uniform Data Variables for the Shader Program
    glUniformMatrix4fv(plane_mvpID, 1, GL_FALSE, &planemvpMatrix[0][0]);

    //////// Bind vertex data to vertices of scene
    glBindVertexArray(quadVertexArrayObject);

    //////// Render Data to the Screen
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);
    cubeProgram.Bind();
    GLuint cube_mvpID = glGetUniformLocation(cubeProgram.GetID(), "mvp");
    glUniformMatrix4fv(cube_mvpID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glBindVertexArray(cubeVertexArrayObject);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    glutSwapBuffers(); // Swap buffers (front and back buffers) to give most up to date rendering of scene
}

using namespace std;

int main(int argc, char **argv)
{
    // GLUT Initialization
    glutInitContextVersion(4, 5); // Initialize GLUT Context Version
    glutInit(&argc, argv);        // Initialize GLUT

    // Create a Window
    glutInitWindowSize(window_width, window_height);           // specify width, height of window
    glutInitWindowPosition(0, 0);                              // specify the position of the window (x, y) coordinate for center
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // Initialize the display
    glutCreateWindow("Window Title");

    // Initialize GLEW
    glewInit();

    // Callback Functions
    glutDisplayFunc(render);            // rendering callback function
    glutKeyboardFunc(myKeyboard);       // keyboard callback function
    glutSpecialFunc(mySpecialKeyboard); // special keyboard callback function
    glutMouseFunc(myMouse);             // mouse click event callback function
    glutMotionFunc(myMouseMotion);      // mouse motion callback function
    glutIdleFunc(myIdle);               // IDLE callback function (for animation)

    // Initialize Scene
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);               // Clear the previous color and initialize background to black
    teaCameraInit();                                    // initialize camera
    quadCameraInit();                                   // intialize plane camera
    lightInit();                                        // initialize scene lighting
    bool success = tea_mesh.LoadFromFileObj(argv[1]);   // Load scene from .obj files
    bool success2 = cube_mesh.LoadFromFileObj(argv[2]); // Load scene from .obj files
    teaInit();                                          // load and render the teapot
    // renderBufferInit();
    // quadInit(); // load and render the plane
    cubesInit();
    glutMainLoop(); // Call main loop for rendering
    return 0;       // Exit when main loop is done
}