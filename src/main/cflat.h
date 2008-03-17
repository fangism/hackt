/**
	\file "main/cflat.h"
	Interface header for cflat module.  
	$Id: cflat.h,v 1.13 2008/03/17 23:02:40 fang Exp $
 */

#ifndef	__HAC_MAIN_CFLAT_H__
#define	__HAC_MAIN_CFLAT_H__

#include <iosfwd>
#include "main/hackt_fwd.h"
#include "main/options_modifier.h"

namespace util {
template <class> class getopt_map;
}

namespace HAC {
class cflat_options;
using std::string;
using std::ostream;

//=============================================================================
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class cflat : protected options_modifier_policy<cflat_options> {
	typedef	options_modifier_policy<cflat_options>
						options_modifier_policy_type;
public:
	/// defined in "main/cflat_options.h"
	typedef	cflat_options			options;
private:
	/// the top-level options map
	typedef	util::getopt_map<options>	master_options_map_type;
	// derive from options_modifier_policy::register_options_modifier_base
	class register_options_modifier;

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

private:
	static
	void
	usage(void);

	static
	int
	parse_command_options(const int, char*[], options&);

	static
	const size_t
	program_id;

	// no reason why these have to be static members, 
	// other than convenience
	// complete presets, from original cflat (CAST)
	static const register_options_modifier
		_default, _prsim, _lvs, _ergen, _prlint, _prs2tau,
		_connect, _wire, _aspice, _env, _check, _csim, _LVS,
		_Aspice, _ADspice, _alint;

	// individual flag modifiers
	static const register_options_modifier
		_connect_none, _no_connect, _connect_equal,
		_connect_connect, _connect_wire,
		_include_prs, _exclude_prs, _no_include_prs, _no_exclude_prs,
		_self_aliases, _no_self_aliases,
		_quote_names, _no_quote_names,
		_check_mode, _no_check_mode,
		_wire_mode, _no_wire_mode,
		_dsim_prs, _no_dsim_prs,
		_size_prs, _no_size_prs, 
		_strengths, _no_strengths, 
		_SEU, _no_SEU;

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

