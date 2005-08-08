/**
	\file "main/hackt_fwd.h"
	Forward declarations for hackt program interface.  
	$Id: hackt_fwd.h,v 1.2 2005/08/08 16:51:12 fang Exp $
 */

#ifndef	__MAIN_HACKT_FWD_H__
#define	__MAIN_HACKT_FWD_H__

#include "util/size_t.h"

namespace ART {

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

}	// end namespace ART

#endif	// __MAIN_HACKT_FWD_H__

