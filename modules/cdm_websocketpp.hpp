#ifndef CDM_WEBSOCKETPP_HPP
#define CDM_WEBSOCKETPP_HPP

#include "cdm_boost.hpp"
#include <ventura/file_operations.hpp>

namespace cdm
{
	struct websocketpp_paths
	{
		ventura::absolute_path include;
		ventura::absolute_path boost_root;
	};

	inline websocketpp_paths install_websocketpp(ventura::absolute_path const &original_source,
	                                             ventura::absolute_path const &boost_source,
	                                             ventura::absolute_path const &temporarily_writable,
	                                             ventura::absolute_path const &install_root, unsigned cpu_parallelism,
	                                             Si::Sink<char, Si::success>::interface &output)
	{
		websocketpp_paths result;
		{
			ventura::absolute_path const boost_temp = temporarily_writable / "boost";
			ventura::create_directories(boost_temp, Si::throw_);
			cdm::boost_paths const boost_installed =
			    cdm::install_boost(boost_source, boost_temp, install_root, cpu_parallelism, output);
			result.boost_root = boost_installed.root;
		}

		ventura::absolute_path const in_cache = install_root / "websocketpp";
		if (!ventura::file_exists(in_cache, Si::throw_))
		{
			ventura::absolute_path const construction = temporarily_writable / "websocketpp";
			ventura::absolute_path const construction_include = construction / "include";
			ventura::create_directories(construction_include, Si::throw_);
			ventura::copy_recursively(original_source / "websocketpp", construction_include, &output, Si::throw_);
			ventura::rename(construction, in_cache, Si::throw_);
		}
		result.include = in_cache / "include";
		return result;
	}
}

#endif
