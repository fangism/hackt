/**
	\file "main/program_registry.h"
	Header for the interface to the main program.  
	$Id: program_registry.h,v 1.2 2005/08/08 16:51:13 fang Exp $
 */

#ifndef	__MAIN_PROGRAM_REGISTRY_H__
#define	__MAIN_PROGRAM_REGISTRY_H__

#include <iosfwd>
#include "main/hackt_fwd.h"
#include <string>
#include "util/macros.h"
#include "util/qmap.h"
#include "util/memory/count_ptr.h"

namespace ART {
using std::string;
using std::ostream;
using util::memory::count_ptr;
using util::qmap;

//=============================================================================
/**
	The structure of a sub-program.  
	TODO: add a hidden flag for undocumented internal functions.  
 */
class program_entry {
	typedef	program_entry			this_type;
public:
	typedef	main_program_type		main_type;
private:
	/**
		Pointer to the main program.  
	 */
	main_type*				main_ptr;
	/**
		One-line description of the program.  
	 */
	string					brief;

public:
	ostream&
	dump(ostream&) const;

	program_entry() : main_ptr(NULL), brief() { }

	program_entry(main_type* const m, const char* b) :
		main_ptr(m), brief(b) {
			NEVER_NULL(m);
	}

	// need to define copy-constructor
	program_entry(const program_entry& p) :
		main_ptr(p.main_ptr), 
		brief(p.brief) {
	}

	~program_entry() { }

	bool
	is_valid(void) const { return main_ptr; }

	const string&
	get_brief(void) const { return brief; }

	this_type&
	operator = (const this_type& t) {
		main_ptr = t.main_ptr;
		NEVER_NULL(main_ptr);
		brief = t.brief;
		return *this;
	}

	int
	operator () (const int argc, char* argv[],
			const global_options& gopt) const;

	/**
		The type of the global program registry.  
		Just a standard map.
		Lesson: never use map of const char*, 
		it compares the POINTERs, not the strings themselves, 
			whereas hash_map performs a proper 
			hash on const char*.  
	 */
	typedef	qmap<string, program_entry>	program_registry_type;

	typedef	count_ptr<program_registry_type>
						program_registry_ptr_type;

	static
	util::memory::raw_count_ptr<program_registry_type>
	get_program_registry(void);

};	// end struct program entry

//=============================================================================
#if 0
/**
	NOTE: every translation unit that includes this header will
	automatically acquire a counted pointer to the global registry.  
	This ensures proper lifetime of the registry w.r.t.
	global static initialization and destruction.  
 */
static
const count_ptr<program_entry::program_registry_type>
__program_registry(program_entry::get_program_registry());
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this to register a program.  
	\param name the name of the program entry, usually the name
		of the program itself.
	\param main pointer to the main program.  
	\param brief one-line string description.  
	\return a unique integer for initialization consumption.  
	Uses private (local) static global __program_registry.
	TODO: check for entry conflicts.
	An example of usage (in a translation unit):
	static int foo = register_hackt_program("help", &help::main, "helps");
 */
extern
size_t
register_hackt_program(const char* name, 
		program_entry::main_type* const main, 
		const char* brief);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenient template wrapper for program class registration.
	Recommend using this over register_hackt_program_directly.  
 */
template <class Prog>
inline
size_t
register_hackt_program_class(void) {
	return register_hackt_program(Prog::name, Prog::main, Prog::brief_str);
}

//=============================================================================
}	// end namespace ART

#endif	// __MAIN_PROGRAM_REGISTRY_H__

