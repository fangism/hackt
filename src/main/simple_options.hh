/**
	\file "main/simple_options.hh"
	Re-usable template function for parsing -f style command options
	only.  This has rather limited use.  
	$Id: simple_options.hh,v 1.3 2006/07/30 16:04:42 fang Exp $
 */

#ifndef	__HAC_MAIN_SIMPLE_OPTIONS_H__
#define	__HAC_MAIN_SIMPLE_OPTIONS_H__

namespace HAC {

/**
	OBSOLETE: phased out in favor of util::getopt_mapped.  
	\param Opt the options type.  
	\param ModMap the options modifoer map type.  
 */
template <class Opt, class ModMap>
int
parse_simple_command_options(const int, const char*[], Opt&, const ModMap&);

}	// end namespace HAC

#endif	// __HAC_MAIN_SIMPLE_OPTIONS_H__

