#pragma once
#include <memory>

//図形データ
#include "Object.h"

//図形の描写
class Shape{
    //図形データ
    std::shared_ptr<const Object> object;

protected:
    //描写に使う頂点の数
    const GLsizei vertexcount;

public:
    //コンストラクタ
    // size:頂点の位置の次元
    // vertexcount: 頂点の数
    // vertex: 頂点属性を格納した配列
    Shape(GLint size, GLsizei vertexcount,const Object::Vertex *vertex)
    : object(new Object(size,vertexcount,vertex))//代入
    , vertexcount(vertexcount){
    }

    Shape(GLint size, GLsizei vertexcount,const Object::Vertex6 *vertex)
    : object(new Object(size,vertexcount,vertex))//代入
    , vertexcount(vertexcount){
    }

    Shape(GLint size, GLsizei vertexcount,const Object::Vertex8 *vertex)
            : object(new Object(size,vertexcount,vertex))//代入
            , vertexcount(vertexcount){
    }

    //描写
    void draw() const
    {
        //頂点配列オブジェクトを結合する
        object->bind();
        //描写を実行する
        execute();
    }

    //描写の実行
    virtual void execute() const{
        //折れ線で描写する
        glDrawArrays(GL_TRIANGLE_FAN,0,vertexcount);
    }
};