/**
	\file "main/compile_options.h"
	Common compile options class.  
	$Id: compile_options.h,v 1.12 2010/08/05 18:25:34 fang Exp $
 */

#ifndef	__HAC_MAIN_COMPILE_OPTIONS_H__
#define	__HAC_MAIN_COMPILE_OPTIONS_H__

#include <list>
#include <string>
#include "AST/parse_options.h"
#include "main/create_options.h"

namespace HAC {
using std::list;
using std::string;

namespace lexer {
class file_manager;
}	// end namespace lexer

//=============================================================================
/**
	Options for compile phase.  
 */
class compile_options {
public:
	typedef	list<string>			include_paths_type;
	/// whether or not to dump the module after it is built and saved
	bool					dump_module;
	/// debugging switch to show include paths
	bool					dump_include_paths;
	/// whether or not the persistent object header is dumped
	bool					dump_object_header;

// The following options should be recorded in the module.
	/// whether or not to emit make dependencies
	bool					make_depend;
	/// true if input is redirected from stdin
	bool					use_stdin;
	/**
		Whether or not input file should be compiled (default no)
		Alternatively, input may already be an object file
		for other compiler tools.  
		This flag is not applicable to the compile program, of course.
	 */
	bool					compile_input;
	/// the name of the make-dependency target
	string					make_depend_target;
	/// the name of the output target
	string					source_file;
	/// the name of the output target
	string					target_object;
	/**
		Q: should include paths be a part of global options?
	 */
	include_paths_type			include_paths;
	/**
		User-tweaked options for type checking.  
	 */
	parser::parse_options			parse_opts;
	/**
		Create-time parameters forwarded to the create phase
		automatically by the compiler driver.
	 */
	create_options				create_opts;

	compile_options() : dump_module(false),
		dump_include_paths(false), 
		dump_object_header(false),
		make_depend(false),
		use_stdin(false),
		compile_input(false),
		make_depend_target(),
		source_file(), 
		target_object(), 
		include_paths(), 
		parse_opts(), 
		create_opts()
		{ }

	// NOTE: this is defined in "main/main_funcs.cc"
	void
	export_include_paths(lexer::file_manager&) const;

	bool
	have_target(void) const { return target_object.size(); }

	ostream&
	dump(ostream&) const;

	void
	write_object(ostream&) const;

	void
	load_object(istream&);

};	// end class compile_options

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_COMPILE_OPTIONS_H__
