;; "scm/hackt/hackt-primitives.scm"
;; (use-module <this module> to load the primitives as an extension

;; looks for hackt-guile.{so,dylib} plug-in
(define-module (hackt hackt-primitives))
(use-modules (ice-9 getopt-long))

; (use-modules (system repl common))	; new in guile-2.0
(catch #t (lambda () (use-modules (system repl common))) (lambda (key . args) #t))

(load-extension "hackt-guile" "libhackt_guile_init")
;; user needs to immediately load a HAC object file
;; with (hac:load-object OBJFILE)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; command-line option processing

(define option-spec '(
  (batch	(single-char #\b) (value #f))
  (help		(single-char #\h) (value #f))
  (interactive	(single-char #\i) (value #f))
))

; (define options (getopt-long (command-line) option-spec))
; (display "OPTIONS:\n") (display options) (newline)

(define-public (hacguile:usage) (display "\
guile module: hackt hackt-primitives
usage: [options] OBJFILE
  where OBJFILE is a compiled HAC object file
options:
  -b, --batch		Batch mode, suppress prompt.
  -h, --help		Display this help.
  -i, --interactive	Interactive mode, show prompt (default).
"))

; default interactive
; (define batch #f)

(define (display-nl x) (display x) (newline))
(define (obj-reminder) (display-nl "Don't forget: (hac:load-object OBJFILE)"))

(define replace-prompt!
  (if (defined? 'repl-default-prompt-set!)
    repl-default-prompt-set!
    set-repl-prompt!
  )	; end if
) ; end define

; typically pass (command-line)
(define-public (hacguile:parse-command-line args)
  (let* (
    (options (getopt-long args option-spec))
    (help-wanted (option-ref options 'help #f))
    (__batch (option-ref options 'batch #f))
    (__interactive (option-ref options 'interactive #t))
    (batch (or __batch (not __interactive)))
    (non-option-args (option-ref options '() '()))
    ) ; end let-bindings
    (if help-wanted (hacguile:usage))
    (if (not (null? non-option-args))
      (hac:load-object (car non-option-args)))
    (if (not (hac:have-object?)) (obj-reminder))
    (replace-prompt! (if batch "" "hacguile> "))
  ) ; end let
) ; end define

; (display-nl "Hint: (hacguile:parse-command-line (command-line))")

