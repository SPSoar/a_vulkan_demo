#ifndef S_SPSMESH_H_
#define S_SPSMESH_H_

#include "../vulkan_common.h"

#include "SPComponent.h"

class SPSSubMesh;
class SPSNode;
class SPSMesh : public SPComponent
{
public:
	SPSMesh(const std::string& name);
	virtual ~SPSMesh() = default;
	virtual std::type_index get_type() override;
public:
	void add_submesh(SPSSubMesh& submesh);
	void add_node(SPSNode& node);
private:
	std::vector<SPSSubMesh*> submeshes_;
	std::vector<SPSNode*> nodes_;
};

#endif