#ifndef S_SPOBJECT_H
#define S_SPOBJECT_H

#include "SPVariable.h"
#include "SPMacro.h"

#pragma warning(disable:4251)
class SPObject
{
public:
	SPObject() = delete;
	SPObject(const sp_uuid& id, const sp_string& name);
	virtual ~SPObject() = default;
	SP_CLASS_DELETE_COPY(SPObject)
	SP_CLASS_MEMBER(SPObject, sp_uuid, id)
	SP_CLASS_MEMBER(SPObject, sp_string, name)

};

#endif