// "multikey_qmap.h"

#ifndef	__MULTIKEY_QMAP_H__
#define	__MULTIKEY_QMAP_H__

#include "qmap.h"
#include "multikey_map.h"

namespace MULTIKEY_MAP_NAMESPACE {
using namespace QMAP_NAMESPACE;

template <size_t D, class K, class T>
class multikey_qmap : public multikey_map<D, K, T, qmap> {
private:
	/** this is the representation-type */
	typedef multikey_map<D,K,T,qmap>		map_type;
	typedef map_type				mt;
public:
	typedef typename mt::key_type			key_type;
	typedef typename mt::mapped_type		mapped_type;
	typedef typename mt::value_type			value_type;
	typedef typename mt::key_compare		key_compare;
	typedef typename mt::allocator_type		allocator_type;

	typedef typename mt::reference			reference;
	typedef typename mt::const_reference		const_reference;
	typedef typename mt::iterator			iterator;
	typedef typename mt::const_iterator		const_iterator;
	typedef typename mt::reverse_iterator		reverse_iterator;
	typedef typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef typename mt::size_type			size_type;
	typedef typename mt::difference_type		difference_type;
	typedef typename mt::pointer			pointer;
	typedef typename mt::const_pointer		const_pointer;
	typedef typename mt::allocator_type		allocator_type;

public:
	// everything else plain inherited

};	// end class multikey_qmap

}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_QMAP_H__

