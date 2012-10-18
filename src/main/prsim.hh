/**
	\file "main/prsim.hh"
	Interface header for prsim module.  
	$Id: prsim.hh,v 1.5 2010/08/30 23:51:49 fang Exp $
 */

#ifndef	__HAC_MAIN_PRSIM_H__
#define	__HAC_MAIN_PRSIM_H__

#include "main/hackt_fwd.hh"
#include "main/options_modifier.hh"

namespace HAC {
class prsim_options;

//=============================================================================
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class prsim : protected options_modifier_policy<prsim_options> {
private:
	typedef	options_modifier_policy<prsim_options>
					options_modifier_policy_type;
public:
	typedef	prsim_options		options;
	typedef	options_modifier_policy_type::register_options_modifier_base
					register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	prsim();

	static
	int
	main(const int, char*[], const global_options&);

public:
	static
	void
	usage(void);

	// so vpi-prsim.cc may use it
	static
	int
	parse_command_options(const int, char* const [], options&);

private:
	static
	const size_t
	program_id;

	static const register_options_modifier
		_default, _run, _no_run, 
		_dump_expr_alloc, _no_dump_expr_alloc,
		_check_structure, _no_check_structure,
		_dump_dot_struct, _no_dump_dot_struct,
		_fold_literals, _no_fold_literals,
		_denormalize_negations, _no_denormalize_negations,
		_fast_weak_keepers, _no_weak_keepers,
		_precharge_invariants, _no_precharge_invariants,
		_dynamic_ground_supply, _no_dynamic_ground_supply, 
		_dynamic_power_supply, _no_dynamic_power_supply;

};	// end class prsim

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_PRSIM_H__

