#include "SPSMesh.h"
#include "SPSSubMesh.h"
#include "SPSNode.h"


SPSMesh::SPSMesh(const std::string& name)
	: SPComponent{ name }
{
}

std::type_index SPSMesh::get_type()
{
	return typeid(SPSMesh);
}

void SPSMesh::add_submesh(SPSSubMesh& submesh)
{
	submeshes_.push_back(&submesh);
}

void SPSMesh::add_node(SPSNode& node)
{
	nodes_.push_back(&node);
}
