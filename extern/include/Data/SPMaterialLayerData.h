#ifndef S_SPMATERIALLAYERDATA_H
#define S_SPMATERIALLAYERDATA_H


#include "SPEditData.h"



namespace spidx {

	namespace material_layer {
		const sp_string MaterialType = "materialType_s_";

		const sp_string UVType = "uvType_s_";
		const sp_string UVScale = "uvScale_s_";
		const sp_string UVOffset = "uvOffset_s_";
		const sp_string Rotation = "rotation_s_";

		const sp_string BaseColor = "baseColor_s_";
		const sp_string Metallic = "metallic_s_";
		const sp_string Roughness = "roughness_s_";
		const sp_string Reflectance = "reflectance_s_";
		const sp_string SheenColor = "sheenColor_s_";
		const sp_string SheenRoughness = "sheenRoughness_s_";
		const sp_string ClearCoat = "clearCoat_s_";
		const sp_string ClearCoatRoughness = "clearCoatRoughness_s_";
		const sp_string Anisotropy = "anisotropy_s_";
		const sp_string AnisotropyDirection = "anisotropyDirection_s_";
		const sp_string AmbientOcclusion = "ambientOcclusion_s_";
		const sp_string Normal = "normal_s_";
		const sp_string BentNormal = "bentNormal_s_";
		const sp_string ClearCoatNormal = "clearCoatNormal_s_";
		const sp_string Emissive = "emissive_s_";
		const sp_string IOR = "ior_s_";
		const sp_string Transmission = "transmission_s_";
		const sp_string Absorption = "absorption_s_";
		const sp_string MicroThickness = "microThickness_s_";
		const sp_string Thickness = "thickness_s_";

	}
	
}

enum class ColorSpace : sp_uint_8
{
	None = 0,
	linear,
	sRgb
};

class ImageInfo
{
public:
	ImageInfo()
		: with_(0)
		, height_(0)
		, channels_(0)
		, element_size_(0)
		, dpi_x_(0)
		, dpi_y_(0)
		, color_space_(ColorSpace::None)
		, data_(nullptr)
		, image_path_("")
		, is_used_(false)
		, is_deleted_(true)
	{}
	ImageInfo(const ImageInfo& other) = delete;
	ImageInfo& operator=(const ImageInfo& other) = delete;
	ImageInfo(ImageInfo&& other) noexcept
	{
		this->with_ = other.with_;
		this->height_ = other.height_;
		this->channels_ = other.channels_;
		this->element_size_ = other.element_size_;
		this->dpi_x_ = other.dpi_x_;
		this->dpi_y_ = other.dpi_y_;
		this->color_space_ = other.color_space_;
		this->data_ = std::move(other.data_);
		this->is_deleted_ = other.is_deleted_;
		this->is_used_ = other.is_used_;
		this->image_path_ = other.image_path_;

	}
	ImageInfo& operator=(ImageInfo&& other) noexcept
	{
		this->with_ = other.with_;
		this->height_ = other.height_;
		this->channels_ = other.channels_;
		this->element_size_ = other.element_size_;
		this->dpi_x_ = other.dpi_x_;
		this->dpi_y_ = other.dpi_y_;
		this->color_space_ = other.color_space_;
		this->data_ = std::move(other.data_);
		this->is_deleted_ = other.is_deleted_;
		this->is_used_ = other.is_used_;
		this->image_path_ = other.image_path_;
		return *this;
	}
	~ImageInfo()
	{
		SP_DELETE_ARRAY_PTR(data_);
	}
public:
	sp_uint_64 get_data_length() 
	{
		return with_ * height_ * channels_ * element_size_;
	}
	sp_uchar* get_data_ptr() 
	{
		return data_;
	}
public:
	sp_uint_32 with_;
	sp_uint_32 height_;
	sp_uint_8  channels_;
	sp_uint_8  element_size_;
	sp_uint_16 dpi_x_;
	sp_uint_16 dpi_y_;
	ColorSpace color_space_;
	sp_string image_path_;
	sp_bool is_used_;
	sp_bool is_deleted_;
	sp_uchar*  data_;
private:
};

class ParameterInfo 
{
public:
	ParameterInfo() = default;
	ParameterInfo(const sp_string& index)
		: index_(index)
		, value_()
	{}
	ParameterInfo(const sp_string& index, const SPVariant& value)
		: index_(index)
		, value_(value)
	{}
	ParameterInfo(const ParameterInfo& other) = delete;
	ParameterInfo& operator=(const ParameterInfo& other) = delete;
	ParameterInfo(ParameterInfo&& other) noexcept
	{
		this->image_info_ = std::move(other.image_info_);
		this->index_ = other.index_;
		this->value_ = other.value_;
	}
	ParameterInfo& operator=(ParameterInfo&& other) noexcept
	{
		this->image_info_ = std::move(other.image_info_);
		this->index_ = other.index_;
		this->value_ = other.value_;
		return *this;
	}
	~ParameterInfo() = default;
public:
	SP_CLASS_MEMBER(ParameterInfo, sp_string, index)
	SP_CLASS_MEMBER(ParameterInfo, SPVariant, value)
public:
	ImageInfo image_info_;
};


#define SP_PARAMETER_SET(map, index, value) \
	map[index] = std::move(ParameterInfo(index, value));

using ParameterInfoMap = std::unordered_map<sp_string, ParameterInfo>;

class SPMaterialLayerData : public SPEditData, public SPBaseData<DataInfo>
{
	friend class SPSceneFileController;
	friend class SPMaterialLayerParser;
public:
	SPMaterialLayerData() = delete;
	SPMaterialLayerData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid);
	virtual ~SPMaterialLayerData();

public:
	virtual void pick_observers(std::multiset<SPEditor*, SPEditorComp> observers) override;
	virtual void add_observer(SPEditor* observer) override;
	virtual void remove_observer(SPEditor* observer) override;
	virtual void notify_observer_for_read_data(CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_write_data(const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_save_data(const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_edit_data(const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_read_modified_data(const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_command(const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_command_modified(const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;

public:
	void init();
	ParameterInfo& get_parameter_info(const sp_string& index);
	ParameterInfoMap& get_parameter_map();
	static std::unordered_map<sp_string, bool> Parameter_Tag_Map_;
	static std::unordered_map<sp_string, sp_string> Parameter_UI_Name_Map_;
protected:
	ParameterInfoMap parameter_info_map_;
};

using SPMaterialLayerDataPtr = SPMaterialLayerData*;

#endif