#include <atomic>
#include <thread>
#include <iostream>

#include <stdio.h>
#include <unistd.h>

#include "screen.h"
#include "canvas.h"

Screen screen;
Canvas canvas;

void prompt();
void print_screen();

std::atomic<bool> running(true);

int main(int argc, const char **argv)
{
	static uint width = 20;
	static uint height = 15;
	static char * frag_path = (char*)"test.frag";

	screen.init(width, height);
	canvas.init(&screen);
	canvas.load(frag_path);
	canvas.use();

	std::thread prompt_t(&prompt);

	while (running.load()) {
		canvas.update();
	}

	pthread_cancel(prompt_t.native_handle());
	exit(0);
}

void prompt() {
	std::string line;
	printf("> ");
	while (std::getline(std::cin, line)) {
		if (line == "q") {
			running.store(false);
		} else if (line == "fps") {
			printf("%.2f", canvas.fps());
		} else if (line == "t") {
			printf("%.2f", canvas.t());
		} else if (line == "p") {
			print_screen();
		} else {
			printf("???");
		}
		if (running.load()) {
			printf("\n> ");
		}
	}
}

void print_screen() {
	static uint i, w = screen.width();
	static const unsigned char* p = canvas.buffer();

	for (i = 0; i < canvas.size(); i+=3) {
		printf("%02X%02X%02X ", p[i], p[i+1], p[i+2]);
		if (((i/3) % w) == w - 1) {
			printf("\n");
		}
	}
}
