/**
	\file "main/cflat.h"
	Interface header for cflat module.  
	$Id: cflat.h,v 1.2.6.1 2005/11/02 06:18:08 fang Exp $
 */

#ifndef	__MAIN_CFLAT_H__
#define	__MAIN_CFLAT_H__

#include <iosfwd>
#include "main/hackt_fwd.h"
#include "util/string_fwd.h"
#include "util/qmap_fwd.h"

namespace ART {
class cflat_options;
using std::string;
using std::ostream;
//=============================================================================
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class cflat {
public:
	/// defined in "main/cflat_options.h"
	typedef	cflat_options			options;
	typedef	void (*options_modifier)(options&);
private:
	/**
		Use string instead of char* for alphabetization.  
	 */
	typedef	util::qmap<std::string, options_modifier>
						options_modifier_map_type;
	static const options_modifier_map_type	options_modifier_map;
public:
	class register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	cflat();

	static
	int
	main(const int, char*[], const global_options&);

	static
	void
	print_alias(ostream&, const string&, const string&, const options&);

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

	static const register_options_modifier	_default;
	static const register_options_modifier	_prsim;
	static const register_options_modifier	_lvs;
	static const register_options_modifier	_ergen;
	static const register_options_modifier	_prlint;
	static const register_options_modifier	_prs2tau;
	static const register_options_modifier	_connect;
	static const register_options_modifier	_wire;
	static const register_options_modifier	_aspice;
	static const register_options_modifier	_env;
	static const register_options_modifier	_check;
	static const register_options_modifier	_csim;
	static const register_options_modifier	_LVS;
	static const register_options_modifier	_Aspice;
	static const register_options_modifier	_ADspice;
	static const register_options_modifier	_alint;
};	// end class cflat

//=============================================================================
}	// end namespace ART

#endif	// __MAIN_CFLAT_H__

