/**
	\file "Object/global_entry.h"
	$Id: global_entry.h,v 1.1.2.1 2005/09/06 05:56:45 fang Exp $
 */

#ifndef	__OBJECT_GLOBAL_ENTRY_H__
#define	__OBJECT_GLOBAL_ENTRY_H__

#include <vector>
// #include <valarray>		// may be more efficient
#include "Object/traits/class_traits.h"

namespace ART {
namespace entity {

class footprint;
typedef	std::vector<size_t>		footprint_frame_map_type;

//=============================================================================
typedef	enum {
	NONE = 0,
	PROCESS = 1,
	CHANNEL = 2, 
	STRUCT = 3
}	parent_tag_enum;

//=============================================================================
/**
	Yes, I know nothing depends on the template parameter.  
	Rationale becomes clear in the use in class footprint_frame.  
	See the constructor that takes a footprint argument.  
 */
template <class Tag>
struct footprint_frame_map {
	footprint_frame_map_type			id_map;
	footprint_frame_map();

	explicit
	footprint_frame_map(const footprint&);

	~footprint_frame_map();
};

//=============================================================================
/**
	Translates local to global offset.  
	Contains a map from local to global IDs.
	Mapping is (injective?) may be many to few, because of 
	external aliases.  

	This is constructed when global aliases are passed down to
	the subinstances.  Each map is as large as the corresponding
	placeholder pool in the reference footprint.  
 */
struct footprint_frame :
	public footprint_frame_map<process_tag>, 
	public footprint_frame_map<channel_tag>, 
	public footprint_frame_map<datastruct_tag>, 
	public footprint_frame_map<enum_tag>, 
	public footprint_frame_map<int_tag>, 
	public footprint_frame_map<bool_tag> {
	typedef	footprint_frame_map<process_tag>	process_map_type;
	typedef	footprint_frame_map<channel_tag>	channel_map_type;
	typedef	footprint_frame_map<datastruct_tag>	struct_map_type;
	typedef	footprint_frame_map<enum_tag>		enum_map_type;
	typedef	footprint_frame_map<int_tag>		int_map_type;
	typedef	footprint_frame_map<bool_tag>		bool_map_type;
	// or use never_ptr<const footprint>
	const footprint*			_footprint;

	footprint_frame();

	explicit
	footprint_frame(const footprint&);

	~footprint_frame();

	template <class Tag>
	footprint_frame_map_type&
	get_frame_map(void);

	template <class Tag>
	const footprint_frame_map_type&
	get_frame_map(void) const;

private:
	void
	get_frame_map_test(void) const;

};	// end struct footprint_frame

//=============================================================================
/**
	For meta types without substructure.
	Intentionally empty.  
	\param B whether or not the meta type has substructure.  
 */
template <bool B>
struct global_entry_base {
};	// end struct global_entry_base

//-----------------------------------------------------------------------------
/**
	Specialization for types with substructure.  
 */
template <>
struct global_entry_base<true> {
	footprint_frame			_frame;
};	// end struct global_entry_base

//=============================================================================
/**
 */
template <class Tag>
struct global_entry :
	public global_entry_base<class_traits<Tag>::has_substructure> {
	typedef	global_entry_base<class_traits<Tag>::has_substructure>
						parent_type;
	/**
		Uses parent_tag_enum.
	 */
	char		parent_tag_value;
	/**
		Used as a global index to parent structure.  
	 */
	size_t		parent_id;
	/**
		The placeholder id in the corresponding parent type.  
		This used to lookup the parent's footprint_frame.
		The position in the frame is used to identify
		the canonical entry in the footprint.  
		Invariant: the id obtained from looking up the 
		parent's footprint frame corresponds to the 
		global ID of this entry.  
		Because of aliasing, multiple entries in the footprint
		frame may point to the same global entry.  
	 */
	size_t		local_offset;

public:
	global_entry();
	~global_entry();

};	// end struct global_entry

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_GLOBAL_ENTRY_H__

