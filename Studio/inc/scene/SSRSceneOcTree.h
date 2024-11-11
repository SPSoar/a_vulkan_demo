#pragma once
#ifndef SSR_SSRSCENEOCTREE_H_
#define SSR_SSRSCENEOCTREE_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

constexpr double  CV_Scene_Tree_Box_Min_Length  = 1000;
constexpr SUINT32 CV_Scene_Tree_Box_Group_Level = 2;

_SSR_PR_DEF(SSRPrimitiveManagerIn);
_SSR_PR_DEF(SSRRenderManager);

class SSRSceneOcTreeNode;
class SSRSceneOcTree;
class SSRSceneOcTreeItem : public _SSR::SSROctreeItem<SSRSceneOcTreeItem, SSRSceneOcTreeNode>
{
    using Entity = SSRPrimitiveManager::Entity;
    friend class SSRSceneOcTree;
    friend class SSRSceneOcTreeNode;
public:
    _SSR_CLASS_DELETE_COPY(SSRSceneOcTreeItem);
    _SSR_CLASS_DELETE_MOVE(SSRSceneOcTreeItem);
    SSRSceneOcTreeItem() = delete;
protected:
    explicit SSRSceneOcTreeItem(Entity entity) noexcept;
    ~SSRSceneOcTreeItem() = default;
public:
    auto node() const noexcept -> RealNode*
    {
        return mOctreeNode;
    }
    auto is_added() const noexcept -> bool
    {
        return node() != nullptr;
    }

    auto is_in_tree() const noexcept -> bool
    {
        return mSceneOcTree != nullptr;
    }

    auto tree() const noexcept
    {
        return mSceneOcTree;
    }
    auto entity() const noexcept
    {
        return mEntity;
    }
    auto boundingbox() const noexcept -> const SSRBoundingBox&;
protected:
    auto set_in_tree(SSRSceneOcTree* tree) noexcept
    {
        mSceneOcTree = tree;
    }
    auto invalid_entity() noexcept
    {
        mEntity.reset();
    }
    auto set_in_node(SSRSceneOcTreeNode* node) noexcept
    {
        mOctreeNode = node;
    }
protected:
    SSRSceneOcTreeNode* mOctreeNode{ nullptr };
    Entity              mEntity{};
    SSRSceneOcTree*     mSceneOcTree{ nullptr };
};

class SSRSceneGraphNode;
class SSRSceneOcTreeNode : public _SSR::SSROcTreeNode<SSRSceneOcTreeNode, SSRSceneOcTreeItem>
{
    using Entity = SSRPrimitiveManager::Entity;
public:
    enum BoxDir : SENUM8 
    {
        eX = 0x1,
        eY = 0x2,
        eZ = 0x4
    };
public:
    _SSR_CLASS_DELETE_COPY(SSRSceneOcTreeNode);
    _SSR_CLASS_DELETE_MOVE(SSRSceneOcTreeNode);
    SSRSceneOcTreeNode() noexcept = default;
    ~SSRSceneOcTreeNode() noexcept;
public:
    auto add(Item* item) noexcept -> bool;
    auto remove(Item* item) noexcept -> bool;
    auto setModifiedEntity(const Item* item) noexcept -> void;

    void clear_scene_node(SSRRenderManagerRef renderManager) noexcept;

    auto is_contains(const SSRBoundingBox& box) const noexcept -> bool;
    auto set_cube_boundingbox(const SSRBoundingBox& box) noexcept -> void;
    auto cube_boundingbox() const noexcept -> const SSRBoundingBox&;

    auto scene_node() noexcept -> SSRSceneGraphNode*
    {
        return mSceneNode;
    }
    auto scene_node() const noexcept -> const SSRSceneGraphNode*
    {
        return mSceneNode;
    }
    auto level() const noexcept -> SUINT32 
    {
        return mLevel;
    }
    auto max_depth() const noexcept -> bool;
    auto is_leaf() const noexcept -> bool;
    auto items_count() const noexcept -> SUINT32;
    auto grow_to_include(const SSRBoundingBox& box) noexcept -> Node*;
    auto split() noexcept -> bool;
    auto get_index(const SSRBoundingBox& box) const noexcept -> SINT32;
    auto child(SINT32 index) const noexcept -> Node*;
    auto parent() const noexcept -> Node*;
public:
    auto rebuild_items_boundingbox() noexcept -> void;
    auto items_boundingbox() const noexcept -> SSRBoundingBox;
public:
    template<typename KeyCompare>
    auto select(const SelectFunc& func, const KeyCompare& comp, SelectMethod method) const noexcept
    {
        SSRSelectResult<RealNode, KeyCompare>  result{ comp };
       
        auto selectFunc = [&func, &result](auto&& self, const RealNode* currentNode) -> void
            {
                if (!func(currentNode->cube_boundingbox())) 
                {
                    return;
                }
                if (currentNode->items_count()) 
                {
                    if (func(currentNode->items_boundingbox())) 
                    {
                        result.add(const_cast<RealNode*>(currentNode));
                    }
                }
                if (currentNode->is_leaf()) 
                {
                    return;
                }

                for (SINT32 i = 0; i < 8; i++) 
                {
                    Node* child = currentNode->child(i);
                    self(self, upcast<RealNode*>(child));
                }
            };
        selectFunc(selectFunc, this);
        return result;
    }
    template<typename KeyCompare>
    auto select(SSRSelectResult<RealNode, KeyCompare >& corssNodes, SSRSelectResult<RealNode, KeyCompare >& containNodes, const BoxSelectFunc& func) const noexcept
    {
        auto selectFunc = [&func, &containNodes, &corssNodes](auto&& self, const RealNode* currentNode)
            {
                auto res = func(currentNode->cube_boundingbox());
                if (res == E_IntersectMath::eSeparate) 
                {
                    return;
                }
                else if (res == E_IntersectMath::eFormerContainLatter)
                {
                    auto collectNodes = [&currentNode, &containNodes, &corssNodes, &func](auto&& self, const RealNode* curNode) -> void
                        {
                            if (currentNode->items_count())
                            {
                                auto selRes = func(currentNode->items_boundingbox());
                                if(selRes != E_IntersectMath::eSeparate)
                                {
                                    containNodes.add(const_cast<RealNode*>(curNode));
                                }
                            }
                            if (currentNode->is_leaf())
                            {
                                return;
                            }

                            for (SINT32 i = 0; i < 8; i++)
                            {
                                Node* child = currentNode->child(i);
                                self(self, upcast<RealNode*>(child));
                            }
                        };
                    collectNodes(collectNodes, currentNode);
                    return;
                }

                if (currentNode->items_count())
                {
                    res = func(currentNode->items_boundingbox());
                    if (res != E_IntersectMath::eSeparate) 
                    {
                        corssNodes.add(const_cast<RealNode*>(currentNode));
                    }
                }
                if (currentNode->is_leaf())
                {
                    return;
                }

                for (SINT32 i = 0; i < 8; i++)
                {
                    Node* child = currentNode->child(i);
                    self(self, upcast<RealNode*>(child));
                }
            };
        selectFunc(selectFunc, this);
    }
public:
    auto item(SUINT32 index) const noexcept -> SSRSceneOcTreeItem*;
protected:
    auto split_box(const SSRBoundingBox& box, SUINT32 index) noexcept -> SSRBoundingBox;
private:
    SSRBoundingBox                       mCubeBox{};
    SSRSceneGraphNode*                   mSceneNode{nullptr};
    SSRFixVector<SSRSceneOcTreeNode*, 8> mChildren{ nullptr };
    _SSR_STD::list<SSRSceneOcTreeItem*>  mItems{};
    SSRSceneOcTreeNode*                  mParent{ nullptr };
    SUINT32                              mLevel{ 2 };
};

_SSR_PR_DEF(SSRCameraIn);

class SSRSceneOcTree 
{
    using Entity = SSRPrimitiveManager::Entity;
public:
    using Tree = SSROctree<SSRSceneOcTreeNode, SSRSceneOcTreeItem>;
    using KeyCompare = _SSR_STD::less<const SSRSceneOcTreeNode::Node*>;
    using SelectResult = SSRSelectResult<Tree::RealNode, KeyCompare>;
public:
    SSRSceneOcTree() = delete;
    explicit SSRSceneOcTree(SSRPrimitiveManagerInRef primitiveManager) noexcept;
    ~SSRSceneOcTree() noexcept;
    _SSR_CLASS_DELETE_COPY(SSRSceneOcTree);
    _SSR_CLASS_DELETE_MOVE(SSRSceneOcTree);
public:
    void init() noexcept;

    void clear_scene_node(SSRRenderManagerRef renderManager) noexcept;

    auto add_enity(Entity entity) noexcept -> bool;
    auto remove_enity(Entity entity) noexcept -> bool;
    auto modify_enity(Entity entity, bool isPositionChanged) noexcept -> bool;

    auto update_octree() noexcept -> void;

    auto boundingbox(Entity entity) noexcept -> const SSRBoundingBox&;

    auto select_by_functor(const Tree::SelectFunc& func) noexcept -> SelectResult;
    auto select_by_box_functor(SelectResult& crossNodes, SelectResult& containNodes, const Tree::BoxSelectFunc& func) noexcept -> void;

    auto select_octree_node(SSRCameraInPtr camera) noexcept -> SelectResult;

    auto& get_primitive_manager() const noexcept
    {
        return mPrimitiveManager;
    }
protected:
    auto add(SSRSceneOcTreeItem* item) noexcept -> bool;
    auto remove(SSRSceneOcTreeItem* item) noexcept -> bool;

    auto make_item(Entity entity) noexcept -> SSRSceneOcTreeItem*;
    auto delete_item(const SSRSceneOcTreeItem* item) noexcept -> void;

private:
    auto get_entity_tree_item(Entity entity) noexcept -> SSRSceneOcTreeItem*;
    auto clear_entity_tree_item(Entity entity) noexcept -> void;
private:
    SSRPrimitiveManagerInRef mPrimitiveManager;

    SSRSet<SSRSceneOcTreeItem*> mAddList{};
    SSRSet<SSRSceneOcTreeItem*> mRemoveList{};
    SSRSet<SSRSceneOcTreeItem*> mModifyList{};

    SSRSceneOcTreeNode::Node * mRoot{ nullptr };
    Tree*                      mTree{ nullptr };
};

_SSR_END

#endif

