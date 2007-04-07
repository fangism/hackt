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

;; shorthand for extracting embedded documentation
;; also works on non-hackt-related procedures that are so-documented
(define-public (help-hackt proc) procedure-documentation proc)

