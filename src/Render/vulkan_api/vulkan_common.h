#ifndef S_VULKAN_COMMON_H_
#define S_VULKAN_COMMON_H_

#include "vk_vma/vk_mem_alloc.h"
#include "vk_volk/volk.h"

#include <Base/base_common.h>
#include <numbers>
#include <limits> 
#include <unordered_set>
#include <future>
#include <functional>
#include <format>
#include <memory>
#include <typeindex>

#define VK_CHECK(x)\
	{\
	  VkResult err = x; \
      SPAssert(err, std::to_string(err)); \
	}


#define ASSERT_VK_HANDLE(handle)        \
	SPAssert((handle) == VK_NULL_HANDLE, "Handle is NULL");\



#define VKBP_DISABLE_WARNINGS() \
	__pragma(warning(push, 0))

#define VKBP_ENABLE_WARNINGS() \
	__pragma(warning(pop))

#define VK_FLAGS_NONE 0        // Custom define for better code readability

#define DEFAULT_FENCE_TIMEOUT 100000000000        // Default fence timeout in nanoseconds

VKBP_DISABLE_WARNINGS()
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
VKBP_ENABLE_WARNINGS()

#include "vulkan_common_tool.h"



namespace spp 
{
inline void hash_combine(size_t & seed, size_t hash)
{
	hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= hash;
}
}




namespace sp 
{

	template <typename T>
	inline void write(std::ostringstream& os, const T& value)
	{
		os.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	inline void write(std::ostringstream& os, const std::string& value)
	{
		write(os, value.size());
		os.write(value.data(), value.size());
	}

	template <class T>
	inline void write(std::ostringstream& os, const std::set<T>& value)
	{
		write(os, value.size());
		for (const T& item : value)
		{
			os.write(reinterpret_cast<const char*>(&item), sizeof(T));
		}
	}

	template <class T>
	inline void write(std::ostringstream& os, const std::vector<T>& value)
	{
		write(os, value.size());
		os.write(reinterpret_cast<const char*>(value.data()), value.size() * sizeof(T));
	}

	template <class T, class S>
	inline void write(std::ostringstream& os, const std::map<T, S>& value)
	{
		write(os, value.size());

		for (const std::pair<T, S>& item : value)
		{
			write(os, item.first);
			write(os, item.second);
		}
	}

	template <class T, uint32_t N>
	inline void write(std::ostringstream& os, const std::array<T, N>& value)
	{
		os.write(reinterpret_cast<const char*>(value.data()), N * sizeof(T));
	}

	template <typename T, typename... Args>
	inline void write(std::ostringstream& os, const T& first_arg, const Args &... args)
	{
		write(os, first_arg);

		write(os, args...);
	}



	template <class T>
	inline void hash_combine(size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		spp::hash_combine(seed, hasher(v));
	}

	template <class T>
	uint32_t to_u32(T value)
	{
		static_assert(std::is_arithmetic<T>::value, "T must be numeric");

		if (static_cast<uintmax_t>(value) > static_cast<uintmax_t>(std::numeric_limits<uint32_t>::max()))
		{
			throw std::runtime_error("to_u32() failed, value is too big to be converted to uint32_t");
		}

		return static_cast<uint32_t>(value);
	}

	inline std::string to_snake_case(const std::string& text)
	{
		std::stringstream result;

		for (const auto ch : text)
		{
			if (std::isalpha(ch))
			{
				if (std::isspace(ch))
				{
					result << "_";
				}
				else
				{
					if (std::isupper(ch))
					{
						result << "_";
					}

					result << static_cast<char>(std::tolower(ch));
				}
			}
			else
			{
				result << ch;
			}
		}

		return result.str();
	}
}

class VulkanException : public std::runtime_error
{
public:
	/**
	 * @brief Vulkan exception constructor
	 */
	VulkanException(VkResult result, const std::string& msg = "Vulkan error") :
		result{ result },
		std::runtime_error{ msg }
	{
		error_message = std::string(std::runtime_error::what()) + std::string{ " : " } + std::to_string(result);
	}

	/**
	 * @brief Returns the Vulkan error code as string
	 * @return String message of exception
	 */
	const char* what() const noexcept override 
	{
		return error_message.c_str();
	}

	VkResult result;

private:
	std::string error_message;
};

struct PerFrame
{
	//由SPSwapChain创建
	VkFence queue_submit_fence = VK_NULL_HANDLE;
	//由SPSwapChain创建
	VkCommandBuffer primary_command_buffer = VK_NULL_HANDLE;
	//由SPSwapChain创建
	VkCommandPool primary_command_pool = VK_NULL_HANDLE;
	//？
	VkSemaphore swapchain_acquire_semaphore = VK_NULL_HANDLE;
	//？
	VkSemaphore swapchain_release_semaphore = VK_NULL_HANDLE;
	//由SPSwapChain更新
	VkDevice device = VK_NULL_HANDLE;
	//由SPSwapChain更新
	int32_t queue_index;

};

struct SwapchainDimensions
{
	/// Width of the swapchain.
	uint32_t width = 0;

	/// Height of the swapchain.
	uint32_t height = 0;

	/// Pixel format of the swapchain.
	VkFormat format = VK_FORMAT_UNDEFINED;
};

template <class T>
using SPBindingMap = std::map<uint32_t, std::map<uint32_t, T>>;

struct SPLoadStoreInfo
{
	VkAttachmentLoadOp load_op_ = VK_ATTACHMENT_LOAD_OP_CLEAR;

	VkAttachmentStoreOp store_op_ = VK_ATTACHMENT_STORE_OP_STORE;
};

struct ImageMemoryBarrier
{
	VkPipelineStageFlags src_stage_mask_{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };

	VkPipelineStageFlags dst_stage_mask_{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };

	VkAccessFlags src_access_mask_{ 0 };

	VkAccessFlags dst_access_mask_{ 0 };

	VkImageLayout old_layout_{ VK_IMAGE_LAYOUT_UNDEFINED };

	VkImageLayout new_layout_{ VK_IMAGE_LAYOUT_UNDEFINED };

	uint32_t old_queue_family_{ VK_QUEUE_FAMILY_IGNORED };

	uint32_t new_queue_family_{ VK_QUEUE_FAMILY_IGNORED };
};

struct BufferMemoryBarrier
{
	VkPipelineStageFlags src_stage_mask_{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };

	VkPipelineStageFlags dst_stage_mask_{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };

	VkAccessFlags src_access_mask_{ 0 };

	VkAccessFlags dst_access_mask_{ 0 };
};


#endif