;; "hackt/chpsim-trace.h"
;;	$Id: chpsim-trace.scm,v 1.9 2008/05/08 04:34:25 fang Exp $
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
#:autoload (srfi srfi-1) (any fold)
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
(use-modules (hackt hackt-primitives)) ; for hac:parse-reference
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

(define-public (chpsim-state-trace-single-reference-values s rpair)
"Similar to chpsim-state-trace-single-reference, but this restructures the 
data to just the pair<event-trace-id, value>, omitting the reference pair."
  (stream-map (lambda (e) (cons (car e) (cddr e)))
    (chpsim-state-trace-single-reference s rpair)
  ) ; end stream-map
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

;; this is useful for detailed critical path
(define-public (display-trace-entry-verbose entry)
"Print a detailed report of the chpsim event trace @var{entry}"
  (display "trace-index: ") (display (chpsim-trace-entry-index entry))
  (newline)
  (display "cause-index: ") (display (chpsim-trace-entry-critical entry))
  (newline)
  (display "time: ") (display (chpsim-trace-entry-time entry))
  (newline)
  (let* ((eid (chpsim-trace-entry-event entry))
         (evp (hac:chpsim-get-event eid))
         (ev (cdr evp))
         (pid (hac:chpsim-event-process-id ev)))
    (display "event-index: ") (display eid)
    (newline)
    (display "process[") (display pid)
    (display "]: ") (display (process-id->string pid))
    (newline)
    (display "source-context: ") (display (hac:chpsim-event-source ev))
    (newline)
  )
)

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
      (rb-tree/lookup-mutate! y x 1+ #f)
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
Result is a map of number of times each loop entered (first event executed)."
  (let ((loop-heads (force static-loop-head-events-delayed))
        (loop-histo (make-rb-tree = <))
;        (branch-histo (make-select-branch-histogram trace-stream))
		; can this be memoized?
       )
       ; copy over keys into new histo, initialize counts to 0
    (rb-tree/for-each (lambda (p) (rb-tree/insert! loop-histo (car p) 0))
      loop-heads)
    (stream-for-each
      (lambda (e)
         (let ((eid (chpsim-trace-entry-event e)))
           (if (chpsim-event-loop-head? eid)
             (rb-tree/lookup-mutate! loop-histo eid 1+ #f)
             ; else ignore
           )
         ) ; end let
      ) ; end lambda
      trace-stream
    ) ; end stream-for-each
;    ; back-propagating branch counts (no longer necessary)
;    (rb-tree/for-each
;      (lambda (p)
;        (let ((eid (car p)))
;          (if (chpsim-event-branch-head? eid)
;            (begin
;            (rb-tree/lookup-mutate! loop-histo eid
;              (lambda (x)
;                (accumulate (lambda (j k) (+ (cdr j) k)) 0
;                  (rb-tree->alist (rb-tree/lookup branch-histo eid #f)))
;              ) ; end lambda
;              #f
;            )
;            ) ; end begin
;          ) ; end if
;        ) ; end let
;      ) ; end lambda
;      loop-histo
;    ) ; end for-each
    loop-histo
  ) ; end let
) ; end define

(define-public (make-event-adjacency-histogram crit-stream)
"Constructs a critical event histogram from the critical path, by counting
occurrences of critical event pairs.  @var{crit-stream} should be an
already computed critical path, because this operates on any stream of events.  
Returns a sorted, two-level tree-of-tree mapping event to critical predecessor 
and occurrences."
  (define (init-bin key) ; initialize a tree with pair [key, 1]
    (let ((tree (make-rb-tree = <)))
      (rb-tree/insert! tree key 1)
      tree))
  (let ((edge-histo (make-rb-tree = <))
        (crit-ev (stream-map chpsim-trace-entry-event crit-stream)))
    (stream-for-each
      (lambda (event cause)
	; slow: unnecessary allocation every time
        (let ((n (rb-tree/insert-if-new! edge-histo event (init-bin cause))))
          (if (not (unspecified? n)) ; then we found previous entry
            ; n is a tree: [cause, count]
            ; TODO: rewrite without using two lookups (common case)
            (let ((c (rb-tree/insert-if-new! n cause 1)))
              (if (not (unspecified? c)) ; then we found previous entry
                (rb-tree/lookup-mutate! n cause 1+ #f) ; increment
              )
            ) ; end let
          ) ; end if
        ) ; end let
      ) ; end lambda
      crit-ev
      (stream-cdr crit-ev)
    ) ; end steram-map
    edge-histo
  ) ; end let
) ; end define

;; alias
(define-public make-critical-event-histogram make-event-adjacency-histogram)

#!
"Translates critical path to a critical process histogram."
!#
(define-public (make-critical-process-histogram crit-path)
  (let ((proc-histo (make-rb-tree = <)))
    (stream-for-each
      (lambda (pid) (rb-tree/increment! proc-histo pid))
      (stream-map
        (lambda (e)
          (hac:chpsim-event-process-id (static-event-raw-entry
            (hac:chpsim-get-event (chpsim-trace-entry-event e))))
        ) ; end lambda
      crit-path) ; end for-each
    ) ; end for-each
    proc-histo
  ) ; end let
) ; end define

#!
"Print critical processes histogram with names."
!#
(define-public (print-named-critical-process-histogram proc-histo)
  (rb-tree/for-each
    (lambda (p)
      (display (process-id->string (car p)))
      (display ": ")
      (display p)
      (newline)
    ) ; end lambda
  proc-histo)
) ; end define

; TODO: 3-level tree
#!
"Takes a critical event stream and returns a stream of send-receive event-pairs
in the trace that occurred on the critical path.  Recall that the critical path is listed
in reverse-chronological order.  The result will be in forward-chronological order."
!#
(define-public (make-critical-channel-event-pairs-list crit-stream cid)
(let* ((cev (alist->rb-tree (stream->list (stream-map (lambda (e) (cons (car e) #t))
         (chpsim-find-events-involving-channel-id cid all-static-events-stream))) = <))
       (sr-estrm (stream-filter
         (lambda (e) (rb-tree/lookup cev (chpsim-trace-entry-event e) #f))
         crit-stream)))
  (stream-fold (lambda (elem init)
    ; init will accumulate as a list
    (if (null? init)
      (list (list elem))
      (let ((recent (car init)) (rest (cdr init)))
        (if (= (length recent) 2)
          (cons (list elem) init) ; last event already paired
          (let* ((prev (car recent))
                 (crit-prev (chpsim-trace-entry-critical prev)))
            (if (and (= (chpsim-trace-entry-index elem) crit-prev)
                 (= (1+ crit-prev) (chpsim-trace-entry-index prev))
                 (not (= (chpsim-trace-entry-event elem) (chpsim-trace-entry-event prev))))
              (cons (cons elem recent) rest) ; pair up events
              (cons (list elem) init)
            ) ; end if
          ) ; end let
        ) ; end if
      ) ; end let
    ) ; end if
    ) ; end lambda
    '() sr-estrm) ; end stream-fold
) ; end let
) ; end define

#!
"Unfiltered list may contain singleton lists or lists of length 2.
Since we want critical send-receive pairs, we preserve elements of length 2."
!#
(define-public (filter-critical-send-receive-pairs-list unfiltered-list)
  (filter (lambda (x) (= (length x) 2)) unfiltered-list))

#!
"Given a channel id, return a list of critical event pairs, consisting of only send-receive
atomic event pairs."
!#
(define-public (make-critical-send-receive-pairs-list crit-stream cid)
  (filter-critical-send-receive-pairs-list (make-critical-channel-event-pairs-list crit-stream cid)))

#!
"Takes a list of send-receive critical event pairs, and returns a pair of counters, where the
first value is the number of times sender was critical, and the second value is the number of times
the receiver was more critical."
!#
(define-public (count-send-receive-criticality lst)
  (fold (lambda (elem init)
    (let ((ev (cdr (hac:chpsim-get-event (chpsim-trace-entry-event (car elem))))))
      ; check the first element of each send-receive pair: which is it?
      (cond
        ((hac:chpsim-event-send? ev) (cons (1+ (car init)) (cdr init)))
        ((hac:chpsim-event-receive? ev) (cons (car init) (1+ (cdr init))))
        (else init)
      ) ; end cond
    ) ; end let
    ) ; end lambda
  '(0 . 0) lst) ; end fold
) ; end define

#!
"Combined procedure to report channel criticality given a named channel."
!#
(define-public (channel-send-receive-criticality crit-stream cname)
  (count-send-receive-criticality
    (make-critical-send-receive-pairs-list crit-stream
      (cdr (hac:parse-reference cname)))
  ) ; end count
) ; end define

