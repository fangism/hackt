;; "hackt/chpsim-trace.h"
;;	$Id: chpsim-trace.scm,v 1.1.2.4 2007/03/28 06:11:57 fang Exp $
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
;; (use-modules (hackt hackt-primitives))
(use-modules (hackt chpsim-trace-primitives))
(use-modules (ice-9 streams))
(use-modules (hackt streams))

;; open-chpsim-trace is defined as a primitive in guile/chpsim-wrap.cc
;; use it to create a stream to pass to as a trace-stream argument.  
;; \param trace-stream an already opened tracefile, ready to stream entries
;; Make sure we return a proper end-of-stream marker '() eventually
;; test error handling here.
(define-public (make-chpsim-trace-stream trace-stream)
"Creates a trace stream from an opened trace file (smob)."
;; if stream is still valid, note: only evaluate current-trace-entry ONCE!
  (make-stream
    (lambda (s) 
      (let ((p (hac:current-trace-entry s)))
        (if (null? p) '()
	  (cons p s)
	) ; end if
      ) ; end let
    ) ; end lambda
    trace-stream
  ) ; end make-stream
) ; end define

;; convenient combined definition
(define-public (open-chpsim-trace-stream tf)
  "Opens the named trace file and returns a stream interface in one fell swoop."
  (make-chpsim-trace-stream (hac:open-chpsim-trace tf))
) ; end define

;; These struct accessors must be kept consistent with the construct
;; used in guile/chpsim-wrap.cc's wrap_chpsim_trace_entry_to_scm.
;; Later, thes may be replaced with primitive implementations if needed.  
(define-public (chpsim-trace-entry-index e)
  "Extracts the trace-entry's event index." (car e))
(define-public (chpsim-trace-entry-time e)
  "Extracts the trace-entry's event timestamp." (cadr e))
(define-public (chpsim-trace-entry-event e)
  "Extracts the trace-entry's static event index" (caddr e))
(define-public (chpsim-trace-entry-critical e)
  "Extracts the trace-entry's last arriving event index." (cdddr e))

