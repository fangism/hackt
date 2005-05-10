/**
	\file "util/conditional.h"
	Functor compositions for conditional or predicated execution.  
	$Id: conditional.h,v 1.7 2005/05/10 04:51:22 fang Exp $
 */

#ifndef	__UTIL_CONDITIONAL_H__
#define	__UTIL_CONDITIONAL_H__

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
/**
	Consider writing may and must binary comparisons, using for-loops.  
**/

//=============================================================================
/**
	Compare, in order, the elements of two sequences that meet 
	certain criteria, predicates.  
	Ranges that do not satisfy predicate will be ignored.  
	The number of predicate-satisfying elements in each sequence
	must also be equal for the comparison to return true.  
	\return true if sequences are equal.
 */
template <class In1, class In2, class Pred1, class Pred2, class Comp>
bool
compare_if(In1 l_first, In1 l_last, In2 r_first, In2 r_last, 
		Pred1 p1, Pred2 p2, Comp comp) {
	while (l_first != l_last && r_first != r_last) {
		while (l_first != l_last && !p1(*l_first))
			l_first++;
		while (r_first != r_last && !p2(*r_first))
			r_first++;
		if (!comp(*l_first, *r_first)) {
			return false;
		}	// else continue comparing
	}
	// if both are at end, then they are equal
	return (l_first == l_last && r_first == r_last);
}

//=============================================================================
}	// end namespace std

#endif	// __UTIL_CONDITIONAL_H__

