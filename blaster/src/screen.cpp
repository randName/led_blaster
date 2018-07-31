#include "screen.h"

Screen::Screen() {
	EGLint numConf;

	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (m_display == EGL_NO_DISPLAY) {
		return;
	}

	if (eglInitialize(m_display, NULL, NULL) == EGL_FALSE) {
		return;
	}

	if (!eglChooseConfig(m_display, MAIN_ATT, &m_config, 1, &numConf)) {
		return;
	}
}

Screen::~Screen() {
	eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(m_display, m_context);
	eglDestroySurface(m_display, m_surface);
	eglTerminate(m_display);
	eglReleaseThread();
}

void Screen::init(unsigned int w, unsigned int h) {
	m_width = w;
	m_height = h;

	const EGLint pbuffer_attr[] = {
		EGL_WIDTH, (EGLint)m_width,
		EGL_HEIGHT, (EGLint)m_height,
		EGL_NONE
	};

	m_surface = eglCreatePbufferSurface(m_display, m_config, pbuffer_attr);
	if (m_surface == EGL_NO_SURFACE) {
		return;
	}

	eglBindAPI(EGL_OPENGL_ES_API);

	m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, CONT_ATT);
	eglMakeCurrent(m_display, m_surface, m_surface, m_context);
}

void Screen::render() const {
	eglSwapBuffers(m_display, m_surface);
}
