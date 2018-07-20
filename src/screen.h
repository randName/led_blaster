#pragma once

#include <EGL/egl.h>

static const EGLint MAIN_ATT[] = {
	EGL_RED_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_DEPTH_SIZE, 8,
	EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
	EGL_NONE
};

static const EGLint CONT_ATT[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};

class Screen {
public:
	Screen();
	virtual ~Screen();
	void init(unsigned int w, unsigned int h);
	void render() const;

	const double width() const { return m_width; }
	const double height() const { return m_height; }

private:
	unsigned int m_width;
	unsigned int m_height;

	EGLConfig m_config;
	EGLDisplay m_display;
	EGLSurface m_surface;
	EGLContext m_context;
};
