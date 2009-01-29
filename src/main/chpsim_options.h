/**
	\file "main/chpsim_options.h"
	Main module for new CHPSIM.
	$Id: chpsim_options.h,v 1.4.6.1 2009/01/29 21:45:45 fang Exp $
 */

#ifndef	__HAC_MAIN_CHPSIM_OPTIONS_H__
#define	__HAC_MAIN_CHPSIM_OPTIONS_H__

#include <string>
#include <list>
#include "sim/chpsim/graph_options.h"
#include "main/compile_options.h"

namespace HAC {
//=============================================================================
/**
	Has a lot in common with chpsim_options (main/prsim.cc)
 */
class chpsim_options {
public:
	/// just print help and exit
	bool				help_only;
	/// interactive vs. batch mode
	bool				interactive;
	/// whether or not to run, or just run some other diagnostics
	bool				run;
	/// show result of graph allocation
	bool				dump_graph_alloc;
	/// check structure
	bool				check_structure;
	/// whether or not to produce dot graph output before running
	bool				dump_dot_struct;
	/// whether or not to print checkpoint dump
	bool				dump_checkpoint;
	bool				autosave;
	bool				autotrace;
	/// list of paths to search for sourced scripts
	typedef	std::list<std::string>	source_paths_type;
	source_paths_type		source_paths;
	/// fine-tuning graph options
	SIM::CHPSIM::graph_options	graph_opts;
	/// whether or not operation on type is requested
	bool				use_type;
	/// whether or not instantiated type should be expanded recursively
	bool				instantiate_type_recursively;
	/// name of type to instantiate
	std::string			complete_type_name;
	std::string			autosave_name;
	std::string			autotrace_name;
	/// compiler-driver flags
	compile_options			comp_opt;

	chpsim_options() : help_only(false), interactive(true), 
		run(true), dump_graph_alloc(false), check_structure(true),
		dump_dot_struct(false), dump_checkpoint(false), 
		autosave(false),
		autotrace(false),
		source_paths(), 
		use_type(false), instantiate_type_recursively(false),
		complete_type_name(), 
		autosave_name("autosave.chpsimckpt"),
		autotrace_name("autotrace.chpsimtrace"),
		comp_opt() { }
};	// end class chpsim_options

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_CHPSIM_OPTIONS_H__

