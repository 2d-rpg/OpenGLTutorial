#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shape.h"

// �V�F�[�_�I�u�W�F�N�g�̃R���p�C�����ʂ�\��
// shader: �V�F�[�_�I�u�W�F�N�g��
// str: �R���p�C���G���[�����������ꏊ������������
GLboolean printShaderInfoLog(GLuint shader, const char *str) {
	// �R���p�C�����ʎ擾
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "�R���p�C���G���[: " << str << std::endl;

	// �V�F�[�_�[�̃R���p�C�����̃��O�̒������擾
	GLsizei bufSize;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &bufSize);
	
	if (bufSize > 1) {
		// �V�F�[�_�[�̃R���p�C�������O�̓��e���擾
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

// �v���O�����I�u�W�F�N�g�̃����N���ʂ�\��
// program: �v���O�����I�u�W�F�N�g��
GLboolean printProgramInfoLog(GLuint program) {
	// �����N���ʎ擾
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "�����N�G���[" << std::endl;

	// �V�F�[�_�̃����N���̃��O�̒������擾
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// �V�F�[�_�̃����N���̃��O�̓��e���擾
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

// �v���O�����I�u�W�F�N�g����
// vsrc: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̕�����
// fsrc: �t���O�����g�V�F�[�_�̃\�[�X�v���O�����̕�����
GLuint createProgram(const char *vsrc, const char *fsrc) {
	// ��̃v���W�F�N�g�v���O��������
	const GLuint program(glCreateProgram());

	if (vsrc != NULL) {
		// �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g����
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		// �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
		if(printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc != NULL) {
		// �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g�쐬
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		// �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g����r�O�����I�u�W�F�N�g�ɑg�ݍ���
		if (printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}

	// �v���O�����I�u�W�F�N�g�������N
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);


	// �쐬�����v���O�����I�u�W�F�N�g��Ԃ�
	if (printProgramInfoLog(program))
		return program;

	// �v���O�����I�u�W�F�N�g���쐬�ł��Ȃ����0��Ԃ�
	glDeleteProgram(program);
	return 0;
}

// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ��񂾃�������Ԃ�
// name: �V�F�[�_�̃\�[�X�t�@�C����
// buffer: �ǂݍ��񂾃\�[�X�t�@�C���̃e�L�X�g
bool readShaderSource(const char *name, std::vector<GLchar> &buffer) {
	// �t�@�C������NULL������
	if (name == NULL) return false;

	// �\�[�X�t�@�C�����J��
	std::ifstream file(name, std::ios::binary);
	if (file.fail()) {
		// ���s��
		std::cerr << "�t�@�C�����J���܂���ł���: " << name << std::endl;
		return false;
	}

	// �t�@�C���̖����Ɉړ����A���݈ʒu�A���Ȃ킿�t�@�C���T�C�Y�𓾂�
	file.seekg(0L, std::ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	// �t�@�C���T�C�Y�̃������m��
	buffer.resize(length + 1);

	// �t�@�C����擪����ǂݍ���
	file.seekg(0L, std::ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail()) {
		// ���s��
		std::cerr << "�\�[�X�t�@�C���𐳂����ǂݍ��߂܂���ł���: " << name << std::endl;
		file.close();
		return false;
	}

	// ������
	file.close();
	return true;
}

// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���ŁA�v���O�����I�u�W�F�N�g�𐶐�
// vert: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�t�@�C����
// frag: �t���O�����g�V�F�[�_�̃\�[�X�t�@�C����
GLuint loadProgram(const char *vert, const char *frag) {
	// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���
	std::vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	std::vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	// �v���O�����I�u�W�F�N�g�𐶐�
	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// ��`�̒��_�̈ʒu
constexpr Object::Vertex rectangleVertex[] = {
	{-0.5f, -0.5f},
    { 0.5f, -0.5f},
	{ 0.5f,  0.5f},
	{-0.5f,  0.5f}
};

int main() {
	// GLFW������
	if (glfwInit() == GL_FALSE) {
		// ���������s��
		std::cerr << "GLFW�̏������Ɏ��s���܂����B" << std::endl;
		return 1;
	}

	// �v���O�����I���������̓o�^
	atexit(glfwTerminate);

	// OpenGL Version 3.2 Core Profile ��I��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// �E�B���h�E�쐬
	GLFWwindow *const window(glfwCreateWindow(1920, 1080, "2D_Game", NULL, NULL));
	if (window == NULL) {
		// �E�B���h�E�������s��
		std::cerr << "GLFW�E�B���h�E�̐����Ɏ��s���܂����B" << std::endl;
		return 1;
	}

	// �쐬�����E�B���h�E��OpenGL�̏����Ώۂɐݒ�
	glfwMakeContextCurrent(window);

	// GLEW������
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		// GLEW���������s��
		std::cerr << "GLEW�������Ɏ��s���܂����B" << std::endl;
		return 1;
	}

	// ����������ҋ@
	glfwSwapInterval(1);

	// �w�i�F�w��
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// �v���O�����I�u�W�F�N�g����
	const GLuint program(loadProgram("point.vert", "point.frag"));

	// �}�`�f�[�^���쐬
	std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));

	// �E�B���h�E���J���Ă���Ԃ̃��[�v
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// �E�B���h�E������
		glClear(GL_COLOR_BUFFER_BIT);

		// �V�F�[�_�v���O�����̎g�p�J�n
		glUseProgram(program);

		// �`�揈��
		shape->draw();

		// �J���[�o�b�t�@����
		glfwSwapBuffers(window);

		// �C�x���g�������ł���܂őҋ@
		glfwWaitEvents();
	}
}