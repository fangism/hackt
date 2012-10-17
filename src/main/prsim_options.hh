/**
	\file "main/prsim_options.hh"
	PRSIM options class.
	$Id: $
 */

#ifndef	__hAC_MAIN_PRSIM_OPTIONS_H__
#define	__hAC_MAIN_PRSIM_OPTIONS_H__

#include "main/compile_options.hh"
#include "sim/prsim/ExprAllocFlags.hh"

namespace HAC {
using std::string;
using std::list;
using SIM::PRSIM::ExprAllocFlags;

class prsim_options {
public:
	// none
	/// just print help and exit
	bool			help_only;
	/// just print command help and exit
	bool			command_help_only;
	/// interactive (default true) vs. batch mode
	bool			interactive;
	/// whether or not to show result of expression allocation
	bool			dump_expr_alloc;
	/// whether or not to run the simulation or just terminate after setup
	bool			run;
	/// check structure
	bool			check_structure;
	/// whether or not to produce a dot-format structure dump before running
	bool			dump_dot_struct;
	/// whether or not checkpoint dump is requested
	bool			dump_checkpoint;
	/// whether or not to automatically save
	bool			autosave;
	/// whether or not to startup tracing all events
	bool			autotrace;
	/**
		Copied from cflat_options.
		Ignore top-level instances and flatten one anonymous
		instance of a named complete process type.  
		Flag to do 'cast2lvs'-like behavior.  
		Tip: protect argument with "quotes" in command-line
			to protect shell-characters.  
	 */
	bool			use_referenced_type_instead_of_top_level;
	/**
		The string of the complete process type to process
		in lieu of the top-level instance hierarchy.  
	 */
	string			named_process_type;
	string			autosave_name;
	string			autotrace_name;
	ExprAllocFlags		expr_alloc_flags;
	/// compiler-driver flags
	compile_options		comp_opt;

	typedef	std::list<string>	source_paths_type;
	/// include search paths for sources
	source_paths_type	source_paths;

	prsim_options() : 
		help_only(false), command_help_only(false),
		interactive(true), 
		dump_expr_alloc(false), run(true),
		check_structure(true), dump_dot_struct(false), 
		dump_checkpoint(false),
		autosave(false),
		autotrace(false),
		use_referenced_type_instead_of_top_level(false),
		named_process_type(),
		autosave_name("autosave.prsimckpt"),
		autotrace_name("autotrace.prsimtrace"),
		expr_alloc_flags(), 
		comp_opt(),
		source_paths() { }

};	// end class prsim_options

}	// end namespace HAC

#endif	// __hAC_MAIN_PRSIM_OPTIONS_H__
