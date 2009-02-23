;; startup script for the hacguile wrapper script/executable
;; load this script to startup
;; guile -l hacchpsimguile.scm
;;	$Id: hacchpsimguile.scm,v 1.1 2009/02/23 09:11:23 fang Exp $

(use-modules (hackt hackt-primitives))
(use-modules (hackt chpsim-primitives))
(use-modules (hackt chpsim-trace-primitives))

(hacchpsimguile:parse-command-line (command-line))

(if (and (hac:have-object?) (not (hac:have-chpsim-state?)))
  (hac:init-chpsim-state))

