#include <vector>

#include "arduino.h"

#define BOARDS 8
#define BOARD_READY 0xFF
#define BLAST_LIMIT 0.02f

class Blaster {
public:
	Blaster();
	virtual ~Blaster();
	int init(int, int, const unsigned char *);
	bool load(const int);
	void blast(const double);

	const unsigned char id(const int b) const { return m_id[b]; };
private:
	int m_size;
	int m_width;
	int m_height;
	unsigned char * m_leds;
	unsigned char m_id[BOARDS];

	Arduino m_ard[BOARDS];
	std::vector<int> m_index[BOARDS];
};
