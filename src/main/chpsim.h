/**
	\file "main/chpsim.h"
	Interface header for the chpsim module.  
	$Id: chpsim.h,v 1.1 2006/01/27 08:07:20 fang Exp $
 */

#ifndef	__HAC_MAIN_CHPSIM_H__
#define	__HAC_MAIN_CHPSIM_H__

#include "main/hackt_fwd.h"

namespace HAC {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class chpsim {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	chpsim();

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
};	// end class chpsim

}	// end namespace HAC

#endif	// __HAC_MAIN_CHPSIM_H__

