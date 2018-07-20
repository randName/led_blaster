#pragma once

#include <time.h>
#include <unistd.h>

#include <GLES2/gl2.h>
#include "screen.h"

#define NUM_INDICES 6 * sizeof(GLushort)
#define NUM_VERTICES 18 * sizeof(GLfloat)

const GLushort indices[] = { 0, 1, 3, 0, 2, 3 };
const GLfloat vertices[] = {
	-1.0f, -1.0f, 0.0f,	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,	1.0f,  1.0f, 0.0f,
};

class Canvas {
public:
	Canvas();
	virtual ~Canvas();
	void init(Screen * screen);
	bool load(const char * frag_path);
	void use() const;
	void update();

	const double t() const { return m_t; }
	const double dt() const { return m_dt; }
	const double fps() const { return m_fps; }
	const unsigned int size() const { return m_bufsize; }
	const unsigned char * buffer() const { return m_buffer; }
private:
	Screen * m_screen;

	struct tm * m_d;
	struct timespec m_timestart;

	double m_t = 0.0f;
	double m_dt = 0.0f;
	double m_fps = 0.0f;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_bufsize;
	unsigned char * m_buffer = NULL;

	GLuint m_vert;
	GLuint m_frag;
	GLuint m_program;
	GLuint m_indexbuffer;
	GLuint m_vertexbuffer;

	double timenow();
	bool compile(const char * frag_src);
	void set_uniform(GLint loc, float _x) const;
	void set_uniform(GLint loc, float _x, float _y) const;
	void set_uniform(GLint loc, float _x, float _y, float _z) const;
	void set_uniform(GLint loc, float _x, float _y, float _z, float _w) const;
};
