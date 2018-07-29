#include <vector>

#include "arduino.h"

#define BOARDS 4
#define BOARD_READY 0xFF
#define BLAST_LIMIT 0.025f

class Blaster {
public:
	Blaster();
	virtual ~Blaster();
	int init(int, int, const unsigned char *);
	void blast(const double);

private:
	int m_width;
	int m_height;
	unsigned char * m_leds;
	unsigned char m_info[BOARDS][2];

	Arduino m_ard[BOARDS];
	std::vector<int> m_index[4];
};
