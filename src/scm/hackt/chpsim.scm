;; "hackt/chpsim.scm"
;;	$Id: chpsim.scm,v 1.1.2.6 2007/04/05 01:04:52 fang Exp $
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

;; accessing type-specific subsets of dependence sets
(define-public (dependence-set-bools d)
  "Extracts the bool subset of dependencies." (car d))
(define-public (dependence-set-ints d)
  "Extracts the integer subset of dependencies." (cadr d))
(define-public (dependence-set-enums d)
  "Extracts the enumeral subset of dependencies." (caddr d))
(define-public (dependence-set-channels d)
  "Extracts the channel subset of dependencies." (cadddr d))

;; dispatch based on type-tag, returns a procedure object
(define-public (dependence-set-subset t)
  "Dispatches a subset extractor based on tag @var{t}"
  (cond
    ((= t 'bool) dependence-set-bools)
    ((= t 'int) dependence-set-ints)
    ((= t 'enum) dependence-set-enums)
    ((= t 'channel) dependence-set-channels)
    (else (error "Invalid tag: " t))
  ) ; end cond
) ; end define

;; adds proper tags to construct (type, index)-pair references
(define-public (chpsim-event-may-block-deps ev)
  "Extracts the set of instances that event @var{ev} may block on."
  (define (attach-tag tag lst)
    (map (lambda (i) (cons tag i)) lst)
  ) ; end define
  ; rr is untagged set of indices, now we pair them up with type tags
  (let ((rr (hac:chpsim-event-may-block-deps-internal ev)))
    (list (attach-tag 'bool (dependence-set-bools rr))
      (attach-tag 'int (dependence-set-ints rr))
      (attach-tag 'enum (dependence-set-enums rr))
      (attach-tag 'channel (dependence-set-channels rr))
    ) ; end list
  ) ; end let
) ; end define

