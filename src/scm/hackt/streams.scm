;; "streams.scm"
;;	$Id: streams.scm,v 1.1.2.3 2007/03/22 21:33:45 fang Exp $
;; Extensions to guile's stream module.
;; e.g. this supplies a 'filter' interface
;; This file should be installed in $(pkgdatadir)/scm/hackt.
;; TODO: make sure that path is added to search path.

(define-module (hackt streams))

(use-modules (ice-9 streams))

;; could also use define-public instead of exporting... same thing

(define-public the-empty-stream '())

;; delayed tail construction
;; (define-public (cons-stream h t) (cons h (delay t)))
(define-public (cons-stream h t) (delay (cons h t)))

;; produces a stream as a filtered subset of the argument stream
(define-public (stream-filter pred stream)
  (cond ((stream-null? stream) (delay the-empty-stream))
	((pred (stream-car stream))
	  (cons-stream (stream-car stream)
		(stream-filter pred (stream-cdr stream))))
	(else (stream-filter pred (stream-cdr stream)))
  )
)

(define-public (stream-accumulate op initial stream)
  (if (stream-null? stream) initial
    (op (stream-car stream) (stream-accumulate op initial (stream-cdr stream)))
  )
)

;; like append-streams from SICP, exhaust first finite stream, then use 2nd
(define-public (stream-concat s1 s2)
  (if (stream-null? s1) s2
     (cons-stream (stream-car s1) (stream-concat (stream-cdr s1) s2))
  )
)

;; like flatten from SICP, 
;; converts stream-of-streams into single stream via concatenation
(define-public (stream-flatten strstr)
  (stream-accumulate stream-concat the-empty-stream strstr)
)

