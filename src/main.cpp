#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <iostream>

#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void display();
void init();

// display settings
const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

//data variables
const unsigned int NUM_POINTS = 5000;
vec3 points[NUM_POINTS];

//OpenGL data
unsigned int VBO, VAO;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sierpinski Gasket", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    init();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, NUM_POINTS);
    glFlush();
}

void init()
{
    //specify the vertices of the tetrahedron
    vec3 vertices[4] = {
        vec3(-1.0,-1.0,-1.0),
        vec3(1.0,-1.0,-1.0),
        vec3(0.0,1.0,-1.0),
        vec3(0.0,0.0,1.0)
    };

    vec3 random_point_inside_tetrahedron = vec3(0.0, 0.0, 0.0);

    points[0] = random_point_inside_tetrahedron;
    for (int i = 1; i < NUM_POINTS; ++i) {
        //generates a random number between 0 and 4
        int j = rand() % 4;
        //computes the halfwaty between the selected vertex and the previous point
        points[i] = (points[i - 1] + vertices[j]) / 2.0f;
    }


    //Load shaders and use it.
    Shader ourShader(RESOURCES_PATH "vertex.vc", RESOURCES_PATH "fragment.fc");
    ourShader.use();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}



