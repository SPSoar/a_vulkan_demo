#ifndef S_SPTIMER_H_
#define S_SPTIMER_H_

#include "SPVariable.h"
#include <sys/timeb.h>

class SPTimer
{

public:
	SPTimer();
	virtual ~SPTimer();
	void set_current_time(void);
	void set_time(sp_uint_64 seconds);

	sp_int_64  get_seconds(void) const;
	sp_int_64  get_miliseconds(void) const;

	sp_bool operator == (const SPTimer& time) const;
	sp_bool operator != (const SPTimer& time) const;

	sp_bool operator <  (const SPTimer& time) const;
	sp_bool operator <= (const SPTimer& time) const;
	sp_bool operator >= (const SPTimer& time) const;
	sp_bool operator >  (const SPTimer& time) const;

	static sp_decimal_d Diff_time(const SPTimer& timeStart, const SPTimer& timeEnd = SPTimer());
	static SPTimer Clear_time();

	sp_bool is_time_empty() const;
private:
	struct _timeb time_buffer_;
};

#endif 

