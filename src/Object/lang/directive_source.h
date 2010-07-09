/**
	\file "Object/lang/SPEC.h"
	Common base class for spec-like directives, including PRS macros.  
	$Id: directive_source.h,v 1.6 2010/07/09 00:03:36 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_DIRECTIVE_SOURCE_H__
#define	__HAC_OBJECT_LANG_DIRECTIVE_SOURCE_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/lang/SPEC_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
#include "util/persistent_fwd.h"

namespace HAC {
namespace entity {
class param_expr;
class expr_dump_context;
using std::ostream;
using std::istream;
using std::string;
using std::vector;
using util::good_bool;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Template-independent stuff.
 */
class directive_source_common {
public:
	typedef	directive_source_params_type		params_type;
	typedef	directive_base_params_type		unrolled_params_type;
protected:
	string						name;
	params_type					params;
public:
	directive_source_common();

	explicit
	directive_source_common(const string&);

	~directive_source_common();

	params_type&
	get_params(void) { return params; }

	const params_type&
	get_params(void) const { return params; }

	static
	size_t
	unroll_params(const params_type&, const unroll_context&, 
		unrolled_params_type&);

	size_t
	unroll_params(const unroll_context&, unrolled_params_type&) const;

	static
	ostream&
	dump_params(const params_type&, ostream&, const expr_dump_context&);

	static
	ostream&
	dump_params_bare(const params_type&, ostream&,
		const expr_dump_context&);

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// directive_source_common

//-----------------------------------------------------------------------------
/**
	This is a directive that is pre-unrolled, what appears in the source.
 */
template <class L>
class directive_source : public directive_source_common {
public:
#if 1
	typedef	L					literal_type;
	typedef	vector<literal_type>			source_group_type;
	typedef	vector<source_group_type>		source_nodes_type;
	typedef	source_nodes_type			nodes_type;
#else
	typedef	bool_literal				literal_type;
	typedef	directive_source_nodes_type		nodes_type;
#endif
	// resolved IDs
	typedef	directive_base_nodes_type		unrolled_nodes_type;
	struct dumper;
	struct unroller;
protected:
	nodes_type					nodes;
public:
	directive_source();

	explicit
	directive_source(const string&);

	~directive_source();

	nodes_type&
	get_nodes(void) { return nodes; }

	const nodes_type&
	get_nodes(void) const { return nodes; }

	static
	size_t
	unroll_nodes(const nodes_type&, const unroll_context&, 
		unrolled_nodes_type&);

	size_t
	unroll_nodes(const unroll_context&, unrolled_nodes_type&) const;

	ostream&
	dump_nodes(ostream&, const PRS::rule_dump_context&) const;

	static
	ostream&
	dump_group(const typename nodes_type::value_type&, ostream&,
		const PRS::rule_dump_context&);

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class directive

//=============================================================================
typedef	directive_source<bool_literal>			bool_directive_source;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_DIRECTIVE_SOURCE_H__

