/**
	\file "lexer/yyin_manager.cc"
	$Id: yyin_manager.cc,v 1.7 2010/03/11 18:39:24 fang Exp $
 */

#include <iostream>
#include <cstdio>
#include "lexer/yyin_manager.hh"
#include "lexer/file_manager.hh"

#define	ENABLE_STACKTRACE			0
#include "util/stacktrace.hh"

namespace HAC {
namespace lexer {
#include "util/using_ostream.hh"
//=============================================================================
// class yyin_manager method definitions

/**
	\param fn file name, may be null to represent stdin.
	\param b whether or not to perform path searching.  
		Generally the top-most level need not be searched
		in the include paths.  
 */
yyin_manager::yyin_manager(file_manager& fm, const char* fn, 
		const bool b) : _yyin(NULL), _file_manager(fm) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("b = " << (b ? "true" : "false") << endl);
	_status = enter_file(_yyin, fm, fn, NULL, b);
	if (_status == file_status::NEW_FILE)
		_yyin = fm.current_FILE()->file;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restore the yyin from previous file.  
 */
yyin_manager::~yyin_manager() {
	STACKTRACE_VERBOSE;
if (_status == file_status::NEW_FILE) {
	// close the current file and restore the last stream
	leave_file(_yyin, _file_manager);
} else {
	// do nothing, leave _yyin alone.  
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param b true if search paths are to be used in opening the file.
		The top-level file should be opened without searching.  
	\return the error status of attempting to open the file.  
 */
yyin_manager::status
yyin_manager::enter_file(FILE*& _yyin, file_manager& _file_manager,
		const char* fn, ostream* o, const bool b) {
	STACKTRACE_VERBOSE;
if (b && fn) {
	// search paths
	// is irrelevant if input is stdin (when fn is NULL)
	NEVER_NULL(fn);	// if you meant stdin, then pass b = false
	const file_manager::return_type fp(_file_manager.open_FILE(fn));
	switch (fp.second) {
	case file_status::NEW_FILE:
		INVARIANT(fp.first && fp.first->file);
		if (o) {
			*o << "Open: " << fn << endl;
		}
		_yyin = fp.first->file;
		break;
	case file_status::SEEN_FILE:
		// fall-through
	case file_status::CYCLE:
		// fall-through
	case file_status::NOT_FOUND:
		// ignore and leave _yyin untouched
		break;
	default:
		DIE;
	}
	return fp.second;
} else {
	// don't search include paths, just open the file or stdin
	// we already checked the file
	// often called for the top-most file
	FILE* f = (fn ? fopen(fn, "r") : stdin);
	NEVER_NULL(f);
	_yyin = f;
	const file_manager::return_type fp(_file_manager.open_FILE(fn, f));
	if (o) {
		*o << "Open: " << (fn ? fn : "-stdin-") << endl;
	}
	NEVER_NULL(fp.first);
	INVARIANT(fp.second == file_status::NEW_FILE);
	// this is the first file, can't already be opened
	INVARIANT(f == fp.first->file);
	return fp.second;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param o (option) output stream, passed in for verbose exiting, 
		useful for dumping errors.  
	\return value that yywrap would normally return:
		1 if last file, else 0 to continue.  
 */
void
yyin_manager::leave_file(FILE*& _yyin, file_manager& _file_manager, 
		ostream* o) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(_yyin);
	fclose(_yyin);
	if (o) {
		_file_manager.dump_file_stack_top(*o);
	}
	file_position* fp = _file_manager.close_FILE();
	NEVER_NULL(fp);
	_yyin = fp->file;
}

//=============================================================================
}	// end namespace lexer
}	// end namespace HAC

