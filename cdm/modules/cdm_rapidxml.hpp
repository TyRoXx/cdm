#ifndef CDM_RAPIDXML_HPP
#define CDM_RAPIDXML_HPP

#include <silicium/file_operations.hpp>

namespace cdm
{
	struct rapidxml_paths
	{
		Si::absolute_path include;
		Si::absolute_path library;
	};

	inline rapidxml_paths install_rapidxml(
		Si::absolute_path const &original_source,
		Si::absolute_path const &temporarily_writable,
		Si::absolute_path const &install_root,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::absolute_path const in_cache = install_root / Si::relative_path("rapidxml");
		if (!Si::file_exists(in_cache, Si::throw_))
		{
			Si::absolute_path const construction = temporarily_writable / Si::relative_path("rapidxml");
			Si::copy_recursively(original_source, construction, &output, Si::throw_);
			Si::rename(construction, in_cache, Si::throw_);
		}
		rapidxml_paths result;
		result.include = in_cache;
		return result;
	}
}

#endif
