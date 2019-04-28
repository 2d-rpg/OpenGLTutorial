#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include "Window.h"
#include "Shape.h"
#define  STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//プログラムオブジェクトのリンク結果を表示する
// program: プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program){
    //リンクを取得する
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)std::cerr << "Link Error." << std::endl;

    //シェーダのリンク時のログの長さを取得する
    GLsizei bufSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

    if(bufSize > 1){
        //シェーダのリンク時のログを取得する
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std:: endl;
    }
    return static_cast<GLboolean>(status);
}

//シェーダオブジェクトのコンパイル結果を表示する
// shader: シェーダオブジェクト名
// str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str){
    //コンパイル結果を取得する
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

    //シェーダのコンパイル時のログの長さを取得する
    GLsizei bufSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

    if(bufSize > 1){
        //シェーダのコンパイル時のログの内容を取得する
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetShaderInfoLog(shader, bufSize,&length,&infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }
    return static_cast<GLboolean>(status);
}

//プログラムを作成する
// vstc: バーテックスジェーダのソースプログラムの文字列
// fsrc: フラグメントシェーダのソースプログラムの文字列
GLuint createProgram(const char *vsrc, const char *fsrc){
    //からのプログラムオブジェクトを作成する
    const GLuint program(glCreateProgram());

    if(vsrc != NULL){
        //バーテックスシェーダのシェーダオブジェクトを作成する
        const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
        glShaderSource(vobj,1,&vsrc,NULL);
        glCompileShader(vobj);

        //バーテックスのシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
        if(printShaderInfoLog(vobj, "vertex shader"))
            glAttachShader(program,vobj);
        glDeleteShader(vobj);
    }

    if(fsrc != NULL){
        // フラグメントシェーダのシェーダオブジェクトを作成する
        const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
        glShaderSource(fobj,1,&fsrc,NULL);
        glCompileShader(fobj);

        //フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
        if(printShaderInfoLog(fobj, "fragment shader"))
            glAttachShader(program,fobj);
        glDeleteShader(fobj);
    }

    //プログラムオブジェクトをリンクする
    glBindAttribLocation(program,0,"aPos");
    glBindAttribLocation(program,1,"aPos");
    glBindAttribLocation(program,2,"aPos");
    glBindFragDataLocation(program,0,"FragColor");
    glLinkProgram(program);

    //作成したプログラムオブジェクトを返す
    if(printProgramInfoLog(program)){
        return program;
    }

    //プログラムオブジェクトが作成できなければ0を返す
    glDeleteProgram(program);
    return 0;
}

//シェーダのそーづファイるを読み込んだメモリを返す
// name:シェーダのそーづファイル名
// buffer: 読み込んだソースファイルのテキスト
bool readShaderSource(const char *name, std::vector<GLchar> &buffer){
    //ファイル名がNULLだった
    if(name == NULL)return false;

    //ソースファイルを開く
    std::ifstream file(name, std::ios::binary);
    if(file.fail()){
        //開かなかった
        std::cerr << "Error: Can't open source file: " << name << std::endl;
        return false;
    }

    file.seekg(0L,std::ios::end);
    GLsizei length = static_cast<GLsizei>(file.tellg());

    //ファイルサイズのメモリ確保
    buffer.resize(length+1);

    //ファイルを先頭から読み込む
    file.seekg(0L, std::ios::beg);
    file.read(buffer.data(),length);
    buffer[length] = '\0';

    if(file.fail()){
        //うまく読み込めめなかった
        std::cerr << "Error: Could not read source file: " << name << std::endl;
        file.close();
        return false;
    }

    //読み込み成功
    file.close();
    return true;
}

GLuint loadProgram(const char * vert,const char *frag){
    //シェーダのソースファイルを読み込む
    std::vector<GLchar> vsrc;
    const bool vstat(readShaderSource(vert,vsrc));
    std::vector<GLchar> fsrc;
    const bool fstat(readShaderSource(frag,fsrc));

    //プログラムオブジェクトを作成する
    return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

//短形の頂点の位置
constexpr Object::Vertex rectangleVertex[] =
        {
                { -0.5f, -0.5f },
                { 0.5f,-0.5f},
                { 0.5f, 0.5f },
                { -0.5f, 0.5f }
        };

constexpr Object::Vertex6 rectangleVertex6[] ={
        {0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f},  // bottom right
        {-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f},   // bottom left
        {0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f}    // top
};

constexpr Object::Vertex8 rectangleVertex8[] = {
        // positions          // colors           // texture coords
        {0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f},   // top right
        {0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f},   // bottom right
        {-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f},   // bottom left
        {-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f}    // top left
};


int main() {

    // GLFWを初期化する
    if (glfwInit() == GL_FALSE) {
        //初期化に失敗した
        std::cerr << "Can't initialize GLFW" << std::endl;
        return 1;
    }

    //プログラムの終了時の処理を登録する
    atexit(glfwTerminate);

    // OpenGL Version 3.2 Core Profile を選択する
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //ウィンドウを作成する
    Window window;

    //背景色を指定する
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    //プログラムオブジェクトを作成する
    const GLuint program(loadProgram("pointTexture.vert", "pointTexture.frag"));

    //uniform変数の場所を取得する
    const GLint sizeLoc(glGetUniformLocation(program,"size"));
    const GLint scaleLoc(glGetUniformLocation(program,"scale"));
    const GLint locationLoc(glGetUniformLocation(program,"location"));

    // uniform 変数の場所を取得する
    const GLint aspectLoc(glGetUniformLocation(program,"aspect"));

    //図形データを作成する
    // size:頂点の位置の次元
    // vertexcount: 頂点の数
    // vertex: 頂点属性を格納した配列
    //std::unique_ptr<const Shape> shape(new Shape(2,4,rectangleVertex));
    //std::unique_ptr<const Shape> shape(new Shape(6,3,rectangleVertex6));
    std::unique_ptr<const Shape> shape(new Shape(8,4,rectangleVertex8));

    //tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    //-----------------------------------------
    glUseProgram(program);

    //ウィンドウが開いている間繰り返す
    while(window.shouldClose() == GL_FALSE){
        //ウィンドウを消去する
        glClear(GL_COLOR_BUFFER_BIT);

        //glBindTexture(GL_TEXTURE_2D, texture);// bind Texture

        //シェーダプログラムの使用開始
        glUseProgram(program);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 2);

        //create transformations
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform,glm::vec3(0.5f,-0.5f,0.0f));
        transform = glm::rotate(transform,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));

        //get matrix's uniform location and set matrix
        const GLint transformLoc = glGetUniformLocation(program,"transform");
        glUniformMatrix4fv(transformLoc,1,GL_FALSE,glm::value_ptr(transform));




        // uniform変数に値を設定する
        //glUniform1f(aspectLoc, window.getAspect());
        //glUniform2fv(sizeLoc,1,window.getSize());
        //glUniform1f(scaleLoc,window.getScale());
        glUniform2fv(locationLoc,1,window.getLocation());

        glUniform1i(glGetUniformLocation(program, "texture1"), 0);
        glUniform1i(glGetUniformLocation(program, "texture2"), 1);

        //
        //ここで描写処理を行う
        shape->draw();
        //

        //カラーバッファを入れ替える
        //glfwSwapBuffers(window);
        window.swapBuffers();

        //イベントを取り出す
        //glfwWaitEvents();
    }
}
