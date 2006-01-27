/**
	\file "main/prsobjdemo.h"
	Interface header for prsobjdemo module.  
	$Id: prsobjdemo.h,v 1.4 2006/01/27 08:07:21 fang Exp $
 */

#ifndef	__HAC_MAIN_PRSOBJDEMO_H__
#define	__HAC_MAIN_PRSOBJDEMO_H__

#include "main/hackt_fwd.h"

namespace HAC {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class prsobjdemo {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	prsobjdemo();

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
};	// end class prsobjdemo

}	// end namespace HAC

#endif	// __HAC_MAIN_PRSOBJDEMO_H__

