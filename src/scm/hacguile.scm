;; startup script for the hacguile wrapper script/executable
;; load this script to startup
;; guile -l hacguile.scm
;;	$Id: hacguile.scm,v 1.1 2009/02/23 09:11:23 fang Exp $

(use-modules (hackt hackt-primitives))
(hacguile:parse-command-line (command-line))

