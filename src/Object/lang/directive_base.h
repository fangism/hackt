/**
	\file "Object/lang/directive_base.h"
	$Id: directive_base.h,v 1.6 2010/07/14 18:12:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_DIRECTIVE_BASE_H__
#define	__HAC_OBJECT_LANG_DIRECTIVE_BASE_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/lang/SPEC_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/persistent_fwd.h"

namespace HAC {
namespace entity {
class const_param;
template <class> class instance_pool;
using std::istream;
using std::ostream;
using std::string;
using util::memory::count_ptr;
using util::persistent_object_manager;
//=============================================================================
/**
	A common base class, for macros and directives, since they're
	practically identical.  
	This represents directives that are already unrolled and resolved.  
 */
class directive_base {
public:
	typedef	directive_base_params_type		params_type;
// too lazy to privatize for now...
public:
	string				name;
	params_type			params;
public:
	directive_base();

	explicit
	directive_base(const string&);

	~directive_base();

	/// \return 1-indexed offset of the first error (if any), else 0
	size_t
	first_param_error(void) const;

	static
	ostream&
	dump_params_bare(const params_type&, ostream&);

	static
	ostream&
	dump_params(const params_type&, ostream&);

	ostream&
	dump_params(ostream&) const;

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end struct directive_base

//-----------------------------------------------------------------------------
class generic_directive_base : public directive_base {
public:
	/**
		Valid entries are non-zero.
	 */
	typedef	directive_base_nodes_type		nodes_type;
	typedef	SPEC::node_pool_type			node_pool_type;
	nodes_type			nodes;

	generic_directive_base();

	explicit
	generic_directive_base(const string&);

	~generic_directive_base();


	/// \return 1-indexed offset of the first error (if any), else 0
	size_t
	first_node_error(void) const;

	template <class PTag>
	static
	ostream&
	dump_group(const directive_node_group_type&, ostream&, 
		const PTag&);

	template <class PTag>
	ostream&
	dump_groups(ostream&, const PTag&) const;

	static
	ostream&
	dump_node_group(const directive_node_group_type& g, ostream& o, 
		const node_pool_type& p) {
		return dump_group(g, o, p);
	}

	void
	collect_transient_info_base(persistent_object_manager&) const;

protected:
	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	void
	write_object(const persistent_object_manager& m, ostream& o) const {
		write_object_base(m, o);
	}

	void
	load_object(const persistent_object_manager& m, istream& i) {
		load_object_base(m, i);
	}

};	// end class generic_directive_base

//-----------------------------------------------------------------------------
typedef	generic_directive_base		bool_directive_base;
//-----------------------------------------------------------------------------
// coming soon: proc_directive_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_DIRECTIVE_BASE_H__

