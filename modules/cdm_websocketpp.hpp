#ifndef CDM_WEBSOCKETPP_HPP
#define CDM_WEBSOCKETPP_HPP

#include <silicium/file_operations.hpp>

namespace cdm
{
	struct websocketpp_paths
	{
		Si::absolute_path include;
	};

	inline websocketpp_paths install_websocketpp(
		Si::absolute_path const &original_source,
		Si::absolute_path const &temporarily_writable,
		Si::absolute_path const &install_root,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::absolute_path const in_cache = install_root / Si::relative_path("websocketpp");
		if (!Si::file_exists(in_cache, Si::throw_))
		{
			Si::absolute_path const construction = temporarily_writable / Si::relative_path("websocketpp");
			Si::absolute_path const construction_include = construction / Si::relative_path("include");
			Si::create_directories(construction_include, Si::throw_);
			Si::copy_recursively(original_source / Si::relative_path("websocketpp"), construction_include, &output, Si::throw_);
			Si::rename(construction, in_cache, Si::throw_);
		}
		websocketpp_paths result;
		result.include = in_cache / Si::relative_path("include");
		return result;
	}
}

#endif
