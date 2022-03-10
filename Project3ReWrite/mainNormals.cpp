// Set up Vertex Array Data
glGenVertexArrays(1, &vertexArrayObject);
glBindVertexArray(vertexArrayObject);

// Load Scene
bool success = mesh.LoadFromFileObj(argv[1]); // Load scene .obj data

// Load Triangle Data
for (int i = 0; i < mesh.NF(); i++)
{
    cy::Vec3f vertex1 = mesh.V(mesh.F(i).v[0]);
    cy::Vec3f vertex2 = mesh.V(mesh.F(i).v[1]);
    cy::Vec3f vertex3 = mesh.V(mesh.F(i).v[2]);
    triangles.push_back(vertex1);
    triangles.push_back(vertex2);
    triangles.push_back(vertex3);
}

// Load Triangle Data
for (int j = 0; j < mesh.NF(); j++)
{
    cy::Vec3f normals1 = mesh.VN(mesh.FN(j).v[0]);
    cy::Vec3f normals2 = mesh.VN(mesh.FN(j).v[1]);
    cy::Vec3f normals3 = mesh.VN(mesh.FN(j).v[2]);
    normals.push_back(normals1);
    normals.push_back(normals2);
    normals.push_back(normals3);
}

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

// Compile Shader
bool shader_compile = program.BuildFiles("shader.vert", "shader.frag");

// Bind Vertex Buffer to Vertex Attributes
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
