#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <iostream>

#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void triangle(const vec3& a, const vec3& b, const vec3& c);
void tetra(const vec3& a, const vec3& b, const vec3& c, const vec3& d);
void divide_tetra(const vec3& a, const vec3& b, const vec3& c, const vec3& d, int m);
void processInput(GLFWwindow* window);
void display();
void init();

// display settings
const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

//data variables
const int NumTimesToSubdivide = 4;
const int NumTriangles = 2048;
const int NumVertices = 4 * NumTriangles;
const unsigned int NUM_POINTS = 5000;
vec3 points[NumVertices];
int index = 0;
int colorIndex;

vec3 base_colors[4] = {
    vec3(1.0,0.0,0.0),
    vec3(0.0,1.0,0.0),
    vec3(0.0,0.0,1.0),
    vec3(0.0,0.0,0.0)
};
vec3 colors[NumVertices];

//OpenGL data
unsigned int VBO, VAO;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
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
    divide_tetra(vertices[0], vertices[1], vertices[2], vertices[3], NumTimesToSubdivide);
    //Load shaders and use it.
    Shader ourShader(RESOURCES_PATH "vertex.vc", RESOURCES_PATH "fragment.fc");
    ourShader.use();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
    glEnableVertexAttribArray(1);


    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void divide_tetra(const vec3& a, const vec3& b, const vec3& c, const vec3& d, int m)
{
    if (m > 0)
    {
        vec3 mid[6];

        //compute 6 midpoints
        mid[0] = (a + b) / 2.0f;
        mid[1] = (a + c) / 2.0f;
        mid[2] = (a + d) / 2.0f;
        mid[3] = (b + c) / 2.0f;
        mid[4] = (c + d) / 2.0f;
        mid[5] = (b + d) / 2.0f;

        //create 4 tetrahedrons by subdivision

        divide_tetra(a, mid[0], mid[1], mid[2], m - 1);
        divide_tetra(mid[0], b, mid[3], mid[5], m - 1);
        divide_tetra(mid[1], mid[3], c, mid[4], m - 1);
        divide_tetra(mid[2], mid[5], mid[5], d, m - 1);

    }
    else {
        tetra(a, b, c, d);
    }
}

void triangle(const vec3& a, const vec3& b, const vec3& c) {
    colors[index] = base_colors[colorIndex];
    points[index] = a;
    index++;
    colors[index] = base_colors[colorIndex];
    points[index] = b;
    index++;
    colors[index] = base_colors[colorIndex];
    points[index] = c;
    index++;
}

void tetra(const vec3& a, const vec3& b, const vec3& c, const vec3& d) {
    colorIndex = 0;
    triangle(a, b, c);
    colorIndex = 1;
    triangle(a, c, d);
    colorIndex = 2;
    triangle(a, d, b);
    colorIndex = 3;
    triangle(b, d, c);
}