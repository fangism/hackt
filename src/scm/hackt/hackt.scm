;; "hackt/hackt.scm"
;;	$id: $
;; module for routines built on top of libhackt primitives
;; The primitives are defined in module (hackt hackt-primitives), as 
;; defined by "guile/libhackt-wrap.cc"

(define-module (hackt hackt))

; already pre-loaded
; (use-modules (hackt hackt-primitives))

;; from "wrap_parse_global_reference",
;; the global reference is represented as a type-index pair
(define-public (reference-type ref)
  "Extracts the meta-type tag from a pair-reference." (car ref))
(define-public (reference-index ref)
  "Extracts the global index from a pair-reference." (cdr ref))

; reference equality comparison
(define-public (reference-equal? a b) 
"Return true if type-index pair reference is the same.
For example: (reference-equal? '(channel . 1) '(channel . 1)) -> #t."
  (and (pair? a) (equal? a b))
) ; end define

;; shorthand for extracting embedded documentation
;; also works on non-hackt-related procedures that are so-documented
(define-public (help-hackt proc) procedure-documentation proc)
