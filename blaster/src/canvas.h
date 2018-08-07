#pragma once

#include <map>
#include <time.h>
#include <unistd.h>
#include <GLES2/gl2.h>

#define NUM_INDICES 6
#define NUM_VERTICES 12
#define SZ_TRIANGLE 3 * sizeof(GLfloat)
#define SZ_INDICES NUM_INDICES * sizeof(GLushort)
#define SZ_VERTICES NUM_VERTICES * sizeof(GLfloat)

#define WAIT_FRAME 0.0166f

const GLushort indices[NUM_INDICES] = { 0, 1, 3, 0, 2, 3 };
const GLfloat vertices[NUM_VERTICES] = {
	-1.0f, -1.0f, 0.0f,	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,	1.0f,  1.0f, 0.0f,
};

struct Uniform {
	size_t size;
	float * value = NULL;
};

typedef std::map<std::string, Uniform> UniformMap;

class Canvas {
public:
	Canvas();
	virtual ~Canvas();
	void init(int width, int height);
	bool load(const char * frag_path);
	void update(const double);
	int set_uniform(std::string, size_t, float *);
	int get_uniform(std::string, float *);

	const double t() const { return m_t; }
	const double fps() const { return m_fps; }
	const unsigned int size() const { return m_bufsize; }
	const unsigned char * buffer() const { return m_buffer; }
private:
	struct tm * m_d;

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

	UniformMap m_uniforms;

	void uniforms() const;
	bool compile(const char * frag_src);
};
