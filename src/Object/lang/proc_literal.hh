/**
	\file "Object/lang/proc_literal.hh"
	Reusable boolean literal wrapper class.  
	$Id: proc_literal.hh,v 1.2 2010/08/24 21:05:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PROC_LITERAL_H__
#define	__HAC_OBJECT_LANG_PROC_LITERAL_H__

#include <iosfwd>
#include <vector>		// only need fwd decl.
#include "Object/ref/references_fwd.hh"
#include "util/memory/count_ptr.hh"
#include "util/persistent_fwd.hh"
#include "util/size_t.h"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
using std::ostream;
using std::istream;
using util::memory::count_ptr;
using util::persistent_object_manager;
using util::good_bool;
struct process_tag;
class unroll_context;
struct expr_dump_context;

namespace PRS {
	// apologies for confusion...
	struct expr_dump_context;
}

typedef	count_ptr<const simple_process_meta_instance_reference>	
						proc_literal_base_ptr_type;

//=============================================================================
/**
	Cloned from bool_literal.
	Base class with minimal functionality for process literal reference.  
 */
class proc_literal {
public:
	typedef	std::vector<size_t>			group_type;
	typedef	process_tag				tag_type;
protected:
	/**
		The underlying reference to bool, must be scalar for 
		production rule literals, but may be aggregate for
		macros and spec directives.
	 */
	proc_literal_base_ptr_type			var;
public:
	proc_literal();

	proc_literal(const proc_literal_base_ptr_type&);

	// The following are un-inlined to not have to require complete type
	// on simple_node_meta_instance_reference:

	// copy-ctor
	proc_literal(const proc_literal&);

	proc_literal&
	operator = (const proc_literal&);

	bool
	operator == (const proc_literal&) const;

	bool
	valid(void) const { return var; }

	~proc_literal();

	const proc_literal_base_ptr_type&
	get_proc_var(void) const { return var; }

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&, const PRS::expr_dump_context&) const;

	proc_literal
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
		operator () (const proc_literal& b) const {
			return b.unroll_base(_context);
		}
	};	// end struct unroller

};	// end struct proc_literal

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PROC_LITERAL_H__

