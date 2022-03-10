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

// LODE
#include "lodepng.h"

// Global variable initializations
// Object and Mesh Data Variables
cy::TriMesh mesh;        // global variable for loading and storing scene data (.obj files)
cy::GLSLProgram program; // global variable for compiling and binding shader program
cy::GLTexture2D tex;
GLuint vertexArrayObject;           // global variable for the vertex array object
GLuint vertexBufferObject;          // global variable for the vertex buffer object
GLuint normalBufferObject;          // global variable for the normal buffer object
GLuint diffuseTextureBufferObject;  // global variable for the diffuse texture buffer object
GLuint specularTextureBufferObject; // global vafiable for the specular texture buffer object;

// Speed Variables
float speed = 0.2f; // global variable for controlling speed of zoom and rotation

// Matrix Transform Variables
// CAMERA TRANSFORM VARIABLES
glm::mat4 modelMatrix;      // global variable for representing the teapot model space
glm::mat4 viewMatrix;       // global variable for representing the camera/view space
glm::mat4 projectionMatrix; // global variable for representing the world space
glm::mat4 mvMatrix;         // global variable for model view space
glm::mat4 mvpMatrix;        // global variable for model view projection space
glm::mat4 mvnMatrix;        // global variable for model view normal space

// Camera Variables
glm::vec3 cameraPosition;      // global variable for recording the position of the camera
glm::vec3 cameraTarget;        // global variable for recording where the camera is pointing in the scene
glm::vec3 cameraUp;            // global variable for recording the camera up/down status
float angle_prev_x;            // global variable for storing previous x angle for yaw
float angle_prev_y;            // global variable for storing previous y angle for pitch
float current_angle_x;         // global variable for storing current x value from mouse motion
float current_angle_y;         // global variable for storing current y value from mouse motion
float current_camera_distance; // global variable for storing current camera distance for zoom
float camera_distance_prev;    // global variable for storing previous camera distance for zoom
float yaw = 0.0f;              // global variable for controlling horizontal camera movement
float pitch = 0.0f;            // global variable for controlling vertical camera movement
float FOV = 45.0f;             // global variable for controlling field of view (zoom)

// LIGHT TRANSFORM VARIABLES
glm::mat4 lightModelMatrix; // global variable for representing the light model space
glm::mat4 lightViewMatrix;  // global variable for light view space
glm::mat4 lightMVMatrix;    // global variable for light model view space

// Light Variables
glm::vec4 lightPosition;
glm::vec3 lightViewPosition; // global variable for recording the position of the camera
glm::vec3 lightTarget;       // global variable for recording where the camera is pointing in the scene
glm::vec3 lightUp;           // global variable for recording the camera up/down status

// Light Variables
float light_angle_prev_x;    // global variable for storing previous x angle for yaw
float light_angle_prev_y;    // global variable for storing previous y angle for pitch
float light_current_angle_x; // global variable for storing current x value from mouse motion
float light_current_angle_y; // global variable for storing current y value from mouse motion
float light_yaw = 0.0f;      // global variable for controlling horizontal camera movement
float light_pitch = 0.0f;    // global variable for controlling vertical camera movement

// Mouse Event Variables (Interaction Booleans and speed
bool leftDown = false;  // global variable for stating whether the camera angle is being set
bool rightDown = false; // global variable for stating whether the camera distance is being set
bool ctrlDown = false;  // global variable for stating whether the light angle is being set

// Scene Variables
std::vector<cy::Vec3f> triangles;
std::vector<cy::Vec3f> normals;
std::vector<cy::Vec2f> diffuseTextureCoords;
std::vector<cy::Vec2f> specularTextureCoords;

// Texture Variables
std::vector<unsigned char> diffuseImage;
std::vector<unsigned char> specularImage;
unsigned diffuseWidth, diffuseHeight;
unsigned specularWidth, specularHeight;
std::vector<unsigned char> specular_brick_image;
unsigned specular_brick_width, specular_brick_height;

// CALLBACK FUNCTIONS AND HELPER FUNCTIONS

/// <summary>
/// Initialize Camera Variables
/// </summary>
void cameraInitialize()
{
    // CAMERA MATRIX INTITIALIZATIONS
    // CAMERA MODEL MATRIX
    modelMatrix = glm::mat4(1.0f);

    // CAMERA VIEW MATRIX
    cameraPosition = glm::vec3(0.0f, -2.0f, 1.0f);
    cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    viewMatrix = glm::lookAt(
        cameraPosition, // CAMERA POSITION
        cameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        cameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    // CAMERA PROJECTION MATRIX
    projectionMatrix = glm::perspective(
        glm::radians(FOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);
}

/// <summary>
/// Initialize lighting variables
/// </summary>
void lightInitialize()
{

    // LIGHT MATRIX INITIALIZATIONS
    // LIGHT MODEL MATRIX
    lightModelMatrix = glm::mat4(1.0f);

    // LIGHT VIEW MATRIX
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
/// Function for computing the bounding box for centering the rotation center for objects in the scene
/// </summary>
void boundingBox()
{
    mesh.ComputeBoundingBox();
    cy::Vec3f boundBoxMin = mesh.GetBoundMin();
    cy::Vec3f boundBoxMax = mesh.GetBoundMax();
    float zMidpoint = -0.05 * (boundBoxMax.z + boundBoxMin.z) / 2;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, zMidpoint));
    // lightModelMatrix = modelMatrix;
}

/// <summary>
/// Function for computing the camera motion in the scene
/// </summary>
void cameraTransform()
{
    // Set up projection view based on the FOV
    projectionMatrix = glm::perspective(
        glm::radians(FOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);

    // Set up view based on the current camera information
    viewMatrix = glm::lookAt(
        cameraPosition, // CAMERA POSITION
        cameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        cameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    // Transformations for motion
    viewMatrix = glm::translate(viewMatrix, cameraTarget);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(pitch), glm::vec3(1, 0, 0));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(yaw), glm::vec3(0, 0, 1));
}

/// <summary>
/// Function for computing the light motion in the scene
/// </summary>
void lightTransform()
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
/// Function for rendering to the display
/// </summary>
void myDisplay()
{
    // OpenGLdraw calls here

    // Clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render Stuff
    program.Bind(); // Use the shader program

    // Calculate Matrices
    mvMatrix = viewMatrix * modelMatrix;
    lightPosition = glm::vec4(107.780487f, 83.130577f, 0.0f, 1.0f);
    lightMVMatrix = lightViewMatrix * lightModelMatrix;
    lightPosition = lightMVMatrix * lightPosition;
    mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    mvnMatrix = glm::inverseTranspose(mvMatrix);

    // Get the matrix ID
    GLuint mvpID = glGetUniformLocation(program.GetID(), "mvp");
    GLuint mvID = glGetUniformLocation(program.GetID(), "mv");
    GLuint lightPosID = glGetUniformLocation(program.GetID(), "lightPos");
    GLuint mvnID = glGetUniformLocation(program.GetID(), "mvn");
    GLuint texID;

    // Update the matrix transform
    glUniformMatrix4fv(mvID, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(mvnID, 1, GL_FALSE, &mvnMatrix[0][0]);
    glUniform3f(lightPosID, lightPosition.x, lightPosition.y, lightPosition.z);
    // Bind vertex data to vertices of scene
    glBindVertexArray(vertexArrayObject);
    glUseProgram(program.GetID());

    // Render Data to the Screen
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());

    // Swap buffers
    glutSwapBuffers();
}

/// <summary>
/// Function for handling keyboard in put
/// </summary>
/// <param name="key">Variable storing information about what key was pressed (ASCII)</param>
/// <param name="x">Variable storing information about the x coordinate for key event</param>
/// <param name="y">Variable storing informationabout the y coordinate for key event</param>
void myKeyboard(unsigned char key, int x, int y)
{

    // GLUT uses ascii for key events
    switch (key)
    {
        // escape key
    case 27:
        // Exit main loop and clean up resources
        glutLeaveMainLoop();
        break;
    default:
        break;
    }
    return;
}
/// <summary>
/// Function for handling keyboard in put
/// </summary>
/// <param name="key">Variable storing information about what key was pressed (ASCII)</param>
/// <param name="x">Variable storing information about the x coordinate for key event</param>
/// <param name="y">Variable storing informationabout the y coordinate for key event</param>
void mySpecialKeyboard(int key, int x, int y)
{
    // GLUT uses ascii for key events
    switch (key)
    {
        // F6 - Recompile Shaders
    case GLUT_KEY_F6:
        // Recompile Shaders
        bool shader_recompile = program.BuildFiles("shader.vert", "shader.frag");
        break;
    }

    return;
}

/// <summary>
/// Function for handling mouse key events
/// </summary>
/// <param name="button">Variable storing which mouse key was pressed</param>
/// <param name="state">Variable storing the state of the mouse key event</param>
/// <param name="x">Variable storing information about the x coordinate for the mouse key event</param>
/// <param name="y">Variable storing information about the y coordinate for the mouse key event</param>
void myMouse(int button, int state, int x, int y)
{
    int mod;
    switch (button)
    {
    case GLUT_LEFT_BUTTON: // set the camera angle
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
/// Function for handling mouse motion events
/// </summary>
/// <param name="x">Variable storing information about the x coordinate for mouse motion event</param>
/// <param name="y">Variable storing information about the y coordinate for mouse motion event</param>
void myMouseMotion(int x, int y)
{

    // Variables for storing the current camera angle and current distance
    current_angle_x = (float)x;
    current_angle_y = (float)y;
    current_camera_distance = (float)y;
    light_current_angle_x = (float)x;
    light_current_angle_y = (float)y;

    // Update Camera angle
    /*  if (leftDown == true && ctrlDown == false)*/
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

        // Update matrix transformations
        cameraTransform();
    }

    // Update Light Angle
    if (leftDown == true && ctrlDown == true)
    // if (leftDown == true)
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

        // Call the Light matrix transform function
        lightTransform();
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
        // Update matrix transformations
        cameraTransform();
    }
}

// Reshape window function
void myReshape(int x, int y)
{
}

/// <summary>
/// Function for handling animation events in FreeGlut and updating the redisplay of buffers
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
void myIdle()
{

    // Tell GLUT to redraw in the main loop
    glutPostRedisplay();
}

/// <summary>
/// The main function that calls the initialization functions and loads in the scene data, command line arguments
/// and calls functions to render stuff
/// </summary>
/// <param name="argc"></param>
/// <param name="argv">Command line arguments</param>
/// <returns></returns>
int main(int argc, char **argv)
{
    // GLUT INITIALIZATION

    // Initialize GLUT Context Version
    glutInitContextVersion(4, 5);

    // Initialize GLUT
    glutInit(&argc, argv);

    // Create a Window
    glutInitWindowSize(1920, 1080);                            // specify width, height of window
    glutInitWindowPosition(0, 0);                              // specify the position of the window (x, y) coordinate for center
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // Initialize the display
    glutCreateWindow("Window Title");                          // specify the title of the window

    // REGISTER CALLBACK FUNCTIONS
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LESS);
    glEnable(GL_CULL_FACE);

    // Display callback function
    glutDisplayFunc(myDisplay);

    // Keyboard event callback function
    glutKeyboardFunc(myKeyboard);

    // GLUT Keyboard Event callback function
    glutSpecialFunc(mySpecialKeyboard);

    // Mouse event callback function
    glutMouseFunc(myMouse);

    // Mouse motion callback function
    glutMotionFunc(myMouseMotion);

    // Idle callback function (used for animation)
    glutIdleFunc(myIdle);

    // OpenGL INITIALIZATION
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Set the background color

    // Initialize the matrices for the rendering and camera
    cameraInitialize();

    // Initialize Lighting
    lightInitialize();

    // Initialize GLEW
    glewInit();

    // VERTEX ARRAY OBJECT
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // LOAD SCENE
    bool success = mesh.LoadFromFileObj(argv[1], true);

    // LOAD MTL FILE
    std::string diffuse(mesh.M(0).map_Kd);
    // std::string specular(mesh.M(1)

    // LOAD TEXTURES
    unsigned diffuse_success = lodepng::decode(diffuseImage, diffuseWidth, diffuseHeight, diffuse);
    // unsigned specular_success = lodepng::decode(specularImage, specularWidth, specularHeight, argv[3]);

    // LOAD DATA INTO BUFFER

    // MESH FACE DATA
    for (int i = 0; i < mesh.NF(); i++)
    {
        cy::Vec3f vertex1 = mesh.V(mesh.F(i).v[0]);
        cy::Vec3f vertex2 = mesh.V(mesh.F(i).v[1]);
        cy::Vec3f vertex3 = mesh.V(mesh.F(i).v[2]);
        triangles.push_back(vertex1);
        triangles.push_back(vertex2);
        triangles.push_back(vertex3);
    }

    // NORMAL FACE DATA
    for (int j = 0; j < mesh.NF(); j++)
    {
        cy::Vec3f normals1 = mesh.VN(mesh.FN(j).v[0]);
        cy::Vec3f normals2 = mesh.VN(mesh.FN(j).v[1]);
        cy::Vec3f normals3 = mesh.VN(mesh.FN(j).v[2]);
        normals.push_back(normals1);
        normals.push_back(normals2);
        normals.push_back(normals3);
    }

    // TEXTURE COORD DATA
    for (int k = 0; k < mesh.NF(); k++)
    {
        // Diffuse Texture
        cy::Vec2f diffuseTextures1 = mesh.VT(mesh.FT(k).v[0]).XY();
        cy::Vec2f diffuseTextures2 = mesh.VT(mesh.FT(k).v[1]).XY();
        cy::Vec2f diffuseTextures3 = mesh.VT(mesh.FT(k).v[2]).XY();
        diffuseTextureCoords.push_back(diffuseTextures1);
        diffuseTextureCoords.push_back(diffuseTextures2);
        diffuseTextureCoords.push_back(diffuseTextures3);
    }

    // Specular Texture Coords
    for (int n = 0; n < mesh.NF(); n++)
    {
        // Specular Texture
        cy::Vec2f specularTextures1 = mesh.VT(mesh.FT(n).v[0]).XY();
        cy::Vec2f specularTextures2 = mesh.VT(mesh.FT(n).v[1]).XY();
        cy::Vec2f specularTextures3 = mesh.VT(mesh.FT(n).v[2]).XY();
        specularTextureCoords.push_back(specularTextures1);
        specularTextureCoords.push_back(specularTextures2);
        specularTextureCoords.push_back(specularTextures3);
    }

    // Shader Compilation
    bool shader_compile = program.BuildFiles("shader.vert", "shader.frag");
    if (shader_compile == false)
    {
        std::cout << "Shader has failed!" << std::endl;
        return -1;
    }

    // VERTEX BUFFER OBJECT
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec3f>) * triangles.size(),
        &triangles[0],
        GL_STATIC_DRAW);

    // NORMAL BUFFER OBJECT
    glGenBuffers(1, &normalBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec3f>) * normals.size(),
        &normals[0],
        GL_STATIC_DRAW);

    // Diffuse TEXTURE BUFFER OBJECT
    glGenBuffers(1, &diffuseTextureBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseTextureBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec2f>) * diffuseTextureCoords.size(),
        &diffuseTextureCoords[0],
        GL_STATIC_DRAW);

    // SPECULAR TEXTURE BUFFER OBJECT
    glGenBuffers(1, &specularTextureBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, specularTextureBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec2f>) * specularTextureCoords.size(),
        &specularTextureCoords[0],
        GL_STATIC_DRAW);

    // ASSIGN VERTEX BUFFER OBJECTS TO VERTEX ATTRIBUTES
    GLuint pos = glGetAttribLocation(program.GetID(), "pos");
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glVertexAttribPointer(
        pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // ASSIGN NORMAL BUFFER OBJECTS TO VERTEX ATTRIBUTES
    GLuint normalPos = glGetAttribLocation(program.GetID(), "normalPos");
    glEnableVertexAttribArray(normalPos);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
    glVertexAttribPointer(
        normalPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // ASSIGN TEXTURE BUFFER OBJECTS TO VERTEX ATTRIBUTES
    GLuint diffuseTexturePos = glGetAttribLocation(program.GetID(), "txc");
    glEnableVertexAttribArray(diffuseTexturePos);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseTextureBufferObject);
    glVertexAttribPointer(
        diffuseTexturePos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // TEXTURE BINDING SET UP
    tex.SetImage(&diffuseImage[0], 4, diffuseWidth, diffuseHeight);
    tex.BuildMipmaps();
    tex.Bind(0);
    program["tex"] = 0;

    // CREATE THE BOUNDING BOX VIEW FOR THE SCENE DATA
    boundingBox();

    glutMainLoop(); // Call main loop

    return 0; // Exit when main loop is done
}