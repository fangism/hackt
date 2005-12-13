/**
	\file "main/hackt_fwd.h"
	Forward declarations for hackt program interface.  
	$Id: hackt_fwd.h,v 1.3 2005/12/13 04:15:47 fang Exp $
 */

#ifndef	__MAIN_HACKT_FWD_H__
#define	__MAIN_HACKT_FWD_H__

#include "util/size_t.h"

namespace HAC {

struct global_options;
class program_entry;

/**
	Prototype for main programs.  
	May add environment pointer later...
	NOTE: want to make argument const, but getopt spoils it.  
 */
typedef int (main_program_type)(const int, char*[],
		const global_options&);

typedef	main_program_type*	main_program_ptr_type;

}	// end namespace HAC

#endif	// __MAIN_HACKT_FWD_H__

