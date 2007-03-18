/**
	\file "main/chpsim-guile.h"
	Interface header for chpsim-guile module.  
	$Id: chpsim-guile.h,v 1.1 2007/03/18 00:25:00 fang Exp $
 */

#ifndef	__HAC_MAIN_CHPSIM_GUILE_H__
#define	__HAC_MAIN_CHPSIM_GUILE_H__

#include "main/hackt_fwd.h"
#if 0
#include "main/options_modifier.h"
#endif

namespace HAC {
class chpsim_options;

//=============================================================================
/**
	Instance-less class.  
	NOTE: unlike other module dispatch classes, we do not intend
	to make this a dispatchable subprogram.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
	This will try to leech off of class chpsim as much as possible.
 */
class chpsim_guile 
#if 0
	: protected options_modifier_policy<guile_options>
#endif
{
	typedef	chpsim_options		options;
#if 0
private:
	typedef	options_modifier_policy<guile_options>
					options_modifier_policy_type;
public:
	typedef	options_modifier_policy_type::register_options_modifier_base
					register_options_modifier;
#endif
public:
#if 0
	static const char		name[];
	static const char		brief_str[];
#endif

	chpsim_guile();

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

#if 0
	static
	const size_t
	program_id;
#endif

	static
	void
	main_interactive(void*, int, char**);

	static
	void
	main_script(void*, int, char**);

#if 0
	static const register_options_modifier
		_default;
#endif
};	// end class guile

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_CHPSIM_GUILE_H__

