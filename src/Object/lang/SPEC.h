/**
	\file "Object/lang/SPEC.h"
	$Id: SPEC.h,v 1.5.84.1 2008/01/17 23:01:54 fang Exp $
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
namespace SPEC {

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
	unroll(const unroll_context&, const node_pool_type&, 		\
		SPEC::footprint&) const

virtual	SPEC_UNROLL_DIRECTIVE_PROTO = 0;

virtual	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const = 0;

};	// end class directive_abstract

//=============================================================================
/**
	Directive placeholder.  
 */
class directive : public directive_abstract, public directive_source {
	typedef	directive				this_type;
public:
	directive();

	explicit
	directive(const string&);

	~directive();

	SPEC_UNROLL_DIRECTIVE_PROTO;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class directive

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
	typedef	directives_set::value_type	value_type;
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
private:
	typedef	std::vector<directives_set>	clause_list_type;
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

