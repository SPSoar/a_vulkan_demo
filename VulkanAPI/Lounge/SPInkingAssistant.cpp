#include "SPInkingAssistant.h"
#include "../SPMangaka.h"
#include "../Interface/SPDraft.h"
#include "../SPNeMu.h"

struct SPInkingAssistant::InkingDetails {
    using Entry = SPInkingAssistant::Inking::Entry;
    std::vector<Entry> mEntries;

    explicit InkingDetails(size_t count)
        : mEntries(count){
    }
    InkingDetails() = default;
};

SPInkingAssistant::SPInkingAssistant(SPMangaka* boss)
	: boss(boss)
	, manager(boss->s_manager)
{

}

SPInkingAssistant::~SPInkingAssistant()
{
}
//SPDraft bufferÊÍ·Å slConfig ÊÍ·Å
void SPInkingAssistant::inkDraft(SPModelConfig& slConfig)
{
	SPDraft* draft = manager->createDraft();
    SPInkingAssistant::Inking(1)
        .resources(0, slConfig.setting.primTopology, slConfig.setting.primRestartEnable, slConfig.vertices, slConfig.indices)
        .ink(*manager, *draft);
    boss->receiveDraft(draft);
}

void SPInkingAssistant::destoryDraft(const spstr& id, SPNeMu* nemu)
{
    if (nemu->drafts.find(id) != nemu->drafts.end())
    {
        manager->destroyVertexBuffer(nemu->drafts[id]->vertexBuffer);
        manager->destroyIndexBufferr(nemu->drafts[id]->indexBuffer);
        delete nemu->drafts[id];
        nemu->drafts.erase(id);
    }
}

void SPInkingAssistant::destoryAllDrafts(SPNeMu* nemu)
{
    std::unordered_map<spstr, SPDraft*>::iterator iter;
    for (iter = nemu->drafts.begin(); iter != nemu->drafts.end(); iter++)
    {
        manager->destroyVertexBuffer(iter->second->vertexBuffer);
        manager->destroyIndexBufferr(iter->second->indexBuffer);
        delete iter->second;
    }
    nemu->drafts.clear();
}


using BuilderType = SPInkingAssistant;
BuilderType::Inking::Inking(size_t count) noexcept 
    : SPSkillBase<BuilderType::InkingDetails>(count) {
    assert(mImpl->mEntries.size() == count);
}
BuilderType::Inking::~Inking() noexcept = default;
BuilderType::Inking::Inking(BuilderType::Inking&& inking) noexcept = default;
BuilderType::Inking& BuilderType::Inking::operator=(BuilderType::Inking&& inking) noexcept = default;


BuilderType::Inking& BuilderType::Inking::resources(
    size_t index, PrimitiveTopology type, spbool primitiveRestartEnable, 
    std::vector<SPVertex>& vertices, std::vector<spint>& indices/*, size_t offset, size_t minIndex,
    size_t maxIndex, size_t count*/) noexcept
{
    std::vector<Entry>& entries = mImpl->mEntries;
    if (index < entries.size()) {
        entries[index].vertices = &vertices;
        entries[index].indices = &indices;
        entries[index].primitiveRestartEnable = primitiveRestartEnable;
        //entries[index].offset = offset;
        //entries[index].minIndex = minIndex;
        //entries[index].maxIndex = maxIndex;
        //entries[index].count = count;
        entries[index].type = type;
    }
	return *this;
}

BuilderType::Inking::Result SPInkingAssistant::Inking::ink(SPStudioManager& manager, SPDraft& draft)
{
    for (size_t i = 0, c = mImpl->mEntries.size(); i < c; i++) {
        draft.vertexBuffer= manager.createVertexBuffer(*mImpl->mEntries[i].vertices);
        draft.indexBuffer = manager.createIndexBuffer(*mImpl->mEntries[i].indices);
        draft.setPrimitiveTopology(mImpl->mEntries[i].type);
        draft.setPrimitiveRestartEnable(mImpl->mEntries[i].primitiveRestartEnable);
    }
   
    return Result::Success;
}
