#include "scene/SSRSceneOcTree.h"
#include "studio/SSRCameraIn.h"
#include "manager/SSRPrimitiveManagerIn.h"
#include "manager/SSRRenderManager.h"
#include "scene/SSRSceneGraphNode.h"

_SSR_STUDIO_BEGIN

namespace 
{
    auto dir(const SSRBoundingBox& nodeBox, const SSRBoundingBox& itemBox) noexcept -> SUINT32
    {
        SUINT32 dir{ 0 };
        SSRBoundingBox::ValueType itemMaxX = itemBox.x_max();
        SSRBoundingBox::ValueType itemMaxY = itemBox.y_max();
        SSRBoundingBox::ValueType itemMaxZ = itemBox.z_max();

        SSRBoundingBox::ValueType nodeMaxX = nodeBox.x_max();
        SSRBoundingBox::ValueType nodeMaxY = nodeBox.y_max();
        SSRBoundingBox::ValueType nodeMaxZ = nodeBox.z_max();

        if (itemMaxX > nodeMaxX) 
        {
            dir |= SSRSceneOcTreeNode::BoxDir::eX;
        }
        if (itemMaxY > nodeMaxY)
        {
            dir |= SSRSceneOcTreeNode::BoxDir::eY;
        }
        if (itemMaxZ > nodeMaxZ)
        {
            dir |= SSRSceneOcTreeNode::BoxDir::eZ;
        }
        return dir;
    }

    auto get_frustum_planes(SSRCameraInPtr camera) noexcept
    {
        auto&& projection = camera->get_projection_matrix();
        auto&& view  = camera-> get_view_matrix();
        auto matrix = view * projection;
        SSRVector<SVEC4> planes{};
        planes.reserve(6);
        //ÏÂ
        planes.emplace_back(matrix(0, 3) + matrix(0, 1),
                            matrix(1, 3) + matrix(1, 1),
                            matrix(2, 3) + matrix(2, 1),
                            matrix(3, 3) + matrix(3, 1));
        //ÉÏ
        planes.emplace_back(matrix(0, 3) - matrix(0, 1),
                            matrix(1, 3) - matrix(1, 1),
                            matrix(2, 3) - matrix(2, 1),
                            matrix(3, 3) - matrix(3, 1));
        //×ó
        planes.emplace_back(matrix(0, 3) + matrix(0, 0),
                            matrix(1, 3) + matrix(1, 0),
                            matrix(2, 3) + matrix(2, 0),
                            matrix(3, 3) + matrix(3, 0));
        //ÓÒ
        planes.emplace_back(matrix(0, 3) - matrix(0, 0),
                            matrix(1, 3) - matrix(1, 0),
                            matrix(2, 3) - matrix(2, 0),
                            matrix(3, 3) - matrix(3, 0));
        //½ü
        planes.emplace_back(matrix(0, 3) + matrix(0, 2),
                            matrix(1, 3) + matrix(1, 2),
                            matrix(2, 3) + matrix(2, 2),
                            matrix(3, 3) + matrix(3, 2));
        //Ô¶
        planes.emplace_back(matrix(0, 3) - matrix(0, 2),
                            matrix(1, 3) - matrix(1, 2),
                            matrix(2, 3) - matrix(2, 2),
                            matrix(3, 3) - matrix(3, 2));
        return planes;
    }

    auto get_vertex_of_box(const SSRBoundingBox& box) noexcept -> SSRVector<SVEC3> 
    {
        SSRVector<SVEC3> vertices{};
        vertices.reserve(8);
        vertices.emplace_back(box.x_min(), box.y_min(), box.z_min());
        vertices.emplace_back(box.x_min(), box.y_min(), box.z_max());
        vertices.emplace_back(box.x_min(), box.y_max(), box.z_min());
        vertices.emplace_back(box.x_min(), box.y_max(), box.z_max());
        vertices.emplace_back(box.x_max(), box.y_min(), box.z_min());
        vertices.emplace_back(box.x_max(), box.y_min(), box.z_max());
        vertices.emplace_back(box.x_max(), box.y_max(), box.z_min());
        vertices.emplace_back(box.x_max(), box.y_max(), box.z_max());
        return vertices;
    }
}

SSRSceneOcTreeItem::SSRSceneOcTreeItem(Entity entity) noexcept
    : mEntity{ entity }
{
}

auto SSRSceneOcTreeItem::boundingbox() const noexcept -> const SSRBoundingBox&
{
    _SSR_ASSERT(mSceneOcTree != nullptr, _SSR_RDERROR);
    _SSR_ASSERT(mEntity, _SSR_RDERROR);

    return mSceneOcTree->boundingbox(mEntity);
}


SSRSceneOcTreeNode::~SSRSceneOcTreeNode() noexcept
{
    for (auto&& child : mChildren) 
    {
        _SSR_DELETE_OBJECT_PTR(child);
    }
    if (mSceneNode)
    {
        _SSR_DELETE_OBJECT_PTR(mSceneNode);
    }
}

auto SSRSceneOcTreeNode::add(Item* item) noexcept -> bool
{
    _SSR_ASSERT(item != nullptr, _SSR_RDERROR);
    if (item->node() != nullptr) 
    {
        return false;
    }

    if (mLevel % CV_Scene_Tree_Box_Group_Level == 0) 
    {
        if (mSceneNode == nullptr)
        {
            mSceneNode = new SSRSceneGraphNode(item->cast()->tree()->get_primitive_manager());
        }
        mSceneNode->add_entity(item->cast()->entity());
        mItems.push_back(item->cast());
        item->cast()->set_in_node(this);
    }
    else 
    {
        auto currentNode = parent();
        while (true) 
        {
            if (currentNode == nullptr) 
            {
                break;
            }
            if (currentNode->cast()->level() % CV_Scene_Tree_Box_Group_Level == 0)
            {
                currentNode->add(item);
                break;
            }
            else 
            {
                currentNode = currentNode->parent();
            }
        }
    }

    return true;
}

auto SSRSceneOcTreeNode::remove(Item* item) noexcept -> bool
{
    _SSR_ASSERT(item != nullptr, _SSR_RDERROR);
    if (item->node() == nullptr)
    {
        return false;
    }

    if (mLevel % CV_Scene_Tree_Box_Group_Level == 0)
    {
        if (mSceneNode)
        {
            mSceneNode->delete_entity(item->cast()->entity());
        }
        mItems.remove(item->cast());
        item->cast()->set_in_node(nullptr);
    }
    else
    {
        auto currentNode = parent();
        while (true)
        {
            if (currentNode == nullptr)
            {
                break;
            }
            if (currentNode->cast()->level() % CV_Scene_Tree_Box_Group_Level == 0)
            {
                currentNode->remove(item);
                break;
            }
            else
            {
                currentNode = currentNode->parent();
            }
        }
    }
    return true;
}

auto SSRSceneOcTreeNode::setModifiedEntity(const Item* item) noexcept -> void
{
    if (mSceneNode)
    {
        mSceneNode->modify_entity(item->cast()->entity());
    }
}

void SSRSceneOcTreeNode::clear_scene_node(SSRRenderManagerRef renderManager) noexcept
{
    if (mSceneNode)
    {
        mSceneNode->clear(renderManager);
    }

    for (auto&& child : mChildren)
    {
        if (child) 
        {
            child->cast()->clear_scene_node(renderManager);
        }
    }

}

auto SSRSceneOcTreeNode::is_contains(const SSRBoundingBox& box) const noexcept -> bool
{
    return mCubeBox.contains(box);
}

auto SSRSceneOcTreeNode::set_cube_boundingbox(const SSRBoundingBox& box) noexcept -> void
{
    //_SSR_ASSERT(box.x_len() == box.y_len() && box.x_len() == box.z_len(), _SSR_RDERROR);
    mCubeBox = box;
}

auto SSRSceneOcTreeNode::cube_boundingbox() const noexcept -> const SSRBoundingBox&
{
    return mCubeBox;
}

auto SSRSceneOcTreeNode::max_depth() const noexcept -> bool
{
    if (mCubeBox.x_len() <= CV_Scene_Tree_Box_Min_Length)
    {
        return true;
    }
    return false;
}

auto SSRSceneOcTreeNode::is_leaf() const noexcept -> bool
{
    return mChildren[0] == nullptr;
}

auto SSRSceneOcTreeNode::items_count() const noexcept -> SUINT32
{
    return _SSR::cast<SUINT32>(mItems.size());
}

auto SSRSceneOcTreeNode::grow_to_include(const SSRBoundingBox& box) noexcept -> Node*
{
    if (!(parent() == nullptr && mCubeBox.valid())) 
    {
        return nullptr;
    }

    SUINT8 indexGrow{ 0x7 };
    auto grow = [this, &box, &indexGrow]() -> decltype(auto)
        {
            auto d = dir(mCubeBox, box);

            bool xDir{ false };
            bool yDir{ false };
            bool zDir{ false };
            if (d & eX) 
            {
                xDir = true;
                indexGrow &= ~eX;
            }
            if (d & eY)
            {
                yDir = true;
                indexGrow &= ~eY;
            }
            if (d & eZ)
            {
                zDir = true;
                indexGrow &= ~eZ;
            }
            SSRBoundingBox::ValueType newX = xDir ? mCubeBox.x_min() : mCubeBox.x_min() - mCubeBox.x_len();
            SSRBoundingBox::ValueType newY = yDir ? mCubeBox.y_min() : mCubeBox.y_min() - mCubeBox.x_len();
            SSRBoundingBox::ValueType newZ = zDir ? mCubeBox.z_min() : mCubeBox.z_min() - mCubeBox.x_len();

            SSRBoundingBox::ValueType newXMax = newX + mCubeBox.x_len() * 2.0f;
            SSRBoundingBox::ValueType newYMax = newY + mCubeBox.x_len() * 2.0f;
            SSRBoundingBox::ValueType newZMax = newZ + mCubeBox.x_len() * 2.0f;
            return SSRBoundingBox(newX, newY, newZ, newXMax, newYMax, newZMax);
        };
    Node* newRoot = new RealNode();
    newRoot->set_cube_boundingbox(grow());
    newRoot->cast()->mLevel = this->mLevel + 1;
    if (!newRoot->split()) 
    {
        _SSR_DELETE_OBJECT_PTR(newRoot);
        return nullptr;
    }

    auto node = newRoot->child(indexGrow);
    upcast<SSRSceneOcTreeNode::RealNode*>(newRoot)->mChildren[indexGrow] = this;
    _SSR_DELETE_OBJECT_PTR(node);
    mParent = upcast<SSRSceneOcTreeNode::RealNode*>(newRoot);

    if (newRoot->cast()->mLevel % CV_Scene_Tree_Box_Group_Level != 0) 
    {
        return newRoot->grow_to_include(box);
    }

    return newRoot;
}

auto SSRSceneOcTreeNode::split() noexcept -> bool
{
    if (!mCubeBox.valid()) 
    {
        return false;
    }

    if (!is_leaf()) 
    {
        return true;
    }

    if (max_depth()) 
    {
        return false;
    }

    if (is_leaf()) 
    {
        for (SUINT32 i = 0; i < 8; i++)
        {
            _SSR_ASSERT(mChildren[i] == nullptr, _SSR_RDERROR);
            mChildren[i] = new RealNode;
            mChildren[i]->set_cube_boundingbox(split_box(mCubeBox, i));
            mChildren[i]->mParent = this;
            mChildren[i]->mLevel = this->mLevel - 1;
        }
    }

    auto copy = mItems;
    for (auto&& item : copy)
    {
        auto index = get_index(item->boundingbox());
        if (index != -1) 
        {
            _SSR_ASSERT(index >= 0 && index < 8, _SSR_RDERROR);
            remove(item);
            mChildren[index]->add(item);
        }
    }

    return true;
}

auto SSRSceneOcTreeNode::get_index(const SSRBoundingBox& box) const noexcept -> SINT32
{
    auto mid = mCubeBox.center();
    if (box.x_min() < mid[0] && box.x_max() > mid[0] ||
        box.y_min() < mid[1] && box.y_max() > mid[1] ||
        box.z_min() < mid[2] && box.z_max() > mid[2]) 
    {
        return -1;
    }
    return (box.x_min() >= mid[0] ? 1 : 0) + (box.y_min() >= mid[1] ? 2 : 0) + (box.z_min() >= mid[2] ? 4 : 0);
}

auto SSRSceneOcTreeNode::child(SINT32 index) const noexcept -> Node*
{
    _SSR_ASSERT(index >= 0 && index < 8, _SSR_RDERROR);
    return mChildren[index];
}

auto SSRSceneOcTreeNode::parent() const noexcept -> Node*
{
    return mParent;
}

auto SSRSceneOcTreeNode::rebuild_items_boundingbox() noexcept -> void
{
    //mItemBox.init();
    //for (auto&& item : mItems)
    //{
    //    mItemBox.expand_by(item->boundingbox());
    //}
}

auto SSRSceneOcTreeNode::items_boundingbox() const noexcept -> SSRBoundingBox
{
    return mSceneNode->get_boundingbox();
}

auto SSRSceneOcTreeNode::item(SUINT32 index) const noexcept -> SSRSceneOcTreeItem*
{
    if (index < items_count()) 
    {
        auto begin = mItems.begin();
        _SSR_STD::advance(begin, index);
        return *begin;
    }
    return nullptr;
}

auto SSRSceneOcTreeNode::split_box(const SSRBoundingBox& box, SUINT32 index) noexcept -> SSRBoundingBox
{
    using ValueType = SSRBoundingBox::ValueType;
    ValueType div = box.x_len() / 2.0f;
    auto mid      = box.center();

    ValueType newX;
    ValueType newY;
    ValueType newZ;

    if (index & eX) 
    {
        newX = mid[0];
    }
    else 
    {
        newX = box.x_min();
    }
    if (index & eY)
    {
        newY = mid[1];
    }
    else
    {
        newY = box.y_min();
    }
    if (index & eZ)
    {
        newZ = mid[2];
    }
    else
    {
        newZ = box.z_min();
    }

    return SSRBoundingBox{ newX, newY, newZ, newX + div, newY + div, newZ + div };
}

SSRSceneOcTree::SSRSceneOcTree(SSRPrimitiveManagerInRef primitiveManager) noexcept
    : mPrimitiveManager{ primitiveManager }
{
   
}

SSRSceneOcTree::~SSRSceneOcTree() noexcept
{
    mAddList.clear();
    mModifyList.clear();
    mRemoveList.clear();
    auto realRoot = mRoot->cast();
    _SSR_DELETE_OBJECT_PTR(realRoot);
    _SSR_DELETE_OBJECT_PTR(mTree);
}

void SSRSceneOcTree::init() noexcept
{
    mRoot = new SSRSceneOcTreeNode();
    mTree = new Tree(mRoot, SSRBoundingBox(-CV_Scene_Tree_Box_Min_Length, -CV_Scene_Tree_Box_Min_Length, -CV_Scene_Tree_Box_Min_Length,
        CV_Scene_Tree_Box_Min_Length, CV_Scene_Tree_Box_Min_Length, CV_Scene_Tree_Box_Min_Length));
}

void SSRSceneOcTree::clear_scene_node(SSRRenderManagerRef renderManager) noexcept
{
    mRoot->cast()->clear_scene_node(renderManager);
}

auto SSRSceneOcTree::add_enity(Entity entity) noexcept -> bool
{
    if (!entity) 
    {
        return false;
    }
    if (!boundingbox(entity).valid()) 
    {
        return false;
    }

    if (get_entity_tree_item(entity) != nullptr) 
    {
        return false;
    }

    auto item = make_item(entity);

    auto res = add(item);

    _SSR_ASSERT(res, _SSR_RDERROR);

    return res;
}

auto SSRSceneOcTree::remove_enity(Entity entity) noexcept -> bool
{
    if (!entity) 
    {
        return false;
    }
    auto item = get_entity_tree_item(entity);
    if (item == nullptr) 
    {
        return false;
    }
    if (item->tree() != this) 
    {
        return false;
    }

    item->invalid_entity();

    clear_entity_tree_item(entity);

    auto res = remove(item);

    delete_item(item);

    _SSR_ASSERT(res, _SSR_RDERROR);

    return res;
}

auto SSRSceneOcTree::modify_enity(Entity entity, bool isPositionChanged) noexcept -> bool
{
    if (!entity) 
    {
        return false;
    }

    auto item = get_entity_tree_item(entity);

    if (item == nullptr) 
    {
        return false;
    }

    if (item->is_in_tree()) 
    {
        return false;
    }

    if (item->is_added()) 
    {
        if (isPositionChanged) 
        {
            mModifyList.insert(item);
        }
        item->node()->cast()->setModifiedEntity(item);
    }
    else 
    {
        _SSR_ASSERT(mAddList.find(item) != mAddList.end(), _SSR_RDERROR);
    }

    return true;
}

auto SSRSceneOcTree::update_octree() noexcept -> void
{
    using ItemVector = SSRVector<SSRSceneOcTreeItem::Item*>;
    ItemVector add;
    for (auto item : mAddList) 
    {
        add.push_back(item);
    }

    ItemVector motify;
    for (auto item : mModifyList)
    {
        motify.push_back(item);
    }

    ItemVector move;
    for (auto item : mRemoveList)
    {
        move.push_back(item);

        item->invalid_entity();
        clear_entity_tree_item(item->entity());
        delete_item(item);
    }

    auto res = mTree->process(add, motify, move);

    _SSR_ASSERT(res, _SSR_RDERROR);

    mAddList.clear();
    mModifyList.clear();
    mRemoveList.clear();
}

auto SSRSceneOcTree::boundingbox(Entity entity) noexcept -> const SSRBoundingBox&
{
    return mPrimitiveManager.get_boundingbox(entity);
}

auto SSRSceneOcTree::select_by_functor(const Tree::SelectFunc& func) noexcept -> SSRSceneOcTree::SelectResult
{
    update_octree();
    return mTree->select(func, KeyCompare{}, Tree::SelectMethod::eFrustumCull);
}

auto SSRSceneOcTree::select_by_box_functor(SelectResult& crossNodes, SelectResult& containNodes, const Tree::BoxSelectFunc& func) noexcept -> void
{
    update_octree();
    return mTree->select(crossNodes, containNodes, func);
}

auto SSRSceneOcTree::select_octree_node(SSRCameraInPtr camera) noexcept -> SelectResult
{
    update_octree();
    auto frustumPlane = get_frustum_planes(camera);
    Tree::SelectFunc func = [&frustumPlane](const SSRBoundingBox& box) -> bool
        {
            auto boxVertices = get_vertex_of_box(box);
            for (SUINT32 i = 0; i < 6; i++) 
            {
                bool isOutSide{ true };
                for (SUINT32 j = 0; j < 8; j++)
                {
                    if ((frustumPlane[i][0] * boxVertices[j][0] + frustumPlane[i][1] * boxVertices[j][1] + frustumPlane[i][2] * boxVertices[j][2] + frustumPlane[i][3]) >= 0)
                    {
                        isOutSide = false;
                        break;
                    }
                }
                if (isOutSide) 
                {
                    return false;
                }
            }
            return true;
        };
    return mTree->select(func, KeyCompare{}, Tree::SelectMethod::eFrustumCull);
}

auto SSRSceneOcTree::add(SSRSceneOcTreeItem* item) noexcept -> bool
{
    if (item == nullptr) 
    {
        return false;
    }
    if (item->is_in_tree())
    {
        return false;
    }
    _SSR_ASSERT(!item->is_added(), _SSR_RDERROR);
    auto pos = mAddList.insert(item);
    _SSR_ASSERT(pos.second, _SSR_RDERROR);
    item->set_in_tree(this);
    return true;
}

auto SSRSceneOcTree::remove(SSRSceneOcTreeItem* item) noexcept -> bool
{
    if (item == nullptr) 
    {
        return false;
    }
    if (!item->is_in_tree()) 
    {
        return false;
    }
    if (item->is_added()) 
    {
        if (mModifyList.find(item) != mModifyList.end()) 
        {
            mModifyList.erase(item);
        }

        auto res = mTree->remove(item);
        _SSR_ASSERT(res, _SSR_RDERROR);
        return false;
    }
    else 
    {
        if (mAddList.find(item) != mAddList.end())
        {
            mAddList.erase(item);
        }
    }

    item->set_in_tree(nullptr);

    return true;
}

auto SSRSceneOcTree::make_item(Entity entity) noexcept -> SSRSceneOcTreeItem*
{
    _SSR_ASSERT(entity, _SSR_RDERROR);
    auto item = new SSRSceneOcTreeItem(entity) ;
    mPrimitiveManager.set_octree_item(entity, item);
    return item;
}

auto SSRSceneOcTree::delete_item(const SSRSceneOcTreeItem* item) noexcept -> void
{
    _SSR_DELETE_OBJECT_PTR(item);
}

auto SSRSceneOcTree::get_entity_tree_item(Entity entity) noexcept -> SSRSceneOcTreeItem*
{
    _SSR_ASSERT(entity, _SSR_RDERROR);
    return mPrimitiveManager.get_octree_item(entity);
}

auto SSRSceneOcTree::clear_entity_tree_item(Entity entity) noexcept -> void
{    
    _SSR_ASSERT(entity, _SSR_RDERROR);
    mPrimitiveManager.set_octree_item(entity, nullptr);
}


_SSR_END

