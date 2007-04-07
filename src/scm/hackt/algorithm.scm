;; "hackt/algorithm.scm"
;;	$Id: algorithm.scm,v 1.1.2.3 2007/04/07 20:28:44 fang Exp $
;; Additional handy algorithms not fund in guile scheme libs.

(define-module (hackt algorithm))

; std::accumulate: iteratively applies binary operator over elements (forward)
(define-public (forward-accumulate binop init lst)
  "Runs an accumulator functor from front-to-back.  binop accumulates through
its second argument"
  (if (null? lst) init
    (forward-accumulate binop (binop (car lst) init) (cdr lst)) ; forward
  ) ; end if
) ; end define

; reverse-accumulate, from cdr to car
(define-public (reverse-accumulate binop init lst)
  "Runs an accumulator functor from back-to-front."
  (if (null? lst) init
    (binop (car lst) (reverse-accumulate binop init (cdr lst))) ; reverse
  ) ; end if
) ; end define

; alias, by default
(define-public accumulate forward-accumulate)

;; private definition
; (define (filter-split-helper) ...)

;; NOTE: filter algorithm will construct result lists reversed from source
;; could also define this in terms of accumulate
(define-public (filter-split pred lst)
  "Partitions a list into a pair of lists, the first of which satisfied the 
predicate, the second of which failed the predicate."
  (if (null? lst) (cons '() '() ) ; return pair of empty lists (true, false)
  (let ((head (car lst))
      (rem (filter-split pred (cdr lst)))) ; yuck, not tail-recursive...
    (if (pred head)
      (cons (cons head (car rem)) (cdr rem))
      (cons (car rem) (cons head (cdr rem)))
    ) ; end if
  ) ; end let
  ) ; end if
) ; end define

;; more 'STL' algorithms
;; map == transform

;; searching algorithms (using predicates)
(define-public (list-contains? pred lst)
"Predicate is true if list contains at least one element that satisfies 
the predicate.  Implemented as short-circuit: return as soon as true."
(if (null? lst)
  #f
  (if (pred (car lst))
    #t
    (list-contains? pred (cdr lst))
  ) ; end if
)) ; end if, end define
; list-find-first
; list-count
; search (find matching subsequences)
; binary-search

; set algorithm (sorted precondition)
; set-intersection
; set-union
; set-difference
; set-symmetric-difference

