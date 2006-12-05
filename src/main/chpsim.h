/**
	\file "main/chpsim.h"
	Interface header for the chpsim module.  
	$Id: chpsim.h,v 1.1.72.1 2006/12/05 01:49:25 fang Exp $
 */

#ifndef	__HAC_MAIN_CHPSIM_H__
#define	__HAC_MAIN_CHPSIM_H__

#include "main/hackt_fwd.h"
#include "main/options_modifier.h"

namespace HAC {
class chpsim_options;

//=============================================================================
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class chpsim : protected options_modifier_policy<chpsim_options> {
private:
	typedef	options_modifier_policy<chpsim_options>
					options_modifier_policy_type;
public:
	typedef	chpsim_options		options;
	typedef	options_modifier_policy_type::register_options_modifier_base
					register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	chpsim();

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

	// declare registered option modifiers

};	// end class chpsim

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_CHPSIM_H__

