/**
	\file "Object/lang/PRS_footprint_macro.h"
	$Id: PRS_footprint_macro.h,v 1.1.2.1 2006/01/23 06:17:56 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "util/macros.h"
#include "Object/lang/cflat_visitee.h"

namespace HAC {
namespace entity {
namespace PRS {
using std::istream;
using std::ostream;
using std::string;
//=============================================================================
/**
	A named macro taking node indices as arguments.  
 */
class footprint_macro : public cflat_visitee {
public:
	/**
		Valid entries are non-zero.
	 */
	typedef	std::vector<size_t>	node_args_type;
	typedef	node_args_type::const_iterator	const_iterator;
	typedef	node_args_type::const_reference	const_reference;

// too lazy to privatize for now...
public:
	string				name;
	node_args_type			node_args;

public:
	footprint_macro() { }

	explicit
	footprint_macro(const string& s) : name(s), node_args() { }

	// default destructor

	const_iterator
	begin(void) const { return node_args.begin(); }

	const_iterator
	end(void) const { return node_args.end(); }

	/// \return 1-indexed offset of the first error (if any), else 0
	size_t
	first_error(void) const;

	void
	push_back(const_reference n) { node_args.push_back(n); }

	void
	write_object_base(ostream&) const;

	void
	load_object_base(istream&);

	void
	accept(cflat_visitor&) const;

};	// end struct footprint_macro

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__

