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
// #include "../libraries/glm/gtc/matrix_transform.hpp"
// #include "../libraries/glm/gtx/transform.hpp"
// #include "../libraries/glm/gtc/matrix_inverse.hpp"
// #include "../libraries/glm/gtx/string_cast.hpp"

// Global Variable Initialization
cy::TriMesh mesh;                   // global variable for storing the scene data
cy::GLSLProgram program;            // global variable for compiling and running shader program
GLuint vertexArrayObject;           // global variable for storing vertex array data
GLuint vertexBufferObject;          // global variable for strogin vertex buffer data
GLuint normalBufferObject;          // global variable for storing normal buffer data
GLuint diffuseTextureBufferObject;  // global variable for storing diffuse texture buffer data
GLuint specularTextureBufferObject; // global variable for storing specular texture buffer data

// Speed Variable - for controlling refresh/rendering rate
float speed = 0.2f;

// Matrix Transform Variables
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

// Camera Variables
// Camera Vectors
glm::vec3 cameraPosition; // global variable for representing the camera location
glm::vec3 cameraTarget;   // global variable for representing where the camera is pointing at
glm::vec3 cameraUp;       // global variable for representing whether the camera is pointing up or down
// Camera Previous State Variables
float angle_prev_x;
float angle_prev_y;
float camera_distance_prev;
// Camera Current State Variables
float current_angle_x;
float current_angle_y;
float current_camera_distance;
// Camera Default Settings
float yaw = 0.0f;   // global variable for controlling horiziontal camera movement
float pitch = 0.0f; // global variable for controlling vertical camera movement
float FOV = 45.0f;  // global variable for controlling field of view (zoom)

// Light Variables
// Light Vectors
glm::vec4 lightPosition;     // global variable for representing the light position for light movement
glm::vec3 lightViewPosition; // global variable for representing the light location
glm::vec3 lightTarget;       // global variable for representing
glm::vec3 lightUp;           // global variable for representing where whether the light is pointing up or down
// Light Previous State Variables
float light_angle_prev_x;
float light_angle_prev_y;
// Light Current State Variables
float light_current_angle_x;
float light_current_angle_y;
// Light Default Settings
float light_yaw = 0.0f;   // global variabe for controlling horizontal light movement
float light_pitch = 0.0f; // global variable for controlling vertical light movement

// Mouse Event Variables (User Interaction Controls)
bool leftDown = false;
bool rightDown = false;
bool ctrlDown = false;

// Scene Variables
std::vector<cy::Vec3f> triangles; // global variable for storing triangle faces from the scene
std::vector<cy::Vec3f> normals;   // global variable for storing normal faces from the scene

// Texture Coordinates
std::vector<cy::Vec2f> diffuseTextureCoords;
std::vector<cy::Vec2f> specularTextureCoords;

// Texture Data
// Diffuse Texture
std::vector<unsigned char> diffuseTexture;
unsigned diffuseWidth, diffuseHeight;
// Specular Texture
std::vector<unsigned char> specularTexture;
unsigned specularWidth, specularHeight;
// Initialization Functions

/// <summary>
/// Initialize the scene camera
/// </summary>
void initializeCamera()
{
    modelMatrix = glm::mat4(1.0f);

    cameraPosition = glm::vec3(0.0f, -2.0f, 1.0f);
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
void initializeLight()
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
/// Define Bounding Box for rotation about center for scene
/// </summary>
void boundingBox()
{
    mesh.ComputeBoundingBox();
    cy::Vec3f boundBoxMin = mesh.GetBoundMin();
    cy::Vec3f boundBoxMax = mesh.GetBoundMax();
    float zMidpoint = -0.05 * (boundBoxMax.z + boundBoxMin.z) / 2;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, zMidpoint));
}

// Transform Functions

/// <summary>
/// Camera motion in the horizontal and vertical directions
/// </summary>
void transformCamera()
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
    viewMatrix = glm::rotate(viewMatrix, glm::radians(pitch), glm::vec3(1, 0, 0));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(yaw), glm::vec3(0, 0, 1));
}

// Callback Functions

/// <summary>
/// Render Scene data to screen
/// </summary>
void myDisplay()
{
    // OpenGL Draw Calls Here
    // Clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute the different matrices and vectors for rendering the scene
    mvMatrix = viewMatrix * modelMatrix;
    lightPosition = glm::vec4(107.780487f, 83.130577f, 0.0f, 1.0f);
    lightMVMatrix = lightViewMatrix * lightModelMatrix;
    lightPosition = lightMVMatrix * lightPosition;
    mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    mvnMatrix = glm::inverseTranspose(mvMatrix);

    // Get ID values for passing to shader code to render scene
    GLuint mvpID = glGetUniformLocation(program.GetID(), "mvp");
    GLuint mvID = glGetUniformLocation(program.GetID(), "mv");
    GLuint lightPosID = glGetUniformLocation(program.GetID(), "lightPos");
    GLuint mvnID = glGetUniformLocation(program.GetID(), "mvn");

    // Store matrix and vector data as a uniform data variable for use in shader code
    glUniformMatrix4fv(mvID, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(mvnID, 1, GL_FALSE, &mvnMatrix[0][0]);
    glUniform3f(lightPosID, lightPosition.x, lightPosition.y, lightPosition.z);

    // Bind vertex data to vertices of scene
    glBindVertexArray(vertexArrayObject);

    // Apply Shaders to the Scene
    /*glUseProgram(program.GetID());*/
    program.Bind();

    // Render Data to the Screen
    // glDrawArrays(GL_POINTS, 0, sizeof(cy::Vec3f) * mesh.NV()); // render points
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());

    glutSwapBuffers(); // Swap buffers (front and back buffers) to give most up to date rendering of scene
}

/// <summary>
/// Light motion in the horizontal and vertical directions
/// </summary>
void transformLight()
{
    lightViewMatrix = glm::lookAt(
        lightViewPosition, // CAMERA POSITION
        lightTarget,       // and looks at the origin (camera location) CAMERA DIRECTION
        lightUp            // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    lightViewMatrix = glm::translate(lightViewMatrix, lightTarget);
    lightViewMatrix = glm::rotate(lightViewMatrix, glm::radians(-light_pitch), glm::vec3(0, 0, 1));
    lightViewMatrix = glm::rotate(lightViewMatrix, glm::radians(light_yaw), glm::vec3(0, 1, 0));
}

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
        bool shader_recompile = program.BuildFiles("shader.vert", "shader.frag");
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
    int mod; // variable for storing GLUT Modifier State
    switch (button)
    {
    case GLUT_LEFT_BUTTON: // set the camera angle / light angle
        if (state == GLUT_DOWN)
        {
            mod = glutGetModifiers();
            if (mod == (GLUT_ACTIVE_CTRL))
            {
                leftDown = true;
                ctrlDown = true;
                light_angle_prev_x = (float)x;
                light_angle_prev_y = (float)y;
            }
            else
            {
                leftDown = true;
                ctrlDown = false;
                angle_prev_x = (float)x;
                angle_prev_y = (float)y;
            }
        }
        if (state == GLUT_UP)
        {
            leftDown = false;
            ctrlDown = false;
        }
        break;
    case GLUT_RIGHT_BUTTON: // set the camera distance
        if (state == GLUT_DOWN)
        {
            camera_distance_prev = (float)y;
            rightDown = true;
        }
        if (state == GLUT_UP)
        {
            rightDown = false;
        }
    default:
        break;
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
    current_angle_x = (float)x;
    current_angle_y = (float)y;
    current_camera_distance = (float)y;
    light_current_angle_x = (float)x;
    light_current_angle_y = (float)y;

    // Update Camera angle
    if (leftDown == true && ctrlDown == false)
    {
        // Calculate the differential x and y when the mouse moves and store the previous coordinate
        float dx = current_angle_x - angle_prev_x;
        float dy = current_angle_y - angle_prev_y;
        angle_prev_x = current_angle_x;
        angle_prev_y = current_angle_y;

        // Scale the differential for a smooth transition
        dx *= speed;
        dy *= speed;

        // Set yaw and pitch
        yaw += dx;
        pitch += dy;

        // Update Camera View
        transformCamera();
    }

    // Update Light Angle
    if (leftDown == true && ctrlDown == true)
    {
        float dx = light_current_angle_x - light_angle_prev_x;
        float dy = light_current_angle_y - light_angle_prev_y;
        light_angle_prev_x = light_current_angle_x;
        light_angle_prev_y = light_current_angle_y;

        // Scale the differential for a smooth transition
        dx *= speed;
        dy *= speed;

        // Set yaw and pitch
        light_yaw += dx;
        light_pitch += dy;

        // Update Scene Lighting
        transformLight();
    }

    // Update Field of View
    if (rightDown == true && ctrlDown == false)
    {
        float dy = 0.0f;
        dy += current_camera_distance - camera_distance_prev;
        camera_distance_prev = current_camera_distance;
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
        transformCamera();
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

using namespace std;
int main(int argc, char **argv)
{
    // GLUT Initialization
    glutInitContextVersion(4, 5); // Initialize GLUT Context Version
    glutInit(&argc, argv);        // Initialize GLUT

    // Create a Window
    glutInitWindowSize(1920, 1080);                            // specify width, height of window
    glutInitWindowPosition(0, 0);                              // specify the position of the window (x, y) coordinate for center
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // Initialize the display
    glutCreateWindow("Window Title");

    // Initialize GLEW
    glewInit();

    // Specify OpenGL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LESS);
    glEnable(GL_CULL_FACE);

    // Callback Functions
    glutDisplayFunc(myDisplay);         // rendering callback function
    glutKeyboardFunc(myKeyboard);       // keyboard callback function
    glutSpecialFunc(mySpecialKeyboard); // special keyboard callback function
    glutMouseFunc(myMouse);             // mouse click event callback function
    glutMotionFunc(myMouseMotion);      // mouse motion callback function
    glutIdleFunc(myIdle);               // IDLE callback function (for animation)

    // Initialize Scene
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);         // Clear the previous color and initialize background to black
    initializeCamera();                           // initialize camera
    initializeLight();                            // initialize scene lighting
    bool success = mesh.LoadFromFileObj(argv[1]); // Load scene from .obj files
    // Load Triangles
    for (int i = 0; i < mesh.NF(); i++)
    {
        cy::Vec3f vertex1 = mesh.V(mesh.F(i).v[0]);
        cy::Vec3f vertex2 = mesh.V(mesh.F(i).v[1]);
        cy::Vec3f vertex3 = mesh.V(mesh.F(i).v[2]);
        triangles.push_back(vertex1);
        triangles.push_back(vertex2);
        triangles.push_back(vertex3);
    }
    // Load Normals
    for (int j = 0; j < mesh.NF(); j++)
    {
        cy::Vec3f normals1 = mesh.VN(mesh.FN(j).v[0]);
        cy::Vec3f normals2 = mesh.VN(mesh.FN(j).v[1]);
        cy::Vec3f normals3 = mesh.VN(mesh.FN(j).v[2]);
        normals.push_back(normals1);
        normals.push_back(normals2);
        normals.push_back(normals3);
    }

    // Vertex Array Object
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

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
    // Create Textures
    // Compile Shaders
    bool shader_compile = program.BuildFiles("shader.vert", "shader.frag");

    // Assign Vertex Buffer Objects to Vertex Attributes
    GLuint pos = glGetAttribLocation(program.GetID(), "pos");
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glVertexAttribPointer(
        pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // Assign Normal Buffer Objects to Vertex Attributes
    GLuint normalPos = glGetAttribLocation(program.GetID(), "normalPos");
    glEnableVertexAttribArray(normalPos);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
    glVertexAttribPointer(
        normalPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // Bounding Box
    boundingBox();

    // Set up rendering for the scene
    glutMainLoop(); // Call main loop for rendering
    return 0;       // Exit when main loop is done
}