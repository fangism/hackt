/**
	\file "main/alloc.h"
	Interface header for alloc module.  
	$Id: alloc.h,v 1.4 2006/01/27 08:07:20 fang Exp $
 */

#ifndef	__HAC_MAIN_ALLOC_H__
#define	__HAC_MAIN_ALLOC_H__

#include "main/hackt_fwd.h"

namespace HAC {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class alloc {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	alloc();

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
};	// end class alloc

}	// end namespace HAC

#endif	// __HAC_MAIN_ALLOC_H__

