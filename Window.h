/*
 * @file Window.h
 * @brief ウィンドウ処理のクラス
 * @detail 画面上に表示する部分をクリッピング空間にはめ込む座標変換を行う
 *         ウィンドウのサイズが変更された時だけglViewport()を実行して、ビューポートの設定を行う
 */

#ifndef INC_2D_GAME_WINDOW_H
#define INC_2D_GAME_WINDOW_H

#endif //INC_2D_GAME_WINDOW_H

#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ウィンドウ関連処理
class Window {
    // ウィンドウのハンドル
    GLFWwindow *const window;

    // ウィンドウのサイズ
    GLfloat size[2];

    // ワールド座標系に対するデバイス座標系の拡大率
    GLfloat scale;

public:
    // コンストラクタ
    Window(int width = 640, int height = 480, const char *title = "Hello!")
    : window(glfwCreateWindow(width, height, title, nullptr, nullptr))
    , scale(100.0f) {
        if (window == nullptr) {
            // ウィンドウが作成できなかった
            std::cerr << "Can't create GLFW window." << std::endl;
            exit(1);
        }

        // 現在のウィンドウを処理対象にする
        glfwMakeContextCurrent(window);

        // GLFWを初期化する
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            // GLEWの初期化に失敗した
            std::cerr << "Can't initialize GLEW" << std::endl;
            exit(1);
        }

        // 垂直同期のタイミングを待つ
        glfwSwapInterval(1);

        // このインスタンスのthisポインタを記録しておく
        glfwSetWindowUserPointer(window, this);

        // ウィンドウのサイズ変更時に呼び出す処理の登録
        glfwSetWindowSizeCallback(window, resize);

        // 開いたウィンドウの初期設定
        resize(window, width, height);
    }

    // デストラクタ
    virtual ~Window() {
        glfwDestroyWindow(window);
    }

    // ウィンドウを閉じるべきか判定する
    int shouldClose() {
        return glfwWindowShouldClose(window);
    }

    // カラーバッファを入れ替えてイベントを取り出す
    void swapBuffers() {
        // カラーバッファを入れ替える
        glfwSwapBuffers(window);

        // イベントを取り出す
        glfwWaitEvents();
    }

    // ウィンドウのサイズを取り出す
    const GLfloat *getSize() const {return size; }

    // ワールド座標系に対するデバイス座標系の拡大率を取り出す
    GLfloat getScale() const { return scale; }

    // ウィンドウのサイズ変更時の処理
    static void resize(GLFWwindow *const window, int width, int height) {
        // ウィンドウ全体をビューポートに設定する
        glViewport(0, 0, width, height);

        // このインスタンスのthisポインタを得る
        auto *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

        if (instance != nullptr) {
            // このインスタンスが保持する縦横比を更新する
            instance->size[0] = static_cast<GLfloat>(width);
            instance->size[1] = static_cast<GLfloat>(height);
        }
    }
};