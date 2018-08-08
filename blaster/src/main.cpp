#include <atomic>
#include <sstream>
#include <iostream>

#include <pthread.h>

#include "timer.h"
#include "screen.h"
#include "canvas.h"
#include "blaster.h"
#include "tcpserver.h"

bool halt = false;
std::atomic<bool> running(true);
std::atomic<bool> reload(false);

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
const char * frag_path = NULL;

void * prompt(void *);

int main(int argc, const char **argv)
{
	int port = 8080;
	int width = 128;
	int height = 128;

	int i;
	std::string arg;

	if ( argc < 2 ) {
		printf("usage: %s [-p port] [fragment shader]\n", argv[0]);
		exit(1);
	};

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
		} else {
			frag_path = argv[i];
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

		if (reload.load()) {
			canvas.load(frag_path);
			reload.store(false);
		}
	}

	server.stop();
	pthread_cancel(prompt_t);
	if ( halt ) {
		system("halt");
	}
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
		if ( ( ss >> cmd ) && cmd == "halt" ) {
			halt = true;
		}
		return sprintf(reply, "bye!\n");
	}

	if ( cmd == "fps" ) {
		return sprintf(reply, "%.3f\n", canvas.fps());
	}

	if ( cmd == "t" ) {
		if ( ss >> l_double ) {
			timer.shift(l_double);
		}
		return sprintf(reply, "%.5f\n", timer.now());
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

	if ( cmd == "r" ) {
		if ( ss >> cmd ) {
			frag_path = strdup(cmd.c_str());
		}
		reload.store(true);
		return sprintf(reply, "loading %s\n", frag_path);
	}

	if ( cmd == "u" ) {
		float value[32];
		if ( ss >> cmd ) {
			if ( ss >> l_int ) {
				i = 0;
				while ( ss >> l_double ) {
					value[i] = (float)l_double;
					++i;
					if ( (int)i == l_int ) {
						break;
					}
				}

				if ( i > 0 ) {
					canvas.set_uniform(cmd, i, value);
					return sprintf(reply, "%d\n", i);
				}
			} else {
				l_int = canvas.get_uniform(cmd, value);
				switch ( l_int ) {
					case 1:
						return sprintf(reply, "%.5f\n", value[0]);
					case 2:
						return sprintf(reply, "%.4f\t%.4f\n", value[0], value[1]);
					case 3:
						return sprintf(reply, "%.3f\t%.3f\t%.3f\n", value[0], value[1], value[2]);
					default:
						break;
				}
			}
		}
	}

	return sprintf(reply, "???\n");
}
