/**
	\file "util/STL/queue_fwd.hh"
	Forward declarations for default queue types.  
	Also includes a default reverse priority_queue.  
	$Id: queue_fwd.hh,v 1.2 2006/04/18 18:42:45 fang Exp $
 */

#ifndef	__UTIL_STL_QUEUE_FWD_HH__
#define	__UTIL_STL_QUEUE_FWD_HH__

#include "util/STL/deque_fwd.hh"
#include "util/STL/vector_fwd.hh"

BEGIN_NAMESPACE_STD

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

	template <class T2>
	struct rebind : public default_queue<T2> { };
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

	template <class T2>
	struct rebind : public default_priority_queue<T2> { };
};	// end struct default_priority_queue

END_NAMESPACE_STD

#endif	// __UTIL_STL_QUEUE_FWD_HH__

