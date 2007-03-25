;; "hackt/chpsim.scm"
;;	$Id: chpsim.scm,v 1.1.2.1 2007/03/25 02:25:43 fang Exp $
;; Scheme module for chpsim-specific functions (without trace file)
;; hackt-generic functions belong in hackt.scm, and
;; chpsim-trace specific functions belong in chpsim-trace.scm.

(define-module (hackt chpsim))

; (use-modules (hackt hackt-primitives))
(use-modules (hackt chpsim-primitives))
(use-modules (hackt streams))
(use-modules (ice-9 streams))

;; represent all statically allocated events as a stream
(define-public (static-event-stream)
  (stream-map (lambda (i) (chpsim-get-event i))
    (enumerate-interval-stream 0 (1- (chpsim-num-events))))
) ; end define

