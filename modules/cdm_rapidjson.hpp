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
		ventura::absolute_path const in_cache = install_root / "rapidjson";
		if (!ventura::file_exists(in_cache, Si::throw_))
		{
			ventura::absolute_path const construction = temporarily_writable / "rapidjson";
			ventura::create_directories(construction, Si::throw_);
			ventura::copy_recursively(original_source / "include/rapidjson", construction / "rapidjson", &output,
			                          Si::throw_);
			ventura::create_directories(install_root, Si::throw_);
			ventura::rename(construction, in_cache, Si::throw_);
		}
		rapidjson_paths result;
		result.include = in_cache;
		return result;
	}
}

#endif
