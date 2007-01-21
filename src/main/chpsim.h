/**
	\file "main/chpsim.h"
	Interface header for the chpsim module.  
	$Id: chpsim.h,v 1.2 2007/01/21 06:00:05 fang Exp $
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
	static const register_options_modifier
		_default, _run, _no_run, 
		_dump_graph_alloc, _no_dump_graph_alloc,
		_check_structure, _no_check_structure,
		_dump_dot_struct, _no_dump_dot_struct,
		_show_event_index, _no_show_event_index, 
		_show_instances, _no_show_instances,
		_antidependencies, _no_antidependencies,
		_process_clusters, _no_process_clusters;

};	// end class chpsim

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_CHPSIM_H__

