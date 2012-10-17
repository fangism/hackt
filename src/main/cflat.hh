/**
	\file "main/cflat.hh"
	Interface header for cflat module.  
	$Id: cflat.hh,v 1.20 2010/05/13 00:32:02 fang Exp $
 */

#ifndef	__HAC_MAIN_CFLAT_H__
#define	__HAC_MAIN_CFLAT_H__

#include <iosfwd>
#include "main/hackt_fwd.hh"

// define to 1 to use optparse over old options_modifier
// this has been tested, ready to perm.
#define HFLAT_USE_OPTPARSE                            1

#if HFLAT_USE_OPTPARSE
#include "util/optparse.hh"
#else
#include "main/options_modifier.hh"
#endif

namespace util {
template <class> class getopt_map;
}

namespace HAC {
struct cflat_options;
using std::string;
using std::ostream;

//=============================================================================
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class cflat
#if !HFLAT_USE_OPTPARSE
		: protected options_modifier_policy<cflat_options>
#endif
{
#if !HFLAT_USE_OPTPARSE
	typedef	options_modifier_policy<cflat_options>
						options_modifier_policy_type;
#endif
public:
	/// defined in "main/cflat_options.hh"
	typedef	cflat_options			options;

#if !HFLAT_USE_OPTPARSE
	// derive from options_modifier_policy::register_options_modifier_base
#endif
	class register_options_modifier;
private:
	/// the top-level options map
	typedef	util::getopt_map<options>	master_options_map_type;

	static
	master_options_map_type			master_options;

	static
	int
	initialize_master_options_map(void);

	static
	const int
	master_options_initialized;
public:
	static const char		name[];
	static const char		brief_str[];


	cflat();

	static
	int
	main(const int, char*[], const global_options&);

	static
	void
	usage(void);

private:
	static
	int
	parse_command_options(const int, char*[], options&);

	static
	const size_t
	program_id;

	static
	void
	getopt_f_options(options&, const char*);

	static
	void
	getopt_cflat_type_only(options&, const char*);

};	// end class cflat

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_CFLAT_H__

