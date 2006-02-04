/**
	\file "Object/lang/bool_literal.h"
	Reusable boolean literal wrapper class.  
	$Id: bool_literal.h,v 1.2 2006/02/04 06:43:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_BOOL_LITERAL_H__
#define	__HAC_OBJECT_LANG_BOOL_LITERAL_H__

#include <iosfwd>
#include "Object/object_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/persistent_fwd.h"
#include "util/size_t.h"

namespace HAC {
namespace entity {
using std::ostream;
using std::istream;
using util::memory::count_ptr;
using util::persistent_object_manager;
class unroll_context;
struct expr_dump_context;

namespace PRS {
	// apologies for confusion...
	struct expr_dump_context;
}

// const?
typedef	count_ptr<simple_bool_meta_instance_reference>	
						bool_literal_base_ptr_type;

//=============================================================================
/**
	Base class with minimal functionality for boolean literal reference.  
 */
struct bool_literal {
protected:
	bool_literal_base_ptr_type				var;

public:
	bool_literal();

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

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end struct bool_literal

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_BOOL_LITERAL_H__

