/**
	\file "Object/common/dump_flags.h"
	Dump attributes class.  
	$Id: dump_flags.h,v 1.5 2006/01/22 18:19:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_DUMP_FLAGS_H__
#define	__HAC_OBJECT_COMMON_DUMP_FLAGS_H__

namespace HAC {
namespace entity {

/**
	Common dump flags used to tweak output formatting and style.  
 */
struct dump_flags {
	/**
		Whether or not to print the scope to which a particuluar
		hierarchical instance belongs.  
		Default value: true.
		Will want to be false when piecing together hierarchical
		canonical names, see global_entry::dump_canonical_name().  
		See use in instance_collection_base::dump_hierarchical_name.
	 */
	bool	show_owner;

	dump_flags() : show_owner(true) { }
private:
	explicit
	dump_flags(const bool _show_owner);
public:
	~dump_flags() { }

	/**
		Static const global default values.  
		(Shouldn't be used before the end of global initialization.)
		None of these should be referenced until main begins.  
	 */
	static const dump_flags			default_value;
	/**
	 */
	static const dump_flags			no_owner;
};	// end class dump_flags

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_DUMP_FLAGS_H__

