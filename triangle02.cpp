#include <stdio.h>
#include <vector>
#include <fstream>
#include <iostream>
#include "Shape.h"
#include "Window.h"
//#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cmath>

/*
 * @fn
 * シェーダオブジェクトのコンパイル結果を表示する
 * @param shader シェーダオブジェクト名
 * @param str コンパイルエラーが発生した場所を示す文字列
 * @return エラーならば0を返す
 */
GLboolean printShaderInfoLog(GLuint shader, const char *str) {
    // コンパイル結果を取得する
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

    // シェーダのコンパイル時のログの長さを取得する
    GLsizei bufSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1) {
        // シェーダのコンパイル時のログの内容を取得する
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }

    return static_cast<GLboolean>(status);
}

/*
 * @fn
 * プログラムオブジェクトのリンク結果を表示する
 * @param program プログラムオブジェクト名
 * @return エラーならば0を返す
 */
GLboolean printProgramInfoLog(GLuint program) {
    // リンク結果を取得する
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;

    // シェーダのリンク時のログの長さを取得する
    GLsizei bufSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1) {
        // シェーダのリンク時のログ内容を取得する
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }

    return static_cast<GLboolean>(status);
}

/*
 * @fn
 * プログラムオブジェクトの実行可能結果を表示する
 * @param program プログラムオブジェクト名
 * @return エラーならば0を返す
 */
GLboolean printValidateInfoLog(GLuint program) {
    // 実行可能結果を取得する
    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Validate Error." << std::endl;

    // シェーダの描画実行時のログの長さを取得する
    GLsizei bufSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1) {
        // シェーダのリンク時のログ内容を取得する
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }

    return static_cast<GLboolean>(status);
}

/*
 * @fn
 * プログラムオブジェクトを作成する
 * @param vsrc バーテックスシェーダのソースプログラムの文字列
 * @param gsrc フラグメントシェーダのソースプログラムの文字列
 * @return シェーダプログラムオブジェクト
 */
GLuint createProgram(const char *vsrc, const char *fsrc) {
    // 空のプログラムオブジェクトを作成する
    const GLuint program(glCreateProgram());

    if (vsrc != nullptr) {
        // バーテックスシェーダのシェーダオブジェクトを作成する
        const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
        glShaderSource(vobj, 1, &vsrc, nullptr);
        glCompileShader(vobj);

        // バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
        if (printShaderInfoLog(vobj, "vertex shader"))
            glAttachShader(program, vobj);
        glDeleteShader(vobj);
    }

    if (fsrc != nullptr) {
        // フラグメントシェーダのシェーダオブジェクトを作成する
        const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
        glShaderSource(fobj, 1, &fsrc, nullptr);
        glCompileShader(fobj);

        // フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
        if (printShaderInfoLog(fobj, "fragment shader"))
            glAttachShader(program, fobj);
        glDeleteShader(fobj);
    }

    // プログラムオブジェクトをリンクする
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "aColor");
    glBindFragDataLocation(program, 0, "FragColor");
    glLinkProgram(program);

    // 作成したプログラムオブジェクトを返す
    if (printProgramInfoLog(program) /*&& printValidateInfoLog(program)*/)
        return program;

    // プログラムオブジェクトが作成できなければ0を返す
    glDeleteProgram(program);
    return 0;
}

/*
 * @fn
 * シェーダのソースファイルを読み込んだメモリを返す
 * @param name シェーダのソースファイル名
 * @param buffer 読み込んだソースファイルのテキスト
 * @return 読み込み成功時のみtrueを返す
 */
bool readShaderSource(const char *name, std::vector<GLchar> &buffer) {
    // ファイル名がNULLだった
    if (name == nullptr) return false;

    // ソースファイルを開く
    std::ifstream file(name, std::ios::binary);
    if (file.fail()) {
        // 開けなかった
        std::cerr << "Error: Can't open source file: " << name << std::endl;
        return false;
    }

    // ファイルの末尾に移動し現在位置(=ファイルサイズ)を得る
    file.seekg(0L, std::ios::end);
    auto length = static_cast<GLsizei>(file.tellg());

    // ファイルサイズのメモリを確保
    buffer.resize(length + 1);

    // ファイルを先頭から読み込む
    file.seekg(0L, std::ios::beg);
    file.read(buffer.data(), length);
    buffer[length] = '\0';

    if (file.fail()) {
        // うまく読み込めなかった
        std::cerr << "Error; Could not read source file: " << name << std::endl;
        file.close();
        return false;
    }

    // 読み込み成功
    file.close();
    return true;
}

/*
 * @fn
 * シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
 * @param vert バーテックスシェーダのソースファイル名
 * @param frag フラグメントシェーダのソースファイル名
 * @return エラーならば0を返す
 */
GLuint loadProgram(const char *vert, const char *frag) {
    // シェーダのソースファイルを読み込む
    std::vector<GLchar> vsrc;
    const bool vstat(readShaderSource(vert, vsrc));
    std::vector<GLchar> fsrc;
    const bool fstat(readShaderSource(frag, fsrc));

    // プログラムオブジェクトを作成する
    return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// 矩形の頂点の位置
constexpr Object::Vertex_With_Color triangleVertex[] =
        {
                { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f },
                {  1.0f, -1.0f, 0.0f, 1.0f, 0.0f },
                {  0.0f,  1.0f, 0.0f, 0.0f, 1.0f },
        };

int main(int argc, char * argv[]) {
    // GLFW を初期化する
    if (glfwInit() == GL_FALSE) {
        // 初期化に失敗した
        std::cerr << "Can't initialize GLFW" << std::endl;
        return 1;
    }

    // プログラム終了時の処理を登録する
    atexit(glfwTerminate);

    // OpenGL Version 4.1 Core Profile を選択する
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ウィンドウを作成する
    Window window;

    // 背景色を指定する
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // プログラムオブジェクトを作成する
    const GLuint program(loadProgram("triangle02.vert", "triangle02.frag"));

    // プログラムオブジェクトからuniform変数の場所を取得する
    const GLint sizeLoc(glGetUniformLocation(program, "size"));
    const GLint scaleLoc(glGetUniformLocation(program, "scale"));
    const GLint locationLoc(glGetUniformLocation(program, "location"));

    // 図形データを作成する
    std::unique_ptr<const Shape> shape(new Shape(2, 3, triangleVertex));

    // このPCの最大vertex attribute数
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    // ウィンドウが開いている間繰り返す
    while (window.shouldClose() == GL_FALSE) {
        // ウィンドウを削除する
        glClear(GL_COLOR_BUFFER_BIT);

        // シェーダプログラムの使用開始
        glUseProgram(program);

        // uniform変数に値を設定する
        glUniform2fv(sizeLoc, 1, window.getSize());
        glUniform1f(scaleLoc, window.getScale());
        glUniform2fv(locationLoc, 1, window.getLocation());

        // ここで描画処理を行う
        // 図形を描画する
        if (printValidateInfoLog(program))
            shape->draw();

        // カラーバッファを入れ替えて、イベントを取り出す
        window.swapBuffers();
    }
}