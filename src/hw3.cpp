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

const char *vertexShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 color;\n"
"uniform float scale;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"void main()\n"
"{\n"
" gl_Position = proj * view * model * vec4(aPos, 1.0);\n"
" color = aColor;\n"
"}\0";

const char *fragmentShader = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 vertexColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(vertexColor, 1.0f);\n"
"}\0";

glm::mat4 convert_matrix(Matrix4x4f m){
    glm::mat4 mat = glm::mat4(1.0f);
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            mat[i][j] = m(i,j);
        }
    }
    return mat;
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
    double width = scene.camera.resolution.x;
    double height = scene.camera.resolution.x;
    float aspect_ratio = width/height;

    Vector3 bg_color = scene.background;
    float s = scene.camera.s; // scaling factor of the view frustrum
    float z_near = scene.camera.z_near; // distance of the near clipping plane
    float z_far = scene.camera.z_far; // distance of far clipping plane
    Matrix4x4f V = inverse(scene.camera.cam_to_world);
    glm::mat4 view = convert_matrix(V);


    glfwInit();
    
    // Designate OpenGL version to GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use CORE profile for modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float vertices[] = 
    {
        0.5f,  0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
         -0.5f, 0.5f, 0.0f, // top left
    };

    unsigned int indices[] = {
        0, 1, 3, //first triangle
        1, 2, 3  //second triangle
    };

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

    for (int scene_id = 0; scene_id < scene.meshes.size(); scene_id++){
        TriangleMesh mesh = scene.meshes[scene_id];
        Matrix4x4 T = V * mesh.model_matrix;
        for (int i = 0; i < mesh.vertices.size(); i+=3){
            vertices[i] = mesh.vertices[i][0];
            vertices[i+1] = mesh.vertices[i][1];
            vertices[i+2] = mesh.vertices[i][2];
        }
        for (int i = 0; i < mesh.faces.size(); i+=3){
            indices[i] = mesh.faces[i][0];
            indices[i+1] = mesh.faces[i][1];
            indices[i+2] = mesh.faces[i][2];
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f,-0.5f,-2.0f));
        proj = glm::perspective(glm::radians(45.0f), aspect_ratio, z_near, z_far);
    }


    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    unsigned int VBO_vertex;
    glGenBuffers(1, &VBO_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0 /* layout index */,
                        3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(0);

    unsigned int VBO_color;
    glGenBuffers(1, &VBO_color);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(1 /* layout index */,
                        3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    // Main while loop
    while(!glfwWindowShouldClose(window)){
        // Add a background color 
        glClearColor(1.0f, 0.35f, 0.3f, 1.0f);
        // Clean back buffer and assign new color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f,-0.5f,-2.0f));
        proj = glm::perspective(glm::radians(45.0f), aspect_ratio, z_near, z_far);

        int modelLocation = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

        int viewLocation = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        int projLocation = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));


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
    glDeleteBuffers(1, &VBO_vertex);
    glDeleteBuffers(1, &VBO_color);
    glDeleteBuffers(1, &EBO);
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
