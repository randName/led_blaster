// Header-only TCP Socket Server. Implement your own Connection Handler
#pragma once

#include <vector>
#include <string>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_SIZE 4096

template <class T> void * run_thread(void * obj) {
	pthread_detach(pthread_self());
	reinterpret_cast<T*>(obj)->_run();
	return (void *)0;
}

class ConnHandler {
public:
	ConnHandler(int fd, char * addr) : m_fd(fd), m_running(true) {
		m_addr.append(addr);
		pthread_create(&m_thread, NULL, run_thread<ConnHandler>, this);
	}

	virtual ~ConnHandler() {
		pthread_cancel(m_thread);
		close(m_fd);
	}

	void _run() {
		static int read, to_send;
		static char read_buf[BUFFER_SIZE], reply_buf[BUFFER_SIZE];

		while (m_running) {
			read = recv(m_fd, read_buf, BUFFER_SIZE, 0);
			if (read < 0) m_running = false;
			if (read < 1) break;

			to_send = handle(read, read_buf, reply_buf);
			if (to_send > 0) {
				send(m_fd, reply_buf, to_send, 0);
			}
		}
	};

protected:
	int m_fd;
	std::string m_addr;
	bool m_running = false;

	virtual int handle(const int len, const char* msg, char* reply) {
		printf("msg: %.*s\n", len, msg);
		return 0;
	}

private:
	pthread_t m_thread;
};

template <class Handler> class Server {
public:
	Server() {
		m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_server_fd < 0) {
			return;
		}

		m_server.sin_family = AF_INET;
		m_server.sin_addr.s_addr = htonl(INADDR_ANY);

		int opt = 1;
		setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	}

	virtual ~Server() {
		typename std::vector<Handler*>::iterator i;
		for (i = m_conns.begin(); i != m_conns.end(); ++i) { delete (*i); }
		m_conns.clear();
		pthread_cancel(m_thread);
		close(m_server_fd);
	}

	int init(int port) {
		static int err;

		if (m_server_fd < 0) {
			return m_server_fd;
		}

		if (m_running) {
			return 0;
		}

		m_server.sin_port = htons(port);
		err = bind(m_server_fd, (struct sockaddr *)&m_server, sizeof(m_server));
		if ( err < 0 ) {
			return err;
		}

		err = listen(m_server_fd, 8);
		if ( err < 0 ) {
			return err;
		}

		m_running = true;

		return pthread_create(&m_thread, NULL, run_thread<Server>, this);
	}

	void _run() {
		static int c_fd;
		static struct sockaddr_in c;
		static socklen_t sosz = sizeof(c);

		while (m_running) {
			c_fd = accept(m_server_fd, (struct sockaddr *)&c, &sosz);
			if ( c_fd < 0 ) {
				continue;
			}

			m_conns.push_back(new Handler(c_fd, inet_ntoa(c.sin_addr)));
		}
	}

	void stop() { m_running = false; }
	const bool running() const { return m_running; }
	void serve() const { while (m_running) usleep(100000); }

private:
	int m_server_fd;
	bool m_running = false;
	struct sockaddr_in m_server;
	pthread_t m_thread;
	std::vector<Handler*> m_conns;
};
