/**
	\file "main/shell.h"
	Interface header for shell module.  
	$Id: shell.h,v 1.4 2005/12/13 04:15:48 fang Exp $
 */

#ifndef	__MAIN_SHELL_H__
#define	__MAIN_SHELL_H__

#include <iosfwd>
#include "main/hackt_fwd.h"

namespace HAC {
using std::ostream;

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class shell {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];
	static const char		prompt[];

	shell();

	static
	int
	main(const int, char*[], const global_options&);

private:
	static
	void
	usage(void);

	static
	int
	parse_command_options(const int, char*[], options&);

	static
	const size_t
	program_id;


	static
	ostream&
	banner(ostream&);

	static
	ostream&
	farewell(ostream&);

};	// end class shell

}	// end namespace HAC

#endif	// __MAIN_SHELL_H__

