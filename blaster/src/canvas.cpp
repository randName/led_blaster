#include <stdlib.h>

#include "canvas.h"
#include "fs.h"

#define U_LOC(n) glGetUniformLocation(m_program, n)

const char * vsh = "attribute vec3 p;void main(){gl_Position=vec4(p,1.0);}";

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

	glShaderSource(m_vert, 1, &vsh, NULL);
	glCompileShader(m_vert);
	glAttachShader(m_program, m_vert);

	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, SZ_VERTICES, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, SZ_INDICES, indices, GL_STATIC_DRAW);
}

bool Canvas::load(const char * frag_path) {
	static char * frag_src;
	load_file(frag_path, &frag_src);
	if ( ! compile(frag_src) ) {
		return false;
	}

	glAttachShader(m_program, m_frag);
	glLinkProgram(m_program);
	glUseProgram(m_program);

	GLint _l = glGetAttribLocation(m_program, "p");
	glEnableVertexAttribArray(_l);
	glVertexAttribPointer(_l, 3, GL_FLOAT, GL_FALSE, SZ_TRIANGLE, (void*)0);

	return true;
}

bool Canvas::compile(const char * frag_src) {
	static GLint compiled, info_len;

	glShaderSource(m_frag, 1, &frag_src, NULL);
	glCompileShader(m_frag);

	glGetShaderiv(m_frag, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(m_frag, GL_INFO_LOG_LENGTH, &info_len);

	return (compiled || info_len <= 1);
}

void Canvas::update(const double now) {
	static time_t rt;
	static int frames = 0;
	static double fps_t = 0.0f;
	static double last_frame = 0.0f;

	if (now - last_frame < WAIT_FRAME) {
		return;
	}

	last_frame = now;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	time(&rt);
	m_d = localtime(&rt);
	m_dt = now - m_t;
	m_t = now;

	glUniform2f(U_LOC("u_resolution"), m_width, m_height);
	glUniform1f(U_LOC("u_time"), m_t);
	glUniform1f(U_LOC("u_delta"), m_dt);
	glUniform4f(U_LOC("u_date"),
		m_d->tm_wday, m_d->tm_hour, m_d->tm_min, m_d->tm_sec);

	uniforms();

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

int Canvas::set_uniform(std::string name, size_t size, float * value) {
	static unsigned int i;

	if ( m_uniforms[name].value == NULL ) {
		m_uniforms[name].value = new float[size];
		m_uniforms[name].size = size;
	} else if ( size != m_uniforms[name].size ) {
		float *tmp = (float *)realloc(m_uniforms[name].value, size*sizeof(float));
		if ( tmp != NULL ) {
			m_uniforms[name].value = tmp;
			m_uniforms[name].size = size;
		}
	}

	for ( i = 0; i < m_uniforms[name].size; ++i ) {
		m_uniforms[name].value[i] = *(value + i);
	}

	return 0;
}

void Canvas::uniforms() const {
	static GLint loc;
	static const float * _f;
	static UniformMap::const_iterator itr;

	for ( itr = m_uniforms.begin(); itr != m_uniforms.end(); ++itr ) {
		loc = U_LOC(itr->first.c_str());

		if ( loc == -1 ) {
			continue;
		}

		_f = itr->second.value;

		switch (itr->second.size) {
			case 1:
				glUniform1f(loc, _f[0]);
				break;
			case 2:
				glUniform2f(loc, _f[0], _f[1]);
				break;
			case 3:
				glUniform3f(loc, _f[0], _f[1], _f[2]);
				break;
			case 4:
				glUniform4f(loc, _f[0], _f[1], _f[2], _f[3]);
				break;
			default:
				glUniform1fv(loc, itr->second.size, _f);
				break;
		}
	}
}
