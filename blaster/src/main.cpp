#include <atomic>
#include <sstream>
#include <iostream>

#include <pthread.h>

#include "timer.h"
#include "screen.h"
#include "canvas.h"
#include "blaster.h"
#include "tcpserver.h"

std::atomic<bool> running(true);

int cli(std::string, char*);

class GLHandler : ConnHandler {
public:
	GLHandler(int f, char *a) : ConnHandler(f, a) {}
private:
	int handle(const int len, const char *msg, char *reply) {
		return cli(msg, reply);
	}
};

Timer timer;
Screen screen;
Canvas canvas;
Blaster blaster;
Server<GLHandler> server;

void * prompt(void *);

int main(int argc, const char **argv)
{
	int port = 8080;
	int width = 128;
	int height = 128;
	const char * frag_path = "default.frag";

	int i;
	std::string arg;

	for ( i = 0; i < argc; ++i ) {
		arg = std::string(argv[i]);
		if ( arg == "-f" ) {
			frag_path = argv[++i];
		} else if ( arg == "-w" ) {
			std::stringstream(argv[++i]) >> width;
		} else if ( arg == "-h" ) {
			std::stringstream(argv[++i]) >> height;
		} else if ( arg == "-p" ) {
			std::stringstream(argv[++i]) >> port;
		}
	}

	server.init(port);
	screen.init(width, height);
	canvas.init(width, height);
	canvas.load(frag_path);
	blaster.init(width, height, canvas.buffer());
	timer.start();

	pthread_t prompt_t;
	pthread_create(&prompt_t, NULL, prompt, NULL);

	double now;

	while (running.load()) {
		now = timer.update();
		canvas.update(now);
		blaster.blast(now);
	}

	server.stop();
	pthread_cancel(prompt_t);
	return 0;
}

void * prompt(void * a) {
	static std::string line;
	static char reply[256];

	printf("> ");
	while (std::getline(std::cin, line)) {
		cli(line, reply);
		printf("%s", reply);
		if (running.load()) {
			printf("\n> ");
		}
	}
	return (void *)0;
}

int cli(std::string line, char *reply) {
	static int l_int;
	static double l_double;
	static unsigned int i = 0;

	std::stringstream ss(line);
	std::string cmd;
	ss >> cmd;

	if ( cmd == "q" ) {
		running.store(false);
		return sprintf(reply, "bye!\n");
	}

	if ( cmd == "fps" ) {
		return sprintf(reply, "%.2f\n", canvas.fps());
	}

	if ( cmd == "t" ) {
		if ( ss >> l_double ) {
			timer.shift(l_double);
		}
		return sprintf(reply, "%.3f\n", timer.now());
	}

	if ( cmd == "p" ) {
		const unsigned char * p = canvas.buffer();
		if ( ss >> l_int ) {
			i = l_int * 3;
			if ( i > canvas.size() ) {
				i = 0;
			}
		}
		return sprintf(reply, "#%02X%02X%02X\n", p[i], p[i+1], p[i+2]);
	}

	if ( cmd == "u" ) {
		float value[32];
		if ( ss >> cmd ) {
			i = 0;
			while ( ss >> l_double ) {
				value[i++] = (float)l_double;
			}

			if ( i > 0 ) {
				l_int = line.find('.') == std::string::npos;
				canvas.set_uniform(cmd, i, value, (bool)l_int);
				return sprintf(reply, "%d\n", i);
			}
		}
	}

	return sprintf(reply, "???\n");
}
