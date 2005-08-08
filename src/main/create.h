/**
	\file "main/create.h"
	Interface header for create module.  
	$Id: create.h,v 1.2 2005/08/08 16:51:12 fang Exp $
 */

#ifndef	__MAIN_CREATE_H__
#define	__MAIN_CREATE_H__

#include "main/hackt_fwd.h"

namespace ART {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class create {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	create();

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
};	// end class create

}	// end namespace ART

#endif	// __MAIN_CREATE_H__

