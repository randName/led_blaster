#include <atomic>
#include <sstream>
#include <iostream>

#include <time.h>
#include <pthread.h>

#include "screen.h"
#include "canvas.h"
#include "tcpserver.h"

std::atomic<bool> running(true);

void halt() { running.store(false); }

Screen screen;
Canvas canvas;

class GLHandler : ConnHandler {
public:
	GLHandler(int f, char *a) : ConnHandler(f, a) {}
private:
	int handle(const int len, const char *msg, char *reply) {
		printf("\n%s - %.*s\n", m_addr.c_str(), len, msg);
		return sprintf(reply, "FPS: %.3f\n", canvas.fps());
	}
};

Server<GLHandler> server;

void * prompt(void *);

int main(int argc, const char **argv)
{
	int port = 8080;
	int width = 256;
	int height = 128;
	char * frag_path = (char *)"default.frag";

	int i;
	std::string arg;

	for ( i = 0; i < argc; ++i ) {
		arg = std::string(argv[i]);
		if ( arg == "-f" ) {
			frag_path = (char *)argv[++i];
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
	canvas.use();

	pthread_t prompt_t;
	pthread_create(&prompt_t, NULL, prompt, NULL);

	double now;
	timespec ts_s, ts_n;
	clock_gettime(CLOCK_MONOTONIC, &ts_s);

	while (running.load()) {
		clock_gettime(CLOCK_MONOTONIC, &ts_n);
		now = double(ts_n.tv_nsec - ts_s.tv_nsec)/1000000000.0;
		now += double(ts_n.tv_sec - ts_s.tv_sec);

		canvas.update(now);
	}

	server.stop();
	pthread_cancel(prompt_t);
	return 0;
}

void * prompt(void * a) {
	static std::string line;

	printf("> ");
	while (std::getline(std::cin, line)) {
		if (line == "q") {
			halt();
		} else if (line == "fps") {
			printf("%.2f", canvas.fps());
		} else if (line == "t") {
			printf("%.2f", canvas.t());
		} else {
			printf("???");
		}
		if (running.load()) {
			printf("\n> ");
		}
	}
	return (void *)0;
}
