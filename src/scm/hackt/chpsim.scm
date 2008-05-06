;; "hackt/chpsim.scm"
;;	$Id: chpsim.scm,v 1.5 2008/05/06 17:43:14 fang Exp $
;; Scheme module for chpsim-specific functions (without trace file)
;; hackt-generic functions belong in hackt.scm, and
;; chpsim-trace specific functions belong in chpsim-trace.scm.

(define-module (hackt chpsim)
#:autoload (ice-9 streams) (stream-map stream-for-each)
#:autoload (ice-9 q) (make-q q-push! q-pop! q-empty?)
#:autoload (hackt streams) (stream-filter enumerate-interval-stream)
#:autoload (hackt rb-tree) (make-rb-tree rb-tree/insert!)
)

(use-modules (srfi srfi-1))		; for 'any'
(use-modules (hackt chpsim-primitives))	; defined in C++
; (use-modules (hackt streams))		; now autoloaded
; (use-modules (ice-9 streams))		; now autoloaded

; when we want confirmation:
; (display "Loading module: (hackt chpsim)") (newline)

(define root-event-id 0)

; constant: number of allocated events in static event graph
(define-public chpsim-num-events (hac:chpsim-num-events))

; (display "Enumerating events... ")
#!
"Stream of integers from 0 to (num-static-events -1)"
!#
(define-public chpsim-static-event-index-stream
  (enumerate-interval-stream root-event-id (1- chpsim-num-events))
)
; (display "done.") (newline)

(define (static-event-stream)
  "Represents the set of all statically allocated events as a stream."
  (stream-map (lambda (i) (hac:chpsim-get-event i))
    chpsim-static-event-index-stream)
) ; end define

; (display "Caching all-static-events-stream... ")
#!
"global stream variable, lazy evaluated stream, but this is not a delayed obj."
!#
(define-public all-static-events-stream (static-event-stream))
; (display "done.") (newline)

; this is only defined for chpsim modules, not hackt in general
(define-public (type-tag->offset t)
"Translates one of the symbolic type tags to an offset, keeping consistent
with the convention that type-binned structures follow the same ordering
of construction.  For example, 'bool -> 0, and 'int -> 1.
Primitive implementations *should* adhere to this ordering."
(cond 
	((eq? t 'bool) 0)
	((eq? t 'int) 1)
	((eq? t 'enum) 2)
	((eq? t 'channel) 3)
	(else (error "Unreconginzed symbolic type-tag."))
) ; end cond
) ; end define

; just a reverse map from offset to type-tag
(define-public type-tag-list '(bool int enum channel))

;; dispatch based on type-tag, returns a procedure object
(define-public (dependence-set-subset lst tag)
  "Dispatches a subset extractor based on tag @var{t}"
  (list-ref lst (type-tag->offset tag)) ; end cond
) ; end define

;; accessing type-specific subsets of dependence sets
(define-public (dependence-set-bools d)
  "Extracts the bool subset of dependencies."
  (dependence-set-subset d 'bool))
(define-public (dependence-set-ints d)
  "Extracts the integer subset of dependencies."
  (dependence-set-subset d 'int))
(define-public (dependence-set-enums d)
  "Extracts the enumeral subset of dependencies."
  (dependence-set-subset d 'enum))
(define-public (dependence-set-channels d)
  "Extracts the channel subset of dependencies."
  (dependence-set-subset d 'channel))


;; adds proper tags to construct (type, index)-pair references
(define-public (chpsim-event-may-block-deps ev)
  "Extracts the set of instances that event @var{ev} may block on."
  (define (attach-tag tag lst)
    (map (lambda (i) (cons tag i)) lst)
  ) ; end define
  ; rr is untagged set of indices, now we pair them up with type tags
  (let ((rr (hac:chpsim-event-may-block-deps-internal ev)))
    (map (lambda (tag) (attach-tag tag (dependence-set-subset rr tag)))
      type-tag-list) ; end map
; or equivalently
;    (list (attach-tag 'bool (dependence-set-bools rr))
;      (attach-tag 'int (dependence-set-ints rr))
;      (attach-tag 'enum (dependence-set-enums rr))
;      (attach-tag 'channel (dependence-set-channels rr))
;    ) ; end list
  ) ; end let
) ; end define

; in case the representation changes
(define-public static-event-node-index car)
(define-public static-event-raw-entry cdr)

; generic event filter, takes an arbitrary event predicate
(define-public (chpsim-filter-static-events pred? static-events-stream)
"Select only selection events out of static event stream.  
Argument is a stream of static events.
NOTE: this should really be a map, not a stream."
  (stream-filter (lambda (e) (pred? (static-event-raw-entry e)))
    static-events-stream)
) ; end define

; filters all selection events, deterministic and nondeterministic
(define-public (chpsim-filter-static-events-select static-events-stream)
"Select only selection events out of static event stream.  
Argument is a stream of static events.
NOTE: this should really be a map, not a stream."
  (chpsim-filter-static-events hac:chpsim-event-select? static-events-stream)
) ; end define

#!
"Find all events that may depend on a given channel (id)"
!#
(define-public (chpsim-find-events-involving-channel-id cid static-events-stream)
  (chpsim-filter-static-events
    (lambda (e)
      (any (lambda (i) (= i cid))
        (dependence-set-channels (hac:chpsim-event-may-block-deps-internal e)))
    ) ; end lambda
  static-events-stream) ; end filter
) ; end define

#!
"Memoized stream of all select events (branches, do-while)."
!#
(define-public static-events-selects-stream-delayed
  (delay (chpsim-filter-static-events-select all-static-events-stream)))

(define-public (chpsim-assoc-event-successors static-event-stream)
"Given a static event stream, produces a set of event-successor pairs 
(which look like lists).  The resulting list can be viewed as an associative 
list, with the key being the first element and the value being the rest (cdr).
NOTE: the successors are NOT sorted by index, thus retaining their positional
meaning from even graph construction."
  (stream-map (lambda (e)
    (cons (static-event-node-index e)
      (hac:chpsim-event-successors (static-event-raw-entry e))))
    static-event-stream)
) ; end define

(define successor-map-key car) ; in-source documentation!
(define successor-map-value-list cdr) ; in-source documentation!

#!
"Memoized map of predecessors, constructed from successors map 
to access this, use the (force), Luke."
!#
(define-public static-event-successors-map-delayed
  (delay
    (let ((succs-map (make-rb-tree = <)))
      (stream-for-each
        (lambda (e) (rb-tree/insert! succs-map
          (successor-map-key e) (successor-map-value-list e)
        )) ; end lambda
        (chpsim-assoc-event-successors all-static-events-stream)
      ) ; end stream-for-each
      succs-map
    ) ; end let
  ) ; end delay
) ; end define


#!
"Memoized map of predecessors, events whose edges are incident upon this event.
Predecessor map is constructed as a transpose of the successors map.  
Implemented as a tree-of-trees (map-of-maps) for efficient set membership tests
and set operations.  Use the (force) to evaluate this."
!#
(define-public static-event-predecessors-map-delayed
  (delay
    (let ((preds (make-rb-tree = <)))
      ; construct empty assoc trees for each event slot
      (stream-for-each
        (lambda (e) (rb-tree/insert! preds e (make-rb-tree = <)))
        chpsim-static-event-index-stream
      ) ; end stream-for-each
      (rb-tree/for-each
        (lambda (x) ; x is a key-value pair
          (for-each
            (lambda (y)
              (rb-tree/lookup-mutate! preds y
                (lambda (z)
                  (rb-tree/insert! z (successor-map-key x) '())
                z)
                #f)
            ) ; end lambda
            (successor-map-value-list x)
          ) ; end for-each
        ) ; end lambda
        (force static-event-successors-map-delayed) ; memoized!
      ) ; end stream-for-each
      preds
    ) ; end let
  ) ; end delay
) ; end define

(define-public (chpsim-assoc-event-pred-from-succ succ-assoc-strm)
"Construct predecessor associations given a stream of associated 
successor lists.  Each input stream element is an associated list of a 
static event index paired with its list of successors.  
May result in multiple associations among predecessors in returned list.  
Relies on fact that branch-successors must succeed only a single branch event.
This may become deprecated in favor of sorted, associative storage 
in a red-black tree."
  (stream-map
    (lambda (a)
; don't make rb-tree yet, just leave as stream of lists
      (map
        (lambda (b) (cons b (car a)))
        (cdr a)
      )
    ) ; end lambda
  succ-assoc-strm)
) ; end define

(define-public (chpsim-successor-lists->histogram succ-list)
"Constructs a zero-initialized associative histogram of successor events 
visited, based on the input list of associated successor lists.  
Implementation: map-of-maps, using rb-tree."
(let ((ret-histo (make-rb-tree = <)))
  (map
    (lambda (x)
      (rb-tree/insert! ret-histo (car x)
        (let ((sub-histo (make-rb-tree = <)))
          (for-each (lambda (y) (rb-tree/insert! sub-histo y 0)) (cdr x))
          sub-histo
        ) ; end let
      ) ; end cons
    ) ; end lambda
    succ-list
  ) ; end map, resulting in an alist
  ret-histo
) ; end let
) ; end define

#!
"Sorted set of all events that have multiple entry points (disjunctive), 
which includes loop-heads and branch-joins.
Delayed object is accessed using (force) and is memoized."
!#
(define-public static-events-with-multiple-entries-delayed
(delay
  (let ((ret-map (make-rb-tree = <))
        (preds (force static-event-predecessors-map-delayed)))
    (stream-for-each
      (lambda (e) (rb-tree/insert! ret-map (static-event-node-index e) '()))
      (stream-filter
        (lambda (x)
          (let ((i (static-event-node-index x))
                (e (static-event-raw-entry x)))
            (> (rb-tree/size (rb-tree/lookup preds i #f))
              (hac:chpsim-event-num-predecessors e))
          ) ; end let
        ) ; end lambda
        all-static-events-stream
      ) ; end stream-filter
    ) ; end stream-for-each
    ret-map
  ) ; end let
) ; end delay
) ; end define

; TODO: replace visited rb-tree with pre-allocated vector instead
(define-public (static-events-depth-first-walk-predicated thunk pred?)
"Predicated depth-first walk over static event nodes reachable from the root.
Predicate is tested on the current node to recurse conditionally."
  (let ((visited (make-rb-tree = <))
        (succs-map (force static-event-successors-map-delayed)))
    (let loop ((worklist (list root-event-id)))
      ; start with first top-level node
      (if (not (null? worklist))
        (let ((n (car worklist)))
          (if (not (rb-tree/lookup-key visited n #f))
            (begin
              (rb-tree/insert! visited n '()) ; mark
              (thunk n) ; apply
              (loop (append (if (pred? n) (rb-tree/lookup succs-map n #f) '())
                (cdr worklist)))
              ; for-each - lambda - loop ... is NOT tail recursive
;              (if (pred? n)
;                (for-each (lambda (s) (loop s))
;                  (rb-tree/lookup succs-map n #f))
;              ) ; end if
            ) ; end begin
            ; else keep processing worklist
            (loop (cdr worklist))
          ) ; end if
        ) ; end let
      ) ; end if
    ) ; end let loop
  ) ; end let
) ; end define

#!
"Depth-first walk, visits every reachable event node from the root once."
!#
(define-public (static-events-depth-first-walk thunk)
  (static-events-depth-first-walk-predicated thunk (lambda (x) #t))
) ; end define

#!
"thunk-back is action for back edges, expecting an edge represented as a pair.
Uses a two-level (list-of-list) worklist.
breadcumbs is the growing and shrinking stack of predecessors.  
This is searched linearly (unordered) to detect back edges.  
When *car* of worklist is empty, backtrack *iteratively*.
Iteration has the advantage of taking constant stack space.
Recursive DFS is not tail-recursive, hence this alternative.
Note: breadcrumbs is just the car's of the worklist-of-lists."
!#
(define-public (static-events-depth-first-walk-iterative thunk-node thunk-back)
  (let ((visited (make-vector chpsim-num-events #f))
        (succs-map (force static-event-successors-map-delayed)))
    (let loop ((breadcrumbs '())
               (worklist (list (list root-event-id))))
; (display "crumbs: ") (display breadcrumbs) (display ", ")
; (display "worklist: ") (display worklist) (newline)
      (if (not (null? worklist))
        (let ((l (car worklist))
              (r (cdr worklist))) ; is a list!
          (if (null? l)
            ; tail call, return up, remove last predecessor
            (if (not (null? r))
              (loop (cdr breadcrumbs) (cons (cdar r) (cdr r))))
            ; else we still have outgoing edge in last list
            (let ((n (car l)))
              (if (vector-ref visited n)
                (begin
                  ; then detect cycle
                  (if (any (lambda (x) (= x n)) breadcrumbs) ; O(n) search :S
                      ; long loops will have O(n^2) time :S
                      (thunk-back (cons (caadr worklist) n)) ; cadr is prev!
                    ; else this is a forward-edge, do nothing
                  ) ; end if
                  (loop breadcrumbs (cons (cdr l) (cdr worklist)))
                ) ; end begin
                ; else not already visited
                (begin
                  (vector-set! visited n #t) ; mark
                  (thunk-node n)
                  (loop (cons n breadcrumbs)
                    (cons (rb-tree/lookup succs-map n #f) ; unpredicated
                      worklist))
                ) ; end if-else
              ) ; end if
            ) ; end let
          ) ; end if
        ) ; end let
      ) ; else we're done
    ) ; end let loop
  ) ; end let loop
) ; end define

(define (static-events-depth-first-walk-with-trace thunk)
"Similar to walk, but also retains a stack-set of node, like Ariadne's thread.
The procedure thunk should expect a node-index argument and the a recursive
loop procedure that expects a node-index argument."
  (let ((visited (make-rb-tree = <)) ; ever visited
        ; (visit-stack (make-rb-tree = <)) ; visit stack (balanced)
        (visit-stack '()) ; visit stack (balanced)
       )
    (let loop ((n root-event-id))
      (if (not (rb-tree/lookup-key visited n #f))
        (begin
          (rb-tree/insert! visited n '()) ; mark
          ; (rb-tree/insert! visit-stack n '()) ; mark
          ; (thunk n loop visited visit-stack)
          (thunk n loop visited (cons n visit-stack))
          ; (rb-tree/delete! visit-stack n) ; undo trail
        ) ; end begin
      ) ; end if
    ) ; end let
  ) ; end let
) ; end define



(define static-do-while-bound-events-delayed
(delay
  (let ((succs-map (force static-event-successors-map-delayed))
        ; (all-do-whiles (make-rb-tree = <))
        (do-while-backs (make-rb-tree = <))
        (do-while-heads (make-rb-tree = <))
        ; (visited (make-rb-tree = <))
        ; (do-while-stack '(root-event-id))
       )
    ; one pass over all events
    (stream-for-each
      (lambda (ev) 
        (let* ((n (static-event-node-index ev))
              (this-succs (rb-tree/lookup succs-map n #f)))
          (for-each
            (lambda (s)
              (if (hac:chpsim-event-do-while?
                    (static-event-raw-entry (hac:chpsim-get-event s)))
                (begin
                  ; invariant: head-tails are 1-1 mapping
                  (rb-tree/insert! do-while-backs n s)
                  (rb-tree/insert! do-while-heads s n)
                )
              )
            ) ; end lambda
          this-succs)
        )
      )
    all-static-events-stream)
    (cons do-while-heads do-while-backs)
  ) ; end let
) ; delay
)

#!
"Events that are the start of do-while loops, set is a red-black tree."
!#
(define-public static-do-while-head-events-delayed
  (delay (car (force static-do-while-bound-events-delayed))))

#!
"Events that jump to heads of do-while do-whiles, set is a red-black tree."
!#
(define-public static-do-while-tail-events-delayed
  (delay (cdr (force static-do-while-bound-events-delayed))))


#!
"Produce a pair of re-numeration maps (vectors) to translate to/from 
toplogically sorted event nodes.  This is useful for efficient back-edge 
and cycle detection.  Returns a pair of vector-maps (event->topo, topo->event)."
!#
(define-public topological-sort-map-events-delayed
(delay
  (let ((counter 0)
        (succs-map (force static-event-successors-map-delayed))
        (event->topo (make-vector chpsim-num-events))
        (topo->event (make-vector chpsim-num-events)))
        ; reminder: vectors are 0-indexed
    (static-events-depth-first-walk
      (lambda (n)
        (let ((e (static-event-raw-entry (hac:chpsim-get-event n)))
              (this-succs (rb-tree/lookup succs-map n #f)))
;          (display counter) (display " <-> ") (display n) (newline)
          (vector-set! event->topo n counter)
          (vector-set! topo->event counter n)
          (set! counter (1+ counter))
        ) ; end let
      ) ; end lambda
    ) ; end walk
;    (display "event->topo:") (newline)
;    (display event->topo) (newline)
;    (display "topo->event:") (newline)
;    (display topo->event) (newline)
    (cons event->topo topo->event)
  ) ; end let
) ; end delay
) ; end 

#!
"Find set of loopback events with DFS.
Doesn't need to visit all nodes, only one path per branch/fork because
they reconverge.  For do-while, only the else clause is followed.
The result is a pair of maps, matching loop head-to-tail and vice versa."
!#
(define static-loop-bound-events-delayed
(delay
  (let ((event->topo (car (force topological-sort-map-events-delayed)))
        (succs-map (force static-event-successors-map-delayed))
        (loop-backs (make-rb-tree = <))
        (loop-heads (make-rb-tree = <))
        ; (visited (make-rb-tree = <))
       )
    (static-events-depth-first-walk-iterative
      (lambda (n) #f) ; do nothing
      (lambda (p)
        (let ((n (car p)) (s (cdr p)))
          (rb-tree/insert! loop-backs n s)
          (rb-tree/insert! loop-heads s n)
        ) ; end let
      ) ; end lambda
    ) ; end walk
    (cons loop-heads loop-backs)
  ) ; end let
) ; delay
) ; end define

#!
"Events that are the start of forever loops, set is a red-black tree."
!#
(define-public static-loop-head-events-delayed
  (delay (car (force static-loop-bound-events-delayed))))

#!
"Events that jump to heads of forever loops, set is a red-black tree."
!#
(define-public static-loop-tail-events-delayed
  (delay (cdr (force static-loop-bound-events-delayed))))


(define-public (chpsim-event-loop-head? id)
"Is event id @var{id} a loop-head?"
  (rb-tree/lookup (force static-loop-head-events-delayed) id #f)
)

(define-public (chpsim-event-loop-tail? id)
"Is event id @var{id} a loop-tail? (successor is loop-head)"
  (rb-tree/lookup (force static-loop-tail-events-delayed) id #f)
)

(define static-branch-bound-events-delayed
(delay
  (let ((preds-map (force static-event-predecessors-map-delayed))
;       (succs-map (force static-event-successors-map-delayed))
        (branch-stack (make-q))
        (branch-heads (make-rb-tree = <))
        (branch-tails (make-rb-tree = <)) ; reverse-map
       )
    (static-events-depth-first-walk
      (lambda (n)
        (let ((e (static-event-raw-entry (hac:chpsim-get-event n))))
          (cond ((hac:chpsim-event-branch? e) (q-push! branch-stack n))
            ((and (> (rb-tree/size (rb-tree/lookup preds-map n #f)) 1)
                (= (hac:chpsim-event-num-predecessors e) 1)
                (not (q-empty? branch-stack)) ; not loop-head, basically
                (not (hac:chpsim-event-do-while? e)))
              (let ((bh (q-pop! branch-stack)))
                (rb-tree/insert! branch-heads bh n)
                (rb-tree/insert! branch-tails n bh)
              )) ; end let, end case
            ; else noop
          ) ; end cond
        ) ; end let
      ) ; end lambda
    ) ; end walk
    (cons branch-heads branch-tails)
  ) ; end let
) ; end delay
) ; end define


#!
"Events that are branches, mapped to their respective branch-ends."
!#
(define-public static-branch-head-tail-map-delayed
  (delay (car (force static-branch-bound-events-delayed))))

#!
"Events that are branch-ends mapped to their respective branch-heads."
!#
(define-public static-branch-tail-head-map-delayed
  (delay (cdr (force static-branch-bound-events-delayed))))

(define-public (chpsim-event-branch-head? id)
"Is event id @var{id} a branch(-head)? (like hac:chpsim-event-branch?)"
  (rb-tree/lookup (force static-branch-head-tail-map-delayed) id #f)
)

(define-public (chpsim-event-branch-tail? id)
"Is event id @var{id} a branch-tail? (end-of-branch join)"
  (rb-tree/lookup (force static-branch-tail-head-map-delayed) id #f)
)

#!
"Evaluates fork-join pairs maps."
!#
(define static-fork-join-events-delayed
(delay
  (let ((preds-map (force static-event-predecessors-map-delayed))
        (fork-stack (make-q))
        (fork-heads (make-rb-tree = <))
        (fork-joins (make-rb-tree = <)) ; reverse-map
       )
    (static-events-depth-first-walk
      (lambda (n)
        (let ((e (static-event-raw-entry (hac:chpsim-get-event n))))
          (cond ((hac:chpsim-event-fork? e) (q-push! fork-stack n))
            ((hac:chpsim-event-join? e)
              (let ((jh (q-pop! fork-stack)))
                (rb-tree/insert! fork-heads jh n)
                (rb-tree/insert! fork-joins n jh)
              )) ; end let, end case
            ; else noop
          ) ; end cond
        ) ; end let
      ) ; end lambda
    ) ; end walk
    (cons fork-heads fork-joins)
  ) ; end let
) ; end delay
) ; end define

(define-public static-fork-join-map-delayed
  (delay (car (force static-fork-join-events-delayed))))

(define-public static-join-fork-map-delayed
  (delay (cdr (force static-fork-join-events-delayed))))


#!
"Sorted set of do-while loop events."
!#
(define-public static-do-while-events-delayed
(delay
  (let ((do-whiles (make-rb-tree = <)))
    (stream-for-each
      (lambda (n)
        (let (
               (e (static-event-raw-entry n))
               ; (re (hac:chpsim-get-event n))
              )
          (if (hac:chpsim-event-do-while? e)
            (rb-tree/insert! do-whiles (static-event-node-index n) '())
            ; else ignore
          ) ; end if
        ) ; end let
      ) ; end lambda
      all-static-events-stream
    ) ; end stream-for-each
    do-whiles
  ) ; end let
) ; end delau
) ; end define

