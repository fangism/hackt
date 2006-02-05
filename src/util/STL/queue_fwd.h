/**
	\file "util/STL/queue_fwd.h"
	Forward declarations for default queue types.  
	Also includes a default reverse priority_queue.  
	$Id: queue_fwd.h,v 1.1 2006/02/05 19:45:09 fang Exp $
 */

#ifndef	__UTIL_STL_QUEUE_FWD_H__
#define	__UTIL_STL_QUEUE_FWD_H__

#include "util/STL/deque_fwd.h"
#include "util/STL/vector_fwd.h"

namespace std {

template <typename>
struct less;

template <typename>
struct greater;

// forward declaration of queue class template
template <typename, typename>
class queue;

/**
	The default type, as defined by the standard library.
 */
template <typename _Tp>
struct default_queue {
	typedef	typename default_deque<_Tp>::type	default_sequence_type;
	typedef	queue<_Tp, default_sequence_type>	type;
};	// end struct default_queue

template <typename, typename, typename>
class priority_queue;

// For reference, this is how it is declared in STL:
// template <typename _Tp, typename _Sequence = vector<_Tp>,
//	typename _Compare  = less<typename _Sequence::value_type> >
// class priority_queue ...

template <typename _Tp>
struct default_priority_queue {
	typedef	typename default_vector<_Tp>::type	default_sequence_type;
	typedef	typename default_sequence_type::value_type	value_type;
	typedef	priority_queue<_Tp, default_sequence_type, less<value_type> >
							type;
	typedef	priority_queue<_Tp, default_sequence_type, greater<value_type> >
							reverse_type;
};	// end struct default_priority_queue

}	// end namespace std

#endif	// __UTIL_STL_QUEUE_FWD_H__

