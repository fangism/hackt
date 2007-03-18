/**
	\file "main/chpsim_options.h"
	Main module for new CHPSIM.
	$Id: chpsim_options.h,v 1.1 2007/03/18 00:25:02 fang Exp $
 */

#ifndef	__HAC_MAIN_CHPSIM_OPTIONS_H__
#define	__HAC_MAIN_CHPSIM_OPTIONS_H__

#include <string>
#include <list>
// #include "main/chpsim.h"
#include "sim/chpsim/graph_options.h"

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
	/// list of paths to search for sourced scripts
	typedef	std::list<std::string>	source_paths_type;
	source_paths_type		source_paths;
	/// fine-tuning graph options
	SIM::CHPSIM::graph_options	graph_opts;

	chpsim_options() : help_only(false), interactive(true), 
		run(true), dump_graph_alloc(false), check_structure(true),
		dump_dot_struct(false), dump_checkpoint(false), 
		source_paths() { }
};	// end class chpsim_options

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_CHPSIM_OPTIONS_H__

