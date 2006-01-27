/**
	\file "main/hackt.h"
	Header for the interface to the main program.  
	$Id: hackt.h,v 1.3 2006/01/27 08:07:21 fang Exp $
 */

#ifndef	__HAC_MAIN_HACKT_H__
#define	__HAC_MAIN_HACKT_H__

#include <iosfwd>
#include "main/hackt_fwd.h"
#include "main/program_registry.h"
#include <string>
#include "util/macros.h"
#include "util/qmap.h"
#include "util/memory/count_ptr.h"

namespace HAC {
using std::string;
using std::ostream;
using util::memory::count_ptr;
using util::qmap;

//=============================================================================
/**
	Global program options.  
	TODO: fill me in.
 */
struct global_options {
};

//-----------------------------------------------------------------------------
/**
	Instanceless class for the main program dispatcher.  
	Move this to "hackt.h"?
 */
class hackt {
public:
	static const char		name[];
	static const char		version[];
	static const char		bugreport[];
	static const char		brief_str[];
public:
	typedef	global_options		options;
	typedef	program_entry::program_registry_type	registry_type;

	static
	int
	main(const int, char*[], const options&);

private:
	static
	int
	parse_command_options(const int, char*[], options&);

	static
	void
	usage(void);

	static
	const size_t
	program_id;

};	// end class hackt

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_HACKT_H__


