#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>

class Arduino {
public:
	Arduino();
	virtual ~Arduino();
	void reset();
	int flush() const;
	int begin(const char * port, const int baud = B500000);

	int get(const bool blocking = true) const;
	int send(unsigned char * buf, int size) const;
private:
	int m_fd;
	int m_status;
	int m_bdrate;
	char m_port[20];
	struct termios m_settings;

	int status(const bool s = false);
	int settings(const bool s = false);
};
