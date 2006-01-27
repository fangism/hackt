/**
	\file "main/hackt_fwd.h"
	Forward declarations for hackt program interface.  
	$Id: hackt_fwd.h,v 1.4 2006/01/27 08:07:21 fang Exp $
 */

#ifndef	__HAC_MAIN_HACKT_FWD_H__
#define	__HAC_MAIN_HACKT_FWD_H__

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

#endif	// __HAC_MAIN_HACKT_FWD_H__

