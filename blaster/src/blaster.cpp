#include <fstream>

#include <stdio.h>
#include <stdlib.h>

#include "blaster.h"

Blaster::Blaster() {
}

Blaster::~Blaster() {
}

int Blaster::init(int w, int h, const unsigned char * leds) {
	m_width = w;
	m_height = h;
	m_size = w * h;
	m_leds = (unsigned char *)leds;

	int boards = 0;

	for ( int b = 0; b < BOARDS; ++b ) {
		boards += (int)load(b);
	}

	return boards;
}

bool Blaster::load(const int b) {
	static int x, y, pos;
	static std::ifstream inf;
	static unsigned char layout;
	static char port[14], filename[25];

	if ( b >= BOARDS ) {
		return false;
	}

	if ( m_ard[b].status() == 0 && m_index[b].size() ) {
		return true;
	}

	m_index[b].clear();
	m_id[b] = 0xFF;

	sprintf(port, "/dev/ttyUSB%d", b);
	if ( m_ard[b].begin(port) < 0 ) {
		return false;
	}

	layout = m_ard[b].get();

	sprintf(filename, "layouts/%02X/layout.txt", layout);
	inf.open(filename);
	while ( inf >> x >> y ) {
		pos = x + y * m_height;
		if ( pos >= m_size ) {
			pos = 0;
		}
		m_index[b].push_back(pos * 3);
	}
	inf.close();

	union { unsigned char c[2]; size_t s; } led_length;
	led_length.s = m_index[b].size();
	m_ard[b].send(led_length.c, 2);

	m_id[b] = layout;

	return true;
}

void Blaster::blast(const double now) {
	static int b = 0;
	static double last_blast = 0.0f;
	static std::vector<int>::iterator itr;

	if ( now - last_blast < BLAST_LIMIT ) {
		return;
	}

	last_blast = now;

	for ( b = 0; b < BOARDS; ++b ) {
		if ( m_index[b].size() == 0 || m_ard[b].get(0) != BOARD_READY ) {
			continue;
		}
		for ( itr = m_index[b].begin(); itr != m_index[b].end(); ++itr ) {
			m_ard[b].send(m_leds + *itr, 3);
		}
	}
}
