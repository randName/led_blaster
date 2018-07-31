#include <fstream>

#include <stdio.h>
#include <stdlib.h>

#include "blaster.h"

Blaster::Blaster() {
}

Blaster::~Blaster() {
}

int Blaster::init(int w, int h, const unsigned char * leds) {
	int b, x, y, pos, size;
	char port[14], filename[25];
	std::ifstream inf;

	m_width = w;
	m_height = h;
	size = w * h;
	m_leds = (unsigned char *)leds;

	for ( b = 0; b < BOARDS; ++b ) {
		sprintf(port, "/dev/ttyUSB%d", b);
		if ( m_ard[b].begin(port) < 0 ) {
			continue;
		}

		m_info[b][0] = m_ard[b].get();
		m_info[b][1] = m_ard[b].get();

		if ( m_info[b][0] != m_info[b][1] ) {
			printf("\nmismatch: %d (%02X) (%02X)\n", b, m_info[b][0], m_info[b][1]);
			continue;
		}

		sprintf(filename, "layouts/%02X/layout.txt", m_info[b][1]);

		inf.open(filename);

		while ( inf >> x >> y ) {
			pos = x + y * m_height;
			if ( pos >= size ) {
				pos = size - 1;
			}
			m_index[b].push_back(pos * 3);
		}

		inf.close();
	}
	return 0;
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