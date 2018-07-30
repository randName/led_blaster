#include <time.h>

#define NSECTOSEC 1000000000.0f

class Timer {
public:
	Timer() {}
	virtual ~Timer() {}

	void start() {
		clock_gettime(CLOCK_MONOTONIC, &m_started);
	}

	double update() {
		clock_gettime(CLOCK_MONOTONIC, &m_ts_now);
		m_now = double(m_ts_now.tv_nsec - m_started.tv_nsec)/NSECTOSEC;
		m_now += double(m_ts_now.tv_sec - m_started.tv_sec);

		return m_now;
	}

	void shift(double new_t) {
		static double diff_t;
		static time_t diff_i;

		diff_t = new_t - m_now;
		if ( diff_t <= 0 ) {
			return;
		}

		diff_i = diff_t;
		diff_t -= diff_i;
		m_started.tv_sec -= diff_i;
		m_started.tv_nsec -= diff_t * NSECTOSEC;
	}

	const double now() const { return m_now; }
private:
	timespec m_started;
	timespec m_ts_now;
	double m_now;
};
