/**
	\file "sim/type_scope.hh"
 */

#ifndef	__HAC_SIM_TYPE_SCOPE_HH__
#define	__HAC_SIM_TYPE_SCOPE_HH__

#include <stack>
#include "util/macros.h"

namespace HAC {
namespace entity {
class footprint;
}
namespace SIM {

typedef	std::stack<const entity::footprint*>	type_stack_type;

/**
	Maintains type-scope context for CLI.
 */
class type_scope_manager : public type_stack_type {
public:

	using type_stack_type::push;
	using type_stack_type::pop;
	using type_stack_type::top;
	using type_stack_type::empty;

	bool
	in_local_type(void) const { return !empty(); }

	const entity::footprint&
	current_type(void) const {
		INVARIANT(!empty());
		return *top();
	}

};      // end struct type_scope_manager

}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_TYPE_SCOPE_HH__
