#ifndef S_SPDRAFT_H_
#define S_SPDRAFT_H_
#include "../SPBase.h"
#include "SPBuffer.h"
#include "../studio_enum.h"
class SPMaterial;
class  SPDraft : public SPBase
{
public:
	SPDraft() :SPBase() {}
	virtual ~SPDraft() {}
	virtual void setPrimitiveTopology(PrimitiveTopology topology) = 0;
	virtual void setPrimitiveRestartEnable(spbool enable) = 0;
public:
	SPBuffer* vertexBuffer;
	SPBuffer* indexBuffer;
	SPMaterial* material;
};

#endif
