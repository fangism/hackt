/**
	\file "persistent_object_manager.cc"
	Method definitions for serial object manager.  
	$Id: persistent_object_manager.cc,v 1.11 2004/12/15 23:31:14 fang Exp $
 */

#include <fstream>

#include "hash_specializations.h"	// include this first
	// for hash specialization to take effect
#include "hash_qmap.tcc"
#include "persistent_object_manager.h"
	// includes "count_ptr.h"
#include "macros.h"
#include "IO_utils.tcc"
	// includes <iostream>

//=============================================================================
// flags and switches

#define	DEBUG_ME		0

//=============================================================================

namespace util {
#include "using_ostream.h"
using std::stringbuf;
using std::streamsize;

//=============================================================================
// class reconstruction_table_entry method definitions

const ios_base::openmode
persistent_object_manager::reconstruction_table_entry::mode = 
	ios_base::in | ios_base::out | ios_base::binary;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool
persistent_object_manager::dump_reconstruction_table = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Is statically initialized to NULL before any objects use it.  
 */
persistent_object_manager::reconstruction_function_map_type*
persistent_object_manager::the_reconstruction_function_map_ptr = NULL;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Proper orderly initialization to guarantee that this
	is properly deleted upon program termination.  
	Note the use of unsafe "address-taking" of reference to 
	initialize the pointer-class object.  
 */
excl_ptr<persistent_object_manager::reconstruction_function_map_type>
persistent_object_manager::the_reconstruction_function_map_ptr_wrapped(
	&get_reconstruction_function_map());

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry() :
		otype(), recon_addr(NULL), alloc_arg(0), ref_count(NULL), 
		scratch(false), 
		buf_head(0), buf_tail(0), buffer(new stringstream(mode)) {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	reconstruction_table_entry(
		const persistent::hash_key& t, 
		const streampos hd, const streampos tl) :
		otype(t), recon_addr(NULL), alloc_arg(0), ref_count(NULL), 
		scratch(false), 
		buf_head(hd), buf_tail(tl), buffer(new stringstream(mode)) {
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
		buf_head(hd), buf_tail(tl), buffer(new stringstream(mode)) {
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
		buf_head(0), buf_tail(0), buffer(new stringstream(mode)) {
	assert(buffer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent_object_manager::reconstruction_table_entry::
	~reconstruction_table_entry() {
	// never delete anything manually!
	// not the object pointer, nor the reference count
	// buffer is automatically managed by reference-count.
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
	if (!ref_count) {
		ref_count = new size_t(0);
	}
	return ref_count;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the reconstruction address, and flags as unvisited.  
	The persistent::hash_key is preserved.  
	The state of the buffer is left as is.  
 */
void
persistent_object_manager::reconstruction_table_entry::reset_addr() {
	recon_addr = NULL;		// never delete
	ref_count = NULL;		// never delete
	unflag();
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
	Returns true if address was already visited.  
 */
bool
persistent_object_manager::register_transient_object(
		const persistent* ptr, const persistent::hash_key& t, 
		const aux_alloc_arg_type a) {
	const long probe = addr_to_index_map[ptr];
	if (probe >= 0) {
		// sanity check
		const reconstruction_table_entry& e = 
			reconstruction_table[probe];
		assert(e.type() == t);
		assert(e.addr() == ptr);
		assert(e.get_alloc_arg() == a);
		return true;
	} else {
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
	assert(!reconstruction_table.size());
	register_transient_object(NULL, persistent::hash_key::null);
#if 0
	reconstruction_table_entry& e = reconstruction_table[0];
	e.set_head(0);
	e.set_tail(0);
	assert(!e.head_pos());
	assert(!e.tail_pos());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses the table entry's scratch flag.  
	Sets it to true.  
	\return true if already visited, false if this is first time.
 */
bool
persistent_object_manager::flag_visit(const persistent* ptr) {
	const long probe = addr_to_index_map[ptr];
	assert(probe >= 0);
	reconstruction_table_entry& e = reconstruction_table[probe];
	assert(e.addr() == ptr);		// sanity check
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
long
persistent_object_manager::lookup_ptr_index(const persistent* ptr) const {
	const long probe = addr_to_index_map[ptr];
	// because uninitialized value of Long is -1
	if (probe < 0) {
		// more useful diagnosis message
		if (ptr) {
			ptr->what(cerr << "FATAL: Object (") << ") at addr "
				<< ptr << " has not been registered with "
				"the object manager!" << endl;
			assert(0);
		}
		// else just NULL, don't bother
		exit(1);
	}
	return probe;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the pointer corresponding to the indexed
	reconstruction table entry.  
 */
persistent*
persistent_object_manager::lookup_obj_ptr(const long i) const {
	assert((unsigned long) i < reconstruction_table.size());
	const reconstruction_table_entry& e = reconstruction_table[i];
	return const_cast<persistent*>(e.addr());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the reference count of the corresponding entry.
	If it is NULL, then will allocate and initialize to zero.  
 */
size_t*
persistent_object_manager::lookup_ref_count(const long i) const {
	const reconstruction_table_entry& e = reconstruction_table[i];
	return e.count();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up reference count by pointer.
 */
size_t*
persistent_object_manager::lookup_ref_count(const persistent* p) const {
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
	stringstream& ret =
		reconstruction_table[lookup_ptr_index(ptr)].get_buffer();
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
	stringstream& ret =
		reconstruction_table[lookup_ptr_index(ptr)].get_buffer();
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
	\return valid reference to the only reconstruction function map.  
 */
persistent_object_manager::reconstruction_function_map_type&
persistent_object_manager::get_reconstruction_function_map(void) {
	if (!the_reconstruction_function_map_ptr) {
		the_reconstruction_function_map_ptr = 
			new reconstruction_function_map_type;
		assert(the_reconstruction_function_map_ptr);
	}
	return *the_reconstruction_function_map_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
persistent_object_manager::verify_registered_type(
		const persistent::hash_key& k) {
	const reconstruct_function_ptr_type probe =
		static_cast<const reconstruction_function_map_type&>(
			get_reconstruction_function_map())[k];
	return (probe != NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
persistent_object_manager::dump_registered_type_map(ostream& o) {
	const reconstruction_function_map_type& m =
		get_reconstruction_function_map();
	reconstruction_function_map_type::const_iterator iter = m.begin();
	const reconstruction_function_map_type::const_iterator end = m.end();
	o << "persistent_object_manager::reconstruction_function_map has " <<
		get_reconstruction_function_map().size() << " entries." << endl;
	o << "\tkey\t\twhat" << endl;
	for ( ; iter != end; iter++) {
		excl_ptr<persistent> tmp((*iter->second)(0));
		assert(tmp);
		// DANGER: may not be safe to call what() on uninitialized
		// objects, if it depends on internal field members!
		// Thus, we should guarantee that what() is independent of
		// field members.  
		tmp->what(o << '\t' << iter->first << "    \t") << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
persistent_object_manager::dump_text(ostream& o) const {
	o << "Persistent Object Manager text dump: " << endl;
	long i = 0;
	const long max = reconstruction_table.size();
	o << "\ti\taddr\t\ttype\t\targ\thead\ttail" << endl;
	for ( ; i < max; i++) {
		const reconstruction_table_entry& e = reconstruction_table[i];
		o << '\t' << i << '\t';
		streamsize w = o.width();
		o.width(10);
		o << e.addr();
		o.width(w);
		o << '\t' << e.type() 
			<< '\t' << (size_t) e.get_alloc_arg()
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
	// How many entries to expect?
	const size_t max = reconstruction_table.size();
	write_value(f, max);
	// include NULL in entry in write-out
	size_t i = 0;
	for ( ; i<max; i++) {
		const reconstruction_table_entry& e = reconstruction_table[i];
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
	size_t max;
	read_value(f, max);
	size_t i = 0;
	for ( ; i<max; i++) {
		persistent::hash_key t;
		aux_alloc_arg_type aux;
		streampos head, tail;
		read_value(f, t);
		read_value(f, aux);
#if 0
		cerr << "alloc_arg = " << (size_t) aux << endl;
#endif
		read_value(f, head);
		read_value(f, tail);
		// make sure t is a registered type
		if (t != persistent::hash_key::null && 
				!verify_registered_type(t)) {
			cerr << "FATAL: persistent type code \"" <<
				t << "\" has not been registered!" << endl;
			exit(1);
		}
		reconstruction_table.push_back(
			reconstruction_table_entry(t, aux, head, tail));
	}
	long neg_one;
	read_value(f, neg_one);
	assert(neg_one == -1L);
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
	const size_t max = reconstruction_table.size();
	// skip the 0th entry, is reserved NULL
	addr_to_index_map[NULL] = 0;
	size_t i = 1;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		const persistent::hash_key& t = e.type();
		if (t != persistent::hash_key::null) {	// not NULL_TYPE
			const reconstruct_function_ptr_type f = 
				get_reconstruction_function_map()[t];
			if (f) {
				e.assign_addr((*f)(e.get_alloc_arg()));
				addr_to_index_map[e.addr()] = i;
			} else {
				cerr << "WARNING: don\'t know how to "
					"reconstruct/allocate type " << t <<
					" yet, skipping..." << endl;
				e.assign_addr(NULL);
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
	// First, compute offsets of the stream segments (buffers)
	const size_t max = reconstruction_table.size();
	size_t i = 0;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		// flush out each stream buffer in order
		istream& o = e.get_buffer();	// is a stringstream
		assert(o.good());
		stringbuf* sb = IS_A(stringbuf*, o.rdbuf());
		assert(sb);
		const int size = sb->in_avail();	// characters available
		assert(size == e.tail_pos() -e.head_pos());
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
	const size_t max = reconstruction_table.size();
	size_t i = 1;			// don't initialize the 0th one!
//	dump_text(cerr);		// DEBUG
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
//		persistent* o = const_cast<persistent*>(e.addr());
		const persistent* o = e.addr();
		assert(o);
		o->write_object(*this);	// virtual
		e.initialize_offsets();
	}
	// next pass: translate offsets into absolute positions.  
	i = 0;				// or start at 1, no difference
	streampos tail = 0;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
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
	const size_t max = reconstruction_table.size();
	// skip the 0th object, it's reserved NULL
	size_t i = 1;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		const int size = e.tail_pos() -e.head_pos();
		f.seekg(e.head_pos() +start_of_objects);
		ostream& o = e.get_buffer();
		assert(o.good());
#if 1
		// is there a better way to do this, 
		// eliminating intermediate? and need to allocate/free?
		if (size <= 64) {
			char sbuf[64];	// fixed size buffer on the stack
			f.read(sbuf, size);
			o.write(sbuf, size);
		} else {
			// larger streams will require more temporary space
			char* cbuf = new char [size];
			assert(cbuf);
			f.read(cbuf, size);
			o.write(cbuf, size);
			delete [] cbuf;
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
	const size_t max = reconstruction_table.size();
	// 0th object is reserved NULL, skip it
	size_t i = 1;
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		persistent* o = const_cast<persistent*>(e.addr());
		if (o)
			o->load_object(*this);
			// virtual call, unavoidable const cast
		// else can't load a NULL object
	}
	// Finally, after this is called, immediately assume responsibility
	// for deleting all memory, by wrapping the root pointer
	// to the module containing the global namespace in an excl_ptr
	// Entry at position 1 is ALWAYS the root module.  
	persistent* r = lookup_obj_ptr(1);
	root = excl_ptr<persistent>(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves entire hierarchical object collection to file.  
 */
void
persistent_object_manager::save_object_to_file(const string& s, 
		const persistent& m) {
	ofstream f(s.c_str(), ios_base::binary | ios_base::trunc);
	assert(f.good());
	persistent_object_manager pom;
	pom.initialize_null();			// reserved 0th entry
	m.collect_transient_info(pom);		// recursive visitor
	pom.collect_objects();			// buffers output in segments
	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;	// for debugging
	pom.write_header(f);		// after knowing size of each segment
	pom.finish_write(f);			// serialize objects
	f.close();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the reconstruction table as if it had just been loaded in, 
	which allows testing without actually writing out to a file.  
 */
void
persistent_object_manager::reset_for_loading(void) {
	const size_t max = reconstruction_table.size();
	size_t i = 1;		// 0th object is reserved NULL, skip it
	for ( ; i<max; i++) {
		reconstruction_table_entry& e = reconstruction_table[i];
		e.reset_addr();
		// shouldn't need to manipulate stream positions
	}
}

//=============================================================================
}	// end namespace util

