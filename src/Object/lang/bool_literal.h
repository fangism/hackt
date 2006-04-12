/**
	\file "Object/lang/bool_literal.h"
	Reusable boolean literal wrapper class.  
	$Id: bool_literal.h,v 1.4 2006/04/12 08:53:16 fang Exp $
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

// const?
typedef	count_ptr<simple_bool_meta_instance_reference>	
						bool_literal_base_ptr_type;

//=============================================================================
/**
	Base class with minimal functionality for boolean literal reference.  
 */
struct bool_literal {
public:
	typedef	std::default_vector<size_t>::type	group_type;
protected:
	bool_literal_base_ptr_type				var;

public:
	bool_literal();

	// implicit
	bool_literal(const count_ptr<const PRS::literal>&);
	bool_literal(const count_ptr<PRS::literal>&);

	explicit
	bool_literal(const bool_literal_base_ptr_type&);

	~bool_literal();

	const bool_literal_base_ptr_type&
	get_bool_var(void) const { return var; }

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&, const PRS::expr_dump_context&) const;

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

