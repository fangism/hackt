/**
	\file "main/prsim.h"
	Interface header for prsim module.  
	$Id: prsim.h,v 1.1.2.3 2006/01/11 03:41:14 fang Exp $
 */

#ifndef	__MAIN_PRSIM_H__
#define	__MAIN_PRSIM_H__

#include "main/hackt_fwd.h"
#include "main/options_modifier.h"

namespace HAC {
class prsim_options;

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class prsim : protected options_modifier_policy<prsim_options> {
private:
	typedef	options_modifier_policy<prsim_options>
					options_modifier_policy;
public:
	typedef	prsim_options		options;
	typedef	options_modifier_policy::register_options_modifier_base
					register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	prsim();

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

	static const register_options_modifier
		_default, _run, _no_run, 
		_dump_expr_alloc, _no_dump_expr_alloc,
		_check_structure, _no_check_structure,
		_dump_dot_struct, _no_dump_dot_struct;

};	// end class prsim

}	// end namespace HAC

#endif	// __MAIN_PRSIM_H__

