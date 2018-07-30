#include <unistd.h>

#include "arduino.h"

Arduino::Arduino() {
}

Arduino::~Arduino() {
	close(m_fd);
}

void Arduino::reset() {
	status();
	m_status |= TIOCM_DTR;
	status(1);
	m_status &= ~TIOCM_DTR;
	status(1);
}

int Arduino::flush() const {
	return tcflush(m_fd, TCIOFLUSH);
}

int Arduino::begin(const char * port, const int baud) {
	strcpy(m_port, port);
	m_bdrate = baud;

	m_fd = open(m_port, O_RDWR | O_NOCTTY | O_NDELAY);
	if ( m_fd < 0 ) {
		return m_fd;
	}

	memset(&m_settings, 0, sizeof(m_settings));

	settings();
	m_settings.c_cflag = CS8 | CLOCAL | CREAD;
	m_settings.c_iflag = IGNPAR;
	m_settings.c_oflag = 0;
	m_settings.c_lflag = 0;
	m_settings.c_cc[VMIN] = 0;
	m_settings.c_cc[VTIME] = 0;
	cfsetispeed(&m_settings, m_bdrate);
	cfsetospeed(&m_settings, m_bdrate);
	settings(1);

	reset();
	flush();

	return m_fd;
}

int Arduino::get(const bool blocking) const {
	static int n;
	static unsigned char c;
	n = read(m_fd, &c, 1);
	if ( n > 0 ) return c;
	if ( ! blocking ) return -1;
	while ( n < 1 ) {
		n = read(m_fd, &c, 1);
	}
	return c;
}

int Arduino::send(unsigned char * buf, int size) const {
	static int n;
	n = write(m_fd, buf, size);
	if ( n > 0 ) return n;
	if ( errno == EAGAIN ) return 0;
	return errno;
}

int Arduino::status(const bool s) {
	return ioctl(m_fd, s ? TIOCMSET : TIOCMGET, &m_status);
}

int Arduino::settings(const bool s) {
	if (s) {
		return tcsetattr(m_fd, TCSANOW, &m_settings);
	} else {
		return tcgetattr(m_fd, &m_settings);
	}
}
