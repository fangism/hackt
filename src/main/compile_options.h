/**
	\file "main/compile_options.h"
	Common compile options class.  
	$Id: compile_options.h,v 1.7.40.1 2007/03/11 05:13:55 fang Exp $
 */

#include <list>
#include <string>

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
	/// whether or not to emit make dependencies
	bool					make_depend;
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

	compile_options() : dump_module(false),
		dump_include_paths(false), 
		dump_object_header(false),
		make_depend(false),
		make_depend_target(),
		source_file(), 
		target_object(), 
		include_paths()
		{ }

	// NOTE: this is defined in "main/main_funcs.cc"
	void
	export_include_paths(lexer::file_manager&) const;

};	// end class compile_options

//=============================================================================
}	// end namespace HAC

