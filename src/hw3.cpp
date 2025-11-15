#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "3rdparty/glm/glm.hpp"
#include "3rdparty/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/gtc/type_ptr.hpp"
#include "hw3_scenes.h"

using namespace hw3;

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW
    glfwInit();

    // Designate OpenGL version to GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use CORE profile for modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create 800x800 pixel window
    GLFWwindow* window = glfwCreateWindow(800, 800, "hw3", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return;
    }
    // Introduce window to current context
    glfwMakeContextCurrent(window);

    // Load GLAD to configure OpenGL
    gladLoadGL();

    // Specify size of Viewport
    glViewport(0, 0, 800, 800);

    // Add a background color 
    glClearColor(1.0f, 0.35f, 0.3f, 1.0f);
    // Clean back buffer and assign new color
    glClear(GL_COLOR_BUFFER_BIT);
    // Swap back buffer with front buffer
    glfwSwapBuffers(window);

    // Main while loop
    while(!glfwWindowShouldClose(window)){
        // Take care of all GLFW events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return;
}

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float radians;\n"
"void main()\n"
"{\n"
" float c = cos(radians);\n"
" float s = sin(radians);\n"
" mat2 R = mat2(c, -s, s, c);\n"
" vec2 rotate = R * aPos.xy;\n"
" gl_Position = vec4(rotate, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";


void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle
    glfwInit();
    
    // Designate OpenGL version to GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use CORE profile for modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLfloat vertices[] = 
    {
        0.5f,  0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
         -0.5f, 0.5f, 0.0f, // top left
    };

    GLuint indices[] = {
        0, 1, 3, //first triangle
        1, 2, 3  //second triangle
    };

    // Create 800x800 pixel window
    GLFWwindow* window = glfwCreateWindow(800, 800, "hw3", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return;
    }
    // Introduce window to current context
    glfwMakeContextCurrent(window);

    // Load GLAD to configure OpenGL
    gladLoadGL();

    // Specify size of Viewport
    glViewport(0, 0, 800, 800);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);


    // Add a background color 
    glClearColor(1.0f, 0.35f, 0.3f, 1.0f);
    // Clean back buffer and assign new color
    glClear(GL_COLOR_BUFFER_BIT);
    // Swap back buffer with front buffer
    glfwSwapBuffers(window);

    // Main while loop
    while(!glfwWindowShouldClose(window)){
        // Add a background color 
        glClearColor(1.0f, 0.35f, 0.3f, 1.0f);
        // Clean back buffer and assign new color
        glClear(GL_COLOR_BUFFER_BIT);

        float time = glfwGetTime();
        float theta = time;

        int vertexRotateLocation = glGetUniformLocation(shaderProgram, "radians");
        glUniform1f(vertexRotateLocation, theta);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return;
}

const char *vertexShader3D = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
" gl_Position = proj * view * model * vec4(aPos, 1.0);\n"
" color = aColor;\n"
"}\0";

const char *fragmentShader3D = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 color;\n"
"void main()\n"
"{\n"
" FragColor = vec4(color, 1.0f);\n"
"}\0";

glm::mat4 convert_matrix(Matrix4x4f m){
    glm::mat4 mat = glm::mat4(1.0f);
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            mat[i][j] = m(j,i);
        }
    }
    return mat;
}
glm::vec3 cameraPos, cameraFront, cameraUp;
float lastX, lastY;
glm::mat4 initialCamToWorld, rotatedCamToWorld;
float deltaTime, prevTime;
float pitch, yaw;
bool firstMouse = true;

void processInput(GLFWwindow *window)
{
    // close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // camera movement
    const float cameraSpeed = 50.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    glm::mat4 Ryaw = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), cameraUp);
    glm::mat4 Rpitch = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right);
    glm::mat4 R = Rpitch * Ryaw;
    rotatedCamToWorld = R * initialCamToWorld;
    cameraFront = glm::vec3(-rotatedCamToWorld[2][0], -rotatedCamToWorld[2][1], -rotatedCamToWorld[2][2]);
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
    double width = scene.camera.resolution.x;
    double height = scene.camera.resolution.y;
    float aspect_ratio = width/height;

    Vector3 bg_color = scene.background;
    float s = scene.camera.s; // scaling factor of the view frustrum
    float z_near = scene.camera.z_near; // distance of the near clipping plane
    float z_far = scene.camera.z_far; // distance of far clipping plane


    glfwInit();
    // Designate OpenGL version to GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use CORE profile for modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(scene.camera.resolution.x, scene.camera.resolution.y, "hw3", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return;
    }
    // Introduce window to current context
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        std::cout<< "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Load GLAD to configure OpenGL
    // gladLoadGL();

    // Specify size of Viewport
    glViewport(0, 0, scene.camera.resolution.x, scene.camera.resolution.y);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShader3D, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShader3D, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int total_meshes = scene.meshes.size();
    
    unsigned int VAO[total_meshes];
    unsigned int VBO_vertex[total_meshes];
    unsigned int VBO_color[total_meshes];
    unsigned int EBO[total_meshes];

    glGenVertexArrays(total_meshes, VAO);

    glGenBuffers(total_meshes, VBO_vertex);
    glGenBuffers(total_meshes, VBO_color);
    glGenBuffers(total_meshes, EBO);

    for (int scene_id = 0; scene_id < total_meshes; scene_id++){
        TriangleMesh mesh = scene.meshes[scene_id];
        int num_vertices = mesh.vertices.size();
        int num_indices = mesh.faces.size();

        float vertices[3*num_vertices];
        float colors[3*num_vertices];
        unsigned int indices[3*num_indices];

        glBindVertexArray(VAO[scene_id]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[scene_id]);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex[scene_id]);
        for (int i = 0; i < num_vertices; i++){
            vertices[(3*i)] = mesh.vertices[i].x;
            vertices[(3*i)+1] = mesh.vertices[i].y;
            vertices[(3*i)+2] = mesh.vertices[i].z;
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0 /* layout index */,
                            3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        for (int i = 0; i < num_vertices; i++){
            colors[(3*i)] = mesh.vertex_colors[i].x;
            colors[(3*i)+1] = mesh.vertex_colors[i].y;
            colors[(3*i)+2] = mesh.vertex_colors[i].z;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color[scene_id]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1 /* layout index */,
                            3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        for (int i = 0; i < num_indices; i++){
            indices[(3*i)] = mesh.faces[i].x;
            indices[(3*i)+1] = mesh.faces[i].y;
            indices[(3*i)+2] = mesh.faces[i].z;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // glVertexAttribPointer(0 /* layout index */,
        //                     3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        // glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glm::mat4 proj = glm::mat4(1.0f);
    proj[0][0] = 1/(aspect_ratio*s);
    proj[1][1] = 1/(s);
    proj[2][2] = -z_far/ (z_far-z_near);
    proj[2][3] = -1;
    proj[3][2] = -(z_far*z_near)/(z_far-z_near);
    proj[3][3] = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // set initial cameraPos, cameraUp & cameraFront
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            initialCamToWorld[i][j] = scene.camera.cam_to_world(j, i);
        }
    }

    cameraPos = glm::vec3(scene.camera.cam_to_world(0, 3), scene.camera.cam_to_world(1, 3), scene.camera.cam_to_world(2, 3));
    cameraUp = glm::vec3(scene.camera.cam_to_world(0, 1), scene.camera.cam_to_world(1, 1), scene.camera.cam_to_world(2, 1));
    cameraFront = glm::vec3(-initialCamToWorld[2][0], -initialCamToWorld[2][1], -initialCamToWorld[2][2]);

    lastX = scene.camera.resolution.x / 2.0f;
    lastY = scene.camera.resolution.y / 2.0f;

    while(!glfwWindowShouldClose(window)){
        float time = glfwGetTime();
        deltaTime = time - prevTime;
        prevTime = time;
        processInput(window);

        // Add a background color 
        glClearColor(bg_color.x, bg_color.y, bg_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        for(int n = 0; n < total_meshes; n++){
            TriangleMesh mesh = scene.meshes[n];
            Matrix4x4f V = inverse(scene.camera.cam_to_world);
            glm::mat4 model = convert_matrix(mesh.model_matrix);
            //glm::mat4 view = convert_matrix(V);

            int modelLocation = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            int viewLocation = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

            int projLocation = glGetUniformLocation(shaderProgram, "proj");
            glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));


            // float time = glfwGetTime();
            // float theta = time;

            // int vertexRotateLocation = glGetUniformLocation(shaderProgram, "radians");
            // glUniform1f(vertexRotateLocation, theta);


            glBindVertexArray(VAO[n]);
            glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();
    }

    glDeleteVertexArrays(total_meshes, VAO);
    glDeleteBuffers(total_meshes, VBO_vertex);
    glDeleteBuffers(total_meshes, VBO_color);
    glDeleteBuffers(total_meshes, EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return;
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}
