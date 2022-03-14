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

// Global Variable Initialization
cy::GLSLProgram plane_program; // global variable for compiling and running shader program
GLuint plane_vertexArrayObject;
GLuint plane_vertexBufferObject;

// Speed Variable - for controlling refresh/rendering rate
float speed = 0.2f;

// Matrix Transform Variables
// Camera Transform Variables
glm::mat4 plane_modelMatrix;      // global variable for representing the model space with respect to the camera
glm::mat4 plane_viewMatrix;       // global variable for representing the view space with respect to the camera
glm::mat4 plane_projectionMatrix; // global variable for representing the real world space with respect to the camera
glm::mat4 plane_mvMatrix;         // global variable for representing the model-view space
glm::mat4 plane_mvpMatrix;        // global variable for representing the model-view-projection space

// Camera Variables
// Camera Vectors
glm::vec3 plane_cameraPosition; // global variable for representing the camera location
glm::vec3 plane_cameraTarget;   // global variable for representing where the camera is pointing at
glm::vec3 plane_cameraUp;       // global variable for representing whether the camera is pointing up or down

// Camera Previous State Variables
float plane_angle_prev_x;
float plane_angle_prev_y;
float plane_camera_distance_prev;

// Camera Current State Variables
float plane_current_angle_x;
float plane_current_angle_y;
float plane_current_camera_distance;

// Camera Default Settings
float plane_yaw = 0.0f;   // global variable for controlling horiziontal camera movement
float plane_pitch = 0.0f; // global variable for controlling vertical camera movement
float plane_FOV = 45.0f;  // global variable for controlling field of view (zoom)

// Mouse Event Variables (User Interaction Controls)
bool plane_leftDown = false;
bool plane_rightDown = false;
bool plane_altDown = false;

// Initialization Functions
/// <summary>
/// Initialize the scene camera
/// </summary>
void plane_initializeCamera()
{
    plane_modelMatrix = glm::mat4(1.0f);

    plane_cameraPosition = glm::vec3(0.0f, -2.0f, 1.0f);
    plane_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    plane_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    plane_viewMatrix = glm::lookAt(
        plane_cameraPosition, // CAMERA POSITION
        plane_cameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        plane_cameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    plane_projectionMatrix = glm::perspective(
        glm::radians(plane_FOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);
}

// Transform Functions

/// <summary>
/// Camera motion in the horizontal and vertical directions
/// </summary>
void plane_transformCamera()
{
    plane_projectionMatrix = glm::perspective(
        glm::radians(plane_FOV),
        16.0f / 9.0f,
        0.1f,
        100.0f);

    plane_viewMatrix = glm::lookAt(
        plane_cameraPosition, // CAMERA POSITION
        plane_cameraTarget,   // and looks at the origin (camera location) CAMERA DIRECTION
        plane_cameraUp        // Head is up (set to 0,-1,0 to look upside-down) // CAMERA RIGHT
    );

    plane_viewMatrix = glm::translate(plane_viewMatrix, plane_cameraTarget);
    plane_viewMatrix = glm::rotate(plane_viewMatrix, glm::radians(plane_pitch), glm::vec3(1, 0, 0));
    plane_viewMatrix = glm::rotate(plane_viewMatrix, glm::radians(plane_yaw), glm::vec3(0, 0, 1));
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
    plane_mvMatrix = plane_viewMatrix * plane_modelMatrix;
    plane_mvpMatrix = plane_projectionMatrix * plane_viewMatrix * plane_modelMatrix;

    // Set Uniform ID Variables for the Shader Program
    GLuint mvpID = glGetUniformLocation(plane_program.GetID(), "plane_mvp");

    // Set Uniform Data Variables for the Shader Program
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &plane_mvpMatrix[0][0]);

    // Bind vertex data to vertices of scene
    glBindVertexArray(plane_vertexArrayObject);

    // Apply Shaders to the Scene
    plane_program.Bind();

    // Render Data to the Screen
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // glDrawArrays(GL_POINTS, 0, sizeof(cy::Vec3f) * mesh.NV()); // render points

    glutSwapBuffers(); // Swap buffers (front and back buffers) to give most up to date rendering of scene
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
            if (mod == (GLUT_ACTIVE_ALT))
            {
                plane_leftDown = true;
                plane_altDown = true;
                plane_angle_prev_x = (float)x;
                plane_angle_prev_y = (float)y;
            }
        }
        if (state == GLUT_UP)
        {
            plane_leftDown = false;
            plane_altDown = false;
        }
        break;
    case GLUT_RIGHT_BUTTON: // set the camera distance
        if (state == GLUT_DOWN)
        {
            mod = glutGetModifiers();
            if (mod == (GLUT_ACTIVE_ALT))
            {
                plane_camera_distance_prev = (float)y;
                plane_rightDown = true;
                plane_altDown = true;
            }
        }
        if (state == GLUT_UP)
        {
            plane_rightDown = false;
            plane_altDown = false;
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
    plane_current_angle_x = (float)x;
    plane_current_angle_y = (float)y;
    plane_current_camera_distance = (float)y;

    // Update Camera angle
    if (plane_leftDown == true && plane_altDown == true)
    {
        // Calculate the differential x and y when the mouse moves and store the previous coordinate
        float dx = plane_current_angle_x - plane_angle_prev_x;
        float dy = plane_current_angle_y - plane_angle_prev_y;
        plane_angle_prev_x = plane_current_angle_x;
        plane_angle_prev_y = plane_current_angle_y;

        // Scale the differential for a smooth transition
        dx *= speed;
        dy *= speed;

        // Set yaw and pitch
        plane_yaw += dx;
        plane_pitch += dy;

        // Update Camera View
        plane_transformCamera();
    }

    // Update Field of View
    if (plane_rightDown == true && plane_altDown == true)
    {
        float dy = 0.0f;
        dy += plane_current_camera_distance - plane_camera_distance_prev;
        plane_camera_distance_prev = plane_current_camera_distance;
        dy *= speed;
        plane_FOV += dy;
        if (plane_FOV < 10.0f)
        {
            plane_FOV = 10.0f;
        }
        if (plane_FOV > 90.0f)
        {

            plane_FOV = 90.0f;
        }
        // Update Camera View
        plane_transformCamera();
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

void  createPlane()
{
    static const GLfloat plane_vertices[] =
        {
            -10.0f,
            -10.0f,
            0.0f,
            10.0f,
            -10.0f,
            0.0f,
            -10.0f,
            10.0f,
            0.0f,
            -10.0f,
            10.0f,
            0.0f,
            10.0f,
            -10.0f,
            0.0f,
            10.0f,
            10.0f,
            0.0f,
        };

    glGenVertexArrays(1, &plane_vertexArrayObject);
    glBindVertexArray(plane_vertexArrayObject);

    glGenBuffers(1, &plane_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, plane_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
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
    glutDisplayFunc(myDisplay);    // rendering callback function
    glutKeyboardFunc(myKeyboard);  // keyboard callback function
    glutMouseFunc(myMouse);        // mouse click event callback function
    glutMotionFunc(myMouseMotion); // mouse motion callback function
    glutIdleFunc(myIdle);          // IDLE callback function (for animation)

    // Initialize Scene
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear the previous color and initialize background to black
    plane_initializeCamera();             // initialize camera

    createPlane();

    // Compile Shaders
    bool shader_compile = plane_program.BuildFiles("plane.vert", "plane.frag");

    // Assign Vertex Buffer Objects to Vertex Attributes
    GLuint plane_pos = glGetAttribLocation(plane_program.GetID(), "plane_pos");
    glEnableVertexAttribArray(plane_pos);
    glBindBuffer(GL_ARRAY_BUFFER, plane_vertexBufferObject);
    glVertexAttribPointer(
        plane_pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // Set up rendering for the scene
    glutMainLoop(); // Call main loop for rendering
    return 0;       // Exit when main loop is done
}