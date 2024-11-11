#include "studio/SSRViewIn.h"
#include "studio/SSRCameraIn.h"

_SSR_STUDIO_BEGIN

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Update_PerView_Uniform)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(camera, SSRViewIn::Prepare::eCamera);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(perViewUniform, SSRViewIn::Prepare::ePerViewUniform);

    auto&& view = camera->get_view_matrix();
    auto&& project = camera->get_projection_matrix();

    auto&& uniformBuffer = perViewUniform.get_uniform_cpu_buffer();
    auto&& uniformInterfaceBlock = perViewUniform.get_uniform_interface_block();

    using Uid = SSRPerViewUniform::Uid;

    _SSR_STUDIO_UBO_SET(uniformInterfaceBlock, uniformBuffer, Uid::eViewMatrix, view);
    _SSR_STUDIO_UBO_SET(uniformInterfaceBlock, uniformBuffer, Uid::eProjectionMatrix, project);
    _SSR_STUDIO_UBO_SET(uniformInterfaceBlock, uniformBuffer, Uid::eProjectionXViewMatrix, project* view);
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Update_View_Info)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Update_PerView_Uniform)
_SSR_MOTIONLESS_FLOW_END

_SSR_END
