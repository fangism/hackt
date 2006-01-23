/**
	\file "main/compile_options.h"
	Common compile options class.  
	$Id: compile_options.h,v 1.3.8.1 2006/01/23 06:17:58 fang Exp $
 */

#include <list>
#include <string>

namespace HAC {
#include "util/using_ostream.h"
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
	/**
		Q: should include paths be a part of global options?
	 */
	include_paths_type			include_paths;

	compile_options() : dump_module(false),
		dump_include_paths(false), 
		dump_object_header(false),
		include_paths()
		{ }

	// NOTE: this is defined in "main/compile.cc"
	void
	export_include_paths(lexer::file_manager&) const;

};	// end class compile_options

//=============================================================================
}	// end namespace HAC

