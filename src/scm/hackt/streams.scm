;; "streams.scm"
;;	$Id: streams.scm,v 1.1.2.6 2007/03/24 03:30:43 fang Exp $
;; Extensions to guile's stream module.
;; e.g. this supplies a 'filter' interface
;; This file should be installed in $(pkgdatadir)/scm/hackt.
;; TODO: make sure that path is added to search path.

(define-module (hackt streams))

(use-modules (ice-9 streams))

;; could also use define-public instead of exporting... same thing

(define-public the-empty-stream '())
; (define-public delay-empty-stream (delay the-empty-stream))
; doesn't work as expected

;; delayed tail construction
;; (define-public (cons-stream h t) (cons h (delay t)))
;; NOTE: this is defined as such to match stream-car/cdr in ice-9 streams
(define-public (cons-stream h t) (delay (cons h t)))

;; produces a stream as a filtered subset of the argument stream
(define-public (stream-filter pred stream)
  (cond ((stream-null? stream) (delay the-empty-stream))
	((pred (stream-car stream))
	  (cons-stream (stream-car stream)
		(stream-filter pred (stream-cdr stream))))
	(else (stream-filter pred (stream-cdr stream)))
  )
) ; end define

; returns a pair of streams, satisfying the predicate, and unsatisfied
(define-public (stream-filter-split pred stream)
  (if (stream-null? stream)
    (cons (delay the-empty-stream) (delay the-empty-stream))
    (let ((head (stream-car stream))
	  (rem (stream-filter-split pred (stream-cdr stream))))
	(if (pred head)
	  (cons (cons-stream head (car rem)) (cdr rem))
	  (cons (car rem) (cons-stream head (cdr rem)))
	) ; end if
    ) ; end let
  ) ; end if
) ; end define

(define-public (stream-accumulate op initial stream)
  (if (stream-null? stream) initial
    (op (stream-car stream) (stream-accumulate op initial (stream-cdr stream)))
  )
) ; end define

;; like append-streams from SICP, exhaust first finite stream, then use 2nd
(define-public (stream-concat s1 s2)
  (if (stream-null? s1) s2
     (cons-stream (stream-car s1) (stream-concat (stream-cdr s1) s2))
  )
) ; end define
;; define variadically, to take arbitrary number of streams

;; like flatten from SICP, 
;; converts stream-of-streams into single stream via concatenation
(define-public (stream-flatten strstr)
  (stream-accumulate stream-concat the-empty-stream strstr)
) ; end define


;; random utilities

; finite stream of integers
(define-public (enumerate-interval-stream low high)
  (if (> low high) (delay the-empty-stream)
    (cons-stream low (enumerate-interval-stream (1+ low) high))
  ) ; end if
) ; end define

