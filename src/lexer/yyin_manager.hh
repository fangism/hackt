/**
	\file "lexer/yyin_manager.hh"
	Interface for manipulating the hackt-lexer file stack.  
	TODO: consider templating taking a globally visible pointer
		as a bound template argument.  
	$Id: yyin_manager.hh,v 1.4 2005/12/13 04:15:45 fang Exp $
 */

#ifndef	__LEXER_YYIN_MANAGER_H__
#define	__LEXER_YYIN_MANAGER_H__

#include <iosfwd>
#include "util/attributes.h"
#include "util/file_status.hh"

namespace HAC {
namespace lexer {
class file_manager;
using util::file_status;
//=============================================================================
/**
	Use this instead of manipulating hackt_in directly.  
	This synchronizes the file_manager with hackt_in.   
	A call to the lexer must be in the scope of one of these objects.  
 */
class yyin_manager {
public:
	/// reusing a convenient enumerated type
	typedef	file_status::status	status;
private:
	FILE*				_yyin;
	file_manager&			_file_manager;
	status				_status;
public:
	yyin_manager(file_manager&, const char*, const bool = true);
	~yyin_manager();

	status
	get_status(void) const { return _status; }

	FILE*
	get_file(void) const { return _yyin; }

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
}	// end namespace HAC

#endif	// __LEXER_YYIN_MANAGER_H__

