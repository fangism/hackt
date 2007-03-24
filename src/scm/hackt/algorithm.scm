;; "hackt/algorithm.scm"
;;	$Id: algorithm.scm,v 1.1.2.1 2007/03/24 03:30:41 fang Exp $
;; Additional handy algorithms not fund in guile scheme libs.

(define-module (hackt algorithm))

; std::accumulate: iteratively applies binary operator over elements (forward)
(define-public (forward-accumulate binop init lst)
  (if (null? lst) init
    (forward-accumulate binop (binop (car lst) init) (cdr lst)) ; forward
  ) ; end if
) ; end define

; reverse-accumulate, from cdr to car
(define-public (reverse-accumulate binop init lst)
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

