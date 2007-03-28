;; "hackt/docs.scm"
;;	$Id: docs.scm,v 1.1.2.2 2007/03/28 19:36:59 fang Exp $
;; Documentation module for HACKT scheme.  This is not a module, just source.
;; Blatantly ripped from lilypond source: "scm/document-functions.scm"

;; NOTES: `make' comes from module (oop goops)
;; `hac:get-all-function-documentation' is an exported primitive

; (define-module (hackt docs)) ?
(use-modules (ice-9 regex))
(use-modules (hackt hackt-primitives))

(define (dashify-underscores str)
   "Converts a string's underscores to dashes, duh."
   (regexp-substitute/global #f "_" str 'pre "-" 'post))

(define (format-c-header c-h)
  "Does something to C-header..."
  (regexp-substitute/global
   #f ","
   (regexp-substitute/global #f "(SCM|\\)|\\() *" (dashify-underscores c-h)
                             'pre "" 'post)
   'pre " " 'post))

(define (document-scheme-function name c-header doc-string)
  "Assembles a documentation string from function-name and header-name."
  (string-append
   "@defun " (symbol->string name)  " " (format-c-header c-header) "\n"
   doc-string
   "\n@end defun\n\n"))

; only this function depends on the HAC primitive
; Q: how do we document variables?
(define all-scheme-functions
;  "Structure containing all (unsorted) primitive functions and doc-strings."
   (hash-fold
    (lambda (key val prior)
      (cons (cons key val)  prior))
    '() (hac:get-all-function-documentation)))

; define-public
(define (all-scheme-functions-doc)
  "Generates sorted function-documentation structure."
  (let* ((func-docs (map (lambda (x)
                       (document-scheme-function (car x) (cadr x) (cddr x)))
                     all-scheme-functions))
         (sorted-func-docs (sort func-docs string<?)))
    (make <texi-node>
      #:name "Scheme functions"
      #:desc "Primitive functions exported by HACKT"
      #:text
      (apply string-append sorted-func-docs))))

;; collects documentation strings in all functions per module
;; TODO: work on this some more to emit texinfo
;; look at (procedure-source resolve-module)
(define (all-module-functions m)
  "Collects all procedure documentation for functions in the module."
  (define (document-it sym)
    (if (procedure? sym)
      (let ((doc (procedure-documentation sym)))
	; TODO: add some procedure properties?
        (if (string? doc) doc "TODO: document this procedure!")
      ) ; end lst
      "TODO: document this variable!"
    ) ; end if
  ) ; end define
  (module-map
    (lambda (key val)
      (let ((var (variable-ref val)))
        (cons key (document-it var))
      ) ; end let
    ) ; end lambda
  m) ; end module-for-each
) ; end define

; this is the source for resolve-module... can't grok
; (lambda (name . maybe-autoload)
;   (let* ((full-name (append (quote (%app modules)) name)))
;     (let* ((already (nested-ref the-root-module full-name)))
;       (if already
;         (if (and (or (null? maybe-autoload) (car maybe-autoload)) (not (module-public-interface already))) (begin (try-load-module name) already) already)
;         (begin (if (or (null? maybe-autoload) (car maybe-autoload)) (try-load-module name)) (make-modules-in (current-module) full-name))
;       )
;     )
;   )
; )

