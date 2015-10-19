#ifndef CDM_BUILDING_CONFIGURE_HPP
#define CDM_BUILDING_CONFIGURE_HPP

#include <ventura/absolute_path.hpp>

namespace cdm
{
	void do_configure(
		ventura::absolute_path const &temporary,
		ventura::absolute_path const &module_permanent,
		ventura::absolute_path const &application_source,
		ventura::absolute_path const &application_build_dir,
		Si::optional<ventura::absolute_path> const &boost_root,
		Si::Sink<char, Si::success>::interface &output
	);
}

#endif
