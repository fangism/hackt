// "art_object_IO.h"
// header file for everything related to persistent object management

// include "hash_specializations.h" before this file for 
//	hash<> specializations to take effect

#ifndef	__ART_OBJECT_IO_H__
#define	__ART_OBJECT_IO_H__

#include <iosfwd>
#include "art_object_IO_fwd.h"
#include "hash_qmap.h"
#include "sstream.h"			// reduce to forward decl?
#include "ptrs.h"			// need complete definition
#include "count_ptr.h"			// need complete definition
#include "art_utils.h"			// for read and write to streams

namespace ART {
namespace entity {
#include "art_object_type_enum.h"	// in this namespace
}
}

namespace ART {
namespace entity {
//=============================================================================
using namespace std;
using namespace HASH_QMAP_NAMESPACE;
using namespace COUNT_PTR_NAMESPACE;
using namespace PTRS_NAMESPACE;

// forward declaration
class object;
class name_space;

//=============================================================================
/**
	This is the signature for a function that reconstructs
	a persistent object from a binary input stream.  
	Each class for which objects persist, should implement such
	a function as a static function, to be called indirectly
	from a lookup table.  
 */
typedef	object*	reconstruct_function_type(void);

typedef	reconstruct_function_type*
		reconstruct_function_ptr_type;

//=============================================================================
/**
	Persistent object manager class for ART objects.  
	Note that the pointers kept in this structure are just raw pointers, 
	and are never owned.  This manager is never responsible for 
	deleting the memory created.  
 */
class persistent_object_manager {
private:
	/**
		The class contains the information necessary for reconstructing
		persistent objects associated with a pointer.  
	 */
	class reconstruction_table_entry {
	private:
		/** constant default ios mode */
		static const ios_base::openmode	mode;
	private:
		/** object type enumeration */
		type_index_enum		otype;
		/** location of reconstruction, consider object*? */
		const object*		recon_addr;
		/** reference count for counter pointers */
	mutable	size_t*			ref_count;
		/** scratch flag, general purpose flag */
		bool			scratch;
		/** start of stream position */
		streampos		buf_head;
		/** end of stream position */
		streampos		buf_tail;
		/** stream buffer for temporary storage, count_ptr
			for transferrable semantics with copy assignment */
		count_ptr<stringstream>	buffer;
	public:
	// need default constructor to create an invalid object
		reconstruction_table_entry();
		reconstruction_table_entry(const type_index_enum t, 
			const streampos h, const streampos t);
		reconstruction_table_entry(const object* p,
			const type_index_enum t);
		// default copy constructor suffices
		~reconstruction_table_entry();

		type_index_enum	type(void) const { return otype; }
		const object*	addr(void) const { return recon_addr; }
		size_t*		count(void) const;
		void		assign_addr(object* ptr);
		void		reset_addr();
		void		flag(void) { scratch = true; }
		void		unflag(void) { scratch = false; }
		bool		flagged(void) const { return scratch; }
		stringstream&	get_buffer(void) const { return *buffer; }
		void		initialize_offsets(void);
		void		adjust_offsets(const streampos s);
		streampos	head_pos(void) const { return buf_head; }
		streampos	tail_pos(void) const { return buf_tail; }
	};	// end class reconstruction_table_entry

	/**
		Out of paranoia, writing this class to guarantee
		default value of -1.  
	 */
	class Long {
		public:
			long val;
			Long() : val(-1) { }
			Long(const long v) : val(v) { }
			~Long() { }
			operator long () { return val; }
	};	// end class Long

/**
	Type of map from address to table entry index.  
	Every pointer will be mapped to an auxiliary index.  
	Is the reverse map of reconstruction_table_type.
 */
typedef	hash_qmap<const void*, Long>		addr_to_index_map_type;
/**
	Each persistent object in memory corresponds to one entry
	in this table, whose entries contain information on how to 
	fully reconstruct itself.  
	Is the reverse map of addr_to_index_map_type.  
 */
typedef	vector<reconstruction_table_entry>	reconstruction_table_type;

private:
	/** lookup table of reconstruction functions (allocators) */
	static const reconstruct_function_ptr_type
		reconstruction_function_table[MAX_TYPE_INDEX_ENUM];
	/** maps pointer address to entry index */
	addr_to_index_map_type			addr_to_index_map;
	/** entries containing reconstruction information */
	reconstruction_table_type		reconstruction_table;
	/** file position after the header */
	streampos				start_of_objects;
	/** temporary place to keep ownership of root pointer */
	excl_ptr<name_space>			root;

public:
	static bool				dump_reconstruction_table;

public:
	persistent_object_manager();
	~persistent_object_manager();

// for debugging
	ostream& dump_text(ostream& o) const;

// public interface functions to object class hierarchy
	/** pointer registration interface */
	bool register_transient_object(
		const object* ptr, const type_index_enum t);
	bool flag_visit(const object* ptr);
	ostream& lookup_write_buffer(const object* ptr) const;
	istream& lookup_read_buffer(const object* ptr) const;

	long lookup_ptr_index(const object* ptr) const;
	object*	lookup_obj_ptr(const long i) const;
	size_t*	lookup_ref_count(const long i) const;

	// the following template methods are defined in "art_object_IO.tcc"
	/**
		Doesn't actually write out the pointer, but the index 
		representing the object represented by the pointer.
		Precondition: pointer must already be registered.
		\param f output (file) stream.
		\param ptr the pointer (class) object to translate and 
			write out.
	 */
	template <template <class> class P, class T>
	void write_pointer(ostream& f, const P<T>& ptr) const;

	/**
		ALERT: this intentially and coercively discards const-ness!
		Need to specialize for reference counter pointers!
	 */
	template <template <class> class P, class T>
	void read_pointer(istream& f, const P<T>& ptr) const;

	/**
		Partial specialization of read_pointer for 
		reference-counted pointers.
	 */
	template <class T>
	void read_pointer(istream& f, const count_ptr<T>& ptr) const;
	template <class T>
	void read_pointer(istream& f, const count_const_ptr<T>& ptr) const;

	/**
		Writes a sequence of pointers, mapped to indices.
		Container only needs a simple forward iterator interface.  
	 */
	template <template <class> class L, template <class> class P, class T >
	void write_pointer_list(ostream& f, const L<P<T> >& l) const;

	/**
		Reconstructs a sequence of pointers, mapped to indices.  
		Container only needs a simple forward iterator interface.  
	 */
	template <template <class> class L, template <class> class P, class T >
	void read_pointer_list(istream& f, L<P<T> >& l) const;

	/**
		Writes a map of pointers in some order, ignoring the keys.
	 */
	template <template <class, class> class M, class K, 
		template <class> class P, class T >
	void write_pointer_map(ostream& f, const M<K, P<T> >& l) const;


// two interface functions suffice for file interaction:
	static void	save_object_to_file(const string& s,
				never_const_ptr<name_space> g);
	static excl_ptr<name_space>
			load_object_from_file(const string& s);

	static excl_ptr<name_space>
			self_test(const string& s, 
				never_const_ptr<name_space> g);

	static excl_ptr<name_space>
			self_test_no_file(never_const_ptr<name_space> g);

private:
	void initialize_null(void);
	void collect_objects(void);
	void load_objects(void);
	void write_header(ofstream& f);
	void load_header(ifstream& f);
	/** completes table of computed offsets and writes out all buffers */
	void finish_write(ofstream& f);
	/** reads in buffer into pieces */
	void finish_load(ifstream& f);
	/** just allocate objects without initializing */
	void reconstruct(void);
	excl_ptr<name_space>	get_root_namespace(void);
	void reset_for_loading(void);

};	// end class persistent_object_manager

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__ART_OBJECT_IO_H__

