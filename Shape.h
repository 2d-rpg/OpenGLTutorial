/*
 * @file Shape.h
 * @brief 図形の描画を行うクラス
 * @detail 描画するObjectクラスのインスタンス(図形データ)を指すポインタを保持
 */

#ifndef INC_2D_GAME_SHAPE_H
#define INC_2D_GAME_SHAPE_H

#endif //INC_2D_GAME_SHAPE_H

#pragma once

#include <memory>

// 図形データ
#include "Object.h"

// 図形の描画
class Shape {
    // 図形データ
    std::shared_ptr<const Object> object;

protected:
    // 描画に使う頂点の数
    const GLsizei vertex_count;

public:
    /*
     * @fn
     * コンストラクタ
     * @param size 頂点の位置の次元
     * @param vertex_count 頂点の数
     * @param vertex 頂点属性を格納した配列
     */
    Shape(GLint size, GLsizei vertex_count, const Object::Vertex *vertex)
    : object(new Object(size, vertex_count, vertex))
    , vertex_count(vertex_count){}

    // 描画
    void draw() const {
        // 頂点配列オブジェクトを結合する
        object->bind();

        // 描画の実行
        execute();
    }

    // 描画の実行
    virtual void execute() const {
        glDrawArrays(GL_LINE_LOOP, 0, vertex_count);
    }
};