/**
	\file "Object/ref/simple_meta_indexed_reference_base.h"
	Base implementation class for meta-indexed references in HAC.  
	$Id: simple_meta_indexed_reference_base.h,v 1.3.22.1 2006/10/08 21:52:16 fang Exp $
	This file was "Object/simple_meta_instance_reference_base.h"
		in a previous life.  
	Id: simple_meta_instance_reference_base.h,v 1.9 2006/02/21 04:48:38 fang Exp
	This file was "Object/art_object_inst_ref_base.h"
		in a previous life.  
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__

#include <iosfwd>
#include "Object/devel_switches.h"
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#if REF_COUNT_ARRAY_INDICES
#include "util/memory/count_ptr.h"
#else
#include "util/memory/excl_ptr.h"
#endif
#include "Object/common/util_types.h"
#include "Object/ref/meta_index_list_fwd.h"

namespace HAC {
namespace entity {
class scopespace;
class const_index_list;
struct footprint_frame;
class state_manager;
class definition_base;
class fundamental_type_reference;
class instance_collection_base;
struct expr_dump_context;
class const_range_list;
using std::ostream;
using std::istream;
using util::good_bool;
#if REF_COUNT_ARRAY_INDICES
using util::memory::count_ptr;
#else
using util::memory::excl_ptr;
#endif
using util::persistent_object_manager;

//=============================================================================
/**
	The common base class for meta-indexed references.  
	20060213: this class is now an implementation base for
		any reference that may be indexed, be it instance or value.  
 */
class simple_meta_indexed_reference_base {
private:
	typedef	simple_meta_indexed_reference_base		this_type;

	template <bool>
	struct has_substructure { };
public:
	typedef	meta_index_list_type		index_list_type;
#if REF_COUNT_ARRAY_INDICES
	typedef	count_ptr<const index_list_type>	indices_ptr_type;
	typedef	const indices_ptr_type&			indices_ptr_arg_type;
#else
	typedef	excl_ptr<index_list_type>		indices_ptr_type;
	typedef	indices_ptr_type&			indices_ptr_arg_type;
#endif
protected:
	/**
		The indices (optional) for this particular reference.
		Why modifiable pointer?
		May need method to deep-copy the indices, unless
		this pointer becomes counted.  
		Now, is reference-counted, copy-on-write.  

		This pointer may be null, when no explicit indices are given, 
		not necessarily zero-dimensional (scalar).
		Could be implicit reference to entire collection.  
		TODO: always use dynamic_meta_index_list for simplicity?
	 */
	indices_ptr_type			array_indices;

protected:
	// constructors for children only
	simple_meta_indexed_reference_base();

	explicit
	simple_meta_indexed_reference_base(indices_ptr_arg_type);

public:

virtual	~simple_meta_indexed_reference_base();

virtual	good_bool
	attach_indices(indices_ptr_arg_type) = 0;

	ostream&
	dump_indices(ostream&, const expr_dump_context&) const;

protected:		// for children only
	// persistent object IO helper methods
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

};	// end class simple_meta_indexed_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__

