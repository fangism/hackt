/**
	\file "main/create.h"
	Interface header for create module.  
	$Id: create.h,v 1.5 2007/09/13 01:14:14 fang Exp $
 */

#ifndef	__HAC_MAIN_CREATE_H__
#define	__HAC_MAIN_CREATE_H__

#include "main/hackt_fwd.h"

namespace HAC {
class compile_options;

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class create {
private:
	typedef	compile_options		options;

public:
	static const char		name[];
	static const char		brief_str[];

	create();

	static
	int
	main(const int, char*[], const global_options&);

	static
	void
	usage(void);

	static
	int
	parse_command_options(const int, char*[], options&);

private:
	static
	const size_t
	program_id;
};	// end class create

}	// end namespace HAC

#endif	// __HAC_MAIN_CREATE_H__

