/**
	\file "main/hackt.cc"
	Implementation of core hackt program, contains main().
	$Id: hackt.cc,v 1.5 2007/04/15 05:52:26 fang Exp $
 */

#include <iostream>
#include "config.h"
#include "main/hackt.hh"
#include "main/program_registry.hh"
#include "main/force_load.hh"
#include "util/getopt_portable.h"
#include "util/qmap.tcc"

namespace HAC {
#include "util/using_ostream.hh"

//=============================================================================
const char
hackt::name[] = "hackt";
// should match PACKAGE_NAME from "config.h"

const char
hackt::brief_str[] = "The main hackt program dispatcher";

const char
hackt::version[] = PACKAGE_VERSION;

const char
hackt::bugreport[] = PACKAGE_BUGREPORT;

const size_t
hackt::program_id = register_hackt_program_class<hackt>();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main program dispatcher.  
 */
int
hackt::main(const int argc, char* argv[], const options&) {
	if (argc < 2) {
		usage();
		return 1;
	}
	options opt;
	if (parse_command_options(argc, argv, opt))
		return 1;
        const count_ptr<program_entry::program_registry_type>
		__program_registry(program_entry::get_program_registry());
	NEVER_NULL(__program_registry);
	const registry_type& p_registry(*__program_registry);
	const int index = optind;
	if (index < argc) {
		// cerr << argv[index] << endl;
		const program_entry prog(p_registry[argv[index]]);
		// prog.dump(cerr) << endl;
		if (prog.is_valid()) {
			return prog(argc -index, argv +index, opt);
		} else {
			cerr << "Invalid sub-program: " << argv[index] << endl;
			// usage();
			return 1;
		}
	}
	return 0;
}	// end main

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	getopt side effect:
		sets optind to the index of the next argument to parse.  
	\return 0 if options accepted.
	NOTE: the getopt option string begins with '+' to enforce
		POSIXLY correct termination at the first non-option argument.  
 */
int
hackt::parse_command_options(const int argc, char* argv[], options&) {
	// none yet
	static const char optstring[] = "+";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	default:
		cerr << "Fang, add some options to hackt!" << endl;
		break;
		// abort();
	}	// end switch
	}	// end while
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
hackt::usage(void) {
	cerr << "hackt: Hierarchical Asynchronous Circuit Kompiler Toolkit"
		<< endl;
	cerr << "version: " << version << endl;
	cerr << "usage: hackt [options] command [options] [arguments]" <<
		endl;
	cerr << "available commands: " << endl;
        const count_ptr<program_entry::program_registry_type>
		__program_registry(program_entry::get_program_registry());
	NEVER_NULL(__program_registry);
	const registry_type& p_registry(*__program_registry);
	registry_type::const_iterator i(p_registry.begin());
	const registry_type::const_iterator e(p_registry.end());
	for ( ; i!=e; i++) {
	if (i->second.is_valid()) {
		cerr << '\t' << i->first;
		cerr << ": " <<
			i->second.get_brief() << endl;
	}
	}
	cerr << "Run \'hackt <command>\' to get usage for each command."
		<< endl;
	cerr << "Report bugs to: " << bugreport << endl;
}

}	// end namespace HAC

//=============================================================================
/**
	THE top-level main program.
	A wrapped call to the real main that does the real work.  
 */
int
main(int argc, char* argv[]) {
	HAC::force_load();	// to force compiler to link non-lazily
	return HAC::hackt::main(argc, argv, HAC::hackt::options());
}	// end main

//=============================================================================

