/**
	\file "main/cflat.cc"
	cflat backwards compability module.  

	$Id: cflat.cc,v 1.1.2.3 2005/10/06 04:41:29 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

// #include "util/static_trace.h"

#include <iostream>
#include <string>

#include "main/cflat.h"
#include "main/cflat_options.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"
#include "util/qmap.tcc"

namespace ART {
using std::string;
using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.h"

//=============================================================================
// class cflat static initializers

const char
cflat::name[] = "cflat";

const char
cflat::brief_str[] =
	"Flattens instantiations CAST-style (backward compatibility)";

const size_t
cflat::program_id = register_hackt_program_class<cflat>();

const cflat::options_modifier_map_type
cflat::options_modifier_map;

//=============================================================================
/**
	CFLAT mode registration mechanism.  
 */
class cflat::register_options_modifier {
public:
	register_options_modifier(const string& Mode,
			const cflat::options_modifier COM) {
		options_modifier_map_type&
			omm(const_cast<options_modifier_map_type&>(
				options_modifier_map));
		NEVER_NULL(COM);
		INVARIANT(!omm[Mode]);
		omm[Mode] = COM;
	}
};	// end class register_options_modifier

//=============================================================================
// cflat::options_modifier declarations and definitions

/// Does absolutely nothing.  
static
void
__cflat_default(cflat::options&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_prsim(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_EQUAL;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const cflat::register_options_modifier
	cflat::_prsim("prsim", &__cflat_prsim);

const cflat::register_options_modifier
	cflat::_default("default", &__cflat_default);

//=============================================================================
cflat::cflat() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	argv[1] is the cflat mode.
	argv[2] is the name of the input object file.
	Q: any need to parse command line options? later...
 */
int
cflat::main(const int argc, char* argv[], const global_options&) {
	options opt();
	cflat_options cf;
	// what if there are options? Durrrrr....
	// TODO: repeated calls to getopt
	if (argc < 3) {
		usage();
		return 1;
	}
	// cflat mode
	const options_modifier om(options_modifier_map[argv[1]]);
	if (!om) {
		cerr << "Invalid mode: " << argv[1] << endl;
		usage();
		return 1;
	} else {
		(*om)(cf);
	}

	// additional mode modifiers

	if (!check_object_loadable(argv[2]).good)
		return 1;

	excl_ptr<module> the_module = load_module(argv[2]);
	if (!the_module)
		return 1;

	if (!the_module->allocate_unique().good) {
		cerr << "ERROR in allocating global state.  Aborting." << endl;
		return 1;
	}

	// cflat here!!!
	// based on mode, set the options to pass into the module.
	if (!the_module->cflat(cout, cf).good) {
		cerr << "Error during cflat." << endl;
		return 1;
	}

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat::usage(void) {
	cerr << "Usage: " << name << " <mode> [options] <hackt-obj-infile>"
		<< endl;
	// list modes
	const size_t modes = options_modifier_map.size();
if (modes) {
	cerr << "Modes (" << modes << "):" << endl;
	typedef	options_modifier_map_type::const_iterator	const_iterator;
	const_iterator i(options_modifier_map.begin());
	const const_iterator e(options_modifier_map.end());
	for ( ; i!=e; i++) {
		cerr << '\t' << i->first << endl;
	}
}
	// additional options summary
}

//=============================================================================
}	// end namespace ART

