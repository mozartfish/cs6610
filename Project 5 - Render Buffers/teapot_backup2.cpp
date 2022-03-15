// Libraries
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

int screen_width = 1920;
int screen_height = 1080;

// Global Variable Initialization
cy::TriMesh mesh;                  // global variable for storing the scene data
cy::GLSLProgram program;           // global variable for compiling and running shader program
GLuint vertexArrayObject;          // global variable for storing vertex array data
GLuint vertexBufferObject;         // global variable for strogin vertex buffer data
GLuint normalBufferObject;         // global variable for storing normal buffer data
GLuint diffuseTexture;             // global variable for storing diffuse texture data
GLuint diffuseTextureBufferObject; // global variable for storing diffuse texture buffer data

// Plane Global Variables
cy::GLSLProgram quad_program; // global variable for compiling and running shader program
GLuint quad_vertexArrayObject;
GLuint quad_vertexBufferObject;

// Render to Texture
cy::GLRenderTexture2D renderBuffer;
GLuint frameBuffer;
GLuint depthBuffer;
GLuint renderedTexture;

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
// Diffuse Texture Coordinates
std::vector<cy::Vec2f> diffuseTextureCoords;

// Texture Data
// Diffuse Texture
std::vector<unsigned char> diffuseTextureData;
unsigned diffuseWidth, diffuseHeight;

// PLANE Variables
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
void tea_InitializeCamera()
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
/// Camera motion in the horizontal and vertical directions
/// </summary>
void tea_TransformCamera()
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

/// <summary>
/// Initialize the scene camera
/// </summary>
void plane_InitializeCamera()
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

/// <summary>
/// Camera motion in the horizontal and vertical directions
/// </summary>
void plane_TransformCamera()
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

/// <summary>
/// Initialize the scene lighting
/// </summary>
void InitializeLight()
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
/// Light motion in the horizontal and vertical directions
/// </summary>
void TransformLight()
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
/// Initialize teapot textures
/// </summary>
void tea_InitializeTexture()
{
    std::string diffuseFile(mesh.M(0).map_Kd);                                                                // load diffuse texture from .mtl file
    unsigned diffuse_success = lodepng::decode(diffuseTextureData, diffuseWidth, diffuseHeight, diffuseFile); // load diffuse texture
    glGenTextures(1, &diffuseTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, diffuseWidth, diffuseHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &diffuseTextureData[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

/// <summary>
/// Initialize teapot triangles, normal and diffuse texture data
/// </summary>
void tea_InitializeData()
{
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

    // Load Diffuse Texture
    for (int k = 0; k < mesh.NF(); k++)
    {
        cy::Vec2f diffuseTextures1 = mesh.VT(mesh.FT(k).v[0]).XY();
        cy::Vec2f diffuseTextures2 = mesh.VT(mesh.FT(k).v[1]).XY();
        cy::Vec2f diffuseTextures3 = mesh.VT(mesh.FT(k).v[2]).XY();
        diffuseTextureCoords.push_back(diffuseTextures1);
        diffuseTextureCoords.push_back(diffuseTextures2);
        diffuseTextureCoords.push_back(diffuseTextures3);
    }
}

/// <summary>
/// Initialize Teapot Buffers
/// </summary>
void tea_InitializeBuffers()
{

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
    // Diffuse Texture
    glGenBuffers(1, &diffuseTextureBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseTextureBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(std::vector<cy::Vec2f>) * diffuseTextureCoords.size(),
        &diffuseTextureCoords[0],
        GL_STATIC_DRAW);
}

/// <summary>
/// Bind buffers of teapot to vertices
/// </summary>
void tea_BindBuffers()
{
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

    // Assign Diffuse Texture Buffer Objects to Vertex Attributes
    GLuint diffuseTexturePos = glGetAttribLocation(program.GetID(), "txc");
    glEnableVertexAttribArray(diffuseTexturePos);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseTextureBufferObject);
    glVertexAttribPointer(
        diffuseTexturePos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
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

/// <summary>
/// Load and render the teapot
/// </summary>
void tea_Init()
{
    tea_InitializeTexture();                                          // load and set up texture for teapot
    tea_InitializeData();                                             // load teapot scene data
    tea_InitializeBuffers();                                          // load the buffers for the teapot
    bool shader_compile = program.BuildFiles("tea.vert", "tea.frag"); // compile the shaders
    tea_BindBuffers();                                                // bind the buffers to the vertices
    boundingBox();                                                    // bounding box
}

/// <summary>
/// Load and render the plane
/// </summary>
void quad_Init()
{
    static const GLfloat quad_vertex_buffer_data[] =
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
    // Vertex Array Object
    glGenVertexArrays(1, &quad_vertexArrayObject);
    glBindVertexArray(quad_vertexArrayObject);

    // Vertex Buffer
    glGenBuffers(1, &quad_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_buffer_data), quad_vertex_buffer_data, GL_STATIC_DRAW);

    // Compile shaders
    bool quad_compile_shaders = quad_program.BuildFiles("plane.vert", "plane.frag");

    // Assign Vertex Buffer Objects to Vertex Attributes
    GLuint plane_pos = glGetAttribLocation(quad_program.GetID(), "plane_pos");
    glEnableVertexAttribArray(plane_pos);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexBufferObject);
    glVertexAttribPointer(
        plane_pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
}

void renderBuffer_Init()
{
    // renderBuffer.Initialize(true, 3, screen_width, screen_height);
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //{
    //     std::cout << "Frame Buffer Failed!" << std::endl;
    // }

    // renderBuffer.BuildTextureMipmaps();
    // renderBuffer.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    // renderBuffer.SetTextureMaxAnisotropy();

    // Frame Buffer
    frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // Rendered Texture
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Depth Buffer
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // Set up color attachment
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FRAME BUFFER FAILED" << std::endl;
    }
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
        bool shader_recompile = program.BuildFiles("tea.vert", "tea.frag");
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
            plane_leftDown = false;
            ctrlDown = false;
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
            else
            {
                camera_distance_prev = (float)y;
                rightDown = true;
                plane_altDown = false;
            }
        }
        if (state == GLUT_UP)
        {
            rightDown = false;
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
        tea_TransformCamera();
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
        TransformLight();
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
        tea_TransformCamera();
    }

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
        plane_TransformCamera();
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
        plane_TransformCamera();
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
/// Render to Screen - The Rendering While Loop Equivalent in GLFW
/// </summary>
void render()
{
    // TEAPOT Rendering
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, screen_width, screen_height);

    // Clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor
    // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    //  Use shader
    program.Bind();

    // Compute the different matrices and vectors for rendering the scene
    mvMatrix = viewMatrix * modelMatrix;
    lightPosition = glm::vec4(107.780487f, 83.130577f, 0.0f, 1.0f);
    lightMVMatrix = lightViewMatrix * lightModelMatrix;
    lightPosition = lightMVMatrix * lightPosition;
    mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    mvnMatrix = glm::inverseTranspose(mvMatrix);

    //// Set Uniform ID Variables for the Shader Program
    GLuint mvpID = glGetUniformLocation(program.GetID(), "mvp");
    GLuint mvID = glGetUniformLocation(program.GetID(), "mv");
    GLuint lightPosID = glGetUniformLocation(program.GetID(), "lightPos");
    GLuint mvnID = glGetUniformLocation(program.GetID(), "mvn");
    GLuint diffuseTextureID = glGetUniformLocation(program.GetID(), "tex");

    //// Set Up Texture Unit
    //// Diffuse Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);

    //// Set Uniform Data Variables for the Shader Program
    glUniformMatrix4fv(mvID, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(mvnID, 1, GL_FALSE, &mvnMatrix[0][0]);
    glUniform3f(lightPosID, lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform1i(diffuseTextureID, 0);

    // Bind vertex data to vertices of scene
    glBindVertexArray(vertexArrayObject);

    //// Render Data to the Screen
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    //  PLANE RENDERING
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screen_width, screen_height);

    // Clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply Shaders to the Scene
    quad_program.Bind();

    // Teapot Texture ID
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    GLuint teaTextureID = glGetUniformLocation(quad_program.GetID(), "renderTexture");
    glUniform1i(teaTextureID, 0);
    glGenerateTextureMipmap(renderedTexture);

    // Compute the different matrices and vectors for rendering the scene
    plane_mvMatrix = plane_viewMatrix * plane_modelMatrix;
    plane_mvpMatrix = plane_projectionMatrix * plane_viewMatrix * plane_modelMatrix;

    // Set Uniform ID Variables for the Shader Program
    GLuint plane_mvpID = glGetUniformLocation(quad_program.GetID(), "plane_mvp");

    // Set Uniform Data Variables for the Shader Program
    glUniformMatrix4fv(plane_mvpID, 1, GL_FALSE, &plane_mvpMatrix[0][0]);

    // Bind vertex data to vertices of scene
    glBindVertexArray(quad_vertexArrayObject);
    glEnableVertexAttribArray(0);

    //// Render Data to the Screen
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);

    glutSwapBuffers(); // Swap buffers (front and back buffers) to give most up to date rendering of scene
}

using namespace std;
int main(int argc, char **argv)
{
    // GLUT Initialization
    glutInitContextVersion(4, 5); // Initialize GLUT Context Version
    glutInit(&argc, argv);        // Initialize GLUT

    // Create a Window
    glutInitWindowSize(screen_width, screen_height);           // specify width, height of window
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    // Initialize Scene
    glClearColor(1.0f / 255.0f, 87.0f / 255.0f, 51.0f / 255.0f, 0.0f); // Clear the previous color and initialize background to black
    tea_InitializeCamera();                                            // initialize camera
    plane_InitializeCamera();                                          // intialize plane camera
    InitializeLight();                                                 // initialize scene lighting
    bool success = mesh.LoadFromFileObj(argv[1]);                      // Load scene from .obj files
    tea_Init();                                                        // load and render the teapot
    // renderBuffer_Init();
    // quad_Init(); // load and render the plane

    // Set up rendering for the scene
    glutMainLoop(); // Call main loop for rendering
    return 0;       // Exit when main loop is done
}