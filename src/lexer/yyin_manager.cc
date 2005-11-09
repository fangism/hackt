/**
	\file "lexer/yyin_manager.cc"
	$Id: yyin_manager.cc,v 1.1.2.3 2005/11/09 08:24:00 fang Exp $
 */

#include <iostream>
#include "lexer/yyin_manager.h"
#include "lexer/file_manager.h"

#define	ENABLE_STACKTRACE			0
#include "util/stacktrace.h"

namespace ART {
namespace lexer {
#include "util/using_ostream.h"
//=============================================================================
// class yyin_manager method definitions

/**
	\param fn file name, may be null to represent stdin.
	\param b whether or not to perform path searching.  
		Generally the top-most level need not be searched
		in the include paths.  
 */
yyin_manager::yyin_manager(FILE*& _y, file_manager& fm, const char* fn, 
		const bool b) : _yyin(_y), _file_manager(fm) {
	_status = enter_file(_y, fm, fn, NULL, b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restore the yyin from previous file.  
 */
yyin_manager::~yyin_manager() {
if (_status == SUCCESS) {
	// close the current file and restore the last stream
	leave_file(_yyin, _file_manager);
} else {
	// do nothing, leave _yyin alone.  
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is an error.  
 */
yyin_manager::status
yyin_manager::enter_file(FILE*& _yyin, file_manager& _file_manager,
		const char* fn, ostream* o, const bool b) {
	STACKTRACE_VERBOSE;
if (b) {
	// search paths
	NEVER_NULL(fn);
	const file_manager::return_type fp(_file_manager.open_FILE(fn));
	if (fp.second) {
		return IGNORE;
	} else if (fp.first && fp.first->file) {
		if (o) {
			*o << "Open: " << fn << endl;
		}
		_yyin = fp.first->file;
		return SUCCESS;
	} else {
		// leave _yyin untouched
		return ERROR;
	}
} else {
	// don't search include paths, just open the file
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
	INVARIANT(!fp.second);
	// this is the first file, can't already be opened
	INVARIANT(f == fp.first->file);
	return SUCCESS;
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
}	// end namespace ART

