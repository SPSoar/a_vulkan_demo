#ifndef S_VULKAN_RESOURCE_CACHING_H_
#define S_VULKAN_RESOURCE_CACHING_H_

#include "vulkan_common.h"
#include "Driver/SPDevice.h"
#include "Resource/SPResourceRecord.h"
#include "Resource/SPShaderSource.h"


namespace std 
{
	template <>
	struct hash<SPShaderSource>
	{
		std::size_t operator()(const SPShaderSource& shader_source) const
		{
			std::size_t result = 0;

			sp::hash_combine(result, shader_source.get_id());

			return result;
		}
	};

	template <>
	struct hash<SPShaderVariant>
	{
		std::size_t operator()(const SPShaderVariant& shader_variant) const
		{
			std::size_t result = 0;

			sp::hash_combine(result, shader_variant.get_id());

			return result;
		}
	};

	template <>
	struct hash<SPShaderModule>
	{
		std::size_t operator()(const SPShaderModule& shader_module) const
		{
			std::size_t result = 0;

			sp::hash_combine(result, shader_module.get_id());

			return result;
		}
	};

	template <>
	struct hash<SPShaderResource>
	{
		std::size_t operator()(const SPShaderResource& shader_resource) const
		{
			std::size_t result = 0;

			if (shader_resource.type_ == SPShaderResourceType::Input ||
				shader_resource.type_ == SPShaderResourceType::Output ||
				shader_resource.type_ == SPShaderResourceType::PushConstant ||
				shader_resource.type_ == SPShaderResourceType::SpecializationConstant)
			{
				return result;
			}

			sp::hash_combine(result, shader_resource.set_);
			sp::hash_combine(result, shader_resource.binding_);
			sp::hash_combine(result, static_cast<std::underlying_type<SPShaderResourceType>::type>(shader_resource.type_));
			sp::hash_combine(result, shader_resource.mode_);

			return result;
		}
	};
}

template <typename T>
inline void hash_param(size_t& seed, const T& value)
{
	sp::hash_combine(seed, value);
}

template <>
inline void hash_param<std::vector<uint8_t>>(
	size_t& seed,
	const std::vector<uint8_t>& value)
{
	sp::hash_combine(seed, std::string{ value.begin(), value.end() });
}


template <>
inline void hash_param<std::vector<SPShaderModule*>>(
	size_t& seed,
	const std::vector<SPShaderModule*>& value)
{
	for (auto& shader_module : value)
	{
		sp::hash_combine(seed, shader_module->get_id());
	}
}

template <>
inline void hash_param<std::vector<SPShaderResource>>(
	size_t& seed,
	const std::vector<SPShaderResource>& value)
{
	for (auto& resource : value)
	{
		sp::hash_combine(seed, resource);
	}
}


template <typename T, typename... Args>
inline void hash_param(size_t& seed, const T& first_arg, const Args &... args)
{
	hash_param(seed, first_arg);

	hash_param(seed, args...);
}



template <class T, class... A>
struct SPRecordHelper
{
	size_t record(SPResourceRecord& /*recorder*/, A &... /*args*/)
	{
		return 0;
	}

	void index(SPResourceRecord& /*recorder*/, size_t /*index*/, T& /*resource*/)
	{
	}
};

template <class... A>
struct SPRecordHelper<SPShaderModule, A...>
{
	size_t record(SPResourceRecord& recorder, A &... args)
	{
		return recorder.register_shader_module(args...);
	}

	void index(SPResourceRecord& recorder, size_t index, SPShaderModule& shader_module)
	{
		recorder.set_shader_module(index, shader_module);
	}
};

template <class T, class... A>
T& request_resource(SPDevice& device, SPResourceRecord* recorder, std::unordered_map<uint64_t, T>& resources, A &... args)
{
	SPRecordHelper<T, A...> record_helper;

	std::size_t hash{ 0U };
	hash_param(hash, args...);

	auto res_it = resources.find(hash);

	if (res_it != resources.end())
	{
		return res_it->second;
	}

	// If we do not have it already, create and cache it
	const char* res_type = typeid(T).name();
	uint64_t      res_id = resources.size();

	//LOGD("Building #{} cache object ({})", res_id, res_type);

	// Only error handle in release
#ifndef _DEBUG
	try
	{
#endif
		T resource(device, args...);

		auto res_ins_it = resources.emplace(hash, std::move(resource));

		if (!res_ins_it.second)
		{
			throw std::runtime_error{ std::string{"Insertion error for #"} + std::to_string(res_id) + "cache object (" + res_type + ")" };
		}

		res_it = res_ins_it.first;

		if (recorder)
		{
			uint64_t index = record_helper.record(*recorder, args...);
			record_helper.index(*recorder, index, res_it->second);
		}
#ifndef _DEBUG
	}
	catch (const std::exception& e)
	{
		//LOGE("Creation error for #{} cache object ({})", res_id, res_type);
		throw e;
	}
#endif

	return res_it->second;
}

#endif