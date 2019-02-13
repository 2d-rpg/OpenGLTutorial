/*
 * @file Shape.h
 * @brief 図形の描画を行うクラス
 * @detail 描画するObjectクラスのインスタンス(図形データ)を指すポインタを保持
 */

#pragma once

#include <memory>

// 図形データ
#include "Object.h"
#include "stb_image.h"

// 図形の描画
class Texture {
    // 図形データ
    std::shared_ptr<const Object> object;

protected:
    // 描画に使う頂点の数
    const GLsizei vertex_count;

    GLuint texture;

public:
    /*
     * @fn
     * コンストラクタ
     * @param size 頂点の位置の次元
     * @param vertex_count 頂点の数
     * @param vertex 頂点属性を格納した配列
     */
    Texture(GLint size, GLsizei vertex_count, const Object::Vertex_Textrue *vertex, const Object::indices *indices)
            : object(new Object(size, vertex_count, vertex, indices))
            , vertex_count(vertex_count)
            {
                GLuint texture;
                glGenTextures(1, &texture);
                this->texture = texture;
                glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
                // set the texture wrapping parameters
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                // set texture filtering parameters
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // load image, create texture and generate mipmaps
                int width, height, nrChannels;
                // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
                unsigned char *data = stbi_load("Avicii.png", &width, &height, &nrChannels, 0);
                if (data)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    glGenerateMipmap(GL_TEXTURE_2D);
                }
                else
                {
                    std::cout << "Failed to load texture" << std::endl;
                }
                stbi_image_free(data);
    }

    // 描画
    void draw() const {
        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);
        // 頂点配列オブジェクトを結合する
        object->bind();

        // 描画の実行
        execute();
    }

    // 描画の実行
    virtual void execute() const {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
};
