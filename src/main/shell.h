/**
	\file "main/shell.h"
	Interface header for shell module.  
	$Id: shell.h,v 1.1.2.1 2005/11/03 02:16:37 fang Exp $
 */

#ifndef	__MAIN_SHELL_H__
#define	__MAIN_SHELL_H__

#include "main/hackt_fwd.h"

namespace ART {

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

	shell();

	static
	int
	main(const int, char*[], const global_options&);

private:
	static
	void
	usage(void);

#if 0
	static
	int
	parse_command_options(const int, char*[], options&);
#endif

	static
	const size_t
	program_id;
};	// end class shell

}	// end namespace ART

#endif	// __MAIN_SHELL_H__

