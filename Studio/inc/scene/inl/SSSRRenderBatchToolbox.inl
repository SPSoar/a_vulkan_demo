
//#include "manager/SSRRenderableManager.h"
#include "manager/SSRRenderManager.h"
#include "manager/SSRPrimitiveManagerIn.h"
#include "studio/SSRMaterialInstanceIn.h"
#include "core/SSRUniformBuffer.h"
#include "core/SSRVertexBuffer.h"
#include "core/SSRIndexBuffer.h"
#include "studio/SSRStudioIn.h"
//
//#include "core/SSRPerViewUniform.h"
//#include "core/SSRPerMaterialUniform.h"
#include "core/SSRPerRenderableSSbo.h"
//#include "core/SSRShadowUniform.h"
//#include "core/SSRLightUniform.h"

#include "SSRTriangulationToolbox.inl"

_SSR_STUDIO_BEGIN

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Make_Renderable)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveManager, SSRRenderBlockUnknown::Renderable::ePrimitiveManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRRenderBlockUnknown::Renderable::eRenderableManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::Renderable::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::Renderable::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveEntities, SSRRenderBlockUnknown::Renderable::ePrimitiveEntities);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableEntities, SSRRenderBlockUnknown::Renderable::eRenderableEntities);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(modelingEngine, SSRRenderBlockUnknown::Renderable::eModelingEngine);

    _SSR_ASSERT(renderableEntities.empty(), _SSR_RDERROR);
    _SSR_ASSERT(!renderEntity, _SSR_RDERROR);

    _SSR_LIKELY_IF (!renderEntity)
    {
        renderEntity = renderManager.make_render_info();
    }

    auto& primitiveInfo = renderManager.get_primitive_info(renderEntity);

    renderableEntities.resize(primitiveEntities.size());
    for (auto& entity : renderableEntities) 
    {
        entity = renderableManager.make_renderable();
    }

    for(SUINT32 i = 0; i< primitiveEntities.size(); i++)
    {
        auto&& prim       = primitiveEntities[i];
        auto&& renderable = renderableEntities[i];

        const auto& geometry = primitiveManager.get_geometry(prim);

        auto& primitiveType = renderableManager.get_primitive_type(renderable);
        auto& indexBuffer = renderableManager.get_index_buffer(renderable);
        auto& vertexBuffer = renderableManager.get_vertex_buffer(renderable);

        SUINT32  patchControlPoints{ 0 };

        //modelingEngine

        using Point2D = _SSR_DATA::SSRDGeometry::Point<_SSR_DATA::E_Dimension::e2D>;
        using Point3D = _SSR_DATA::SSRDGeometry::Point<_SSR_DATA::E_Dimension::e3D>;

        using Line2D  = _SSR_DATA::SSRDGeometry::Line<_SSR_DATA::E_Dimension::e2D>;
        using Line3D  = _SSR_DATA::SSRDGeometry::Line<_SSR_DATA::E_Dimension::e3D>;

        using Box2D   = _SSR_DATA::SSRDGeometry::Box<_SSR_DATA::E_Dimension::e2D>;
        using Box3D   = _SSR_DATA::SSRDGeometry::Box<_SSR_DATA::E_Dimension::e3D>;

        using Circle2D = _SSR_DATA::SSRDGeometry::Circle<_SSR_DATA::E_Dimension::e2D>;
        using Circle3D = _SSR_DATA::SSRDGeometry::Circle<_SSR_DATA::E_Dimension::e3D>;

        _SSR_STD::visit(_SSR::SSROverload{
                    [&](const Point2D& point) {
  
                    },
                    [&](const Point3D& point) {

                    },
                    [&](const Line2D& line) {

                    },
                    [&](const Line3D& line) {

#pragma _SSR_TODO("Soar")
                        auto vertex = new SVEC3::ValueType[2 * SVEC3::SCV_Row];

                        const auto& startPoint = line.get_data<Line3D::Key::eStartPoint>();
                        const auto& endPoint   = line.get_data<Line3D::Key::eEndPoint>();
                       
                        vertex[0 * SVEC3::SCV_Row]     = startPoint[0];
                        vertex[0 * SVEC3::SCV_Row + 1] = startPoint[1];
                        vertex[0 * SVEC3::SCV_Row + 2] = startPoint[2];

                        vertex[1 * SVEC3::SCV_Row]     = endPoint[0];
                        vertex[1 * SVEC3::SCV_Row + 1] = endPoint[1];
                        vertex[1 * SVEC3::SCV_Row + 2] = endPoint[2];

                        vertexBuffer.begin()
                            .init_vertex_count(2) //顶点数量
                            .init_attribute_count(1) //属性数量
                            .init_attribute(E_VertexAttribute::ePosition, 0, SSRVertexBuffer::E_AttributeType::eRGB32Float, 0, SVEC3::bsize())
                            .setup()
                            .setup_attribute_at(0, SSRBufferDescriptor(vertex, 2 * SVEC3::bsize()), 0)
                            .end();

                        primitiveType = _SSR_CORE::RE_PrimitiveType::ePatchList;
                        patchControlPoints = 2;
                    },
                    [&](const Box2D& cube) {
                        //const auto& centre = circle.get_centre();
                        //const auto& normal = circle.get_normal();
                        //const auto& radius = circle.get_radius();

                        //auto count = (lodLevel + 1) * 10;
                        //vertex.reserve(count);
                        //vertex.resize(count);
                        //for (SUINT32 i = 0; i < count - 1; i++) 
                        //{
                        //    //auto dir = _SSR_GEOM::CF_Get_Geometry_Rotation_Matrix(90) * normal;
                        //}
                    },
                    [&](const Box3D& cube) {

                        const auto& centre   = cube.get_data<Box3D::Key::eCentre>();
                        const auto& halfEdge = cube.get_data<Box3D::Key::eHalfEdge>();

                        auto trans = SMAT4X4::Translate(centre);
                        auto scale = SMAT4X4::Scale(halfEdge);
#pragma _SSR_TODO("Soar")
                        auto vertex = new SVEC3::ValueType[CCV_Box_Vertices_SIZE * SVEC3::SCV_Row];
                        for (SUINT32 j = 0; j < CCV_Box_Vertices_SIZE; j++) 
                        {
                            vertex[j * SVEC3::SCV_Row ]   = (trans * scale * SVEC4(SCCV_Box_Vertices[j], 1.f))[0];
                            vertex[j * SVEC3::SCV_Row + 1]= (trans * scale * SVEC4(SCCV_Box_Vertices[j], 1.f))[1];
                            vertex[j * SVEC3::SCV_Row + 2]= (trans * scale * SVEC4(SCCV_Box_Vertices[j], 1.f))[2];
                        }

                        auto uv = new SVEC2::ValueType[CCV_Box_Vertices_SIZE * SVEC2::SCV_Row];
                        for (SUINT32 j = 0; j < CCV_Box_Vertices_SIZE; j++)
                        {
                            uv[j * SVEC2::SCV_Row] = SCCV_Box_Uvs[j][0];
                            uv[j * SVEC2::SCV_Row + 1] = SCCV_Box_Uvs[j][1];
                        }
#pragma _SSR_TODO("Soar")
                        auto indices = new SUINT32[CCV_Box_Indices_SIZE];

                        memcpy(indices, SCCV_Box_Indices, CCV_Box_Indices_BSIZE);

                        vertexBuffer.begin()
                            .init_vertex_count(CCV_Box_Vertices_SIZE) //顶点数量
                            .init_attribute_count(2) //属性数量
                            .init_attribute(E_VertexAttribute::ePosition, 0, SSRVertexBuffer::E_AttributeType::eRGB32Float, 0, SVEC3::bsize())
                            .init_attribute(E_VertexAttribute::eUv0, 1, SSRVertexBuffer::E_AttributeType::eRG32Float, 0, SVEC2::bsize())
                            .setup()
                            .setup_attribute_at(0, SSRBufferDescriptor(vertex, CCV_Box_Vertices_SIZE *  SVEC3::bsize()), 0)
                            .setup_attribute_at(1, SSRBufferDescriptor(uv, CCV_Box_Vertices_SIZE *  SVEC2::bsize()), CCV_Box_Vertices_SIZE * SVEC3::bsize())
                            .end();

                        indexBuffer.begin()
                            .init_index_count(CCV_Box_Indices_SIZE)
                            .init_index_type(SSRIndexBuffer::E_IndexType::eUInt)
                            .setup()
                            .setup_buffer(SSRBufferDescriptor(indices, CCV_Box_Indices_SIZE * sizeof(SUINT32)), 0)
                            .end();

                        primitiveType = _SSR_CORE::RE_PrimitiveType::eTriangleList;
                    },
                    [&](const Circle2D& circle) {
                        //const auto& centre = circle.get_centre();
                        //const auto& normal = circle.get_normal();
                        //const auto& radius = circle.get_radius();

                        //auto count = (lodLevel + 1) * 10;
                        //vertex.reserve(count);
                        //vertex.resize(count);
                        //for (SUINT32 i = 0; i < count - 1; i++) 
                        //{
                        //    //auto dir = _SSR_GEOM::CF_Get_Geometry_Rotation_Matrix(90) * normal;
                        //}
                    },
                    [&](const Circle3D& circle) {
                        //const auto& centre = circle.get_centre();
                        //const auto& normal = circle.get_normal();
                        //const auto& radius = circle.get_radius();

                        //auto count = (lodLevel + 1) * 10;
                        //vertex.reserve(count);
                        //vertex.resize(count);
                        //for (SUINT32 i = 0; i < count - 1; i++) 
                        //{
                        //    //auto dir = _SSR_GEOM::CF_Get_Geometry_Rotation_Matrix(90) * normal;
                        //}
                    },
            }, *geometry
        );

        if (i == 0) 
        {
            primitiveInfo.mPrimitiveType = primitiveType;
            primitiveInfo.mPatchControlPoints = patchControlPoints;
            primitiveInfo.mInputVertexInfo = vertexBuffer.get_input_vertex_info();
        }
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Prepare_Command)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::RenderRes::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::RenderRes::eRenderEntity);

    _SSR_ASSERT(renderEntity, _SSR_RDERROR);

    auto& token = renderManager.get_studio().get_token();

    auto& renderPassInfos = renderManager.get_render_pass_infos(renderEntity);

    auto&& renderPipelines = renderManager.get_render_pipelines();

    for (SUINT32 i = 0; i < renderPipelines.size(); i++)
    {
        for (SUINT32 j = 0; j < renderPipelines[i].mRenderPasses.size(); j++)
        {
            auto& pass = renderPassInfos[j + (renderPipelines.size() - 1) * i];
            auto&& passInfo = renderPipelines[i].mRenderPasses[j].mPassInfo;
            _SSR_ASSERT(renderManager.get_frame_count() <= pass.mSubDrawCommand.size(), _SSR_RDERROR);

            _SSR_ASSERT(passInfo, _SSR_RDERROR);

            if (passInfo->mConfig.p.usage == SE_PassUsage::eColor || passInfo->mConfig.p.usage == SE_PassUsage::eOther)
            {
                for (UINT32 z = 0; z < renderManager.get_frame_count(); z++)
                {
                    auto& cmd = pass.mSubDrawCommand[z];
                    if (!cmd)
                    {
                        _SSR_CORE::RI_SubDrawCommandInfo info{};
                        cmd = token.make_sub_draw_command(_SSR_STD::move(info));
                        _SSR_ASSERT(cmd != nullptr, _SSR_RDERROR);
                    }
                }
            }
        }
    }

    auto& subTransferCommands = renderManager.get_render_transfer_command_info(renderEntity);
    _SSR_ASSERT(renderManager.get_frame_count() <= subTransferCommands.mSubDrawCommands.size(), _SSR_RDERROR);

    for (UINT32 j = 0; j < renderManager.get_frame_count(); j++)
    {
        auto& cmd = subTransferCommands.mSubDrawCommands[j];
        if (!cmd) 
        {
            _SSR_CORE::RI_SubTransferCommandInfo info{};
            cmd = token.make_sub_transfer_command(_SSR_STD::move(info));
            _SSR_ASSERT(cmd != nullptr, _SSR_RDERROR);
        }
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Prepare_Material_Instance)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchId, SSRRenderBlockUnknown::RenderRes::eBatchId);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchInfo, SSRRenderBlockUnknown::RenderRes::eBatchInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::RenderRes::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::RenderRes::eRenderEntity);

    _SSR_ASSERT(batchId, _SSR_RDERROR);
    _SSR_UNUSED_P(batchInfo);

    if constexpr (_SSR_STD::decay_t<decltype(batchInfo)>::eBatchMode == BatchMode::eIndirect)
    {

    }
    else 
    {
        auto&& miIn = upcast<SSRMaterialInstanceInSPtr>(batchId);
        miIn->prepare();
    }

_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Prepare_Descriptor_Info)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::RenderRes::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::RenderRes::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchId, SSRRenderBlockUnknown::RenderRes::eBatchId);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchInfo, SSRRenderBlockUnknown::RenderRes::eBatchInfo);

    _SSR_ASSERT(batchId, _SSR_RDERROR);
    _SSR_UNUSED_P(batchInfo);

    SSRMaterialInstanceInSPtr miIn = nullptr;
    if constexpr (_SSR_STD::decay_t<decltype(batchInfo)>::eBatchMode == BatchMode::eIndirect)
    {

    }
    else
    {
        miIn = upcast<SSRMaterialInstanceInSPtr>(batchId);
    }

    auto& renderPassInfos = renderManager.get_render_pass_infos(renderEntity);
    auto&& renderPipelines = renderManager.get_render_pipelines();

    for (SUINT32 i = 0; i < renderPipelines.size(); i++)
    {
        for (SUINT32 j = 0; j < renderPipelines[i].mRenderPasses.size(); j++)
        {
            auto& pass = renderPassInfos[j + (renderPipelines.size() - 1) * i];
            auto&& passInfo = renderPipelines[i].mRenderPasses[j].mPassInfo;

            _SSR_ASSERT(renderManager.get_frame_count() <= pass.mSubDrawCommand.size(), _SSR_RDERROR);
            _SSR_ASSERT(passInfo, _SSR_RDERROR);

            if (passInfo->mConfig.p.usage == SE_PassUsage::eColor || passInfo->mConfig.p.usage == SE_PassUsage::eOther)
            {
                for (UINT32 z = 0; z < renderManager.get_frame_count(); z++) 
                {
                    auto& bindingLayout = pass.mBindingLayout[z];
                    auto& bindingSet = pass.mBindingSet[z];
                    _SSR_ASSERT(!bindingLayout, _SSR_RDERROR);
                    _SSR_ASSERT(!bindingSet, _SSR_RDERROR);

                    _SSR_LIKELY_IF(!bindingLayout && !bindingSet)
                    {
                        auto& token = renderManager.get_studio().get_token();

                        auto& renderableBufferInfo = renderManager.get_renderable_buffer_info(renderEntity);

                        auto&& renderableSsbo = renderableBufferInfo.mRenderableSsbo;
                        auto&& perMaterialBuffer = miIn->get_uniform_buffer();
                        auto&& textureGroup = miIn->get_material()->textureGroup();
                        auto&& perViewBuffer = renderManager.get_per_view_uniform_buffer();
                        auto&& lightBuffer = renderManager.get_light_info();
                        auto&& shadowBuffer = renderManager.get_shadow_info();

                        _SSR_CORE::RI_BindingSetInfo setInfo{};

                        _SSR_LIKELY_IF(perViewBuffer)
                        {
                            _SSR_STUDIO_UNIFORM_BINDING_FUNC(perViewBuffer, 0)
                        }
                        _SSR_LIKELY_IF(perMaterialBuffer)
                        {
                            _SSR_STUDIO_UNIFORM_BINDING_FUNC(perMaterialBuffer, 1)
                        }
                        _SSR_LIKELY_IF(renderableSsbo)
                        {
                            _SSR_STUDIO_SSBO_BINDING_FUNC(renderableSsbo, 2)
                        }

                        _SSR_LIKELY_IF(textureGroup.get_size())
                        {
                            for (SUINT32 k = 0; k < textureGroup.get_size(); k++)
                            {
                                auto&& texture = textureGroup.texture(k);
                                _SSR_STUDIO_TEXTURE_BINDING_FUNC(texture.image(), 3 + i);
                            }

                        }

                        if (!setInfo.mBindings.empty())
                        {
                            _SSR_CORE::RI_BindingLayoutInfo setLayoutInfo{};
                            setLayoutInfo.mVisibility = _SSR_CORE::RE_ShaderType::eAllGraphics;
                            setLayoutInfo.mRegisterSpace = 0;
                            S_CF_Convert_Set_To_Layout(setInfo.mBindings, setLayoutInfo.mBindings);

                            bindingLayout = token.make_binding_layout(_SSR_STD::move(setLayoutInfo));
                            setInfo.mBindingLayout = bindingLayout;
                            bindingSet = token.make_binding_set(_SSR_STD::move(setInfo));
                            _SSR_ASSERT(bindingLayout, _SSR_RDERROR);
                            _SSR_ASSERT(bindingSet, _SSR_RDERROR);
                        }
                    }
                }
            }
        }
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Prepare_Pipeline)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::RenderRes::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::RenderRes::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRRenderBlockUnknown::RenderRes::eRenderableManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableEntities, SSRRenderBlockUnknown::RenderRes::eRenderableEntities);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchId, SSRRenderBlockUnknown::RenderRes::eBatchId);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchInfo, SSRRenderBlockUnknown::RenderRes::eBatchInfo);

    _SSR_ASSERT(batchId, _SSR_RDERROR);
    _SSR_UNUSED_P(batchInfo);

    SSRMaterialInstanceInSPtr miIn = nullptr;
    if constexpr (_SSR_STD::decay_t<decltype(batchInfo)>::eBatchMode == BatchMode::eIndirect)
    {

    }
    else
    {
        miIn = upcast<SSRMaterialInstanceInSPtr>(batchId);
    }

    auto& primitiveInfo = renderManager.get_primitive_info(renderEntity);
    auto& token = renderManager.get_studio().get_token();
    auto& studioIn = renderManager.get_studio();

    auto& renderPassInfos = renderManager.get_render_pass_infos(renderEntity);
    auto&& renderPipelines = renderManager.get_render_pipelines();
    auto&& material = miIn->get_material();

    for (SUINT32 i = 0; i < renderPipelines.size(); i++)
    {
        for (SUINT32 j = 0; j < renderPipelines[i].mRenderPasses.size(); j++)
        {
            auto& pass = renderPassInfos[j + (renderPipelines.size() - 1) * i];
            auto&& passInfo = renderPipelines[i].mRenderPasses[j].mPassInfo;
            _SSR_ASSERT(renderManager.get_frame_count() <= pass.mSubDrawCommand.size(), _SSR_RDERROR);

            _SSR_ASSERT(passInfo, _SSR_RDERROR);

            if (passInfo->mConfig.p.usage == SE_PassUsage::eColor || passInfo->mConfig.p.usage == SE_PassUsage::eOther)
            {
                for (UINT32 z = 0; z < renderManager.get_frame_count(); z++)
                {
                    auto& pipeline = pass.mPipeline[z];
                    if (pipeline)
                    {
                        token.delete_graphics_pipeline(pipeline);
                        pipeline = nullptr;
                    }

                    _SSR_CORE::RI_GraphicsPipelineInfo info{};
                    info.mRenderState.mDepthStencilState.mDepthEnable = passInfo->mConfig.p.depthEnable;
                    info.mRenderState.mDepthStencilState.mDepthWriteEnable = passInfo->mConfig.p.depthWrite;
                    info.mPrimType = primitiveInfo.mPrimitiveType;
                    info.mVertexShader = material->get_shader(_SSR_CORE::RE_ShaderType::eVertex, SE_MaterialVariantFlags::eColor);
                    info.mTessellationControlShader = material->get_shader(_SSR_CORE::RE_ShaderType::eTessellationControl, SE_MaterialVariantFlags::eColor);
                    info.mTessellationEvaluationShader = material->get_shader(_SSR_CORE::RE_ShaderType::eTessellationEvaluation, SE_MaterialVariantFlags::eColor);
                    info.mGeometryShader = material->get_shader(_SSR_CORE::RE_ShaderType::eGeometry, SE_MaterialVariantFlags::eColor);
                    info.mFragmentShader = material->get_shader(_SSR_CORE::RE_ShaderType::eFragment, passInfo->mMaterialVariant.get_value());
                    info.mRenderGraph = renderManager.get_render_graph(pass.mPiplineIndex);
                    info.mInputVertexInfo = &primitiveInfo.mInputVertexInfo;
                    info.mPatchControlPoints = primitiveInfo.mPatchControlPoints;
                    info.mSubpassIndex = j;
                    if (pass.mBindingLayout[z])
                    {
                        info.mBindingLayouts.push_back(pass.mBindingLayout[z]);
                    }
                    pipeline = token.make_graphics_pipeline(_SSR_STD::move(info));
                    _SSR_ASSERT(pipeline, _SSR_RDERROR);
                }
            }
        }
    }

_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Prepare_Buffer)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::RenderRes::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::RenderRes::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveEntities, SSRRenderBlockUnknown::RenderRes::ePrimitiveEntities);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableEntities, SSRRenderBlockUnknown::RenderRes::eRenderableEntities);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRRenderBlockUnknown::RenderRes::eRenderableManager);

    auto& token = renderManager.get_studio().get_token();
    auto& studioIn = renderManager.get_studio();

    auto& drawInfos = renderManager.get_draw_infos(renderEntity);

    auto* drawInfo = new SSRRenderManager::DrawInfo[renderableEntities.size()];
    drawInfos = { drawInfo, renderableEntities.size() };

    SCINT vertexSize{ 0 };
    SCINT indexSize{ 0 };

    for (UINT32 i = 0; i < renderableEntities.size(); i++)
    {
        const auto e = renderableEntities[i];

        auto& vertex = renderableManager.get_vertex_buffer(e);
        auto& index = renderableManager.get_index_buffer(e);

        vertexSize += vertex.get_vertex_size();
        indexSize += index.get_index_size();

        if (index.get_index_count())
        {
            drawInfos[i].mDrawArguments.mVertexCount = index.get_index_count();
        }
        else
        {
            drawInfos[i].mDrawArguments.mVertexCount = vertex.get_vertex_count();
        }
    }

    auto& renderableBufferInfo = renderManager.get_renderable_buffer_info(renderEntity);
    
    _SSR_ASSERT(vertexSize, _SSR_RDERROR);
    _SSR_CORE::RI_BufferInfo vertexBufferInfo{};
    vertexBufferInfo.mBufferUsage = _SSR_CORE::RE_BufferUsage::eVertexBuffer;
    vertexBufferInfo.mMemoryUsage = _SSR_CORE::RE_MemoryUsage::eGpuOnly;
    vertexBufferInfo.mDataSize = vertexSize;
    renderableBufferInfo.mVertexBuffer = token.make_buffer(_SSR_STD::move(vertexBufferInfo));
    _SSR_ASSERT(renderableBufferInfo.mVertexBuffer, _SSR_RDERROR);
    if (indexSize) 
    {
        _SSR_CORE::RI_BufferInfo indexBufferInfo{};
        indexBufferInfo.mBufferUsage = _SSR_CORE::RE_BufferUsage::eIndexBuffer;
        indexBufferInfo.mMemoryUsage = _SSR_CORE::RE_MemoryUsage::eGpuOnly;
        indexBufferInfo.mDataSize = indexSize;
        renderableBufferInfo.mIndexBuffer = token.make_buffer(_SSR_STD::move(indexBufferInfo));

        _SSR_ASSERT(renderableBufferInfo.mIndexBuffer, _SSR_RDERROR);
    }


    SCINT renderableSSboSize = sizeof(SSRPerRenderableBib) * primitiveEntities.size();

    _SSR_CORE::RI_BufferInfo ssboBufferInfo{};
    ssboBufferInfo.mBufferUsage = _SSR_CORE::RE_BufferUsage::eStorageBuffer;
    ssboBufferInfo.mMemoryUsage = _SSR_CORE::RE_MemoryUsage::eGpuOnly;
    ssboBufferInfo.mDataSize = renderableSSboSize;
    renderableBufferInfo.mRenderableSsbo = token.make_buffer(_SSR_STD::move(ssboBufferInfo));

    _SSR_ASSERT(renderableBufferInfo.mRenderableSsbo, _SSR_RDERROR);

    renderableBufferInfo.mVertexTrackerId = renderManager.get_buffer_tracker().require_buffer(vertexSize);
    if (indexSize) 
    {
        renderableBufferInfo.mIndexTrackerId = renderManager.get_buffer_tracker().require_buffer(indexSize);
    }
    renderableBufferInfo.mSsboTrackerId = renderManager.get_buffer_tracker().require_buffer(renderableSSboSize);

_SSR_MOTIONLESS_FLOW_IFUNC_END


_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Recode_Transfer_Command)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchInfo, SSRRenderBlockUnknown::RenderCommand::eBatchInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(batchId, SSRRenderBlockUnknown::RenderCommand::eBatchId);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::RenderCommand::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::RenderCommand::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRRenderBlockUnknown::RenderCommand::eRenderableManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableEntities, SSRRenderBlockUnknown::RenderCommand::eRenderableEntities);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveManager, SSRRenderBlockUnknown::RenderCommand::ePrimitiveManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveEntities, SSRRenderBlockUnknown::RenderCommand::ePrimitiveEntities);
    
    _SSR_ASSERT(batchId, _SSR_RDERROR);
    _SSR_UNUSED_P(batchInfo);

    SSRMaterialInstanceInSPtr miIn = nullptr;
    if constexpr (_SSR_STD::decay_t<decltype(batchInfo)>::eBatchMode == BatchMode::eIndirect)
    {

    }
    else
    {
        miIn = upcast<SSRMaterialInstanceInSPtr>(batchId);
    }

    auto& subTransferCommands = renderManager.get_render_transfer_command_info(renderEntity);
    auto& transferCommand = subTransferCommands.mSubDrawCommands[renderManager.get_current_frame_index()];

    auto& renderableBufferInfo = renderManager.get_renderable_buffer_info(renderEntity);

    _SSR_ASSERT(transferCommand, _SSR_RDERROR);
    _SSR_LIKELY_IF(transferCommand)
    {
        transferCommand->open();

        miIn->build(transferCommand);
        {
            auto& vertexCopyInfo = renderManager.get_buffer_tracker().obtain_buffer(renderableBufferInfo.mVertexTrackerId);

            vertexCopyInfo.mBufferCopyInfo.mBufferOffset = 0;
            vertexCopyInfo.mBufferCopyInfo.mBuffer = renderableBufferInfo.mVertexBuffer;

            auto&& data = vertexCopyInfo.mTarget;

            SCINT offset = vertexCopyInfo.mBufferCopyInfo.mStagingOffset;
            for (UINT32 i = 0; i < renderableEntities.size(); i++)
            {
                const auto e = renderableEntities[i];
                auto& vertex = renderableManager.get_vertex_buffer(e);
                vertex.copy_data(data, offset);
            }

            transferCommand->copy_buffer(vertexCopyInfo.mBufferCopyInfo);
        }
        {
            if (renderableBufferInfo.mIndexBuffer) 
            {
                auto& indexCopyInfo = renderManager.get_buffer_tracker().obtain_buffer(renderableBufferInfo.mIndexTrackerId);

                indexCopyInfo.mBufferCopyInfo.mBufferOffset = 0;
                indexCopyInfo.mBufferCopyInfo.mBuffer = renderableBufferInfo.mIndexBuffer;

                auto&& data = indexCopyInfo.mTarget;

                SCINT offset = indexCopyInfo.mBufferCopyInfo.mStagingOffset;
                for (UINT32 i = 0; i < renderableEntities.size(); i++)
                {
                    const auto e = renderableEntities[i];
                    auto& index = renderableManager.get_index_buffer(e);
                    index.copy_data(data, offset);
                }

                transferCommand->copy_buffer(indexCopyInfo.mBufferCopyInfo);
            }
            
        }
        {
            _SSR_LIKELY_IF (renderableBufferInfo.mRenderableSsbo)
            {
                auto& ssboCopyInfo = renderManager.get_buffer_tracker().obtain_buffer(renderableBufferInfo.mSsboTrackerId);

                ssboCopyInfo.mBufferCopyInfo.mBufferOffset = 0;
                ssboCopyInfo.mBufferCopyInfo.mBuffer = renderableBufferInfo.mRenderableSsbo;

                auto&& data = ssboCopyInfo.mTarget;

                SCINT offset = ssboCopyInfo.mBufferCopyInfo.mStagingOffset;
                for (UINT32 i = 0; i < primitiveEntities.size(); i++)
                {
                    void* target = data + offset;

                    SSRPerRenderableBib bib{};
                    bib.mDepth = 0.6f;

                    memcpy(target, &bib, sizeof(SSRPerRenderableBib));

                    offset += sizeof(SSRPerRenderableBib);
                }

                transferCommand->copy_buffer(ssboCopyInfo.mBufferCopyInfo);
            }

        }
        transferCommand->close();
        subTransferCommands.mNeedTransfer = true;
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Render_Block_Bind_Draw_Command)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderBlockUnknown::RenderCommand::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderBlockUnknown::RenderCommand::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableEntities, SSRRenderBlockUnknown::RenderCommand::eRenderableEntities);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRRenderBlockUnknown::RenderCommand::eRenderableManager);
    
    auto& renderPassInfos = renderManager.get_render_pass_infos(renderEntity);
    auto&& renderPipelines = renderManager.get_render_pipelines();

    auto& drawInfos = renderManager.get_draw_infos(renderEntity);

    auto& renderableBufferInfo = renderManager.get_renderable_buffer_info(renderEntity);
    auto& vertexBuffer = renderableBufferInfo.mVertexBuffer;
    auto& indexBuffer = renderableBufferInfo.mIndexBuffer;

    _SSR_ASSERT(vertexBuffer != nullptr, _SSR_RDERROR);

    for (SUINT32 i = 0; i < renderPipelines.size(); i++)
    {
        for (SUINT32 j = 0; j < renderPipelines[i].mRenderPasses.size(); j++)
        {
            auto& pass = renderPassInfos[j + (renderPipelines.size() - 1) * i];
            auto&& passInfo = renderPipelines[i].mRenderPasses[j].mPassInfo;
            _SSR_ASSERT(renderManager.get_frame_count() <= pass.mSubDrawCommand.size(), _SSR_RDERROR);

            _SSR_ASSERT(passInfo, _SSR_RDERROR);

            if (passInfo->mConfig.p.usage == SE_PassUsage::eColor || passInfo->mConfig.p.usage == SE_PassUsage::eOther)
            {
                auto& drawCommands = pass.mSubDrawCommand;
                auto& pipelines     = pass.mPipeline;
                auto& bindingSet   = pass.mBindingSet;
                for (UINT32 z = 0; z < renderManager.get_frame_count(); z++) 
                {
                    auto& drawCommand = drawCommands[z];
                    _SSR_ASSERT(drawCommand != nullptr, _SSR_RDERROR);
                    _SSR_LIKELY_IF(drawCommand)
                    {
                        drawCommand->open(renderManager.get_render_graph(pass.mPiplineIndex), z, pass.mPassIndex);
                        drawCommand->set_graphics_state(pipelines[z], bindingSet[z]);

                        for (UINT32 s = 0; s < renderableEntities.size(); s++)
                        {
                            const auto e = renderableEntities[s];
                            auto& vertex = renderableManager.get_vertex_buffer(e);

                            auto&& bindBufferInfo = vertex.get_bind_buffer_info();
                            for (auto& bd : bindBufferInfo.mInfo) 
                            {
                                bd.mBuffer = vertexBuffer;
                            }
                            drawCommand->bind_vertex_buffer(bindBufferInfo);
                        }

                        if (indexBuffer)
                        {
                            for (UINT32 s = 0; s < renderableEntities.size(); s++)
                            {
                                const auto e = renderableEntities[s];
                                auto& index = renderableManager.get_index_buffer(e);

                                auto&& bindBufferInfo = index.get_bind_buffer_info();
                                bindBufferInfo.mInfo[0].mBuffer = indexBuffer;
                                drawCommand->bind_index_buffer(bindBufferInfo);
                            }
       
                        }
                        for (SUINT32 s = 0; s < drawInfos.size(); s++) 
                        {
                            auto&& drawArguments = drawInfos[s].mDrawArguments;
                            if (indexBuffer)
                            {
                                drawCommand->draw_indexed(drawArguments);
                            }
                            else 
                            {
                                drawCommand->draw(drawArguments);
                            }
                        }
                        drawCommand->close();
                    }
                }
            }
        }
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Render_Block_Build_Renderable)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Make_Renderable)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Render_Block_Prepare_Render_Res)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Prepare_Command)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Prepare_Material_Instance)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Prepare_Buffer)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Prepare_Descriptor_Info)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Render_Block_Prepare_Render_Pipeline)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Prepare_Pipeline)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Render_Block_Build_Render_Command)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Bind_Draw_Command)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Render_Block_Build_Transfer_Command)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Render_Block_Recode_Transfer_Command)
_SSR_MOTIONLESS_FLOW_END

_SSR_END