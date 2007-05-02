;; "hackt/chpsim-trace.h"
;;	$Id: chpsim-trace.scm,v 1.2.2.1 2007/05/02 00:44:54 fang Exp $
;; Interface to low-level chpsim trace file manipulators.  
;;

;; 31.3.4 Module System Quirks
;; ---------------------------
;; 
;; Although the programming interfaces are relatively stable, the Guile
;; module system itself is still evolving.  Here are some situations where
;; usage surpasses design.
;; 
;;   * When using a module which exports a macro definition, the other
;;     module must export all bindings the macro expansion uses, too,
;;     because the expanded code would otherwise not be able to see these
;;     definitions and issue a "variable unbound" error, or worse, would
;;     use another binding which might be present in the scope of the
;;     expansion.
;;
;; in other words, we have to export the imports.  :S

(define-module (hackt chpsim-trace)
#:autoload (srfi srfi-1) (any)
#:autoload (hackt algorithm) (find-assoc-ref accumulate)
#:autoload (hackt rb-tree)
  (make-rb-tree rb-tree/insert! rb-tree/lookup rb-tree/lookup-mutate!)
;;	#:export (make-chpsim-trace-stream
		;; we need to export what we use, how inconvenient (note above)
		;; the following are defined by guile/chpsim-wrap.cc
;;		foobar
;;	) ;; end export
;;	#:re-export ( current-trace-entry) ;; end re-export
) ;; end define

;; we provided this in C++
;; (use-modules (hackt hackt-primitives))
(use-modules (hackt chpsim-trace-primitives))
(use-modules (hackt chpsim-primitives)) ; for hac:lookup-trace-entry
(use-modules (ice-9 streams))
(use-modules (hackt streams))
(use-modules (hackt hackt)) ; for reference-equal?
(use-modules (hackt chpsim)) ; for type-tag->offset, all-static-events-stream

;; TODO: use symbolic dispatch

;; open-chpsim-trace is defined as a primitive in guile/chpsim-wrap.cc
;; use it to create a stream to pass to as a trace-stream argument.  
;; \param trace-stream an already opened tracefile, ready to stream entries
;; Make sure we return a proper end-of-stream marker '() eventually
;; test error handling here.
(define-public (make-chpsim-trace-stream trace-stream)
"Creates a trace stream from an opened trace file (smob)."
;; if stream is still valid, note: only evaluate current-trace-entry ONCE!
  (make-stream
    (lambda (s) 
      (let ((p (hac:current-trace-entry s)))
        (if (null? p) '()
	  (cons p s)
	) ; end if
      ) ; end let
    ) ; end lambda
    trace-stream
  ) ; end make-stream
) ; end define

; same, but opens a reverse trace stream
(define-public (make-chpsim-trace-reverse-stream trace-stream)
"Creates a trace reverse stream from an opened trace file (smob)."
;; if stream is still valid, note: only evaluate current-trace-entry ONCE!
  (make-stream
    (lambda (s) 
      (let ((p (hac:current-trace-reverse-entry s)))
        (if (null? p) '()
	  (cons p s)
	) ; end if
      ) ; end let
    ) ; end lambda
    trace-stream
  ) ; end make-stream
) ; end define

(define-public (make-chpsim-state-trace-stream trace-stream)
"Creates a state-change trace stream from an opened trace file (smob)."
;; if stream is still valid, note: only evaluate current-trace-entry ONCE!
  (make-stream
    (lambda (s) 
      (let ((p (hac:current-state-trace-entry s)))
        (if (null? p) '()
	  (cons p s)
	) ; end if
      ) ; end let
    ) ; end lambda
    trace-stream
  ) ; end make-stream
) ; end define

; TODO: use symbolic representation of stream type to dispatch accordingly
(define-public (current-trace-entry strm)
"Grabs the current trace stream entry, based on the stream type of @var{strm}."
  (cond ((hac:chpsim-trace? strm)
		(hac:current-trace-entry strm))
	((hac:chpsim-trace-reverse? strm)
		(hac:current-trace-reverse-entry strm))
	((hac:chpsim-state-trace? strm)
		(hac:current-state-trace-entry strm))
	(else (error "Unregistered trace stream type: " strm))
  ) ; end cond
) ; end define

;; convenient combined definition
(define-public (open-chpsim-trace-stream tf)
  "Opens the named trace file and returns a event stream interface 
in one fell swoop."
  (make-chpsim-trace-stream (hac:open-chpsim-trace tf))
) ; end define

(define-public (open-chpsim-trace-reverse-stream tf)
  "Opens the named trace file and returns a reverse-stream interface 
in one fell swoop."
  (make-chpsim-trace-reverse-stream (hac:open-chpsim-trace-reverse tf))
) ; end define

;; convenient combined definition
(define-public (open-chpsim-state-trace-stream tf)
  "Opens the named trace file and returns a state-change stream interface 
in one fell swoop."
  (make-chpsim-state-trace-stream (hac:open-chpsim-state-trace tf))
) ; end define


;; These struct accessors must be kept consistent with the construct
;; used in guile/chpsim-wrap.cc's wrap_chpsim_trace_entry_to_scm.
;; Later, thes may be replaced with primitive implementations if needed.  
(define-public (chpsim-trace-entry-index e)
  "Extracts the trace-entry's event index." (car e))
(define-public (chpsim-trace-entry-time e)
  "Extracts the trace-entry's event timestamp." (cadr e))
(define-public (chpsim-trace-entry-event e)
  "Extracts the trace-entry's static event index" (caddr e))
(define-public (chpsim-trace-entry-critical e)
  "Extracts the trace-entry's last arriving event index." (cdddr e))


;; This section pertains to the state-change stream
(define-public (chpsim-state-trace-entry-index s)
  "Extracts the state-trace-entry's event index." (car s))
(define-public (chpsim-state-trace-entry-subset s tag)
"General interface to extracting a typed subset of modified state variables
from a state-change stream, with @var{tag} being a type symbol, such as 'int.  
Internally, we add 1 to the offset because the first position of the entry is 
the event index."
  (list-ref s (1+ (type-tag->offset tag)))
) ; end define

(define-public (chpsim-state-trace-entry-bools s)
  "Extracts the bool subset of modified variables from state-trace."
  (chpsim-state-trace-entry-subset s 'bool)
)
(define-public (chpsim-state-trace-entry-ints s)
  "Extracts the int subset of modified variables from state-trace."
  (chpsim-state-trace-entry-subset s 'int)
)
(define-public (chpsim-state-trace-entry-enums s)
  "Extracts the snum subset of modified variables from state-trace."
  (chpsim-state-trace-entry-subset s 'enum)
)
(define-public (chpsim-state-trace-entry-channels s)
  "Extracts the channel subset of modified variables from state-trace."
  (chpsim-state-trace-entry-subset s 'channel)
)

(define-public (chpsim-state-trace-filter-reference s rpair)
"Filters out a state-change trace stream @var{s} to only events that modify a 
particular reference, @var{rpair}, a type-index pair, e.g. '(int . 4).  
The stream still retains information for other changes made in the same event, 
so later processing may choose to filter them out.  The resulting stream is
just a subset of the input."
  (stream-filter
    (lambda (t)
	; (car e) refers to the reference of the reference-value pair
	; OPTIMIZATION: since we've already filtered for matching types
	; we really only need to compare just the index of the pairs.
      (any (lambda (e) (reference-equal? (car e) rpair))
        (chpsim-state-trace-entry-subset t (reference-type rpair))
      ) ; end any
    ) ; end lambda
  s) ; end stream-filter
) ; end define

(define-public (chpsim-state-trace-focus-reference s rpair)
"Not only filters out, but *remaps* the event stream to focus only on the 
referenced variable @var{rpair} (dropping other changes in the same event).  
The output stream retains the event-index in car. "
(stream-map
  (lambda (x)
    (cons (chpsim-state-trace-entry-index x)
	; same OPTIMIZATION comment as in -filter-reference
      (let ((p (filter (lambda (e) (reference-equal? (car e) rpair))
          (chpsim-state-trace-entry-subset x (reference-type rpair))
        ) ; end filter
        ))
        (if (null? p) '() (car p))
	; there can be only one! (or none)
      ) ; end let
    ) ; end cons
  ) ; end lambda
s) ; end stream-map
) ; end define

(define-public (chpsim-state-trace-single-reference s rpair)
"Combines chpsim-state-trace-filter-reference and 
chpsim-state-trace-focus-reference into a single call, 
resulting a stream of events that change a single referenced variable
with their repective values."
(chpsim-state-trace-focus-reference
  (chpsim-state-trace-filter-reference s rpair)
  rpair)
) ; end define

; not exported (yet), but can be
(define (chpsim-trace-critical-path-iterator rand-trace entry)
"Returns the index of the next critical event in the trace."
  (hac:lookup-trace-entry rand-trace (chpsim-trace-entry-critical entry))
)

;;;;;;;;;;; CRITICAL PATH ANALYSIS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define-public (chpsim-trace-critical-path-from rand-trace ev)
"Constructs a critical path stream starting from event index @var{ev} in 
opened random-access trace handle @var{rand-trace}."
(make-stream
    (lambda (s) 
      (if (null? s) '()
      (let ((ci (chpsim-trace-critical-path-iterator rand-trace s)))
	; presumably, the only time when an event is self-referential is 0
        (if (= (chpsim-state-trace-entry-index s)
            (chpsim-trace-entry-index ci)) (cons s '())
	  (cons s ci)
	) ; end if
      ) ; end let
      ) ; end if
    ) ; end lambda
    (hac:lookup-trace-entry rand-trace ev)
  )
) ; end define

(define-public (chpsim-trace-critical-path tr-name)
"Reports the precise critical path of events (in reverse-order), given 
trace-file name @var{tr-name}.  Starts at the last event in trace by default."
  (chpsim-trace-critical-path-from
    (hac:open-chpsim-trace-accessor tr-name)
    (1- (hac:chpsim-trace-num-entries tr-name))
  )
) ; end define

;;;;;;;;;;; BRANCH FREQUENCY ANALYSIS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-public (make-select-branch-histogram trace-stream)
"Constructs a histogram of taken branches/selections from event trace stream.
This covers branches and do-whiles, but no forever-loops.
TODO: use memoized structures."
(let* ((select-succ-lists (chpsim-assoc-event-successors
         (force static-events-selects-stream-delayed)))
       (ll-histo (chpsim-successor-lists->histogram
         (stream->list select-succ-lists)))
       (sorted-assoc-pred 
         (let ((pred-map (make-rb-tree = <)))
	; stuff the pairs into a sorted map
            (stream-for-each
              (lambda (s) (rb-tree/insert! pred-map (car s) (cdr s)))
              (stream-of-lists->stream
                (chpsim-assoc-event-pred-from-succ select-succ-lists)))
            pred-map
          ))
       ) ; in let
(define (count-selects x)
"Local function.  Increments histogram counter for each occurred select event."
  (let ((f (rb-tree/lookup sorted-assoc-pred x #f)))
    (if f (let ((y (rb-tree/lookup ll-histo f #f)))
;      (display (car f)) (display ": ") (display (cdr y))
      (rb-tree/lookup-mutate! y x
        (lambda (z) 
;         (display "++") (display z) (newline)
#!
          "This hack is not needed anymore since we now execute branches 
           like any other event."
          (let ((p (hac:chpsim-get-event f)))
;            (display "p: ") (display p) (newline)
            (if (hac:chpsim-event-select? (static-event-raw-entry p))
              ; recurse to predecessor because selections are not 'executed'
              (count-selects f)
            ) ; end if
          ) ; end let
!#
          (1+ z)
        ) ; end lambda
        #f
      ) ; end lookup-mutate
    )) ; end if
  ) ; end let
) ; end define
  (stream-for-each
    (lambda (e) (count-selects (chpsim-trace-entry-event e)))
    trace-stream) ; end stream-for-each
  ll-histo ; return this
) ; end let
) ; end define

;;;;;;;;;;; LOOP REPETTION COUNTS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define-public (make-loop-histogram trace-stream)
"Constructs a histogram of do-forever loop counts for each loop-head event.
Result is a map of number of times each loop entered (first event executed).
NOTE: loop-head events that are selections are not automatically recoreded
because selection events don't execute in the normal sense, they choose
a successor to execute.  Thus, we depend on the branch histogram, which
correctly back-propagates counts to branches.  "
  (let ((loop-heads (force static-loop-head-events-delayed))
        (loop-histo (make-rb-tree = <))
        (branch-histo (make-select-branch-histogram trace-stream))
		; can this be memoized?
       )
       ; copy over keys into new histo, initialize counts to 0
    (rb-tree/for-each (lambda (p) (rb-tree/insert! loop-histo (car p) 0))
      loop-heads)
    (stream-for-each
      (lambda (e)
         (let ((eid (chpsim-trace-entry-event e)))
           (if (chpsim-event-loop-head? eid)
             (rb-tree/lookup-mutate! loop-histo eid
               (lambda (x) (1+ x))
               #f)
             ; else ignore
           )
         ) ; end let
      ) ; end lambda
      trace-stream
    ) ; end stream-for-each
    ; back-propagating branch counts
    (rb-tree/for-each
      (lambda (p)
        (let ((eid (car p)))
          (if (chpsim-event-branch-head? eid)
            (begin
            (rb-tree/lookup-mutate! loop-histo eid
              (lambda (x)
                (accumulate (lambda (j k) (+ (cdr j) k)) 0
                  (rb-tree->alist (rb-tree/lookup branch-histo eid #f)))
              ) ; end lambda
              #f
            )
            ) ; end begin
          ) ; end if
        ) ; end let
      ) ; end lambda
      loop-histo
    ) ; end for-each
    loop-histo
  ) ; end let
) ; end define

