#include <stdlib.h>

#include "canvas.h"
#include "fs.h"

#define U_LOC(n) glGetUniformLocation(m_program, n)

Canvas::Canvas() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

Canvas::~Canvas() {
	free(m_buffer);
}

void Canvas::init(int width, int height) {
	m_width = width;
	m_height = height;
	m_bufsize = m_width * m_height * 3;

	unsigned char* tmp = NULL;
	tmp = (unsigned char*)realloc(m_buffer, m_bufsize);
	if (tmp != NULL) {
		m_buffer = tmp;
	}

	m_program = glCreateProgram();
	glUseProgram(m_program);

	m_frag = glCreateShader(GL_FRAGMENT_SHADER);
	m_vert = glCreateShader(GL_VERTEX_SHADER);

	const char * vsh = "attribute vec3 p;void main(){gl_Position=vec4(p,1.0);}";
	glShaderSource(m_vert, 1, &vsh, NULL);
	glCompileShader(m_vert);
	glAttachShader(m_program, m_vert);

	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, SZ_VERTICES, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, SZ_INDICES, indices, GL_STATIC_DRAW);

	clock_gettime(CLOCK_MONOTONIC, &m_timestart);
}

bool Canvas::load(const char * frag_path) {
	static char * frag_src;
	load_file(frag_path, &frag_src);
	return compile(frag_src);
}

bool Canvas::compile(const char * frag_src) {
	static GLint compiled, info_len;

	glShaderSource(m_frag, 1, &frag_src, NULL);
	glCompileShader(m_frag);

	glGetShaderiv(m_frag, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(m_frag, GL_INFO_LOG_LENGTH, &info_len);
	return (compiled && info_len <= 1);
}

void Canvas::use() const {
	glAttachShader(m_program, m_frag);
	glLinkProgram(m_program);
	glUseProgram(m_program);

	GLint _l = glGetAttribLocation(m_program, "p");
	glEnableVertexAttribArray(_l);
	glVertexAttribPointer(_l, 3, GL_FLOAT, GL_FALSE, SZ_TRIANGLE, (void*)0);
}

void Canvas::update() {
	static double now;
	static int frames = 0;
	static double fps_t = 0.0f;
	static double last_frame = 0.0f;

	now = timenow();

	if (now - last_frame < WAIT_FRAME) {
		return;
	}

	last_frame = now;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_dt = now - m_t;
	m_t = now;

	set_uniform(U_LOC("u_resolution"), m_width, m_height);
	set_uniform(U_LOC("u_time"), m_t);
	set_uniform(U_LOC("u_delta"), m_dt);
	set_uniform(U_LOC("u_date"),
		m_d->tm_wday, m_d->tm_hour, m_d->tm_min, m_d->tm_sec);

	glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_SHORT, 0);
	glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_buffer);

	++frames;
	fps_t += m_dt;
	if (fps_t >= 1.0f) {
		m_fps = (double)frames / fps_t;
		frames = 0;
		fps_t = 0.0f;
	}
}

double Canvas::timenow() {
	static timespec now;
	static time_t rt;
	clock_gettime(CLOCK_MONOTONIC, &now);
	time(&rt);
	m_d = localtime(&rt);
	return double(now.tv_nsec - m_timestart.tv_nsec)/1000000000.0
		+ double(now.tv_sec - m_timestart.tv_sec);
}

void Canvas::set_uniform(GLint loc, float _x) const {
	if (loc == -1) { return; }
	glUniform1f(loc, _x);
}

void Canvas::set_uniform(GLint loc, float _x, float _y) const {
	if (loc == -1) { return; }
	glUniform2f(loc, _x, _y);
}

void Canvas::set_uniform(GLint loc, float _x, float _y, float _z) const {
	if (loc == -1) { return; }
	glUniform3f(loc, _x, _y, _z);
}

void Canvas::set_uniform(GLint loc, float _x, float _y, float _z, float _w) const {
	if (loc == -1) { return; }
	glUniform4f(loc, _x, _y, _z, _w);
}
