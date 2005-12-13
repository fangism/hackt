/**
	\file "main/create.h"
	Interface header for create module.  
	$Id: create.h,v 1.3 2005/12/13 04:15:46 fang Exp $
 */

#ifndef	__MAIN_CREATE_H__
#define	__MAIN_CREATE_H__

#include "main/hackt_fwd.h"

namespace HAC {

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

}	// end namespace HAC

#endif	// __MAIN_CREATE_H__

