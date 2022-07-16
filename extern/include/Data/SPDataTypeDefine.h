#ifndef S_SPDATATYPEDEFINE_H
#define S_SPDATATYPEDEFINE_H

#include <Base/base_common.h>

#define CAMERADIR        "Camera"
#define LIGHTDIR         "Light"
#define SKYBOXDIR        "Skybox"
#define MESHDIR          "Mesh"
#define MATERIALDIR      "Material"
#define MATERIALLAYERDIR "MaterialLayer"
#define HDRPOOLDIR       "HDRPool"
#define MESHPOOLDIR      "MeshPool"
#define TEXTUREPOOLDIR   "TexturePool"


#define CAMERASUFFIX         "camera"
#define LIGHTSUFFIX          "light"
#define SKYBOXSUFFIX         "skybox"
#define MESHSUFFIX           "mesh"
#define MATERIALSUFFIX       "material"
#define MATERIALLAYERSUFFIX  "materiallayer"
#define HDRPOOLSUFFIX        "hdrpool"
#define MESHPOOLSUFFIX       "meshpool"
#define TEXTUREPOOLSUFFIX    "texturepool"
#define SCENESUFFIX          "scene"

#define TEMPORARYNAME       "_temp"

#define ELEMENTCOUNT       "element_count"

typedef std::function<void(bool, SPVariant)> CallbackFunc;


enum class DataType : sp_uint_8
{
	None = 0,
	SceneType,
	CameraType,
	LightType,
	SkyboxType,
	MeshType,
	MaterialType,
	MaterialLayerType,
	HDRBinaryType,
	MeshBinaryType,
	TextureBinaryType,
	NoneType
};

static constexpr sp_uint_8 DATA_LEVEL_FILE   = 1;
static constexpr sp_uint_8 DATA_LEVEL_RENDER = 2;
static constexpr sp_uint_8 DATA_LEVEL_PLUGIN = 3;

struct SPAccess
{

	SPAccess() = delete;
	~SPAccess() = default;
	
	SPAccess(const sp_uuid& data_pid, const sp_uuid& data_id, sp_string data_idx, SPVariant val = SPVariant())
		: data_id_(data_pid)
		, data_idx_(data_idx)
		, data_id_second_(data_id)
		, spare_val_(val)
	{

	}

	SPAccess(SPAccess&& v) noexcept
	{
		this->data_id_ = v.data_id_;
		this->data_idx_ = v.data_idx_;
		this->data_id_second_ = v.data_id_second_;
		this->spare_val_ = v.spare_val_;
	}
	SPAccess& operator=(SPAccess&& v) noexcept
	{
		this->data_id_ = v.data_id_;
		this->data_idx_ = v.data_idx_;
		this->data_id_second_ = v.data_id_second_;
		this->spare_val_ = v.spare_val_;
		return *this;
	}

	SPAccess(const SPAccess& v) noexcept
	{
		this->data_id_ = v.data_id_;
		this->data_idx_ = v.data_idx_;
		this->data_id_second_ = v.data_id_second_;
		this->spare_val_ = v.spare_val_;
	}
	SPAccess& operator=(const SPAccess& v) noexcept
	{
		this->data_id_ = v.data_id_;
		this->data_idx_ = v.data_idx_;
		this->data_id_second_ = v.data_id_second_;
		this->spare_val_ = v.spare_val_;
		return *this;
	}

public:
	sp_uuid data_id_;
	sp_uuid data_id_second_;
	sp_string data_idx_;;
	SPVariant spare_val_;
};


struct SPCommand
{
	enum class CommandType : sp_uint_8
	{
		None = 0,
		DeleteData,
		RenameData,
		CreateData,
	};

	SPCommand() = delete;
	~SPCommand() = default;

	SPCommand(const SPCommand& v) = delete;
	SPCommand& operator=(const SPCommand& v) = delete;

	SPCommand(const CommandType& type, const DataType& data_type, SPVariant val1 = SPVariant(), SPVariant val2 = SPVariant(), SPVariant spare_val = SPVariant())
		: type_(type)
		, data_type_(data_type)
		, var_(val1)
		, var_second_(val2)
		, spare_val_(spare_val)
	{

	}

	SPCommand(SPCommand&& v) noexcept
	{
		this->type_ = v.type_;
		this->var_ = v.var_;
		this->var_second_ = v.var_second_;
		this->data_type_ = v.data_type_;
		this->spare_val_ = v.spare_val_;
	}
	SPCommand& operator=(SPCommand&& v) noexcept
	{
		this->type_ = v.type_;
		this->var_ = v.var_;
		this->var_second_ = v.var_second_;
		this->data_type_ = v.data_type_;
		this->spare_val_ = v.spare_val_;
		return *this;
	}
public:
	CommandType type_;
	SPVariant var_;
	SPVariant var_second_;
	DataType data_type_;
	SPVariant spare_val_;
};

struct Permissions {

	Permissions() noexcept = default;
	constexpr explicit Permissions(sp_uint_8 key) noexcept : key(key) { }

	sp_uint_8 key = 0;

	static constexpr sp_uint_8 READ_PERMISSIONS = 0x01;
	static constexpr sp_uint_8 WRITE_PERMISSIONS = 0x02;
	static constexpr sp_uint_8 EDIT_PERMISSIONS = 0x04;
	static constexpr sp_uint_8 READMODIFIED_PERMISSIONS = 0x08;
	static constexpr sp_uint_8 SAVE_PERMISSIONS = 0x10;
	static constexpr sp_uint_8 COMMAND_PERMISSIONS = 0x20;
	static constexpr sp_uint_8 COMMANDMODIFIED_PERMISSIONS = 0x40;


	inline bool hasReadPermissions() const noexcept { return key & READ_PERMISSIONS; }
	inline bool hasWritePermissions() const noexcept { return key & WRITE_PERMISSIONS; }
	inline bool hasEditPermissions() const noexcept { return key & EDIT_PERMISSIONS; }
	inline bool hasReadModifiedPermissions() const noexcept { return key & READMODIFIED_PERMISSIONS; }
	inline bool hasSavePermissions() const noexcept { return key & SAVE_PERMISSIONS; }
	inline bool hasCommandPermissions() const noexcept { return key & COMMAND_PERMISSIONS; }
	inline bool hasCommandModifiedPermissions() const noexcept { return key & COMMANDMODIFIED_PERMISSIONS; }
	inline bool hasPermissions() const noexcept { return key; }

	inline void setReadPermissions(bool v) noexcept { set(v, READ_PERMISSIONS); }
	inline void setSavePermissions(bool v) noexcept { set(v, SAVE_PERMISSIONS); }
	inline void setWritePermissions(bool v) noexcept { set(v, WRITE_PERMISSIONS); }
	inline void setEditPermissions(bool v) noexcept { set(v, EDIT_PERMISSIONS); }
	inline void setReadModifiedPermissions(bool v) noexcept { set(v, READMODIFIED_PERMISSIONS); }
	inline void setCommandPermissions(bool v) noexcept { set(v, COMMAND_PERMISSIONS); }
	inline void setCommandModifiedPermissions(bool v) noexcept { set(v, COMMANDMODIFIED_PERMISSIONS); }

private:
	inline void set(sp_bool v, sp_uint_8 mask) noexcept {
		key = (key & ~mask) | (v ? mask : sp_uint_8(0));
	}
};


namespace spidx 
{
	static const sp_string Id = "id_s_";
	static const sp_string ResourceId = "resource_id_s_";
	static const sp_string Name = "name_s_";
	static const sp_string Type = "type_s_";
	static const sp_string Path = "path_s_";
	static const sp_string CreatTime = "creat_time_s_";
	static const sp_string EditTime = "edit_time_s_";
	static const sp_string Author = "author_s_";
	static const sp_string Software = "software_s_";
	static const sp_string Version = "version_s_";
	
}


namespace sp_operater
{
	static const sp_string Enable = "enable_s_";
	static const sp_string Delete = "delete_s_";
	static const sp_string Add =    "add_s_";
}

#define SPLITVALUE "@"

class SPIndex 
{
public:
	static sp_string Make_Enable_Index(const sp_string& base_index) 
	{
		return base_index + SPLITVALUE + sp_operater::Enable;
	}
	static sp_string Make_Delete_Index(const sp_string& base_index)
	{
		return base_index + SPLITVALUE + sp_operater::Delete;
	}
	static sp_string Make_Add_Index(const sp_string& base_index)
	{
		return base_index + SPLITVALUE + sp_operater::Add;
	}
	static sp_string Get_Base_Index(const sp_string& index)
	{
		return SPStringTools::Split(index, SPLITVALUE).at(0);
	}
	static sp_bool Has_Sub_Operater(const sp_string& index)
	{
		return index.find(SPLITVALUE) != index.npos;
	}
	static sp_string Get_Operater_Index(const sp_string& index)
	{
		std::vector<sp_string> list = SPStringTools::Split(index, SPLITVALUE);
		if (list.size() == 1)
			return list[0];
		else if (list.size() == 2)
			return list[1];
		return index;
	}
};

#endif
