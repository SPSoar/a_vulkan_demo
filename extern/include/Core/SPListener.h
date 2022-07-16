#ifndef S_SPLISTENER_H
#define S_SPLISTENER_H

#include "core_common.h"

class SPMouseEvent;
class SPKeyEvent;
class SPExtendEvent;
class COREEXPORT SPListener
{
public:
	SPListener():is_on_notify(false){}
	virtual ~SPListener() = default;

	virtual void notify_mouse_event(SPMouseEvent& event) = 0;
	virtual void notify_key_event(SPKeyEvent& event) = 0;
	virtual void notify_extend_event(SPExtendEvent& event) = 0;;
	virtual void on_notify(sp_bool on);
	virtual bool is_notify();

protected:

private:
	sp_bool is_on_notify;
};

#endif
