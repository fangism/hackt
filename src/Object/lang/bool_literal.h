/**
	\file "Object/lang/bool_literal.h"
	Reusable boolean literal wrapper class.  
	$Id: bool_literal.h,v 1.5 2007/10/08 01:21:25 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_BOOL_LITERAL_H__
#define	__HAC_OBJECT_LANG_BOOL_LITERAL_H__

#include <iosfwd>
#include "Object/ref/references_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/persistent_fwd.h"
#include "util/size_t.h"
#include "util/boolean_types.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace entity {
using std::ostream;
using std::istream;
using util::memory::count_ptr;
using util::persistent_object_manager;
using util::good_bool;
class unroll_context;
struct expr_dump_context;

namespace PRS {
	// apologies for confusion...
	struct expr_dump_context;
	class literal;
}

typedef	count_ptr<const simple_bool_meta_instance_reference>	
						bool_literal_base_ptr_type;

typedef	count_ptr<const simple_node_meta_instance_reference>
						node_literal_ptr_type;

//=============================================================================
/**
	Base class with minimal functionality for boolean literal reference.  
 */
struct bool_literal {
public:
	typedef	std::default_vector<size_t>::type	group_type;
protected:
	/**
		The underlying reference to bool, must be scalar for 
		production rule literals, but may be aggregate for
		macros and spec directives.
	 */
	bool_literal_base_ptr_type			var;
	/**
		This pointer is mutually exclusive with the 
		var bool-ref member.
		Really, they could go in a tagged union.  
	 */
	node_literal_ptr_type			int_node;
	/**
		Only applicable to int_node.
		If true, internal node references the pull-down
		variant expression, else references the pull-up.
	 */
	bool					negated;
public:
	bool_literal();

	// implicit
	bool_literal(const count_ptr<const PRS::literal>&);
	bool_literal(const count_ptr<PRS::literal>&);

	explicit
	bool_literal(const bool_literal_base_ptr_type&);

	explicit
	bool_literal(const node_literal_ptr_type&);

	// The following are un-inlined to not have to require complete type
	// on simple_node_meta_instance_reference:

	// copy-ctor
	bool_literal(const bool_literal&);

	bool_literal&
	operator = (const bool_literal&);

	bool
	operator == (const bool_literal&) const;

	bool
	valid(void) const { return var || int_node; }

	~bool_literal();

	const bool_literal_base_ptr_type&
	get_bool_var(void) const { return var; }

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&, const PRS::expr_dump_context&) const;

	bool
	is_internal(void) const { return int_node; }

	const node_literal_ptr_type&
	internal_node(void) const { return int_node; }

	void
	negate_node(void) { negated = true; }

	void
	unnegate_node(void) { negated = false; }

	void
	toggle_negate_node(void) { negated = !negated; }

	bool
	is_negated(void) const { return negated; }

	node_literal_ptr_type
	unroll_node_reference(const unroll_context&) const;

	bool_literal
	unroll_reference(const unroll_context&) const;

	size_t
	unroll_base(const unroll_context&) const;

	good_bool
	unroll_group(const unroll_context&, group_type&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	/// alias wrapper
	void
	write_object(const persistent_object_manager& m, ostream& o) const {
		write_object_base(m, o);
	}

	/// alias wrapper
	void
	load_object(const persistent_object_manager& m, istream& i) {
		load_object_base(m, i);
	}

	/**
		Bound helper functor.  
	 */
	struct unroller {
		const unroll_context&		_context;

		explicit
		unroller(const unroll_context& c) : _context(c) { }

		size_t
		operator () (const bool_literal& b) const {
			return b.unroll_base(_context);
		}
	};	// end struct unroller

};	// end struct bool_literal

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_BOOL_LITERAL_H__

