// "conditional.h"
// functor compositions for conditional or predicated execution

#ifndef	__CONDITIONAL_H__
#define	__CONDITIONAL_H__

namespace std {

/**
	Predicates the execution of each iteration of a for-loop with
	a conditional whose argument type is the same as the function's.  
 */
template <class In, class Pred, class Op>
Op
for_each_if (In first, In last, Pred p, Op f) {
	while (first != last) {
		if (p(*first))
			f(*first);
		first++;
	}
	return f;
};	// end for_each_if

}	// end namespace std

#endif	//	__CONDITIONAL_H__

