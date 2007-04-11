;; "hackt/chpsim.scm"
;;	$Id: chpsim.scm,v 1.1.2.8 2007/04/11 03:05:07 fang Exp $
;; Scheme module for chpsim-specific functions (without trace file)
;; hackt-generic functions belong in hackt.scm, and
;; chpsim-trace specific functions belong in chpsim-trace.scm.

(define-module (hackt chpsim)
#:autoload (ice-9 streams) (stream-map)
#:autoload (hackt streams) (stream-filter enumerate-interval-stream)
)

; (use-modules (hackt hackt-primitives))	; for tag-constants
(use-modules (hackt chpsim-primitives))
; (use-modules (hackt streams))		; now autoloaded
; (use-modules (ice-9 streams))		; now autoloaded

(define (static-event-stream)
  "Represents the set of all statically allocated events as a stream."
  (stream-map (lambda (i) (hac:chpsim-get-event i))
    (enumerate-interval-stream 0 (1- (hac:chpsim-num-events))))
) ; end define

; global stream variable, lazy evaluated
(define-public all-static-events-stream (static-event-stream))

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

; filters all selection events, deterministic and nondeterministic
(define-public (chpsim-filter-static-events-select static-events-stream)
"Select only selection events out of static event stream.  
Argument is a stream of static events."
  (stream-filter (lambda (e) (hac:chpsim-event-select? (cdr e)))
    static-events-stream)
) ; end define

(define-public (chpsim-assoc-event-successors static-event-stream)
"Given a static event stream, produces a set of event-successor pairs 
(which look like lists).  The resulting list can be viewed as an associative 
list, with the key being the first element and the value being the rest (cdr)."
  (stream-map (lambda (e) (cons (car e) (hac:chpsim-event-successors (cdr e))))
    static-event-stream)
)

(define-public (chpsim-assoc-event-pred-from-succ succ-assoc-strm)
"Construct predecessor associations given a stream of associated 
successor lists.  Each input stream element is an associated list of a 
static event index paired with its list of successors.  
May result in multiple associations among predecessors in returned list.  "
  (stream-map
    (lambda (a)
      (map
        (lambda (b) (cons b (car a)))
        (cdr a)
      )
    )
  succ-assoc-strm)
) ; end define

(define-public (chpsim-successor-lists->histogram succ-list)
"Constructs a zero-initialized associative histogram of successor events 
visited, based on the input list of associated successor lists."
  (map
    (lambda (x)
      (cons (car x)
        (map (lambda (y) (cons y 0)) (cdr x))
      ) ; end cons
    ) ; end lambda
  succ-list) ; end map
) ; end define

