// "conditional.h"
// functor compositions for conditional or predicated execution

#ifndef	__CONDITIONAL_H__
#define	__CONDITIONAL_H__

namespace std {

//=============================================================================
#if 0
template <class In, class Pred, class Op>
// really a nullary, or binary function?
class for_each_if_t {
protected:
	Pred pred;
	Op op;
public:
	for_each_if_t(Pred p, Op o) : pred(p), op(o) { }

	Op
	operator() (In first, In, last) const {
		while (first != last) {
			if (pred(*first))
				op(*first);
			first++;
		}
	}
};	// end class for_each_if_t
#endif

//-----------------------------------------------------------------------------
/**
	Predicates the execution of each iteration of a for-loop with
	a conditional whose argument type is the same as the function's.  
	By default, passing and return Op by reference to avoid unnecessary
	copying and assigning.  
 */
template <class In, class Pred, class Op>
Op&
for_each_if (In first, In last, Pred p, Op& f) {
	while (first != last) {
		if (p(*first))
			f(*first);
		first++;
	}
	return f;
};	// end for_each_if

//-----------------------------------------------------------------------------
#if 0
/**
	Predicates the execution of each iteration of a for-loop with
	a conditional whose argument type is the same as the function's.  
	Can't partially specialize a function_template...
		also becomes ambiguous.
	Specialization to pass and return by reference, 
	much more efficient than copying and assigning.  
 */
template <class In, class Pred, class Op>
Op&
for_each_if<In, Pred, Op&> (In first, In last, Pred p, Op& f) {
	while (first != last) {
		if (p(*first))
			f(*first);
		first++;
	}
	return f;
};	// end for_each_if
#endif

//=============================================================================

}	// end namespace std

#endif	//	__CONDITIONAL_H__

