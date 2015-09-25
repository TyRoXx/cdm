#ifndef CDM_RAPIDJSON_HPP
#define CDM_RAPIDJSON_HPP

#include <silicium/file_operations.hpp>

namespace cdm
{
	struct rapidjson_paths
	{
		Si::absolute_path include;
	};

	inline rapidjson_paths install_rapidjson(
		Si::absolute_path const &original_source,
		Si::absolute_path const &temporarily_writable,
		Si::absolute_path const &install_root,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::absolute_path const in_cache = install_root / Si::relative_path("rapidjson");
		if (!Si::file_exists(in_cache, Si::throw_))
		{
			Si::absolute_path const construction = temporarily_writable / Si::relative_path("rapidjson");
			Si::create_directories(construction, Si::throw_);
			Si::copy_recursively(original_source / Si::relative_path("include/rapidjson"), construction / Si::relative_path("rapidjson"), &output, Si::throw_);
			Si::rename(construction, in_cache, Si::throw_);
		}
		rapidjson_paths result;
		result.include = in_cache;
		return result;
	}
}

#endif
