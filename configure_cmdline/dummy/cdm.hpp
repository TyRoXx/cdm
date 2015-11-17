#include <ventura/absolute_path.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(
		ventura::absolute_path const &module_temporaries,
		ventura::absolute_path const &module_permanent,
		ventura::absolute_path const &application_source,
		ventura::absolute_path const &application_build_dir,
		unsigned cpu_parallelism,
	    cdm::configuration const &target,
		Si::Sink<char, Si::success>::interface &output
		)
	{
		Si::ignore_unused_variable_warning(module_temporaries);
		Si::ignore_unused_variable_warning(module_permanent);
		Si::ignore_unused_variable_warning(application_source);
		Si::ignore_unused_variable_warning(application_build_dir);
		Si::ignore_unused_variable_warning(cpu_parallelism);
		Si::ignore_unused_variable_warning(target);
		Si::ignore_unused_variable_warning(output);
	}
}
