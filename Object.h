/*
 * @file Object.h
 * @brief 頂点配列オブジェクトのクラス
 */

#pragma once

#include <GL/glew.h>

// 図形データ
class Object {
private:
    // コピーコンストラクタによるコピー禁止
    Object(const Object &o);

    // 代入によるコピー禁止
    Object &operator=(const Object &o);

    // 頂点配列オブジェクト名 vertex array object
    GLuint vao;

    // 頂点バッファオブジェクト名 vertex buffer object
    GLuint vbo;

public:
    // 頂点配列オブジェクトの結合
    void bind() const {
        // 描写する頂点配列オブジェクトを指定する
        glBindVertexArray(vao);
    }

    // 頂点属性 vertex attribute
    struct Vertex {
        // 位置 position
        GLfloat position[2];
    };

    /*
     * @fn
     * コンストラクタ
     * @param size 頂点の位置の次元
     * @param vertex_count 頂点の数
     * @param vertex 頂点属性を格納した配列
     */
    Object(GLint size, GLsizei vertex_count, const Vertex *vertex) {
        // 頂点配列オブジェクト
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // 頂点バッファオブジェクト
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertex, GL_STATIC_DRAW);

        // 統合されている頂点バッファオブジェクトをin変数から参照できるようにする
        glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    }

    // デストラクタ
    virtual ~Object(){
        // 頂点配列オブジェクトを削除する
        glDeleteBuffers(1, &vao);

        // 頂点バッファオブジェクトを削除する
        glDeleteBuffers(1, &vbo);
    }

};