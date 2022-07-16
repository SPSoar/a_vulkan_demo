#ifndef S_SPEVENT_H
#define S_SPEVENT_H

#include "core_common.h"


enum class COREEXPORT ExtendEventType : sp_uint_8
{
	None
};

enum class COREEXPORT KeyEventType : sp_uint_8
{
	None = 0,
	Press,
	Release,
	Undo,
	Redo
};

enum class COREEXPORT MouseEventType : sp_uint_8
{
	None = 0,
	MouseMove,
	Wheel,
	Press,
	Release,
	DoubleClick
};
enum class COREEXPORT MouseButtonType : sp_uint_8
{
	None = 0,
	LeftButton,
	RightButton,
	MidButton
};

enum class COREEXPORT EventSender : sp_uint_16
{
	None,
	GUIApplication,
	RenderWindow
};


class COREEXPORT SPEvent
{

public:
	SPEvent(const EventSender& event_sender);
	virtual ~SPEvent() = default;
	EventSender get_event_sender();
private:
#pragma warning(push)
#pragma warning(disable:4251)
	EventSender  event_sender_;
#pragma warning(pop)
};


class COREEXPORT SPExtendEvent : public SPEvent
{

public:
	SPExtendEvent(const EventSender& event_sender);
	virtual ~SPExtendEvent() = default;
	SP_CLASS_MEMBER(SPExtendEvent, ExtendEventType, event_type)
};


class COREEXPORT SPKeyEvent : public SPEvent
{

public:
	SPKeyEvent(const EventSender& event_sender);
	virtual ~SPKeyEvent() = default;

	SP_CLASS_MEMBER(SPExtendEvent, sp_string, button_type)
	SP_CLASS_MEMBER(SPExtendEvent, KeyEventType, event_type)
	SP_CLASS_MEMBER(SPExtendEvent, sp_bool, is_control_modifier)
};


class COREEXPORT SPMouseEvent : public SPEvent
{
public:
	SPMouseEvent(const EventSender& event_sender);
	virtual ~SPMouseEvent() = default;

	SP_CLASS_MEMBER(SPMouseEvent, sp_uint_32, x)
	SP_CLASS_MEMBER(SPMouseEvent, sp_uint_32, y)
	SP_CLASS_MEMBER(SPMouseEvent, MouseButtonType, button_type)
	SP_CLASS_MEMBER(SPMouseEvent, MouseEventType, event_type)
};

#endif
