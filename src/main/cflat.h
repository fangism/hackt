/**
	\file "main/cflat.h"
	Interface header for cflat module.  
	$Id: cflat.h,v 1.7 2006/01/27 08:07:20 fang Exp $
 */

#ifndef	__HAC_MAIN_CFLAT_H__
#define	__HAC_MAIN_CFLAT_H__

#include <iosfwd>
#include "main/hackt_fwd.h"
#include "main/options_modifier.h"

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
	// derive from options_modifier_policy::register_options_modifier_base
	class register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	cflat();

	static
	int
	main(const int, char*[], const global_options&);

	static
	void
	print_alias(ostream&, const string&, const string&, const options&);

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
		_dsim_prs, _no_dsim_prs;

};	// end class cflat

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_CFLAT_H__

