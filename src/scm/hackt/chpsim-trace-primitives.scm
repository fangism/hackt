;; "scm/hackt/chpsim-trace-primitives.scm"
;; (use-module <this module> to load the primitives as an extension

;; looks for hackt-guile.{so,dylib} plug-in
(define-module (hackt chpsim-trace-primitives))
; dependencies
(use-modules (hackt hackt-primitives))
(use-modules (hackt chpsim-primitives))

(load-extension "hackt-chpsim-guile" "libhackt_chpsim_trace_guile_init")
;; user needs to immediately load a HAC object file
;; with (hac:load-object OBJFILE)

