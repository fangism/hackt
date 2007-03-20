;; "streams.scm"
;;	$Id: streams.scm,v 1.1.2.1 2007/03/20 23:10:42 fang Exp $
;; Extensions to guile's stream module.
;; e.g. this supplies a 'filter' interface
;; This file should be installed in $(pkgdatadir)/scm/hackt.
;; TODO: make sure that path is added to search path.

(define-module (hackt streams)
  :use-module (ice-9 streams)
  :export (
	the-empty-stream 
	stream-filter
	stream-accumulate
	stream-concat
  )
)

(define the-empty-stream '())

(define (stream-filter pred stream)
  (cond ((stream-null? stream) the-empty-stream)
	((pred (stream-car stream))
	  (make-stream (stream-car) (filter pred (stream-cdr stream))))
	(else (filter pred (stream-cdr stream)))
  )
)

(define (stream-accumulate op initial stream)
  (if (stream-null? stream) initial
    (op (stream-car stream) (stream-accumulate op initial (stream-cdr stream)))
  )
)

;; like append-streams from SICP, exhaust first finite stream, then use 2nd
(define (stream-concat s1 s2)
  (if (stream-null? s1) s2
     (make-stream (stream-car s1) (stream-concat (stream-cdr s1) s2))
  )
)

;; like flatten from SICP, 
;; converts stream-of-streams into single stream via concatenation
(define (stream-flatten strstr)
  (stream-accumulate stream-concat the-empty-stream strstr)
)

