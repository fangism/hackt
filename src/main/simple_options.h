/**
	\file "main/simple_options.h"
	Re-usable template function for parsing -f style command options
	only.  This has rather limited use.  
	$Id: simple_options.h,v 1.1.2.1 2006/01/04 08:42:11 fang Exp $
 */

#ifndef	__HAC_MAIN_SIMPLE_OPTIONS_H__
#define	__HAC_MAIN_SIMPLE_OPTIONS_H__

namespace HAC {

/**
	\param Opt the options type.  
	\param ModMap the options modifoer map type.  
 */
template <class Opt, class ModMap>
int
parse_simple_command_options(const int, const char*[], Opt&, const ModMap&);

}	// end namespace HAC

#endif	// __HAC_MAIN_SIMPLE_OPTIONS_H__

