/*
 * @file Window.h
 * @brief ウィンドウ処理のクラス
 * @detail 画面上に表示する部分をクリッピング空間にはめ込む座標変換を行う
 *         ウィンドウのサイズが変更された時だけglViewport()を実行して、ビューポートの設定を行う
 */

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

    // 図形の正規化デバイス座標系上での位置
    GLfloat location[2];

    // キーボードの状態
    int key_status;

public:
    // コンストラクタ
    Window(int width = 640, int height = 480, const char *title = "Hello!")
    : window(glfwCreateWindow(width, height, title, nullptr, nullptr))
    , scale(100.0f), location{0, 0}, key_status(GLFW_RELEASE) {
        if (window == nullptr) {
            // ウィンドウが作成できなかった
            std::cerr << "Can't create GLFW window." << std::endl;
            exit(1);
        }

        // 現在のウィンドウを処理対象にする
        glfwMakeContextCurrent(window);

        // GLEWを初期化する
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            // GLEWの初期化に失敗した
            std::cerr << "Can't initialize GLEW" << std::endl;
            exit(1);
        }

        // 垂直同期のタイミングを待つ
        glfwSwapInterval(1);

        // ウィンドウのサイズ変更時に呼び出す処理の登録
        glfwSetWindowSizeCallback(window, resize);

        // マウスホイール操作時に呼び出す処理の登録
        glfwSetScrollCallback(window, wheel);

        // キーボード操作時に呼び出す処理の登録
        glfwSetKeyCallback(window, keyboard);

        // このインスタンスのthisポインタを記録しておく
        glfwSetWindowUserPointer(window, this);

        // 開いたウィンドウの初期設定
        resize(window, width, height);
    }

    // デストラクタ
    virtual ~Window() {
        glfwDestroyWindow(window);
    }

    // ウィンドウを閉じるべきか判定する
    int shouldClose() {
        return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
    }

    // カラーバッファを入れ替えてイベントを取り出す
    void swapBuffers() {
        // カラーバッファを入れ替える
        glfwSwapBuffers(window);

        // イベントを取り出す
        if (key_status == GLFW_RELEASE)
            glfwWaitEvents();
        else
            glfwPollEvents();

        // キーボードの状態を調べる
        if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_RELEASE)
            location[0] -= 2.0f / size[0];
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_RELEASE)
            location[0] += 2.0f / size[0];
        if (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_RELEASE)
            location[1] -= 2.0f / size[1];
        else if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_RELEASE)
            location[1] += 2.0f / size[1];

        // マウスの左ボタンの状態を調べる
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
            // マウスの左ボタンが押されていたらマウスカーソルの位置を取得する
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            // マウスカーソルの正規化デバイス座標系上での位置を求める, マウスカーソルの座標系の原点は左上(正規化デバイス座標系と上下反転)
            location[0] = static_cast<GLfloat>(x) * 2.0f / size[0] - 1.0f;
            location[1] = 1.0f - static_cast<GLfloat>(y) * 2.0f / size[1];
        }
    }

    // ウィンドウのサイズを取り出す
    const GLfloat *getSize() const { return size; }

    // ワールド座標系に対するデバイス座標系の拡大率を取り出す
    GLfloat getScale() const { return scale; }

    // 位置を取り出す
    const GLfloat *getLocation() const { return location; }

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

    // マウスホイール操作時の処理
    static void wheel(GLFWwindow *window, double x, double y)
    {
        // このインスタンスのthisポインタを得る
        auto *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

        if (instance != nullptr) {
            if ((y > 0 && instance->scale <= instance->size[0] && instance->scale <= instance->size[1])
                || (y < 0 && instance->scale > 0)) { // 勢いよくスクロールするとバグる
                // ワールド座標系に対するデバイス座標系の拡大率を更新する
                instance->scale += static_cast<GLfloat>(y);
            }
        }
    }

    // キーボード操作時の処理
    static void keyboard(GLFWwindow *window, int key, int scan_code, int action, int mods)
    {
        // このインスタンスのthisポインタを得る
        auto *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

        if (instance != nullptr) {
            // キーの状態を保存する
            instance->key_status = action;
        }
    }
};