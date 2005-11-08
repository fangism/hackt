/**
	\file "lexer/yyin_manager.cc"
	$Id: yyin_manager.cc,v 1.1.2.1 2005/11/08 05:09:44 fang Exp $
 */


#include "lexer/yyin_manager.h"
#include "lexer/file_manager.h"

#define	ENABLE_STACKTRACE			0
#include "util/stacktrace.h"

namespace ART {
namespace lexer {
//=============================================================================
// class yyin_manager method definitions

/**
	\param fn file name, may be null to represent stdin.
	\param b whether or not to perform path searching.  
		Generally the top-most level need not be searched
		in the include paths.  
 */
yyin_manager::yyin_manager(FILE*& _y, file_manager& fm, const char* fn, 
		const bool b) : _yyin(_y), _file_manager(fm), _error(false) {
	_error = enter_file(_y, fm, fn, b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restore the yyin from previous file.  
 */
yyin_manager::~yyin_manager() {
if (_error) {
	// do nothing, leave _yyin alone.  
} else {
	// close the current file and restore the last stream
	leave_file(_yyin, _file_manager);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if there is an error.  
 */
bool
yyin_manager::enter_file(FILE*& _yyin, file_manager& _file_manager,
		const char* fn, const bool b) {
	STACKTRACE_VERBOSE;
if (b) {
	// search paths
	NEVER_NULL(fn);
	file_position* fp = _file_manager.open_FILE(fn);
	if (fp && fp->file) {
		_yyin = fp->file;
		return false;
	} else {
		// leave _yyin untouched
		return true;
	}
} else {
	// don't search include paths, just open the file
	// we already checked the file
	FILE* f = (fn ? fopen(fn, "r") : stdin);
	NEVER_NULL(f);
	_yyin = f;
	file_position* fp = _file_manager.open_FILE(fn, f);
	NEVER_NULL(fp);
	INVARIANT(f == fp->file);
	return false;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return value that yywrap would normally return:
		1 if last file, else 0 to continue.  
 */
void
yyin_manager::leave_file(FILE*& _yyin, file_manager& _file_manager) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(_yyin);
	fclose(_yyin);
	file_position* fp = _file_manager.close_FILE();
	NEVER_NULL(fp);
	_yyin = fp->file;
}

//=============================================================================
}	// end namespace lexer
}	// end namespace ART

