/**
	\file "main/guile.h"
	Interface header for guile module.  
	$Id: guile.h,v 1.1 2007/03/11 21:16:50 fang Exp $
 */

#ifndef	__HAC_MAIN_GUILE_H__
#define	__HAC_MAIN_GUILE_H__

#include "main/hackt_fwd.h"
#include "main/options_modifier.h"

namespace HAC {
class guile_options;

//=============================================================================
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class guile : protected options_modifier_policy<guile_options> {
private:
	typedef	options_modifier_policy<guile_options>
					options_modifier_policy_type;
public:
	typedef	guile_options		options;
	typedef	options_modifier_policy_type::register_options_modifier_base
					register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	guile();

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

#endif	// __HAC_MAIN_GUILE_H__

