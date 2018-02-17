#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

class Player{
public:
    Player(char Side){
        if (Side != 1){
            for(char i = 0; i < 8; i+=2){
                positions[i] *= -1;
            }
        }
    }

public:
    float positions[8] = {
            -0.88, 0.33, //top left
            -0.88, -0.33, //down left
            -0.83, -0.33,  //down right
            -0.83, 0.33   //top right
    };

    unsigned short ibo[6] = {
            0, 1, 2,
            2, 3, 0
    };
};

struct ShaderSource{
    std::string VertexSource;
    std::string FragmentSource;
};

enum class ShaderType{
    NONE = -1, VERTEX = 0, FRAGMENT = 1
};

static ShaderSource ParseShader(const std::string& filepath){
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];

    ShaderType type = ShaderType::NONE;
    while(getline(stream, line)){
        if(line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos){
                type = ShaderType::VERTEX;
            } else if(line.find("fragment") != std::string::npos){
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[(int)type] << line << "\n";
        }
    }
    return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile the shader!" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader){
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(){
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Pong!", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    Player player01(1);
    Player player02(2);

    unsigned int vao_player_01;
    glGenVertexArrays(1, &vao_player_01);
    glBindVertexArray(vao_player_01);

    unsigned int buffer_player_01;
    glGenBuffers(1, &buffer_player_01);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_player_01);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), player01.positions, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    unsigned int ibo_player_01;
    glGenBuffers(1, &ibo_player_01);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_player_01);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), player01.ibo, GL_DYNAMIC_DRAW);

    unsigned int vao_player_02;
    glGenVertexArrays(1, &vao_player_02);
    glBindVertexArray(vao_player_02);

    unsigned int buffer_player_02;
    glGenBuffers(1, &buffer_player_02);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_player_02);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), player02.positions, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    unsigned int ibo_player_02;
    glGenBuffers(1, &ibo_player_02);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_player_02);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), player02.ibo, GL_DYNAMIC_DRAW);

    ShaderSource source = ParseShader("src/shader.glsl");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    int color_uniform = glGetUniformLocation(shader, "u_Color");
    glUniform4f(color_uniform, 0.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)){
        /* Render here */
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader);

        glBindVertexArray(vao_player_01);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

        glBindVertexArray(vao_player_02);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}