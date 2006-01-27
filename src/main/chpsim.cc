/**
	\file "main/chpsim.cc"
	Main module for new CHPSIM.
	Maintained primarily by Rajit Manohar.  
	$Id: chpsim.cc,v 1.1 2006/01/27 08:07:20 fang Exp $
 */

#include <iostream>
#include <list>
#include "main/chpsim.h"
#include "Object/common/namespace.tcc"	// for namespace::collect<>()
#include "Object/common/util_types.h"	// for namespace_collection_type
#include "Object/def/process_definition.h"
#include "main/program_registry.h"	// to register with hackt's dispatcher
#include "main/main_funcs.h"		// for save/load_module()
#include "util/getopt_portable.h"	// for getopt()
#include "util/memory/excl_ptr.h"	// for never_ptr

namespace HAC {
#include "util/using_ostream.h"
using entity::namespace_collection_type;
using entity::process_definition;
using util::memory::excl_ptr;
using util::memory::never_ptr;

//=============================================================================
class chpsim::options {
};	// end class options

//=============================================================================
// class chpsim static initializers

const char
chpsim::name[] = "chpsim";

const char
chpsim::brief_str[] = "Simulates CHP!";

const size_t
chpsim::program_id = register_hackt_program_class<chpsim>();

//=============================================================================
// class chpsim member definitions

chpsim::chpsim() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Add your own getopt(), parse_command_options()...
 */
int
chpsim::main(int argc, char* argv[], const global_options&) {
	if (argc != 2) {
		usage();
		return 0;
	}
	if (!check_object_loadable(argv[1]).good)
		return 1;
	excl_ptr<module> the_module =
		load_module_debug(argv[1]);
		// load_module(argv[1]);
	if (!the_module)
		return 1;
#if 0
	the_module->dump(cerr);
#endif
	typedef	std::list<never_ptr<const process_definition> >
		process_definition_list_type;
	process_definition_list_type processes;
{
	namespace_collection_type nss;
	the_module->collect_namespaces(nss);
	typedef	namespace_collection_type::const_iterator	const_iterator;
	const_iterator i(nss.begin());
	const const_iterator e(nss.end());
	for ( ; i!=e; ++i) {
		(*i)->collect(processes);
	}
	// or std::for_each(i, e, ...);
}
	// now we have a list of all process definitions in 'processes'
{
	typedef	process_definition_list_type::const_iterator	const_iterator;
	const_iterator i(processes.begin());
	const const_iterator e(processes.end());
	for ( ; i!=e; ++i) {
		// do stuff here
		(*i)->dump(cout) << endl;
	}
}

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chpsim::usage(void) {
	cerr << "usage: " << name << " <hackt-obj-file>" << endl;
}

//=============================================================================

}	// end namespace HAC

