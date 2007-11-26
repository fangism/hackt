;; "hackt/algorithm.scm"
;;	$Id: algorithm.scm,v 1.3.24.2 2007/11/26 22:57:51 fang Exp $
;; Additional handy algorithms not fund in guile scheme libs.

(define-module (hackt algorithm)
#:autoload (srfi srfi-1) (partition find find-tail any)
#:autoload (ice-9 receive) (receive)
)

; when we want confirmation:
; (display "Loading module: (hackt algorithm)") (newline)

; std::accumulate: iteratively applies binary operator over elements (forward)
; iteratively runs in bounded space
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

; flatten list-of-lists
(define-public (list-flatten lstlst)
"Converts a list-of-lists into a single flat list."
  (reverse-accumulate append '() lstlst)
) ; end define

; flatten list-of-lists
(define-public (list-flatten-reverse lstlst)
"Converts a list-of-lists into a single flat list (reverse-constructed)."
  (accumulate append '() lstlst)
) ; end define

;; private definition
; (define (filter-split-helper) ...)

;; NOTE: filter algorithm will construct result lists reversed from source
;; could also define this in terms of accumulate
(define-public (filter-split pred? lst)
  "Partitions a list into a pair of lists, the first of which satisfied the 
predicate, the second of which failed the predicate."
  (receive (sat unsat) ; to capture multi-valued return
    (partition pred? lst)
    (cons sat unsat)
  )
;  (if (null? lst) (cons '() '() ) ; return pair of empty lists (true, false)
;  (let ((head (car lst))
;      (rem (filter-split pred? (cdr lst)))) ; yuck, not tail-recursive...
;    (if (pred? head)
;      (cons (cons head (car rem)) (cdr rem))
;      (cons (car rem) (cons head (cdr rem)))
;    ) ; end if
;  ) ; end let
;  ) ; end if
) ; end define

;; more 'STL' algorithms
;; map == transform

#!
(define-public (find-first-sublist pred? lst)
"Finds the sublist of list @var{lst} for which the first element satisfies 
the predicate, using a plain linear search.  Similar to the 
srfi-1 `member' procedure.  Returns #f if none found, like assoc-ref."
  (find-tail pred? lst)
; (if (null? lst)
;   #f
;   (let ((this (car lst)))
;     (if (pred? this) lst (find-first pred? (cdr lst)))
;   ) ; end let
; )
) ; end define
!#

#!
(define-public (find-first pred? lst)
"Finds the first element of the list @var{lst} that satisfies the predicate, 
using a plain linear search.  Returns #f if none found, like assoc-ref."
  (find pred? lst)
; (if (null? lst)
;   #f
;   (let ((this (car lst)))
;     (if (pred? this) this (find-first pred? (cdr lst)))
;   ) ; end let
; )
) ; end define
!#

#!
;; searching algorithms (using predicates)
(define-public (list-contains? pred? lst)
"Predicate is true if list contains at least one element that satisfies 
the predicate.  Implemented as short-circuit: return as soon as true."
  (any pred? lst)
; (if (null? lst)
;   #f
;  (if (pred? (car lst))
;     #t
;     (list-contains? pred? (cdr lst))
;   ) ; end if
; )
) ; end if, end define
!#

(define-public (find-assoc-ref alst key)
"Finds the key-value *pair* in an associative list using equal?, given a key.  
In contrast, assoc-ref returns only the value."
  (find (lambda (x) (equal? (car x) key)) alst)
;  (if (null? alst) #f
;    (if (equal? (caar alst) key) (car alst) (assoc-ref-pair (cdr alst) key))
;  )
)

; EXTREMELY USEFUL TOOLS FOR CONVERTING RECURSIVE ALGORITHMS TO ITERATIVE
; for those pesky non-tail recursive functions...
(define-public (iterate-template prod op index inc term?)
"Iteration template, where @var{prod} is a cumulative value (may be object),
@var{op} is the combining functor operating on (@var{index}, @var{prod}), 
@var{index} is a counter, @var{inc} is an incrementing procedure, 
and @var{term?} is a termination predicate."
  (if (term? index)
    prod
    (iterate-template (op index prod) op (inc index) inc term?)
  ) ; end if
) ; end define

(define-public (iterate-default prod op index limit)
"Iterate from @var{index} up to @{limit}, incrementing."
  (iterate-template prod op index 1+ (lambda (c) (> c limit)))
)

(define-public (iterate-reverse-default prod op index limit)
"Iterate from @var{index} down to @{limit}, decrementing."
  (iterate-template prod op index 1- (lambda (c) (< c limit)))
)

; list-find-first: use srfi-1::find
; list-count: just filter and length
; search (find matching subsequences)
; binary-search

; set algorithm (sorted precondition)
; set-intersection
; set-union
; set-difference
; set-symmetric-difference

