/**
	\file "Object/lang/SPEC.h"
	Common base class for spec-like directives, including PRS macros.  
	$Id: directive_source.h,v 1.5 2009/09/14 21:17:05 fang Exp $
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
using util::good_bool;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	This is a directive that is pre-unrolled, what appears in the source.
 */
class directive_source {
public:
	typedef	directive_source_nodes_type		nodes_type;
	typedef	directive_source_params_type		params_type;
	typedef	directive_base_nodes_type		unrolled_nodes_type;
	typedef	directive_base_params_type		unrolled_params_type;
	struct dumper;
	struct unroller;
protected:
	string						name;
	params_type					params;
	nodes_type					nodes;
public:
	directive_source();

	explicit
	directive_source(const string&);

	~directive_source();

	params_type&
	get_params(void) { return params; }

	const params_type&
	get_params(void) const { return params; }

	nodes_type&
	get_nodes(void) { return nodes; }

	const nodes_type&
	get_nodes(void) const { return nodes; }

	static
	size_t
	unroll_params(const params_type&, const unroll_context&, 
		unrolled_params_type&);

	static
	size_t
	unroll_nodes(const nodes_type&, const unroll_context&, 
		unrolled_nodes_type&);

	size_t
	unroll_params(const unroll_context&, unrolled_params_type&) const;

	size_t
	unroll_nodes(const unroll_context&, unrolled_nodes_type&) const;

	static
	ostream&
	dump_params_bare(const params_type&, ostream&,
		const expr_dump_context&);

	static
	ostream&
	dump_params(const params_type&, ostream&, const expr_dump_context&);

	ostream&
	dump_nodes(ostream&, const PRS::rule_dump_context&) const;

	static
	ostream&
	dump_group(const nodes_type::value_type&, ostream&,
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
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_DIRECTIVE_SOURCE_H__

