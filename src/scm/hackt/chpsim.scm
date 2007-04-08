;; "hackt/chpsim.scm"
;;	$Id: chpsim.scm,v 1.1.2.7 2007/04/08 21:28:55 fang Exp $
;; Scheme module for chpsim-specific functions (without trace file)
;; hackt-generic functions belong in hackt.scm, and
;; chpsim-trace specific functions belong in chpsim-trace.scm.

(define-module (hackt chpsim))

(use-modules (hackt hackt-primitives))	; for tag-constants
(use-modules (hackt chpsim-primitives))
(use-modules (hackt streams))
(use-modules (ice-9 streams))

(define-public (static-event-stream)
  "Represents the set of all statically allocated events as a stream."
  (stream-map (lambda (i) (hac:chpsim-get-event i))
    (enumerate-interval-stream 0 (1- (hac:chpsim-num-events))))
) ; end define

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

