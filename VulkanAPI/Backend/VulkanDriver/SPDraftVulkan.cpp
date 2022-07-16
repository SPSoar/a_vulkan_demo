#include "SPDraftVulkan.h"
#include "../../studio_enum.h"
SPDraftVulkan::SPDraftVulkan()
	: SPDraft()
{

}

SPDraftVulkan::~SPDraftVulkan()
{

	//vkDestroyBuffer(device, indexBuffer, nullptr);
	//vkFreeMemory(device, indexBufferMemory, nullptr);

	//vkDestroyBuffer(device, vertexBuffer, nullptr);
	//vkFreeMemory(device, vertexBufferMemory, nullptr);
}

void SPDraftVulkan::setPrimitiveTopology(PrimitiveTopology topology)
{
	switch (topology)
	{
	case PrimitiveTopology::TRIANGLE:
		this->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	default:
		break;
	}
}

void SPDraftVulkan::setPrimitiveRestartEnable(spbool enable)
{
	primitiveRestartEnable = enable;
}


std::array<VkVertexInputAttributeDescription, 3> SPDraftVulkan::getAttributeDescriptions()
{

	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(SPVertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(SPVertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(SPVertex, texCoord);

	return attributeDescriptions;
}

VkVertexInputBindingDescription SPDraftVulkan::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(SPVertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}
