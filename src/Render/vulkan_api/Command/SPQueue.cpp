#include "SPQueue.h"
#include "../Driver/SPDevice.h"


SPQueue::SPQueue(SPDevice& device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index)
	: device_(device)
    , family_index_(family_index)
    , index_(index)
    , can_present_(can_present)
    , properties_(properties)
{
    init();
}


SPQueue::SPQueue(SPQueue&& other)
    : device_(other.device_)
    , handle_(other.handle_)
    , family_index_(other.family_index_)
    , index_(other.index_)
    , can_present_(other.can_present_)
    , properties_(other.properties_)
{
    other.handle_ = VK_NULL_HANDLE;
    other.family_index_ = {};
    other.properties_ = {};
    other.can_present_ = VK_FALSE;
    other.index_ = 0;
}

void SPQueue::init()
{
    vkGetDeviceQueue(device_.get_handle(), family_index_, index_, &handle_);
}

VkQueueFamilyProperties SPQueue::get_properties() const
{
    return properties_;
}

uint32_t SPQueue::get_family_index() const
{
    return family_index_;
}

VkBool32 SPQueue::support_present() const
{
   return can_present_;
}

const SPDevice& SPQueue::get_device() const
{
    return device_;
}

VkQueue SPQueue::get_handle() const
{
    return handle_;
}

VkResult SPQueue::submit(const SPCommandBuffer& command_buffer, VkFence fence) const
{
    VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer.get_handle();

    return submit({ submit_info }, fence);
}

VkResult SPQueue::submit(const std::vector<VkSubmitInfo>& submit_infos, VkFence fence) const
{
    return vkQueueSubmit(handle_, sp::to_u32(submit_infos.size()), submit_infos.data(), fence);
}

VkResult SPQueue::present(const VkPresentInfoKHR& present_infos) const
{
    if (!can_present_)
    {
        return VK_ERROR_INCOMPATIBLE_DISPLAY_KHR;
    }

    return vkQueuePresentKHR(handle_, &present_infos);
}

VkResult SPQueue::wait_idle() const
{

    return vkQueueWaitIdle(handle_);
}
