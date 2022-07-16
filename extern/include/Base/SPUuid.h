#ifndef S_SPUUID_H_
#define S_SPUUID_H_

#include "SPVariable.h"

// __linux__ 版本等有环境的时候再实现
class SPUuid
{
public:
	SPUuid() = default;
	virtual ~SPUuid() = default;
	static sp_uuid Create_Uuid(void);
	static sp_string Create_Uuid_Str(void);
	static sp_uuid Uuid_Str_To_Sp_Uuid(const sp_string& uuidStr);
	static sp_string Sp_Uuid_To_Uuid_Str(const sp_uuid& uuid);
};
#endif 