/**
	\file "persistent_object_manager_gcc41death.h"
	Clases related to serial, persistent object management.  
	$Id: persistent_object_manager_gcc41death.h,v 1.1.2.2 2005/04/26 00:13:09 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_OBJECT_MANAGER_GCC41DEATH_H__
#define	__UTIL_PERSISTENT_OBJECT_MANAGER_GCC41DEATH_H__

#include "persistent_gcc41death.h"

#include "memory/count_ptr_gcc41death.h"

// #include "IO_utils_fwd.h"		// for read and write to streams
// #include <iosfwd>

namespace util {
using std::ostream;
using std::istream;

template <class T>
void    write_value(ostream& f, const T& v);

template <class T>
void    read_value(istream& f, T& v);

/**
	Generic functor for value-writing.  
	This is partially specializable, 
	whereas the function templates are not.  
 */
template <class>
struct value_writer;

/**
	Generic functor for value-reading.  
	Partially specializable.  
 */
template <class>
struct value_reader;

}	// end namespace util

// instead of including <utility.h>
namespace std {
template <class T1, class T2>
struct pair {
	T1 first;
	T2 second;
};	// end struct pair

}	// end namespace std

//=============================================================================
namespace util {
using std::pair;
using namespace util::memory;

//=============================================================================
/**
	Persistent object manager class for ART objects.  
	Note that the pointers kept in this structure are just raw pointers, 
	and are never owned.  This manager is never responsible for 
	deleting the memory created.  
 */
class persistent_object_manager {
	class visit_info {
	public:
		template <class P>
		void
		mark_visit(const P&);
	};

public:
	static bool				dump_reconstruction_table;

public:
	persistent_object_manager();

	~persistent_object_manager();

private:
	bool
	flag_visit(const persistent* ptr);

public:
	bool
	check_reconstruction_table_range(const size_t) const;


	std::pair<persistent*, visit_info*>
	lookup_ptr_visit_info(const long) const;

	size_t*
	lookup_ref_count(const long i) const;

	size_t*
	lookup_ref_count(const persistent* i) const;

private:

	/// private helper method for reading plain pointers
	template <class P>
	visit_info*
	__read_pointer(istream& f, const P& ptr, 
		const raw_pointer_tag) const;

	/// private helper method for reading non-reference-counted pointers
	template <class P>
	visit_info*
	__read_pointer(istream& f, const P& ptr, 
		const single_owner_pointer_tag) const;

	/// private helper method for reading reference-counted pointers
	template <class P>
	visit_info*
	__read_pointer(istream& f, const P& ptr,
		const shared_owner_pointer_tag) const;

public:
	/**
		ALERT: this intentially and coercively discards const-ness!
		Need to specialize for reference counter pointers!
	 */
	template <class P>
	void
	read_pointer(istream& f, const P& ptr) const;

};	// end class persistent_object_manager

//=============================================================================
}	// end namespace util

#endif	//	__UTIL_PERSISTENT_OBJECT_MANAGER_GCC41DEATH_H__

