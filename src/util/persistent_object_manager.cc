/**
	\file "util/persistent_object_manager.cc"
	Method definitions for serial object manager.  
	$Id: persistent_object_manager.cc,v 1.33 2006/10/24 18:39:04 fang Exp $
 */

// flags and switches
#define	DEBUG_ME			0
#define	ENABLE_STACKTRACE		0

#include <fstream>

#include "config.h"		// for ENABLE_OBJECT_ALIGN_CHECKING
#include "util/hash_specializations.h"	// include this first
	// for hash specialization to take effect
#include "util/new_functor.tcc"
#include "util/list_vector.tcc"
#include "util/persistent_object_manager.tcc"	// for read_pointer
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/memory/excl_array_ptr.h"
#include "util/macros.h"
#include "util/IO_utils.tcc"
#include "util/sstream.h"
#include "util/stacktrace.h"
#include "util/attributes.h"

//=============================================================================
// macros for use in write_object and load_object
// just sanity-check extraneous information, later enable or disable
// with another switch.


// #ifndef ENABLE_OBJECT_ALIGN_CHECKING
// #define ENABLE_OBJECT_ALIGN_CHECKING	1	// default 1, keep sanity checks
// #endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the numeric index of the object, which really isn't
	necessary, but adds a consistency check.  
	This macro can be added inside a write_object method implementation.  
	If used, it should be complemented with STRIP_POINTER_INDEX
	in the load_object counterpart.  
	Consider using a char, instead of unsigned long.
	updated: (2005-02-01)
	This macro only useful in a persistent_object_manager method!  
	Move this definition to persistent_object_manager.cc file.  
	\param f the output stream.
	\param p pointer to the object being written.
 */
#if ENABLE_OBJECT_ALIGN_CHECKING
#define WRITE_POINTER_INDEX(f, p)					\
	write_value(f, lookup_ptr_index(p))
#else
#define WRITE_POINTER_INDEX(f, p)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads the object enumerated index for a consistency check.  
	Consider using a char, instead of unsigned long.
	updated: (2005-02-01)
	This macro only useful in a persistent_object_manager method!  
	Move this definition to persistent_object_manager.cc file.  
	\param f the input stream.
	\param p pointer to the object read.  
 */
#if ENABLE_OBJECT_ALIGN_CHECKING
#define STRIP_POINTER_INDEX(f, p)                                       \
	{								\
        size_t index;							\
        read_value(f, index);						\
        if (index != lookup_ptr_index(p)) {				\
                const size_t hohum = lookup_ptr_index(p);		\
                cerr << "<persistent>::load_object(): " << endl		\
                        << "\tthis = " << p << ", index = " << index	\
                        << ", expected: " << hohum << endl;		\
                INVARIANT(index == lookup_ptr_index(p));		\
        }								\
        }
#else
#define STRIP_POINTER_INDEX(f, p)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Sanity check value for end-of-object.
        To be complemented by STRIP_OBJECT_FOOTER.  
 */
#if ENABLE_OBJECT_ALIGN_CHECKING
#define WRITE_OBJECT_FOOTER(f)			write_value(f, -1L)
#else
#define WRITE_OBJECT_FOOTER(f)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Sanity check for object alignment.  
 */
#if ENABLE_OBJECT_ALIGN_CHECKING
#define STRIP_OBJECT_FOOTER(f)						\
	{								\
	long neg_one;							\
	read_value(f, neg_one);						\
	INVARIANT(neg_one == -1L);					\
	}
#else
#define STRIP_OBJECT_FOOTER(f)
#endif


//=============================================================================

namespace util {
#include "util/using_ostream.h"
using std::ios_base;
using std::stringstream;
using std::stringbuf;
using std::streamsize;
using std::ostringstream;
using util::memory::excl_array_ptr;

//=============================================================================
/**
	The class contains the information necessary for reconstructing
	persistent objects associated with a pointer.  
 */
class persistent_object_manager::reconstruction_table_entry {
	typedef	reconstruction_table_entry	this_type;
private:
	/** constant default ios mode */
	static const ios_base::openmode	mode;
private:
	/** object type enumeration */
	persistent::hash_key	otype;

	/**
		location of reconstruction, consider object*?
		Technically, this is exclusively (transiently) owned.  
	 */
	const persistent*	recon_addr;

	/**
		Auxiliary allocator argument, useful for 2-level 
		constructor tables.  
	 */
	aux_alloc_arg_type	alloc_arg;

	/** reference count for counter pointers */
mutable	size_t*			ref_count;
	/**
		scratch flag, general purpose flag,
		consider making mutable
	 */
	bool			scratch;
	/** start of stream position */
	streampos		buf_head;
	/** end of stream position */
	streampos		buf_tail;
	/** stream buffer for temporary storage, count_ptr
		for transferrable semantics with copy assignment */
	count_ptr<stringstream>	buffer;

mutable	visit_info		visits;
public:
// need default constructor to create an invalid object
	reconstruction_table_entry();

	reconstruction_table_entry(const persistent::hash_key& t, 
		const aux_alloc_arg_type a, 
		const streampos h, const streampos tl);

	reconstruction_table_entry(const persistent::hash_key& t, 
		const streampos h, const streampos tl);

	reconstruction_table_entry(const persistent* p,
		const persistent::hash_key& t, 
		const aux_alloc_arg_type a);

	reconstruction_table_entry(const this_type&);

	~reconstruction_table_entry();

	const persistent::hash_key&
	type(void) const { return otype; }

	const persistent*
	addr(void) const { return recon_addr; }

	aux_alloc_arg_type
	get_alloc_arg(void) const { return alloc_arg; }

	size_t*
	count(void) const;

	visit_info&
	get_visit_info(void) const { return visits; }

	void
	assign_addr(persistent* ptr);

	void
	construct(const persistent*, const persistent::hash_key&, 
		const aux_alloc_arg_type);

	void
	reset_addr();

	void
	flag(void) { scratch = true; }

	void
	unflag(void) { scratch = false; }

	bool
	flagged(void) const { return scratch; }

	stringstream&
	get_buffer(void) const { return *buffer; }

	void
	initialize_offsets(void);

	void
	adjust_offsets(const streampos s);

	streampos
	head_pos(void) const { return buf_head; }

	streampos
	tail_pos(void) const { return buf_tail; }
};	// end class reconstruction_table_entry

//=============================================================================
// class reconstruction_table_entry method definitions

const ios_base::openmode
persistent_object_manager::reconstruction_table_entry::mode = 
	ios_base::in | ios_base::out | ios_base::binary;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool
persistent_object_manager::dump_reconstruction_table = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry() :
		otype(), recon_addr(NULL), alloc_arg(0), ref_count(NULL), 
		scratch(false), 
		buf_head(0), buf_tail(0), buffer(new stringstream(mode)), 
		visits() {
	STACKTRACE("reconstruction_table_entry() (ctor)");
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry(
		const persistent::hash_key& t, 
		const streampos hd, const streampos tl) :
		otype(t), recon_addr(NULL), alloc_arg(0), ref_count(NULL), 
		scratch(false), 
		buf_head(hd), buf_tail(tl), buffer(new stringstream(mode)),
		visits() {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry(
		const persistent::hash_key& t, 
		const aux_alloc_arg_type a, 
		const streampos hd, const streampos tl) :
		otype(t), recon_addr(NULL), alloc_arg(a), ref_count(NULL), 
		scratch(false), 
		buf_head(hd), buf_tail(tl), buffer(new stringstream(mode)), 
		visits() {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry(
		const persistent* p, 
		const persistent::hash_key& t, 
		const aux_alloc_arg_type a) :
		otype(t), recon_addr(p), alloc_arg(a), ref_count(NULL), 
		scratch(false), 
		buf_head(0), buf_tail(0), buffer(new stringstream(mode)),
		visits() {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non trivial copy-copstructor needed because vector allocation
	will occasionally have to copy-transfer all elements.
	The source will be destroyed, and thus must not own the recon_addr
	pointer!
	Thus copy-constructing is destructive transfer.  
	This problem can also be avoided by using a list_vector, 
	which guarantees that data sits in place in all push_back operations.  
 */
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry(const this_type& e) :
		otype(e.otype), recon_addr(e.recon_addr),
		alloc_arg(e.alloc_arg), ref_count(e.ref_count), 
		scratch(e.scratch), 
		buf_head(e.buf_head), buf_tail(e.buf_tail),
		buffer(e.buffer), 
		visits(e.visits) {
	const_cast<this_type&>(e).recon_addr = NULL;
	// ref_count is technically not owned, so the following is unnecessary
	const_cast<this_type&>(e).ref_count = NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sophisticated destructor.  
 */
persistent_object_manager::reconstruction_table_entry::
	~reconstruction_table_entry() {
	STACKTRACE("~reconstruction_table_entry()");
#if ENABLE_STACKTRACE
	cerr << "\t@ " << this << endl;
#endif
	// never delete anything manually!
	// not the object pointer, nor the reference count
	// buffer is automatically managed by reference-count.

	// MARK AND SWEEP!
	// check for un-claimed memory.
if (recon_addr) {
	bool uh_oh = false;
	if (visits.total_visits) {
		if (visits.owned_visits > 1) {
			cerr << "FATAL: object at " << recon_addr <<
			" was claimed as owned multiply, "
			"which will surely result in double-free!"
			<< endl;
			uh_oh = true;
		} else {
		// no exclusive owners
		if (visits.owned_visits && visits.shared_visits) {
			cerr << "FATAL: object at " << recon_addr <<
			" was claimed as both owned and shared, "
			"which will surely result in double-free!"
			<< endl;
			uh_oh = true;
		} else if (!visits.owned_visits && !visits.shared_visits) {
			if (visits.please_delete) {
				delete recon_addr;
			} else {
				cerr << "ERROR: object at " << recon_addr <<
				" was loaded by non-owner(s), but never by "
				"exclusive or shared owners, "
				"which will surely result in a memory-leak!"
				<< endl;
				uh_oh = true;
			}
		}
		}
		// if (visits.raw_visits > 1 && !visits.owned_visits)
		if (visits.raw_visits && !visits.owned_visits
				&& !visits.shared_visits) {
			cerr << "WARNING: object at " << recon_addr <<
				" was loaded as a raw pointer " <<
				visits.raw_visits << " times without "
				"having been claimed by an owner or sharer, "
				"which means the programmer is either "
				"1) lazy or 2) made a grave mistake.  "
				"It is my duty to strongly suggest using "
				"either exclusive or shared ownership.  "
				"Woe be unto you if you shut up this warning "
				"by commenting it out, and you end up "
				"with memory errors." << endl;
				uh_oh = true;
		}
		// nonowned and shared -- dangerous!!!
		// mixing oil and water...
	} else {
		if (!visits.do_not_delete) {
			cerr << "WARNING: object at " << recon_addr <<
				" was constructed but never touched, "
				"so I\'m deleting it." << endl;
			delete recon_addr;
		}
	}
	if (uh_oh) {
		recon_addr->what(cerr << "@ " << recon_addr << ": ");
		cerr << ", unowned: " << visits.unowned_visits <<
			", owned: " << visits.owned_visits <<
			", shared: " << visits.shared_visits <<
			", raw: " << visits.raw_visits <<
			", total: " << visits.total_visits << endl;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a reference count to be loaded into count_ptr or 
	other counted pointer classes.  
	This doesn't modify the count, that's the responsibility of the
	pointer class.  
	If a counter has not been allocated yet, this will allocate it
	but will never delete it, thus, it is the responsibility of the 
	pointer class to manage the memory for the counter.  
 */
size_t*
persistent_object_manager::reconstruction_table_entry::count(void) const {
	STACKTRACE("recon_table_entry::count()");
	if (!ref_count) {
		STATIC_RC_POOL_REF_INIT;
		ref_count = NEW_SIZE_T;
		*ref_count = 0;
	}
	NEVER_NULL(ref_count);
	return ref_count;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the reconstruction address, and flags as unvisited.  
	The persistent::hash_key is preserved.  
	The state of the buffer is left as is.  
	Any outstanding delete-exceptions are cleared in the visit_info
	structure, allowing the load phase to mark-and-sweep.  
 */
void
persistent_object_manager::reconstruction_table_entry::reset_addr() {
	recon_addr = NULL;		// never delete
	ref_count = NULL;		// never delete
	unflag();
	visits.allow_delete();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
persistent_object_manager::reconstruction_table_entry::assign_addr(
		persistent* ptr) {
	assert(!recon_addr);
	recon_addr = ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In-place construction.
 */
void
persistent_object_manager::reconstruction_table_entry::construct(
		const persistent* p, const persistent::hash_key& t, 
		const aux_alloc_arg_type a) {
	recon_addr = p;
	otype = t;
	alloc_arg = a;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just evaluates the size of the buffer, and records the 
	information in the buf_head and buf_tail members.  
	Later, the positions will be adjusted relative to their
	absolute position in the final serialized output stream.  
 */
void
persistent_object_manager::reconstruction_table_entry::initialize_offsets(
		void) {
	// Need to catch case where buffers were not used, 
	// in which case the head and tail positions are -1
	buf_head = buffer->tellg();
	buf_tail = buffer->tellp();
	if (buf_head < 0) {
		assert(buf_tail < 0);		// consistency
		buf_head = buf_tail = 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
persistent_object_manager::reconstruction_table_entry::adjust_offsets(
		const streampos s) {
	buf_head += s;
	buf_tail += s;
}

//=============================================================================
// class persistent_object_manager method definitions

persistent_object_manager::persistent_object_manager() :
		addr_to_index_map(),
		reconstruction_table(),
		start_of_objects(0), 
		root(NULL) {
 	// set list_vector(chunk-size)
	reconstruction_table.set_chunk_size(64);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If the auto-managed excl_ptr for the global root is not 
	transfered away, then the manager will take care of 
	deleting it.  
	By construction of typed and managed pointers, 
	we assume that freeing the root pointer will recursively 
	reclaim all the memory linked thereto.  
 */
persistent_object_manager::~persistent_object_manager() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a persistent object that will be stored.  
	\return true if address was already visited.  
 */
bool
persistent_object_manager::register_transient_object(
		const persistent* ptr, const persistent::hash_key& t, 
		const aux_alloc_arg_type a) {
	STACKTRACE("pom::register_transient_object()");
	const addr_to_index_map_type::const_iterator
		f(addr_to_index_map.find(ptr));
	if (ptr)
		assert(t != persistent::hash_key::null);
	if (f != addr_to_index_map.end())
	{
		const size_t probe = f->second.val;
		// sanity check
		const reconstruction_table_entry& e
			__ATTRIBUTE_UNUSED_CTOR__((reconstruction_table[probe]));
		assert(e.type() == t);
		assert(e.addr() == ptr);	// check reverse mapping
		assert(e.get_alloc_arg() == a);
		return true;
	} else {
		static const reconstruction_table_entry empty;
		// else add new entry
		addr_to_index_map[ptr] = reconstruction_table.size();
		reconstruction_table.push_back(
			reconstruction_table_entry(ptr, t, a));
			// should transfer ownership of newly 
			// constructed stringstream
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes entry 0 with NULL.  
 */
void
persistent_object_manager::initialize_null(void) {
	STACKTRACE("pom::initialize_null()");
	assert(!reconstruction_table.size());
	register_transient_object(NULL, persistent::hash_key::null);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses the table entry's scratch flag.  
	Sets it to true.  
	\return true if already visited, false if this is first time.
 */
bool
persistent_object_manager::flag_visit(const persistent* ptr) {
	const addr_to_index_map_type::const_iterator
		f(addr_to_index_map.find(ptr));
	INVARIANT(f != addr_to_index_map.end());
	const size_t probe = f->second.val;
	reconstruction_table_entry& e(reconstruction_table[probe]);
	INVARIANT(e.addr() == ptr);		// sanity check
	if (e.flagged())
		return true;
	else {
		e.flag();
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns an integer index corresponding to this address.  
	\param ptr must be a registered address in this object manager, 
		and may be NULL.  
	\return index corresponding to the entry.  
 */
size_t
persistent_object_manager::lookup_ptr_index(const persistent* ptr) const {
	const addr_to_index_map_type::const_iterator
		f(addr_to_index_map.find(ptr));
	if (f == addr_to_index_map.end()) {
		// more useful diagnosis message
		if (ptr) {
			ptr->what(cerr << "FATAL: Object (") << ") at addr " <<
				static_cast<const void*>(ptr) <<
				" has not been registered with "
				"the object manager!" << endl;
			// DIE;
		}
		// else just NULL, don't bother
		THROW_EXIT;
	}
	return f->second.val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
const reconstruction_table_entry&
persistent_object_manager::lookup_reconstruction_table_entry(
		const size_t i) const {
	assert(i < reconstruction_table.size());
	return reconstruction_table[i];
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the pointer corresponding to the indexed
	reconstruction table entry.  
 */
persistent*
persistent_object_manager::lookup_obj_ptr(const size_t i) const {
	INVARIANT(i < reconstruction_table.size());
	const reconstruction_table_entry& e(reconstruction_table[i]);
	return const_cast<persistent*>(e.addr());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the pointer corresponding to the indexed
	reconstruction table entry.  
 */
std::pair<persistent*, persistent_object_manager::visit_info*>
persistent_object_manager::lookup_ptr_visit_info(const size_t i) const {
	INVARIANT(i < reconstruction_table.size());
	const reconstruction_table_entry& e(reconstruction_table[i]);
	return std::make_pair(
		const_cast<persistent*>(e.addr()),
		&e.get_visit_info());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
persistent_object_manager::check_reconstruction_table_range(
		const size_t i) const {
	return (i < reconstruction_table.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
persistent_object_manager::please_delete(const persistent* p) const {
	const size_t i = lookup_ptr_index(p);
	INVARIANT(check_reconstruction_table_range(i));
	reconstruction_table[i].get_visit_info().request_delete();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// DISABLED, UNUSED
void
persistent_object_manager::do_not_delete(const persistent* p) const {
	const size_t i = lookup_ptr_index(p);
	INVARIANT(check_reconstruction_table_range(i));
	reconstruction_table[i].get_visit_info().forbid_delete();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the reference count of the corresponding entry.
	If it is NULL, then will allocate and initialize to zero.  
 */
size_t*
persistent_object_manager::lookup_ref_count(const size_t i) const {
	STACKTRACE("lookup_ref_count(size_t)");
	const reconstruction_table_entry& e(reconstruction_table[i]);
	return e.count();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up reference count by pointer.
 */
size_t*
persistent_object_manager::lookup_ref_count(const persistent* p) const {
	STACKTRACE("lookup_ref_count(persistent*)");
	return lookup_ref_count(lookup_ptr_index(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param address of the object, which maps to some reconstruction
		table entry.  
	\return writeable reference to a stream buffer.  
 */
ostream&
persistent_object_manager::lookup_write_buffer(const persistent* ptr) const {
	stringstream&
		ret(reconstruction_table[lookup_ptr_index(ptr)].get_buffer());
#if DEBUG_ME
	cerr << "lookup_write_buffer(): tellg = " << ret.tellg()
		<< ", tellp = " << ret.tellp() << endl;
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param address of the object, which maps to some reconstruction
		table entry.  
	\return readable reference to a stream buffer.  
 */
istream&
persistent_object_manager::lookup_read_buffer(const persistent* ptr) const {
	stringstream&
		ret(reconstruction_table[lookup_ptr_index(ptr)].get_buffer());
#if DEBUG_ME
	cerr << "lookup_read_buffer(): tellg = " << ret.tellg()
		<< ", tellp = " << ret.tellp() << endl;
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static accessor guarantees that when this function is called, 
	the function map will have been initialized exactly once, 
	before all uses.  
	Needs to be be private for protection, because it returns
	a non-const reference!  
	Add-only access is granted through register_persistent_type.

	Don't really care when this is destroyed statically, 
	order is irrelevant, because no other destructors depend on this.  
	\return valid reference to the only reconstruction function map.  
 */
persistent_object_manager::reconstruction_function_map_type&
persistent_object_manager::reconstruction_function_map(void) {
	// function-local static initialized once upon first entry
	static reconstruction_function_map_type
		the_reconstruction_function_map;
	return the_reconstruction_function_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unique number during persistent type registration.  
 */
int
persistent_object_manager::registered_type_sequence_number(void) {
	static int count = 0;
	count++;
	return count;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if key and auxiliary index map to a valid 
		registered type.
 */
bool
persistent_object_manager::verify_registered_type(
		const persistent::hash_key& k, const aux_alloc_arg_type i) {
	const reconstruction_function_map_type&
		m(reconstruction_function_map());
	const reconstruction_function_map_type::const_iterator f(m.find(k));
	if (f == m.end())
		return false;
	const reconstructor_vector_type& ctor_vec(f->second);
	if (size_t(i) >= ctor_vec.size())
		return false;
	else {
		const reconstruct_function_ptr_type probe(ctor_vec[i]);
		return (probe != NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
persistent_object_manager::dump_registered_type_map(ostream& o) {
	const reconstruction_function_map_type&
		m(reconstruction_function_map());
	reconstruction_function_map_type::const_iterator iter(m.begin());
	const reconstruction_function_map_type::const_iterator end(m.end());
	o << "persistent_object_manager::reconstruction_function_map has " <<
		m.size() << " entries." << endl;
	o << "(Each entry may contain multiple constructor functors.)" << endl;
	o << "\tkey[index]\twhat" << endl;
	for ( ; iter != end; ++iter) {
		// this calls the appropriate construct_empty()
		// really should be a unique_ptr, after I finish it...
		// DANGER: may not be safe to call what() on uninitialized
		// objects, if it depends on internal field members!
		// Thus, we should guarantee that what() is independent of
		// field members.  
		const reconstructor_vector_type& ctor_vec(iter->second);
		size_t j = 0;
		for ( ; j < ctor_vec.size(); j++) {
			reconstruct_function_ptr_type ctor(ctor_vec[j]);
			if (ctor) {
				const excl_ptr<persistent> tmp((*ctor)());
				NEVER_NULL(tmp);
				tmp->what(o << '\t' << iter->first << '[' <<
					j << "]\t") << endl;
			}
		}
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
persistent_object_manager::dump_text(ostream& o) const {
	o << "Persistent Object Manager text dump: " << endl;
	size_t i = 0;
	const size_t max = reconstruction_table.size();
	o << "\ti\taddr\t\ttype\t\targ\thead\ttail" << endl;
	for ( ; i < max; i++) {
		const reconstruction_table_entry& e(reconstruction_table[i]);
		o << '\t' << i << '\t';
		streamsize w = o.width();
		o.width(10);
		o << e.addr();
		o.width(w);
		o << '\t' << e.type() 
			<< '\t' << size_t(e.get_alloc_arg())
			<< '\t' << e.head_pos()
			<< '\t' << e.tail_pos() << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just write out the reconstruction_table to output file stream.
	This should only be called AFTER the header entries are finalized.  
	Not constant because it sets start_of_objects.  
	\param f must be open in output mode with binary mode.  
 */
void
persistent_object_manager::write_header(ofstream& f) {
	STACKTRACE("pom::write_header()");
	// How many entries to expect?
	const size_t max = reconstruction_table.size();
	write_value(f, max);
	// include NULL in entry in write-out
	size_t i = 0;
	for ( ; i<max; i++) {
		const reconstruction_table_entry& e(reconstruction_table[i]);
		write_value(f, e.type());
		write_value(f, e.get_alloc_arg());
#if 0
		cerr << "alloc_arg = " << (size_t) e.get_alloc_arg() << endl;
#endif
		write_value(f, e.head_pos());
		write_value(f, e.tail_pos());
	}
	static const long neg_one = -1L;
	write_value(f, neg_one);
	start_of_objects = f.tellp();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the reconstruction_table from the input file stream.  
	\param f must be open in input mode with binary mode.  
 */
void
persistent_object_manager::load_header(ifstream& f) {
	STACKTRACE("pom::load_header()");
	size_t max;
	INVARIANT(f.good());
	read_value(f, max);
	if (!f.good()) {
		// then file is empty
		// can't get name from ifstream?
		cerr << "File is empty." << endl;
		// more creative error-handling later...
		THROW_EXIT;
	}
	size_t i = 0;
	for ( ; i<max; i++) {
		persistent::hash_key t;
		aux_alloc_arg_type aux;
		streampos head, tail;
		read_value(f, t);
		read_value(f, aux);
		read_value(f, head);
		read_value(f, tail);
		// make sure t is a registered type
		if (t != persistent::hash_key::null && 
				!verify_registered_type(t, aux)) {
			cerr << "FATAL: persistent type code \"" <<
				t << "\", index " << aux <<
				" has not been registered!" << endl;
			THROW_EXIT;
		}
		reconstruction_table.push_back(
			reconstruction_table_entry(t, aux, head, tail));
	}
	long neg_one;
	read_value(f, neg_one);
	INVARIANT(neg_one == -1L);
	start_of_objects = f.tellg();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This pass just allocates memory for the entries in the
	reconstruction table, and doesn't initialize any of their contents.  
	Also constructs reverse map from address to index.  
 */
void
persistent_object_manager::reconstruct(void) {
	STACKTRACE("pom::reconstruct()");
	const size_t max = reconstruction_table.size();
	// skip the 0th entry, is reserved NULL
	addr_to_index_map[NULL] = 0;
	size_t i = 1;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e(reconstruction_table[i]);
		const persistent::hash_key& t(e.type());
		if (t != persistent::hash_key::null) {	// not NULL_TYPE
			const reconstruction_function_map_type::const_iterator
				f(reconstruction_function_map().find(t));
			INVARIANT(f != reconstruction_function_map().end());
			const reconstructor_vector_type& ctor_vec(f->second);
			const size_t j = e.get_alloc_arg();
			if (j >= ctor_vec.size() || !ctor_vec[j]) {
				cerr << "WARNING: don\'t know how to "
					"reconstruct/allocate type " << t <<
					" yet, skipping..." << endl;
				e.assign_addr(NULL);
			} else {
				// this allocates and empty constructs
				e.assign_addr((*ctor_vec[j])());
				addr_to_index_map[e.addr()] = i;
#if 0
				e.addr()->what(cerr << i << ": ") << endl;
#endif
			}
		} else {
			e.assign_addr(NULL);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serially writes all registered objects to output stream.
	Not const because the the reconstruction table entries 
	are updated with computed offsets.  
	This should reverse the process of finish_load.
 */
void
persistent_object_manager::finish_write(ofstream& f) {
	STACKTRACE("pom::finish_write()");
	// First, compute offsets of the stream segments (buffers)
	const size_t max = reconstruction_table.size();
	size_t i = 0;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e(reconstruction_table[i]);
		// flush out each stream buffer in order
		istream& o(e.get_buffer());	// is a stringstream
		INVARIANT(o.good());
		stringbuf* sb = IS_A(stringbuf*, o.rdbuf());
		NEVER_NULL(sb);
		const int size = sb->in_avail();	// characters available
		INVARIANT(size == e.tail_pos() -e.head_pos());
#if 0
		cerr << "entry " << i << " wrote " << size << " bytes." << endl;
#endif
		const string str(sb->str());
		f.write(str.c_str(), size);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits each registered object and write the segment of its
	output to its respective buffer in the reconstruction table.  
	Also initializes the buf_head and buf_tail relative offsets.  
	This should reverse the process of load_objects.
 */
void
persistent_object_manager::collect_objects(void) {
	STACKTRACE("pom::collect_objects()");
	const size_t max = reconstruction_table.size();
	size_t i = 1;			// don't initialize the 0th one!
//	dump_text(cerr);		// DEBUG
	for ( ; i<max; i++) {
		reconstruction_table_entry& e(reconstruction_table[i]);
		const persistent* const o(e.addr());
		NEVER_NULL(o);
		{
			ostream& f(lookup_write_buffer(o));
			INVARIANT(f.good());
			WRITE_POINTER_INDEX(f, o);	// sanity check
			o->write_object(*this, f);	// pure virtual
			WRITE_OBJECT_FOOTER(f);		// alignment
		}
		e.initialize_offsets();
	}
#if 0
	// (abandoned attempt)
	// 20060205 hack: in an extremely rare case, we might have had to add
	// an object to the reconstruction table *during* write_object, 
	// in which case, we need to extend the collection :(
	const size_t new_max = reconstruction_table.size();
	INVARIANT(i == max);
	dump_text(cerr << endl) << endl;
	for ( ; i<new_max; ++i) {
		reconstruction_table_entry& e(reconstruction_table[i]);
		const persistent* const o(e.addr());
		NEVER_NULL(o);
		{
			ostream& f(lookup_write_buffer(o));
			INVARIANT(f.good());
			WRITE_POINTER_INDEX(f, o);	// sanity check
			o->write_object(*this, f);	// pure virtual
			WRITE_OBJECT_FOOTER(f);		// alignment
		}
		e.initialize_offsets();
	}
	// we damn well better not have hacked it again...
	INVARIANT(new_max == reconstruction_table.size());
#else
	const size_t new_max = max;
#endif
	// next pass: translate offsets into absolute positions.  
	i = 0;				// or start at 1, no difference
	streampos tail = 0;
	for ( ; i<new_max; ++i) {
		reconstruction_table_entry& e(reconstruction_table[i]);
		streampos len = e.tail_pos();
		e.adjust_offsets(tail);
		tail += len;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	After reading the header, this continues to deserialize
	the objects that follow, loading them into local buffers
	in each reconstruction_table_entry.  
	Recall that the head and tail offsets in the reconstruction table
	are relative to the start of the object section.  
	This should reverse the process of finish_write.
	After this procedure, can close the file.  
	\param f the input file stream.  
 */
void
persistent_object_manager::finish_load(ifstream& f) {
	STACKTRACE("pom::finish_load()");
	const size_t max = reconstruction_table.size();
	// skip the 0th object, it's reserved NULL
	size_t i = 1;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e(reconstruction_table[i]);
		const int size = e.tail_pos() -e.head_pos();
		f.seekg(e.head_pos() +start_of_objects);
		ostream& o = e.get_buffer();
		INVARIANT(o.good());
#if 1
		// is there a better way to do this, 
		// eliminating intermediate? and need to allocate/free?
		// CONSIDER: valarray for fast buffer
		// or an auto-expanding function-local static array
		// CONSIDER: using i/ostream_iterator and std::copy;
		if (size <= 64) {
			char sbuf[64];	// fixed size buffer on the stack
			f.read(sbuf, size);
			o.write(sbuf, size);
		} else {
			// larger streams will require more temporary space
			// consider alloca
			// or valarray<char>
			const excl_array_ptr<char>::type
				cbuf(new char [size]);
			INVARIANT(cbuf);
			f.read(&*cbuf, size);
			o.write(&*cbuf, size);
			// will automatically delete []
		}
#else
		// or block-copy in chunks...
		int i = 0;
		for ( ; i<size; i++) {
			char c[1];
			f.read(c, 1);
			o.write(c, 1);
		}
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Explicit external request to load an object from the persistent 
	object manager.  
	Useful during recursive loads, which should be infrequent.  
	\param p pointer to object to load, never null.  
 */
void
persistent_object_manager::__load_object_once(
		persistent* p, raw_pointer_tag) const {
	NEVER_NULL(p);
	// ugh, const_cast...
	if (!const_cast<persistent_object_manager*>(this)->flag_visit(p)) {
		istream& i = lookup_read_buffer(p);
		INVARIANT(i.good());
		STRIP_POINTER_INDEX(i, p);
		p->load_object(*this, i);
		STRIP_OBJECT_FOOTER(i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should reverse the process of collect_objects.
	After this process, all registered objects should be coherent!
	Calls virtual function for initializing objects.  
	At the end, assumes responsibility for deleting everything
	by wrapping the root namespace pointer.  
	Each call to load_object may, however, freely and recursively
	invoke load_object for its dependencies.  
	The first visit or entry into each object will flag it as
	visited so each object will be initialized once and only once.  
	The only guideline for the recursive invocations is that
	static data such as names for hash keys should be initialized
	before any recursion, so that the depender is sufficiently 
	initialized to be used.  
	An example of this is namespaces whose entries in their
	parents' tables use a hash on the string name.  
 */
void
persistent_object_manager::load_objects(void) {
	STACKTRACE("pom::load_objects()");
	const size_t max = reconstruction_table.size();
	// 0th object is reserved NULL, skip it
	size_t i = 1;
	for ( ; i<max; i++) {
#if 0
		cerr << "Loading object number " << i << "!!!" << endl;
#endif
#if 0
		ostringstream oss;
		oss << "iter: " << i;
		STACKTRACE(oss.str());
#endif
		reconstruction_table_entry& e(reconstruction_table[i]);
		persistent* const o(const_cast<persistent*>(e.addr()));
		if (o) {
		/***
			CONSIDER: moving common header and footer code
			common to all load_object implementations here, 
			including visit-once check.  
			Or factor it out into another (private) proxy
			member function, because load_object is allowed
			to call load_object recursively.
		***/
#if 0
			o->what(cerr << i << " @ " << o << ", ") << endl;
#endif
			__load_object_once(o, raw_pointer_tag());
		}
		// else can't load a NULL object
	}
	// Finally, after this is called, immediately assume responsibility
	// for deleting all memory, by wrapping the root pointer
	// to the module containing the global namespace in an excl_ptr
	// Entry at position 1 is ALWAYS the root module.  
#if 0
	persistent* r = lookup_obj_ptr(1);
	root = excl_ptr<persistent>(r);
#else
	stringstream sstr;
	write_value(sstr, size_t(1));
	read_pointer(sstr, root);	// requires .tcc
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads hierarchical object collection from file.
	\returns a dynamically cast owned pointer to the root object.
 */
excl_ptr<persistent>
persistent_object_manager::load_object_from_file(const string& s) {
	ifstream f(s.c_str(), ios_base::binary);
	persistent_object_manager pom;
	// don't initialize_null, will be loaded in from table
	pom.load_header(f);
	pom.finish_load(f);
	f.close();                              // done with file
	pom.reconstruct();                      // allocate-only pass
	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;    // debugging only
	// Oh no, partially initialized objects!
	pom.load_objects();
	return pom.get_root();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves entire hierarchical object collection to file.  
 */
void
persistent_object_manager::save_object_to_file(const string& s, 
		const persistent& m) {
	ofstream f(s.c_str(), ios_base::binary | ios_base::trunc);
	INVARIANT(f.good());
	persistent_object_manager pom;
	pom.initialize_null();			// reserved 0th entry
	m.collect_transient_info(pom);		// recursive visitor
	pom.collect_objects();			// buffers output in segments
	pom.set_write_mode();
	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;	// for debugging
	pom.write_header(f);		// after knowing size of each segment
	pom.finish_write(f);			// serialize objects
	f.close();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets all the reconstruction_table_entries to do_not_delete, 
	because they are only observed pointers for the sake of
	deep-copying and saving binary to file.  
 */
void
persistent_object_manager::set_write_mode(void) {
	const size_t max = reconstruction_table.size();
	size_t i = 1;		// 0th object is reserved NULL, skip it
	for ( ; i<max; i++) {
		reconstruction_table[i].get_visit_info().forbid_delete();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the reconstruction table as if it had just been loaded in, 
	which allows testing without actually writing out to a file.  
 */
void
persistent_object_manager::reset_for_loading(void) {
	STACKTRACE("pom::reset_for_loading()");
	const size_t max = reconstruction_table.size();
	size_t i = 1;		// 0th object is reserved NULL, skip it
	for ( ; i<max; i++) {
		reconstruction_table_entry& e(reconstruction_table[i]);
		e.reset_addr();
		// shouldn't need to manipulate stream positions
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write the reconstruction table, and loads it back, without
	going through an intermediate file.
	Should essentially make a deep copy of the hierarchical object
	rooted at the global namespace.
 */
excl_ptr<persistent>
persistent_object_manager::self_test_no_file(const persistent& m) {
	STACKTRACE("pom::self_test_no_file()");
	persistent_object_manager pom;
	pom.initialize_null();			// reserved 0th entry
	m.collect_transient_info(pom);		// recursive visitor
	pom.collect_objects();			// buffers output in segments
	pom.set_write_mode();
	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;	// for debugging

	// need to set start of objects? no
	// pretend we wrote it out and read it back in...
	pom.reset_for_loading();
	pom.reconstruct();			// allocate-only pass

	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;	// debugging only

	pom.load_objects();
	// must acquire root object in some owned pointer!
	return pom.get_root();	// only this is templated
	// will get de-allocated after return statement is evaluated
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out and reads back in, through an intermediate file.
 */
excl_ptr<persistent>
persistent_object_manager::self_test(const string& s, const persistent& m) {
	save_object_to_file(s, m);
	return load_object_from_file(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The first non-NULL object is special: it is the root module.
	Returning an excl_ptr guarantees that memory will
	be managed properly.
	When the excl_ptr hits the end of a scope, unless ownership
	has been transferred, the memory should be recursively reclaimed.
	Thus, this is not a const method.
 */
excl_ptr<persistent>
persistent_object_manager::get_root(void) {
	NEVER_NULL(root);           // necessary?
	return root.is_a_xfer<persistent>();
	// this relinquishes ownership and responsibility for deleting
	// to whomever consumes the returned excl_ptr
}


//=============================================================================
}	// end namespace util

