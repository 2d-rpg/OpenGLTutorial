#pragma once

#include <stb/stb_image.h>
#include"include/glad/glad.h"

//図形データ
class Object {
    //頂点配列オブジェクト
    GLuint vao;

    //頂点バッファオブジェクト
    GLuint vbo;

    GLuint ebo;

private:
    //コピーコンストラクタによるコピー禁止
    Object(const Object &o);

    //代入によるコピー禁止
    Object &operator=(const Object &o);

public:
    //頂点属性
    struct Vertex {
        //位置
        float position[2];
    };

    struct Vertex6{
        float position[3];//位置と色を指定する場合
        float aColor[3];
        //GLfloat position[6];
    };

    struct Vertex8{
        float position[3];
        float aColor[3];
        float tCoords[2];
    };


    //配列要素数2
    //コンストラクタ
    //size:頂点の位置の次元
    //vertexcount:頂点の数
    //vertex:頂点属性を格納した配列
    Object(GLint size, GLsizei vertexcount, const Vertex *vertex){
        //頂点配列オブジェクト
        glGenVertexArrays(1,&vao);
        glBindVertexArray(vao);

        //頂点バッファオブジェクト
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,vertexcount * sizeof(Vertex),vertex,GL_STATIC_DRAW);

        //結合されている頂点バッファオブジェクトをin変数から参照できるようにする
        glVertexAttribPointer(0,size,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(0);
    }
    //配列要素数6
    //コンストラクタ
    //size:頂点の位置の次元
    //vertexcount:頂点の数
    //vertex:頂点属性を格納した配列
    Object(GLint size, GLsizei vertexcount, const Vertex6 *vertex){
        //頂点配列オブジェクト
        glGenVertexArrays(1,&vao);
        glBindVertexArray(vao);

        //頂点バッファオブジェクト
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,vertexcount * sizeof(Vertex6),vertex,GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    //配列要素数8
    //コンストラクタ
    //size:頂点の位置の次元
    //vertexcount:頂点の数
    //vertex:頂点属性を格納した配列
    Object(GLint size, GLsizei vertexcount, const Vertex8 *vertex){
        //頂点配列オブジェクト
        glGenVertexArrays(1,&vao);
        glBindVertexArray(vao);

        //頂点バッファオブジェクト
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,vertexcount * sizeof(Vertex8),vertex,GL_STATIC_DRAW);


        unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
        };
        //テクスチャーの頂点バッファオブジェクト
        glGenBuffers(1,&ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // tCoord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        //set the texture  wrapping/filtering options(on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //load and generate the texture
        int width, height, nrChannels;
        unsigned char *data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }else{
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

    virtual ~Object(){
        //頂点配列オブジェクトを削除する
        glDeleteBuffers(1,&vao);

        //頂点バッファオブジェクトを削除する
        glDeleteBuffers(1,&vbo);
    }

    //頂点配列オブジェクトの結合
    void bind() const {
        //描写する頂点配列オブジェクトを指定する
        glBindVertexArray(vao);
    }
};