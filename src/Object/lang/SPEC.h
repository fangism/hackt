/**
	\file "Object/lang/SPEC.h"
	$Id: SPEC.h,v 1.12 2010/07/14 18:12:33 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_H__
#define	__HAC_OBJECT_LANG_SPEC_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/lang/directive_source.h"
#include "Object/lang/bool_literal.h"
#include "Object/unroll/meta_loop_base.h"
#include "Object/unroll/meta_conditional_base.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
namespace PRS {
class prs_expr;
}
namespace SPEC {
using PRS::prs_expr;

//=============================================================================
/**
	Base class for spec directives.  
 */
class directive_abstract : public util::persistent {
public:
	struct dumper;
	struct unroller;
/**
	Defined as a macro just in case we need to re-use it...
 */
#define	SPEC_UNROLL_DIRECTIVE_PROTO					\
	good_bool							\
	unroll(const unroll_context&) const

virtual	SPEC_UNROLL_DIRECTIVE_PROTO = 0;

virtual	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const = 0;

};	// end class directive_abstract

//=============================================================================
/**
	Directive placeholder.  
 */
class bool_directive : public directive_abstract, public bool_directive_source {
	typedef	bool_directive				this_type;
public:
	bool_directive();

	explicit
	bool_directive(const string&);

	~bool_directive();

	SPEC_UNROLL_DIRECTIVE_PROTO;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	PERSISTENT_METHODS_DECLARATIONS
	// is pooling really necessary for these?
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class directive

//=============================================================================
// cloned from bool_directive
class proc_directive : public directive_abstract, public proc_directive_source {
	typedef	proc_directive				this_type;
public:
	proc_directive();

	explicit
	proc_directive(const string&);

	~proc_directive();

	SPEC_UNROLL_DIRECTIVE_PROTO;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class directive

//=============================================================================
/**
	Invariant expression, always-assert!
 */
class invariant : public directive_abstract {
	typedef	invariant				this_type;
	const count_ptr<const prs_expr>			invar_expr;
public:
	invariant();

	explicit
	invariant(const count_ptr<const prs_expr>&);

	~invariant();

	SPEC_UNROLL_DIRECTIVE_PROTO;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	PERSISTENT_METHODS_DECLARATIONS

};	// end class invariant

//=============================================================================
typedef	std::vector<count_ptr<const directive_abstract> >
						directives_set_base;

/**
	A set of spec directives.  
	Analogous to PRS::rule_set.  
 */
class directives_set : private directives_set_base {
	typedef	directives_set_base			parent_type;
public:
	typedef	parent_type::const_reference		const_reference;
	typedef	parent_type::reference			reference;
	typedef	parent_type::value_type			value_type;
public:
	directives_set();
	~directives_set();

	using parent_type::push_back;
	using parent_type::pop_back;
	using parent_type::empty;
	using parent_type::back;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	SPEC_UNROLL_DIRECTIVE_PROTO;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class directives_set

//=============================================================================
/**
	Unroll-time expanded loops.  
 */
class directives_loop : public directive_abstract,
		public directives_set, private meta_loop_base {
	typedef	directives_loop			this_type;
	typedef directives_set			implementation_type;
	typedef	directives_set::value_type	value_type;
	friend struct meta_loop<this_type>;
	typedef	meta_loop<this_type>		meta_loop_type;
public:
	directives_loop();
	directives_loop(const ind_var_ptr_type&, const range_ptr_type&);
	~directives_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	SPEC_UNROLL_DIRECTIVE_PROTO;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class directives_loop

//=============================================================================
/**
	Guarded spec directives construct.  
 */
class directives_conditional :
		public directive_abstract, private meta_conditional_base {
	typedef	directives_conditional		this_type;
public:
	typedef	std::vector<directives_set>	clause_list_type;
	typedef	clause_list_type::const_iterator	clause_iterator;
private:
	friend struct meta_conditional<this_type>;
	typedef	meta_conditional<this_type>	meta_conditional_type;
	clause_list_type			clauses;
public:
	directives_conditional();
	~directives_conditional();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	bool
	empty(void) const;

	void
	append_guarded_clause(const guard_ptr_type&);

	directives_set&
	get_last_clause(void) { return clauses.back(); }

	SPEC_UNROLL_DIRECTIVE_PROTO;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class directives_conditional

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_H__

