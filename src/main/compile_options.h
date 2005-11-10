/**
	\file "main/compile_options.h"
	Common compile options class.  
	$Id: compile_options.h,v 1.1.2.1 2005/11/10 00:47:45 fang Exp $
 */

#include <list>
#include <string>

namespace ART {
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
	bool					dump_module;
	bool					dump_include_paths;
	/**
		Q: should include paths be a part of global options?
	 */
	include_paths_type			include_paths;

	compile_options() : dump_module(false),
		dump_include_paths(false), 
		include_paths()
		{ }

	// NOTE: this is defined in "main/compile.cc"
	void
	export_include_paths(lexer::file_manager&) const;

};	// end class compile_options

//=============================================================================
}	// end namespace ART

