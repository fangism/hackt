/**
	\file "main/hacknet.hh"
	Interface header for hacknet module.  
	$Id: hacknet.hh,v 1.2 2009/08/28 20:45:07 fang Exp $
 */

#ifndef	__HAC_MAIN_HACKNET_H__
#define	__HAC_MAIN_HACKNET_H__

#include "main/hackt_fwd.hh"
#include "main/options_modifier.hh"

namespace HAC {
class hacknet_options;

//=============================================================================
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class hacknet : protected options_modifier_policy<hacknet_options> {
private:
	typedef	options_modifier_policy<hacknet_options>
					options_modifier_policy_type;
public:
	typedef	hacknet_options		options;
	typedef	options_modifier_policy_type::register_options_modifier_base
					register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	hacknet();

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

#if 0
	static const register_options_modifier
		_default, _run, _no_run;
#endif

};	// end class hacknet

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_HACKNET_H__

