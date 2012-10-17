/**
	\file "Object/inst/element_key_dumper.hh"
	$Id: element_key_dumper.hh,v 1.2 2006/10/24 07:27:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ELEMENT_KEY_DUMPER_H__
#define	__HAC_OBJECT_INST_ELEMENT_KEY_DUMPER_H__

#include <ostream>

namespace HAC {
namespace entity {
using std::ostream;
//=============================================================================
/**
	Convenient policy for printing indices.
	The multikeys are already printed with []'s
	but the 1D is specially optimized and thus should be
	printed using a specialized method.  
 */
template <size_t D>
struct element_key_dumper {
	template <class K>
	ostream&
	operator () (ostream& o, const K& k) const {
		return o << k;
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	specialization for 1D indices, simplified to a scalar value.  
 */
template <>
struct element_key_dumper<1> {
	template <class K>
	ostream&
	operator () (ostream& o, const K& k) const {
		return o << '[' << k << ']';
	}
};

//=============================================================================

}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_INST_ELEMENT_KEY_DUMPER_H__

