/**
	\file "main/program_registry.cc"
	Implementation of core hackt program registration interface.  
	$Id: program_registry.cc,v 1.4 2005/12/13 04:15:48 fang Exp $
 */

#include <iostream>
#include "main/program_registry.h"
#include "util/qmap.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/attributes.h"

namespace HAC {
#include "util/using_ostream.h"

//=============================================================================
// class program entry member definitions

/**
	Returns a (counted) pointer to an initialized program registry.  
	Automatically allocates upon first invocation.  
 */
util::memory::raw_count_ptr<program_entry::program_registry_type>
program_entry::get_program_registry(void) {
	static program_registry_type*	reg = new program_registry_type();
	STATIC_RC_POOL_REF_INIT;	// defined in "util/memory/count_ptr.h"
	static size_t*			count = NEW_SIZE_T;
	static const size_t	zero __ATTRIBUTE_UNUSED__ = (*count = 0);
	return util::memory::raw_count_ptr<program_registry_type>(reg, count);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
program_entry::operator () (const int argc, char* argv[], 
		const global_options& gopt) const {
	return (*main_ptr)(argc, argv, gopt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
program_entry::dump(ostream& o) const {
	return o << '@' << reinterpret_cast<void*>(size_t(main_ptr))
		<< ": " << brief;
}

//=============================================================================
size_t
register_hackt_program(const char* name,
		program_entry::main_type* const main,
		const char* brief) {
	static const count_ptr<program_entry::program_registry_type>
		__program_registry(program_entry::get_program_registry());
	NEVER_NULL(__program_registry);
	const program_entry probe((*__program_registry)[name]);
	if (probe.is_valid()) {
		cerr << "FATAL: Program " << name << " is already registered."
			<< endl;
		exit(1);
	}
	(*__program_registry)[name] = program_entry(main, brief);
	return __program_registry->size();
}

//=============================================================================
}	// end namespace HAC

