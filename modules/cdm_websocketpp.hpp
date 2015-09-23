#ifndef CDM_WEBSOCKETPP_HPP
#define CDM_WEBSOCKETPP_HPP

#include "cdm_boost.hpp"
#include <silicium/file_operations.hpp>

namespace cdm
{
	struct websocketpp_paths
	{
		Si::absolute_path include;
		Si::absolute_path boost_root;
	};

	inline websocketpp_paths install_websocketpp(
		Si::absolute_path const &original_source,
		Si::absolute_path const &boost_source,
		Si::absolute_path const &temporarily_writable,
		Si::absolute_path const &install_root,
		Si::Sink<char, Si::success>::interface &output)
	{
		websocketpp_paths result;
		Si::absolute_path const in_cache = install_root / Si::relative_path("websocketpp");
		if (!Si::file_exists(in_cache, Si::throw_))
		{
			{
				Si::absolute_path const boost_temp = temporarily_writable / Si::relative_path("boost");
				Si::create_directories(boost_temp, Si::throw_);
				unsigned make_parallelism = 2;
				cdm::boost_paths const boost_installed = cdm::install_boost(boost_source, boost_temp, install_root, make_parallelism, output);
				result.boost_root = boost_installed.root;
			}
			Si::absolute_path const construction = temporarily_writable / Si::relative_path("websocketpp");
			Si::absolute_path const construction_include = construction / Si::relative_path("include");
			Si::create_directories(construction_include, Si::throw_);
			Si::copy_recursively(original_source / Si::relative_path("websocketpp"), construction_include, &output, Si::throw_);
			Si::rename(construction, in_cache, Si::throw_);
		}
		result.include = in_cache / Si::relative_path("include");
		return result;
	}
}

#endif
