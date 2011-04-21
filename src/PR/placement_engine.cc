/**
	\file "PR/placement_engine.cc"
	$Id: placement_engine.cc,v 1.1.2.8 2011/04/21 01:32:13 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include "PR/placement_engine.h"
#include "PR/tile_type.h"
#include "PR/pr_utils.h"
#include "common/TODO.h"
#include "util/vector_ops.h"
#include "util/array.tcc"
#include "util/indent.h"
#include "util/string.h"
#include "util/numeric/random.h"
#include "util/IO_utils.tcc"
#include "util/iterator_more.h"
#include "util/iomanip.h"
#include "util/value_saver.h"
#include "util/stacktrace.h"

namespace PR {
using std::for_each;
using std::transform;
using std::lower_bound;
using std::upper_bound;
using std::sort;
using std::distance;
using util::auto_indent;
using util::save_precision;
using util::strings::string_to_num;
using util::numeric::rand48;
using util::write_value;
using util::read_value;
using util::value_saver;
using namespace util::vector_ops;		// for many operator overloads
#include "util/using_ostream.h"

//=============================================================================
// class placement_engine method definitions

placement_engine::placement_engine(const size_t d) :
		state_base(), 
		object_types(),
		channel_types(),
		opt(),
		space(d),
#if !PR_LOCAL_PROXIMITY_CACHE
		proximity_cache(),
#endif
		elapsed_time(0.0), 
#if 0
		max_delta_position(0.0),
		max_delta_velocity(0.0),
#endif
		autosave_name() {
	initialize_default_types();
#if !PR_LOCAL_PROXIMITY_CACHE
	proximity_cache.reserve(64);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
placement_engine::~placement_engine() {
if (autosave_name.size()) {
	std::ofstream o(autosave_name.c_str());
	if (o) {
	try {
		save_checkpoint(o);
	} catch (...) {
		cerr << "Fatal error during checkpoint save." << endl;
	}
	} else {
		cerr << "Error opening \'" << autosave_name <<
			"\' for saving checkpoint." << endl;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::initialize_default_types(void) {
	object_types.clear();
	object_types.reserve(64);
	object_types.push_back(tile_type());
	channel_types.clear();
	channel_types.reserve(64);
	channel_types.push_back(channel_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::auto_proximity_radius(void) {
	opt.proximity_radius = space.auto_proximity_radius();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_object_type(const tile_type& t) {
	object_types.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_channel_type(const channel_type& t) {
	channel_types.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_object(const tile_instance& o) {
	space.objects.push_back(o);
	// automatically update proximity radius
	const real_type rad =
		space.objects.back().properties.maximum_dimension();
	if (rad > opt.proximity_radius)
		opt.proximity_radius = rad;
}

#define	CHECK_OBJECT_INDEX(i)						\
	if (i >= space.objects.size()) {				\
		cerr << "Error: object index out-of-bounds." << endl;	\
		return true;						\
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check that object indices are in-bounds.
	\return true on error
 */
bool
placement_engine::add_channel(const channel_instance& c) {
	CHECK_OBJECT_INDEX(c.source)
	CHECK_OBJECT_INDEX(c.destination)
	const tile_instance& s(space.objects[c.source]);
	const tile_instance& d(space.objects[c.destination]);
	// uses spherical approximation of objects for now,
	// could be ellipsoid vector...
	space.springs.push_back(c);
	space.springs.back().properties.equilibrium_distance =
		(s.properties.maximum_dimension() +
			d.properties.maximum_dimension()) * 0.5;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::pin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].fix();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::unpin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].unfix();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::place_object(const size_t i, const real_vector& v) {
	CHECK_OBJECT_INDEX(i)
	real_vector p(v);
	opt.clamp_position(p);
	space.objects[i].place(p);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_parameters(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "parameters:\n";
	INDENT_SECTION(o);
	o << auto_indent << "@time=" << elapsed_time << endl;
	opt.dump_parameters(o);
#if 0
	o << auto_indent << "max_delta_position=" << max_delta_position << endl;
	o << auto_indent << "max_delta_velocity=" << max_delta_velocity << endl;
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Place all free objects somewhere randomly within the bounding box.
 */
void
placement_engine::scatter(void) {
	STACKTRACE_VERBOSE;
	const real_vector box_size(opt.upper_corner -opt.lower_corner);
	typedef	rand48<double>			random_generator;
	const random_generator g;
	vector<tile_instance>::iterator
		i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
	if (!i->is_fixed()) {
		real_vector r;
		// fixed for 3D
		r[0] = g();
		r[1] = g();
		r[2] = g();
		r *= box_size;
		r += opt.lower_corner;
		i->place(r);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::zero_forces(void) {
	for_each(space.objects.begin(), space.objects.end(), 
		std::mem_fun_ref(&tile_instance::zero_force));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Should return some information about this step for 
	determining convergence.
	TODO: compute effective force vector, accumulate per object
	TODO: support different spring types
 */
void
placement_engine::compute_spring_forces(void) {
	STACKTRACE_VERBOSE;
	// compute spring tensions (attraction)
	typedef	vector<channel_instance>::iterator	iterator;
	iterator i(space.springs.begin()), e(space.springs.end());
	for ( ; i!=e; ++i) {
		const int_type& si(i->source);
		const int_type& di(i->destination);
		tile_instance& sobj(space.objects[si]);
		tile_instance& dobj(space.objects[di]);
		i->potential_energy =
			tile_instance::apply_attraction_forces(
				sobj, dobj, i->properties);
	}	// end for each spring
}	// end compute_spring_forces
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
static
bool
dim_less(const vector<tile_instance>::const_iterator l,
		const vector<tile_instance>::const_iterator r) {
	return l->position[D] < r->position[D];
}

template <size_t D>
static
bool
dim_comp(const vector<tile_instance>::const_iterator l, const real_type& r) {
	return l->position[D] < r;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This could go to util lib.
	\param T is an iterator type, preferably random_access.
 */
template <class T>
struct array_offset {
	typedef	T		iterator_type;
	const iterator_type		begin;

	explicit
	array_offset(const T i) : begin(i) { }

	size_t
	operator () (const T i) const {
		return distance(begin, i);
	}

};	// end struct array_offset

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really want to avoid this.
 */
void
placement_engine::clear_proximity_cache(void) {
#if PR_LOCAL_PROXIMITY_CACHE
	for_each(space.objects.begin(), space.objects.end(),
		std::mem_fun_ref(&tile_instance::clear_proximity_cache));
#else
	proximity_cache.clear();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update dynamic graph of near-neighbors.  
	Use a sliding-window cube in each dimension.
	TODO: maintain order as object positions are updated
		and only *incrementally* sort.
	Or do sector binning, near constant time!
 */
void
placement_engine::refresh_proximity_cache(void) {
	STACKTRACE_VERBOSE;
	clear_proximity_cache();		// wipe before recomputing
	// sort by each dimension/index
	typedef	vector<tile_instance>::iterator		iterator;
	vector<iterator> obj_x;
	obj_x.reserve(space.objects.size());
	iterator i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
		obj_x.push_back(i);
	}
	// perform in-place sort on each
	// first, sorted by x-coordinates only
	sort(obj_x.begin(), obj_x.end(), &dim_less<0>);
	// use proximity_radius to find sets of objects in the same window
	vector<iterator>::iterator xb(obj_x.begin()), xe(obj_x.end());
	// Q: is binary search worth it? linear-incremental may suffice
	// A: linear! b/c overall cost O(N) vs. O(N lg N)
	vector<iterator>::iterator xu(xb);
	const array_offset<iterator> vo(space.objects.begin());
	const size_t i1 = vo(*xb);
	const tile_instance& o1(space.objects[i1]);
#if 0
	std::set<size_t> window_set_x;	// a queue is good enough?
	transform(xb, xu, util::set_inserter(window_set_x), vo);
 	INVARIANT(window_set_x.size() >= 1);
#endif
	// x-sweep
for ( ; xb!=xe; ++xb) {
	const real_type x = (*xb)->position[0];
	// [xb, xu] defines a sliding window along the x-dimension
	// linear scan overall costs less than repeated (lg N) binary searches
	while (xu!=xe && (*xu)->position[0] < x+opt.proximity_radius) {
		++xu;
	}
const size_t xw_size = distance(xb, xu);
if (xw_size > 1) {
	STACKTRACE_INDENT_PRINT("have " << xw_size <<
		" x-collision candidates." << endl);
	// recurse in y-dimension, z-dimensions
	// reduce size of window_set in each dimension
	// copy range of iterators, and re-sort by y-dimension
	vector<iterator> obj_y(xb, xu);
	sort(obj_y.begin(), obj_y.end(), &dim_less<1>);
	const real_type& y_ref((*xb)->position[1]);
	const vector<iterator>::iterator
		yb(obj_y.begin()), ye(obj_y.end());
	const vector<iterator>::iterator
		yl(lower_bound(yb, ye, y_ref-opt.proximity_radius, &dim_comp<1>)),
		yu(lower_bound(yl, ye, y_ref+opt.proximity_radius, &dim_comp<1>));
	const size_t yw_size = distance(yl, yu);
	if (yw_size > 1) {
		STACKTRACE_INDENT_PRINT("have " << yw_size <<
			" xy-collision candidates." << endl);
		// copy range of iterators, re-sort by z-dimension
		vector<iterator> obj_z(yl, yu);
		sort(obj_z.begin(), obj_z.end(), &dim_less<2>);
		const real_type& z_ref((*xb)->position[2]);
		const vector<iterator>::iterator
			zb(obj_y.begin()), ze(obj_y.end());
		const vector<iterator>::iterator
			zl(lower_bound(zb, ze, z_ref-opt.proximity_radius,
				&dim_comp<2>)),
			zu(lower_bound(zl, ze, z_ref+opt.proximity_radius,
				&dim_comp<2>));
		const size_t zw_size = distance(zl, zu);
		if (zw_size > 1) {
			STACKTRACE_INDENT_PRINT("have " << zw_size <<
				" xyz-collision candidates." << endl);
			// then we are within proximity
			vector<iterator>::iterator zi(zl);
			for ( ; zi!=zu; ++zi) {
			if (*zi != *xb) {
				const size_t i2 = vo(*zi);
				const tile_instance& o2(space.objects[i2]);
				STACKTRACE_INDENT_PRINT(
					"caching collision candidate pair ("
					<< i1 << ',' << i2 << ")." << endl);
				INVARIANT(i1 != i2);
				// avoid double counting with index ordering
#if PR_LOCAL_PROXIMITY_CACHE
				if (i1 < i2) {
					o1.proximity_cache.insert(i2);
				} else {
					o2.proximity_cache.insert(i1);
				}
#else
				if (i1 < i2) {
					proximity_cache.push_back(
						proximity_edge(i1, i2));
				} else {
					proximity_cache.push_back(
						proximity_edge(i2, i1));
				}
				channel_instance& pe(proximity_cache.back());
				pe.properties.spring_coeff =
					opt.repulsion_coeff;
				pe.properties.equilibrium_distance = 
					(o2.properties.maximum_dimension()
					+o1.properties.maximum_dimension())
					*0.5;
#endif
			}
			}	// end for xyz-slice candidates
		}	// end if z-window
	}	// end if y-window
}	// else no collision candidates
}	// end for each x-ordered object
}	// end placement_engine::refresh_proximity_cache

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compute repulsive forces between near-neighbors.  
 */
void
placement_engine::compute_collision_forces(void) {
	STACKTRACE_VERBOSE;
	proximity_potential_energy = 0.0;
	typedef	vector<tile_instance>::iterator		iterator;
	iterator i(space.objects.begin()), e(space.objects.end());
	const array_offset<iterator> vo(i);
#if PR_LOCAL_PROXIMITY_CACHE
	for ( ; i!=e; ++i) {
		const size_t j1 = vo(i);
		set<int_type>::const_iterator
			ci(i->proximity_cache.begin()),
			ce(i->proximity_cache.end());
		tile_instance& o1(space.objects[j1]);
		for ( ; ci!=ce; ++ci) {
			const size_t j2 = *ci;
#else
	vector<proximity_edge>::iterator
		pi(proximity_cache.begin()), pe(proximity_cache.end());
	for ( ; pi!=pe; ++pi) {
		const size_t& j1(pi->source);
		const size_t& j2(pi->destination);
			tile_instance& o1(space.objects[j1]);
#endif
			// avoid double counting
			INVARIANT(j1 < j2);
			STACKTRACE_INDENT_PRINT("repelling objects " <<
				j1 << " and " << j2 << endl);
			tile_instance& o2(space.objects[j2]);
			pi->potential_energy =
				tile_instance::apply_repulsion_forces(
					o1, o2, pi->properties);
			proximity_potential_energy += pi->potential_energy;
#if PR_LOCAL_PROXIMITY_CACHE
		}	// end for each outgoing edge in local cache
	}	// end for each object/instance
#else
	}	// end for each proximity_edge
#endif
	proximity_potential_energy *= 0.5;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::iterate(void) {
	STACKTRACE_VERBOSE;
	zero_forces();		// reset forces
// begin parallel
	refresh_proximity_cache();
	space.compute_spring_forces();
// end parallel
	// compute proximity repulsions on 'close' objects
	compute_collision_forces();
	// after all forces applied, update position
	space.update_objects(opt);
	// TODO: enforce bounds on object positions: clamp_position
	// energy reported is for *previous* iteration
	if (opt.watch_energy) {
		const real_type T = kinetic_energy();
		const real_type V = potential_energy();
		cout << "T=" << T << ", V=" << V <<
			", E=" << T+V << ", L=" << T-V << '\n' << endl;
		// total energy, Lagrangian
	}
	elapsed_time += opt.time_step;	// increment time
	if (opt.watch_anything()) {
		cout << "@time=" << elapsed_time << endl;
	}
	if (opt.watch_objects) {
		const value_saver<bool>
			_x_(tile_instance::dump_properties, false);
		dump_objects(cout);
	}
	if (opt.watch_deltas) {
#if 0
		cout << "delta-position=" << max_delta_position <<
			", delta-velocity=" << max_delta_velocity << endl;
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Maintaining a constant time_step, 
	iterate until one of the following is true:
	1) change in position/velocity falls below convergence thresholds.
	2) no forward progess has been made for some number of iterations.
 */
void
placement_engine::simple_converge(void) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\param dt time step size
	\param ptol position tolerance, below which is considered converged
	\param ftol force tolerance, below which is considered converged
	\return the number of iterations advanced until convergence.
 */
int_type
placement_engine::solve(void) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
static
ostream&
__dump_array(ostream& o, const T& a) {
	typedef	typename T::const_iterator	const_iterator;
	INDENT_SECTION(o);
	const_iterator i(a.begin()), e(a.end());
	size_t j;
	for (j=0; i!=e; ++i, ++j) {
		i->dump(o << auto_indent << '[' << j << "]: ") << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_object_types(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "object types:" << endl;
	return __dump_array(o, object_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channel_types(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "channel types:" << endl;
	return __dump_array(o, channel_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_objects(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "objects:" << endl;
	return __dump_array(o, space.objects);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channels(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "channels:" << endl;
	return __dump_array(o, space.springs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump(ostream& o) const {
	const save_precision p(o, opt.precision);
	dump_parameters(o);
	dump_object_types(o);
	dump_channel_types(o);
	dump_objects(o);
	dump_channels(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// identifier string for checkpoints
static
const string magic_string("hackt-ipple-ckpt");

// bump this whenever file format is updated
static
const size_t	checkpoint_version = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves entire session to be restored later (checkpoint).
 */
bool
placement_engine::save_checkpoint(ostream& o) const {
	write_value(o, magic_string);
	write_value(o, checkpoint_version);
	util::numeric::write_seed48(o);
	opt.save_checkpoint(o);
	write_value(o, elapsed_time);
// write object types
	save_array(o, object_types);
// write channel types
	save_array(o, channel_types);
// write objects and channels
	space.save_checkpoint(o);

#if 0
	write_value(o, max_delta_position);
	write_value(o, max_delta_velocity);
#endif

	write_value(o, magic_string);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::load_checkpoint(istream& i) {
	static const char bad_ckpt[] =
		"ERROR: not a valid ipple checkpoint file.";
try {
	string header_check;
	read_value(i, header_check);
	if (header_check != magic_string) {
		cerr << bad_ckpt << endl;
		return true;
	}
	size_t version_check;
	read_value(i, version_check);
	if (version_check != checkpoint_version) {
		cerr << "Expecting checkpoint compatibility version " <<
			checkpoint_version << ", but got " <<
			version_check << "." << endl;
		return true;
	}
} catch (...) {
	cerr << bad_ckpt << endl;
	return true;
}
	util::numeric::read_seed48(i);
	opt.load_checkpoint(i);
	read_value(i, elapsed_time);
// read object types
	load_array(i, object_types);
// read channel types
	load_array(i, channel_types);
// read objects and channels
	space.load_checkpoint(i);

#if 0
	read_value(i, max_delta_position);
	read_value(i, max_delta_velocity);
#endif
{
	string temp;
	read_value(i, temp);
	if (temp != magic_string) {
		cerr << "ERROR: detected checkpoint misalignment!" << endl;
		return true;
	}
}
	return !i;
}

// TODO: dump_checkpoint

//=============================================================================
}	// end namespace PR

