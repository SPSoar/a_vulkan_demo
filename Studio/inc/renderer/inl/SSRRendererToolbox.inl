#include "renderer/SSRRenderer.h"

_SSR_STUDIO_BEGIN

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Create_SwapChain)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(studio, RD_Renderer::eStudio);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(swapchain, RD_Renderer::eSwapchain);

     auto& token = studio.get_token();

    _SSR_CORE::RI_SwapchainInfo info{};
    info.mFormat           = _SSR_CORE::RE_ImageFormat::eBGRA8S;
    info.mColorSpace       = _SSR_CORE::RE_ColorSpace::eSrgbNonlinear;
    info.mCompositeAlpha   = _SSR_CORE::RE_CompositeAlpha::eOpaque;
    info.mSurfaceTransform = _SSR_CORE::RE_SurfaceTransform::eIdentity;
    info.mPresentMode      = _SSR_CORE::RE_PresentMode::eFIFO;
    info.mSamples          = _SSR_CORE::RE_SampleCount::eCount1;

    if (swapchain != nullptr) 
    {
        info.mSwapchainWidth  = swapchain->get_config().mSwapchainWidth;
        info.mSwapchainHeight = swapchain->get_config().mSwapchainHeight;
    }
    else 
    {
        info.mSwapchainWidth  = _SSR_CFG::V_WINDOW_WIDTH();
        info.mSwapchainHeight = _SSR_CFG::V_WINDOW_HEIGHT();
    }
    if (swapchain != nullptr) 
    {
        token.delete_swapchain(swapchain);
        swapchain = nullptr;
    }
    swapchain = token.make_swapchain(_SSR_STD::move(info));
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Create_Present_Semaphore_And_Fence)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(studio, RD_Renderer::eStudio);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(presentSemaphores, RD_Renderer::ePresentSemaphores);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderSemaphores, RD_Renderer::eRenderSemaphores);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(fences, RD_Renderer::eFence);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(swapchain, RD_Renderer::eSwapchain);

     auto& token = studio.get_token();

    _SSR_ASSERT(presentSemaphores.empty(), _SSR_RDERROR);
    _SSR_ASSERT(renderSemaphores.empty(), _SSR_RDERROR);
    _SSR_ASSERT(fences.empty(), _SSR_RDERROR);
    _SSR_ASSERT(swapchain->get_config().mImageCount != 0, _SSR_RDERROR);

    presentSemaphores.resize(swapchain->get_config().mImageCount);
    for (auto& presentSemaphore : presentSemaphores)
    {
        _SSR_CORE::RI_SemaphoreInfo info{};
        presentSemaphore = token.make_semaphore(_SSR_STD::move(info));
    }

    renderSemaphores.resize(swapchain->get_config().mImageCount);
    for (auto& renderSemaphore : renderSemaphores)
    {
        _SSR_CORE::RI_SemaphoreInfo info{};
        renderSemaphore = token.make_semaphore(_SSR_STD::move(info));
    }

    fences.resize(swapchain->get_config().mImageCount);
    for (auto& fence : fences) 
    {
        _SSR_CORE::RI_FenceInfo fenceInfo{};
        fence = token.make_fence(_SSR_STD::move(fenceInfo));
    }

    studio.make_primary_draw_commands(swapchain->get_config().mImageCount);
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Create_RenderTargets)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(studio, RD_Renderer::eStudio);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(swapchain, RD_Renderer::eSwapchain);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderTargets, RD_Renderer::eRenderTargets);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderTargetInfo, RD_Renderer::eRenderTargetInfo);

    auto& token = studio.get_token();
    
    renderTargets.resize(swapchain->get_config().mImageCount);

    _SSR_ASSERT(renderTargets.size() == swapchain->get_images().size(), _SSR_RDERROR);
     
    for (SUINT32 i = 0; i < renderTargets.size(); i++)
    {
        auto copyRenderTargetInfo = renderTargetInfo;
        
        auto& flags = copyRenderTargetInfo.mFlags;
        auto& renderTargetsInfo = copyRenderTargetInfo.mRenderTargets;
        for (SUINT32 j = 0; j < renderTargetsInfo.size(); j++)
        {
            auto& rt = renderTargetsInfo[j];
            if (j == _SSR_CORE::RI_RenderTargetInfo::eIndexSwapchain) 
            {
                _SSR_ASSERT(rt.mConfig.p.enable, _SSR_RDERROR);
                _SSR_ASSERT(rt.mConfig.p.windowSize, _SSR_RDERROR);
                _SSR_ASSERT(rt.mConfig.p.usage == _SSR_CORE::RE_AttachmentUsage::eOutput, _SSR_RDERROR);
                _SSR_ASSERT(!rt.mConfig.p.managingLifetime, _SSR_RDERROR);

                rt.mHandle = swapchain->get_images()[i];
                rt.mConfig.p.format = swapchain->get_images()[i]->get_config().mImageParams.i.format;
                rt.mConfig.p.sample = swapchain->get_config().mSamples;
            }
            if (rt.mConfig.p.enable) 
            {
                if (rt.mConfig.p.windowSize)
                {
                    rt.mWidth = swapchain->get_config().mSwapchainWidth;
                    rt.mHeight = swapchain->get_config().mSwapchainHeight;
                }
            }
        }
        renderTargets[i] = token.make_render_target(_SSR_STD::move(copyRenderTargetInfo));
    }
    _SSR_ASSERT(!renderTargets.empty(), _SSR_RDERROR);
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Clear_RenderTargets)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(studio, RD_Renderer::eStudio);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderTargets, RD_Renderer::eRenderTargets);

    auto& token = studio.get_token();
    for(auto& rt : renderTargets)
    {
        if (rt)
        {
            token.delete_render_target(rt);
            rt = nullptr;
        }
    }

_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Clear_SwapChain)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(studio, RD_Renderer::eStudio);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(swapchain, RD_Renderer::eSwapchain);

    auto& token = studio.get_token();
    if (swapchain) 
    {
        token.delete_swapchain(swapchain);
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END


_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Clear_Present_Semaphore_And_Fence)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(studio, RD_Renderer::eStudio);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(presentSemaphores, RD_Renderer::ePresentSemaphores);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderSemaphores, RD_Renderer::eRenderSemaphores);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(fences, RD_Renderer::eFence);

    auto& token = studio.get_token();

    for (auto& presentSemaphore : presentSemaphores)
    {
        token.delete_semaphore(presentSemaphore);
    }
    presentSemaphores.clear();

    for (auto renderSemaphore : renderSemaphores)
    {
        token.delete_semaphore(renderSemaphore);
    }
    renderSemaphores.clear();

    for (auto fence : fences) 
    {
        token.delete_fence(fence);
    }
    fences.clear();
_SSR_MOTIONLESS_FLOW_IFUNC_END


_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Init_Render)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Create_SwapChain)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Create_Present_Semaphore_And_Fence)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Create_RenderTargets)
_SSR_MOTIONLESS_FLOW_END


_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Rebulid_Render)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Create_SwapChain)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Clear_RenderTargets)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Create_RenderTargets)
_SSR_MOTIONLESS_FLOW_END


_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Destory_Render)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Clear_SwapChain)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Clear_Present_Semaphore_And_Fence)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Clear_RenderTargets)
_SSR_MOTIONLESS_FLOW_END

_SSR_END

