#ifndef S_SPINKINGASSISTANT_H_
#define S_SPINKINGASSISTANT_H_
#include "common.h"
#include "../SPBase.h"
class SPMangaka;
class SPStudioManager;
class SPDraft;
class SPNeMu;
class SPBuffer;
class SPDraft;
class SPDrawingPadVulkan;
class RHIEXPORT SPInkingAssistant
{
    struct InkingDetails;
	friend SPNeMu;
	friend SPDrawingPadVulkan;
public:
	SPInkingAssistant(SPMangaka* boss);
	virtual ~SPInkingAssistant();
    void inkDraft(SPModelConfig& slConfig);

    void destoryDraft(const spstr& id, SPNeMu* nemu);
    void destoryAllDrafts(SPNeMu* nemu);

    //创建完资源就析构了
    class Inking : public SPSkillBase<InkingDetails> 
    {
        friend struct InkingDetails;
    public:
        enum Result { Error = -1, Success = 0 };
        explicit Inking(size_t count) noexcept;

        Inking(Inking const& inking) = delete;
        Inking(Inking&& inking) noexcept;
        ~Inking() noexcept;
        Inking& operator=(Inking& inking) = delete;
        Inking& operator=(Inking&& inking) noexcept;
        Inking& resources(size_t index, PrimitiveTopology type, spbool primitiveRestartEnable, std::vector<SPVertex>& vertices, std::vector<spint>& indices) noexcept;
        Result ink(SPStudioManager& manager, SPDraft& model);

    private:
      
        struct Entry {
            std::vector<SPVertex>* vertices = nullptr;
            std::vector<spint>* indices = nullptr;
            PrimitiveTopology type = PrimitiveTopology::TRIANGLE;
            spbool primitiveRestartEnable;
        };
    };

private:
	SPMangaka* boss;
	SPStudioManager* manager;
};

#endif
