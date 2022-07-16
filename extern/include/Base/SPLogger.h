#ifndef S_SPLOGGER_H
#define S_SPLOGGER_H

#include "SPVariable.h"

#include <fstream>

#if defined(_WIN32)
#include <windows.h>
#endif

#define LOG_PATH "log.log"

class SPFiler;
class ofstream : public std::ofstream
{
public:
	ofstream(const SPFiler& fi, ios_base::openmode mode =
		std::ios::out | std::ios::trunc);
	virtual ~ofstream() = default;

};


class SPLogger
{
public:
	static const sp_int_16 Buffer_Size = 4024;
	static SPLogger* Get();
	void init(const sp_string& log_path);
	void message(const sp_char* msg, ...);
	void warning(const sp_char* msg, ...);
	void error(const sp_char* msg, ...);
	void log(const sp_char* msg, ...);
	
protected:
	SPLogger();
	virtual ~SPLogger();
protected:
	
	ofstream* ofstm_;
	static SPLogger* Instance_;
};

#ifndef SP_OUTGOING
#define SP_PRINT(_logfunc, s_func, s_file, s_msg, s_line){\
	std::stringstream s_str;\
	s_str << s_func << " (" << s_line << "): " << s_msg << " <" s_file << ">";\
    s_str << std::endl;\
	SPLogger::Get()->_logfunc(s_str.str().c_str());}
#else
#define SP_PRINT(_logfunc, s_func, s_file, s_msg, s_line){\
	std::stringstream s_str;\
	s_str << s_func <<" (" << s_line << "): "<< s_msg ;\
    s_str << std::endl;\
	SPLogger::Get()->_logfunc(s_str.str().c_str());}
#endif

#define SPMessage(s_msg) SP_PRINT(message,__FUNCTION__,__FILE__ ,s_msg, __LINE__)
#define SPWarning(s_msg) SP_PRINT(warning,__FUNCTION__,__FILE__ ,s_msg, __LINE__)
#define SPError(s_msg) SP_PRINT(error,__FUNCTION__,__FILE__ ,s_msg, __LINE__)
#define SPLog(s_msg) SP_PRINT(log,__FUNCTION__,__FILE__ ,s_msg, __LINE__)


#if defined(_WIN32)
#   define SP_COUT(s_msg){\
	std::stringstream s_str;\
		s_str << "Cout: " << s_msg ;\
		s_str << std::endl;\
		OutputDebugString(s_str.str().c_str());}
#endif

#if defined(_WIN32)
#   define SPCout(s_msg) SP_COUT(s_msg)
# else
#   define SPCout(s_msg)
#endif


#endif