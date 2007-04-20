;;;; "hackt/doc-lib.scm" -- Assorted Functions for generated documentation
;;;;	$Id: doc-lib.scm,v 1.2 2007/04/20 18:26:06 fang Exp $
;;;; blatantly ripped from LilyPond source: "scm/documentation-lib.scm"
;;;;
;;;; This scheme code is to be loaded

(use-modules (oop goops) ; to define class
	     (srfi srfi-13)
	     (srfi srfi-1) ; list and search library
)

; using GOOPS
(define-class <texi-node> ()
  (children #:init-value '() #:accessor node-children #:init-keyword #:children)
  (text #:init-value "" #:accessor node-text #:init-keyword #:text)
  (name #:init-value "" #:accessor node-name #:init-keyword #:name)
  (description #:init-value "" #:accessor node-desc #:init-keyword #:desc))

(define (menu-entry x)
  (cons
   (node-name x)
   (node-desc x)))

(define (dump-node node port level)
  (display
   (string-append
    "\n@node "
    (node-name node)
    "\n\n"
    (texi-section-command level) " "
    (node-name node)
    "\n\n"
    (node-text node)
    "\n\n"
    (if (pair? (node-children node))
	(texi-menu
	 (map (lambda (x) (menu-entry x))
	      (node-children node)))
	""))
   port)
  (map (lambda (x) (dump-node x port (+ 1 level)))
       (node-children node)))

; (define (processing name)
;   (ly:message (_ "Processing ~S...") name))

(define (self-evaluating? x)
  (or (number? x) (string? x) (procedure? x) (boolean? x)))

(define (texify x)
  x)

(define (scm->texi x)
  (string-append "@code{" (texify (scm->string x)) "}"))



(define (texi-section-command level)
  (cdr (assoc level '(
		      ;; Hmm, texinfo doesn't have ``part''
		      (0 . "@top")
		      (1 . "@unnumbered")
		      (2 . "@unnumberedsec")
		      (3 . "@unnumberedsubsec")
		      (4 . "@unnumberedsubsubsec")
		      (5 . "@unnumberedsubsubsec")))))

(define (one-item->texi label-desc-pair)
  "Document one (LABEL . DESC); return empty string if LABEL is empty string. 
"
  (if (eq? (car label-desc-pair) "")
      ""
      (string-append "\n@item " (car label-desc-pair) "\n" (cdr label-desc-pair))))


(define (description-list->texi items-alist)
  "Document ITEMS-ALIST in a table. entries contain (item-label
. string-to-use)
"
  (string-append
   "\n@table @asis\n"
   (apply string-append (map one-item->texi items-alist))
   "\n@end table\n"))

(define (texi-menu items-alist)
  "Generate what is between @menu and @end menu."
  (let ((maxwid
	 (apply max (map (lambda (x) (string-length (car x))) items-alist))))
    
    (string-append
     "\n@menu"
     (apply string-append
	    (map (lambda (x)
		   (string-append
		    (string-pad-right 
		     (string-append "\n* " (car x) ":: ")
		     (+ maxwid 8))
		    (cdr x)))
		 items-alist))
     "\n@end menu\n"
     ;; Menus don't appear in html, so we make a list ourselves
     "\n@ignore\n"
     "\n@ifhtml\n"
     (description-list->texi (map (lambda (x) (cons (ref-ify (car x)) (cdr x)))
				  items-alist))
     "\n@end ifhtml\n"
     "\n@end ignore\n")))

(define (texi-file-head name file-name top)
  (string-append
   "\\input texinfo @c -*-texinfo-*-"
   "\n@setfilename " file-name ".info"
   "\n@settitle " name
   "\n@dircategory HACKT"
   "\n@direntry"
   ;; prepend GNU for dir, must be unique
   "\n* GNU!? " name ": (" file-name ").          " name "."
   "\n@end direntry\n"
   "@documentlanguage en\n"
   "@documentencoding utf-8\n"))

; (define (context-name name)
;   name)

; (define (engraver-name name)
;   name)

; (define (grob-name name)
;   (if (symbol? name)
;       (symbol->string name)
;       name))

; (define (interface-name name)
;   name)

(define (ref-ify x)
  "Add ref to X"
  (string-append "@ref{" x "}"))

(define (human-listify lst)
  "Produce a textual enumeration from LST, a list of strings"
  (cond
   ((null? lst) "none")
   ((null? (cdr lst)) (car lst))
   ((null? (cddr lst)) (string-append (car lst) " and " (cadr lst)))
   (else (string-append (car lst) ", " (human-listify (cdr lst))))))

; (define (writing-wip x)
;   (ly:message (_ "Writing ~S...") x))

; ly's property stuff deleted

