#ifndef S_SPDRAWINGPAD_H_
#define S_SPDRAWINGPAD_H_
class SPNeMu;
class SPWorkFlow;
class  SPDrawingPad
{
public:
	virtual void powerUp(SPStudioManager* manager) = 0;
	virtual void powerOff() = 0;
	virtual void submitGenkoDesign(SPNeMu* nemu) = 0;
	virtual void cleanUpDrawingPad(const Canvas& canvas) = 0;
	virtual void design() = 0;
	virtual void creatGenko(SPWorkFlow* workFlow) = 0;
protected:

private:

};

#endif
