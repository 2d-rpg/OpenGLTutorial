#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shape.h"

// シェーダオブジェクトのコンパイル結果を表示
// shader: シェーダオブジェクト名
// str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str) {
	// コンパイル結果取得
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "コンパイルエラー: " << str << std::endl;

	// シェーダーのコンパイル時のログの長さを取得
	GLsizei bufSize;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &bufSize);
	
	if (bufSize > 1) {
		// シェーダーのコンパイル時ログの内容を取得
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

// プログラムオブジェクトのリンク結果を表示
// program: プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program) {
	// リンク結果取得
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "リンクエラー" << std::endl;

	// シェーダのリンク時のログの長さを取得
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// シェーダのリンク時のログの内容を取得
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

// プログラムオブジェクト生成
// vsrc: バーテックスシェーダのソースプログラムの文字列
// fsrc: フラグメントシェーダのソースプログラムの文字列
GLuint createProgram(const char *vsrc, const char *fsrc) {
	// 空のプロジェクトプログラム生成
	const GLuint program(glCreateProgram());

	if (vsrc != NULL) {
		// バーテックスシェーダのシェーダオブジェクト生成
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		// バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if(printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc != NULL) {
		// フラグメントシェーダのシェーダオブジェクト作成
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		// フラグメントシェーダのシェーダオブジェクトをぷrグラムオブジェクトに組み込む
		if (printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}

	// プログラムオブジェクトをリンク
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);


	// 作成したプログラムオブジェクトを返す
	if (printProgramInfoLog(program))
		return program;

	// プログラムオブジェクトが作成できなければ0を返す
	glDeleteProgram(program);
	return 0;
}

// シェーダのソースファイルを読み込んだメモリを返す
// name: シェーダのソースファイル名
// buffer: 読み込んだソースファイルのテキスト
bool readShaderSource(const char *name, std::vector<GLchar> &buffer) {
	// ファイル名がNULLだった
	if (name == NULL) return false;

	// ソースファイルを開く
	std::ifstream file(name, std::ios::binary);
	if (file.fail()) {
		// 失敗時
		std::cerr << "ファイルを開けませんでした: " << name << std::endl;
		return false;
	}

	// ファイルの末尾に移動し、現在位置、すなわちファイルサイズを得る
	file.seekg(0L, std::ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	// ファイルサイズのメモリ確保
	buffer.resize(length + 1);

	// ファイルを先頭から読み込む
	file.seekg(0L, std::ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail()) {
		// 失敗時
		std::cerr << "ソースファイルを正しく読み込めませんでした: " << name << std::endl;
		file.close();
		return false;
	}

	// 成功時
	file.close();
	return true;
}

// シェーダのソースファイルを読み込んで、プログラムオブジェクトを生成
// vert: バーテックスシェーダのソースファイル名
// frag: フラグメントシェーダのソースファイル名
GLuint loadProgram(const char *vert, const char *frag) {
	// シェーダのソースファイルを読み込む
	std::vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	std::vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	// プログラムオブジェクトを生成
	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// 矩形の頂点の位置
constexpr Object::Vertex rectangleVertex[] = {
	{-0.5f, -0.5f},
    { 0.5f, -0.5f},
	{ 0.5f,  0.5f},
	{-0.5f,  0.5f}
};

int main() {
	// GLFW初期化
	if (glfwInit() == GL_FALSE) {
		// 初期化失敗時
		std::cerr << "GLFWの初期化に失敗しました。" << std::endl;
		return 1;
	}

	// プログラム終了時処理の登録
	atexit(glfwTerminate);

	// OpenGL Version 3.2 Core Profile を選択
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// ウィンドウ作成
	GLFWwindow *const window(glfwCreateWindow(1920, 1080, "2D_Game", NULL, NULL));
	if (window == NULL) {
		// ウィンドウ生成失敗時
		std::cerr << "GLFWウィンドウの生成に失敗しました。" << std::endl;
		return 1;
	}

	// 作成したウィンドウをOpenGLの処理対象に設定
	glfwMakeContextCurrent(window);

	// GLEW初期化
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		// GLEW初期化失敗時
		std::cerr << "GLEW初期化に失敗しました。" << std::endl;
		return 1;
	}

	// 垂直同期を待機
	glfwSwapInterval(1);

	// 背景色指定
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// プログラムオブジェクト生成
	const GLuint program(loadProgram("point.vert", "point.frag"));

	// 図形データを作成
	std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));

	// ウィンドウが開いている間のループ
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// ウィンドウ初期化
		glClear(GL_COLOR_BUFFER_BIT);

		// シェーダプログラムの使用開始
		glUseProgram(program);

		// 描画処理
		shape->draw();

		// カラーバッファ交換
		glfwSwapBuffers(window);

		// イベントが処理できるまで待機
		glfwWaitEvents();
	}
}