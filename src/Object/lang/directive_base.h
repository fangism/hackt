/**
	\file "Object/lang/directive_base.h"
	$Id: directive_base.h,v 1.1.2.1 2006/02/09 07:06:53 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_DIRECTIVE_BASE_H__
#define	__HAC_OBJECT_LANG_DIRECTIVE_BASE_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "util/size_t.h"
#include "util/memory/count_ptr.h"
#include "util/persistent_fwd.h"

namespace HAC {
namespace entity {
class const_param;
using std::istream;
using std::ostream;
using std::string;
using util::memory::count_ptr;
using util::persistent_object_manager;
//=============================================================================
/**
	A common base class, for macros and directives, since they're
	practically identical.  
 */
class directive_base {
public:
	/**
		Valid entries are non-zero.
	 */
	typedef	std::vector<size_t>				nodes_type;
	typedef	std::vector<count_ptr<const const_param> >	params_type;

// too lazy to privatize for now...
public:
	string				name;
	params_type			params;
	nodes_type			nodes;
public:
	directive_base();

	explicit
	directive_base(const string& s);

	~directive_base();

	/// \return 1-indexed offset of the first error (if any), else 0
	size_t
	first_param_error(void) const;

	/// \return 1-indexed offset of the first error (if any), else 0
	size_t
	first_node_error(void) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	void
	write_object(const persistent_object_manager& m, ostream& o) const {
		write_object_base(m, o);
	}

	void
	load_object(const persistent_object_manager& m, istream& i) {
		load_object_base(m, i);
	}

};	// end struct footprint_macro

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_DIRECTIVE_BASE_H__

