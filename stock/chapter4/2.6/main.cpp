#include "VAO.h"
#include "VBO.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
#include <filesystem.h>
#include <shader.h>
#include <camera.h>
#include <model.h>
#include <callback.h>
#include "stb_image.h"

#include <iostream>
// settings
#ifndef SHADER_DIR
#define SHADER_DIR "shaders/"
#endif //SHADER_DIR
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLFWwindow* window;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    AppState state;
    state.lastX = SCR_WIDTH /2.0f;
    state.lastY = SCR_HEIGHT /2.0f;   
    glfwSetWindowUserPointer(window, &state);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, Callback::framebuffer_size_callback);
    glfwSetCursorPosCallback(window, Callback::mouse_callback);
    glfwSetScrollCallback(window, Callback::scroll_callback);


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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    // build and compile shaders
    // -------------------------
    Shader shader(SHADER_DIR"1.1.depth_testing.vs", SHADER_DIR"1.1.depth_testing.fs");
    //Shader shaderSingleColor(SHADER_DIR"1.1.depth_testing.vs", SHADER_DIR"2.stencil_single_color.fs");

    // set up the vertices 
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        // front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        // left face
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
        // right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        // bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        // top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f // bottom-left
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  -5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, 5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
         5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         -5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };
    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
    
    
   // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // VAO & VBO init for lightSpotShader
    VAO cubeVao;
    VBO cubeVbo;
    cubeVao.load();
    cubeVbo.load(vertices,sizeof(vertices));
    cubeVao.bind();
    cubeVbo.bind();
    int stride { 5*sizeof(float)};
    cubeVao.setAttribute(0,3,stride,0);
    cubeVao.setAttribute(1, 2, stride, 3*sizeof(float));
    // plane VAO
    VAO planeVao;
    VBO planeVbo;
    planeVao.load();
    planeVbo.load(planeVertices,sizeof(planeVertices));
    planeVao.bind();
    planeVao.setAttribute(0,3,stride, 0);
    planeVao.setAttribute(1,2,stride, 3*sizeof(float));
     // Grass VAO & VBO
    VAO grassVao;
    VBO grassVbo;
    grassVao.load();
    grassVbo.load(transparentVertices,sizeof(transparentVertices));
    grassVao.bind();
    grassVbo.bind();
    grassVao.setAttribute(0,3,stride, 0);
    grassVao.setAttribute(1,2,stride, 3*sizeof(float));

    // grass
    // Position 
    vector<glm::vec3> vegetation;
    vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    vegetation.push_back(glm::vec3( 1.5f, 0.0f, 0.51f));
    vegetation.push_back(glm::vec3( 0.0f, 0.0f, 0.7f));
    vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    vegetation.push_back(glm::vec3( 0.5f, 0.0f, -0.6f)); 
    
    // load textures
    // -------------
    unsigned int cubeTexture  = Callback::loadTexture(FileSystem::getPath("resources/textures/marble.jpg").c_str());
    unsigned int floorTexture = Callback::loadTexture(FileSystem::getPath("resources/textures/metal.png").c_str());
    unsigned int grassTexture = Callback::loadTexture(FileSystem::getPath("resources/textures/window.png").c_str());
     // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        state.deltaTime = currentFrame - state.lastFrame;
        state.lastFrame = currentFrame;

        // input
        // -----
        Callback::processInput(window,state);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw objects
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(state.camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = state.camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // cubes
        cubeVao.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        planeVao.bind(); 
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // vegetation
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < vegetation.size(); i++)
        {
            float distance = glm::length(state.camera.Position - vegetation[i]);
            sorted[distance] = vegetation[i];
        }
        grassVao.bind();
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        for (std::map<float,glm::vec3>::reverse_iterator it = sorted.rbegin(); it !=
            sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

