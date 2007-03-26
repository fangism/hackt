;; "hackt/chpsim.scm"
;;	$Id: chpsim.scm,v 1.1.2.2 2007/03/26 02:49:08 fang Exp $
;; Scheme module for chpsim-specific functions (without trace file)
;; hackt-generic functions belong in hackt.scm, and
;; chpsim-trace specific functions belong in chpsim-trace.scm.

(define-module (hackt chpsim))

(use-modules (hackt hackt-primitives))	; for tag-constants
(use-modules (hackt chpsim-primitives))
(use-modules (hackt streams))
(use-modules (ice-9 streams))

;; represent all statically allocated events as a stream
(define-public (static-event-stream)
  (stream-map (lambda (i) (chpsim-get-event i))
    (enumerate-interval-stream 0 (1- (chpsim-num-events))))
) ; end define

;; accessing type-specific subsets of dependence sets
(define-public (dependence-set-bools d) (car d))
(define-public (dependence-set-ints d) (cadr d))
(define-public (dependence-set-enums d) (caddr d))
(define-public (dependence-set-channels d) (cadddr d))

;; dispatch based on type-tag, returns a procedure object
(define-public (dependence-set-subset t)
  (cond
    ((= t bool-tag) dependence-set-bools)
    ((= t int-tag) dependence-set-ints)
    ((= t enum-tag) dependence-set-enums)
    ((= t channel-tag) dependence-set-channels)
    (else (error "Invalid tag: " t))
  ) ; end cond
) ; end define

;; adds proper tags to construct (type, index)-pair references
(define-public (chpsim-event-may-block-deps ev)
  (define (attach-tag tag lst)
    (map (lambda (i) (cons tag i)) lst)
  ) ; end define
  ; rr is untagged set of indices, now we pair them up with type tags
  (let ((rr (chpsim-event-may-block-deps-internal ev)))
    (cons (attach-tag bool-tag (dependence-set-bools rr))
      (cons (attach-tag int-tag (dependence-set-ints rr))
        (cons (attach-tag enum-tag (dependence-set-enums rr))
          (cons (attach-tag channel-tag (dependence-set-channels rr)) '())
        ) ; end cons
      ) ; end cons
    ) ; end cons
  ) ; end let
) ; end define

