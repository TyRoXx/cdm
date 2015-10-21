#ifndef CDM_RAPIDJSON_HPP
#define CDM_RAPIDJSON_HPP

#include <ventura/file_operations.hpp>

namespace cdm
{
	struct rapidjson_paths
	{
		ventura::absolute_path include;
	};

	inline rapidjson_paths install_rapidjson(ventura::absolute_path const &original_source,
	                                         ventura::absolute_path const &temporarily_writable,
	                                         ventura::absolute_path const &install_root,
	                                         Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const in_cache = install_root / ventura::relative_path("rapidjson");
		if (!ventura::file_exists(in_cache, Si::throw_))
		{
			ventura::absolute_path const construction = temporarily_writable / ventura::relative_path("rapidjson");
			ventura::create_directories(construction, Si::throw_);
			ventura::copy_recursively(original_source / ventura::relative_path("include/rapidjson"),
			                          construction / ventura::relative_path("rapidjson"), &output, Si::throw_);
			ventura::rename(construction, in_cache, Si::throw_);
		}
		rapidjson_paths result;
		result.include = in_cache;
		return result;
	}
}

#endif
