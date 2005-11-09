/**
	\file "lexer/yyin_manager.h"
	Interface for manipulating the hackt-lexer file stack.  
	TODO: consider templating taking a globally visible pointer
		as a bound template argument.  
	$Id: yyin_manager.h,v 1.1.2.3 2005/11/09 08:24:01 fang Exp $
 */

#ifndef	__LEXER_YYIN_MANAGER_H__
#define	__LEXER_YYIN_MANAGER_H__

#include <iosfwd>
#include <cstdio>
#include "util/attributes.h"

namespace ART {
namespace lexer {
class file_manager;
//=============================================================================
/**
	Use this instead of manipulating hackt_in directly.  
	This synchronizes the file_manager with hackt_in.   
	A call to the lexer must be in the scope of one of these objects.  
 */
class yyin_manager {
public:
	typedef	enum {
		SUCCESS = 0,	///< file opened and being parsed
		IGNORE = 1,	///< file ignored because already included
		ERROR = 2	///< file failed to open or doesn't exist
	} status;
private:
	FILE*&				_yyin;
	file_manager&			_file_manager;
	status				_status;
public:
	yyin_manager(FILE*&, file_manager&, const char*,
		const bool = true);
	~yyin_manager();

	status
	get_status(void) const { return _status; }

	static
	status
	enter_file(FILE*&, file_manager&, const char*, 
		std::ostream* = NULL, const bool = true);

	static
	void
	leave_file(FILE*&, file_manager&, std::ostream* = NULL);

} __ATTRIBUTE_UNUSED__;	// end class yyin_manager

//=============================================================================
}	// end namespace lexer
}	// end namespace ART

#endif	// __LEXER_YYIN_MANAGER_H__

