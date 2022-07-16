#ifndef S_SPEDITDATA_H
#define S_SPEDITDATA_H

#include "SPEditor.h"

template <typename T, DataType type>
class SPDataParser;

class SPEditData : public SPObject
{
public:
	SPEditData() = delete;
	SPEditData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid = 0)
		: SPObject(id, name), parent_id_(pid), data_type_(DataType::None), is_edited_(false), is_deleted_(false) {}
	virtual ~SPEditData() = default;
	virtual void pick_observers(std::multiset<SPEditor*, SPEditorComp> observers) = 0;
	virtual void add_observer(SPEditor* observer) = 0;
	virtual void remove_observer(SPEditor* observer) = 0;
	virtual void notify_observer_for_read_data(CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void notify_observer_for_write_data(const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void notify_observer_for_save_data(const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void notify_observer_for_edit_data(const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void notify_observer_for_read_modified_data(const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void notify_observer_for_command(const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool isAsync = false) = 0;
	virtual void notify_observer_for_command_modified(const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
public:
	SP_CLASS_MEMBER(SPEditData, DataType, data_type)
	SP_CLASS_MEMBER(SPEditData, sp_uuid, parent_id)
	SP_CLASS_MEMBER(SPEditData, sp_bool, is_edited)
	SP_CLASS_MEMBER(SPEditData, sp_bool, is_deleted)
protected:
	std::multiset<SPEditor*, SPEditorComp> observers_;
};

using SPEditDataPtr = SPEditData*;

struct DataInfo
{
	SPVariantDataMap info_;
};


template <typename T>
class SPBaseData {
public:
    template<typename ... ARGS>
    explicit SPBaseData(ARGS&& ...) noexcept;
	SPBaseData() noexcept;
    ~SPBaseData() noexcept;
	SPBaseData(SPBaseData const& data) noexcept;
	SPBaseData& operator = (SPBaseData const& data) noexcept;

	SPBaseData(SPBaseData&& data) noexcept : data_(data.data_) { data.data_ = nullptr; }
	SPBaseData& operator = (SPBaseData&& data) noexcept {
        auto temp = data_;
		data_ = data.data_;
		data.data_ = temp;
        return *this;
    }
	void set_data(const sp_string& id, const SPVariant& value);
	SPVariant& get_data(const sp_string& id);
	void delete_data(const sp_string& id);
protected:
    T* data_ = nullptr;
    inline T* operator->() noexcept { return data_; }
    inline T const* operator->() const noexcept { return data_; }
};


template<typename T>
template<typename ...ARGS>
inline SPBaseData<T>::SPBaseData(ARGS && ... args) noexcept
	: data_(new T(std::forward<ARGS>(args)...))
{
}

template<typename T>
inline SPBaseData<T>::SPBaseData() noexcept
	: data_(new T)
{
}

template<typename T>
inline SPBaseData<T>::~SPBaseData() noexcept
{
	delete data_;
}

template<typename T>
inline SPBaseData<T>::SPBaseData(SPBaseData const& data) noexcept
	: data_(new T(*data.data_))
{
}

template<typename T>
inline SPBaseData<T>& SPBaseData<T>::operator=(SPBaseData const& data) noexcept
{
	*data_ = *data.data_;
	return *this;
}

template<typename T>
inline void SPBaseData<T>::set_data(const sp_string& id, const SPVariant& value)
{
	data_->info_[id] = value;
}

template<typename T>
inline SPVariant& SPBaseData<T>::get_data(const sp_string& id)
{
	return data_->info_[id];
}

template<typename T>
inline void SPBaseData<T>::delete_data(const sp_string& id)
{
	data_->info_.erase(id);
}


#endif
