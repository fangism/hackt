;; "hackt/chpsim-trace.h"
;;	$Id: chpsim-trace.scm,v 1.1.2.1 2007/03/22 05:17:52 fang Exp $
;; Interface to low-level chpsim trace file manipulators.  
;;

;; 31.3.4 Module System Quirks
;; ---------------------------
;; 
;; Although the programming interfaces are relatively stable, the Guile
;; module system itself is still evolving.  Here are some situations where
;; usage surpasses design.
;; 
;;   * When using a module which exports a macro definition, the other
;;     module must export all bindings the macro expansion uses, too,
;;     because the expanded code would otherwise not be able to see these
;;     definitions and issue a "variable unbound" error, or worse, would
;;     use another binding which might be present in the scope of the
;;     expansion.
;;
;; in other words, we have to export the imports.  :S

(define-module (hackt chpsim-trace)
;;	#:export (make-chpsim-trace-stream
		;; we need to export what we use, how inconvenient (note above)
		;; the following are defined by guile/chpsim-wrap.cc
;;		foobar
;;	) ;; end export
;;	#:re-export ( current-trace-entry) ;; end re-export
) ;; end define

;; we provided this in C++
(use-modules (hackt chpsim-trace-primitives))
(use-modules (ice-9 streams))
(use-modules (hackt streams))

;; open-chpsim-trace is defined as a primitive in guile/chpsim-wrap.cc
;; use it to create a stream to pass to as a trace-stream argument.  
;; \param trace-stream an already opened tracefile, ready to stream entries
;; Make sure we return a proper end-of-stream marker '() eventually
;; test error handling here.
(define-public (make-chpsim-trace-stream trace-stream)
;; if stream is still valid, note: only evaluate current-trace-entry ONCE!
  (make-stream
    (lambda (s) 
      (let ((p (current-trace-entry s)))
        (if (null? p) '()
	  (cons p s)
	) ; end if
      ) ; end let
    ) ; end lambda
    trace-stream
  ) ; end make-stream
) ; end define

