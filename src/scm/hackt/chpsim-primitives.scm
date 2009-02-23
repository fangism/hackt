;; "scm/hackt/chpsim-primitives.scm"
;; (use-module <this module> to load the primitives as an extension

;; looks for hackt-guile.{so,dylib} plug-in
(define-module (hackt chpsim-primitives))
(use-modules (hackt hackt-primitives))

(load-extension "hackt-chpsim-guile" "libhackt_chpsim_guile_init")
;; user needs to immediately load a HAC object file
;; with (hac:load-object OBJFILE)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; command-line option processing

(define-public hacchpsimguile:usage hacguile:usage)

; re-use
(define-public hacchpsimguile:parse-command-line hacguile:parse-command-line)

