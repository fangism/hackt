/**
	\file "main/unroll.h"
	Interface header for unroll module.  
	$Id: unroll.h,v 1.1 2005/07/25 02:10:09 fang Exp $
 */

#ifndef	__MAIN_UNROLL_H__
#define	__MAIN_UNROLL_H__

#include "main/hackt_fwd.h"

namespace ART {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class unroll {
private:
	class options;

private:
	static const char		name[];
	static const char		brief_str[];

public:
	unroll();

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
};	// end class unroll

}	// end namespace ART

#endif	// __MAIN_UNROLL_H__

