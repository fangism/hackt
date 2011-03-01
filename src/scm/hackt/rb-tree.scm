; "hackt/rb-tree.scm"
; Adapted from MIT-Scheme-7.7.1 implementation "rbtree.scm".  

; $Id: rb-tree.scm,v 1.6 2011/03/01 17:36:10 fang Exp $

; Copyright (c) 1993-2000 Massachusetts Institute of Technology

; This program is free software; you can redistribute it and/or modify it under 
; the terms of the GNU General Public License as published by the Free Software 
; Foundation; either version 2 of the License, or (at your option) any later 
; version.

; This program is distributed in the hope that it will be useful, but WITHOUT 
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
; FOR A PARTICULAR PURPOSE.  
; See the GNU General Public License for more details.

; You should have received a copy of the GNU General Public License along with 
; this program; if not, write to the Free Software Foundation, Inc., 
; 675 Mass Ave, Cambridge, MA 02139, USA.

;;;; Red-Black Trees
;;; package: (runtime rb-tree)

;;; Cormen, Leiserson, and Rivest, "Introduction to Algorithms",
;;; Chapter 14, "Red-Black Trees".

;;; Properties of Red-Black Trees:
;;; 1. Every node is either red or black.
;;; 2. Every leaf (#F) is black.
;;; 3. If a node is red, then both its children are black.
;;; 4. Every simple path from a node to a descendent leaf contains the
;;;    same number of black nodes.
;;; These algorithms additionally assume:
;;; 5. The root of a tree is black.

(define-module (hackt rb-tree)
  #:autoload (ice-9 streams) (list->stream stream->list)
)

(use-modules (oop goops))

(define-class <rb-tree> ()
  (root #:init-value #f #:setter set-tree-root! #:getter tree-root)
  (key=? #:init-value eq? #:getter tree-key=? #:init-keyword #:eq-comp)
  (key<? #:init-value < #:getter tree-key<? #:init-keyword #:order-comp)
) ; end define-class

(define-public (make-rb-tree key=? key<?)
  (make <rb-tree> #:eq-comp key=? #:order-comp key<?)
;; Optimizations to work around compiler that codes known calls to
;; these primitives much more efficiently than unknown calls.
;  (make-tree (cond ((eq? key=? eq?) (lambda (x y) (eq? x y)))
;		   ((eq? key=? fix:=) (lambda (x y) (fix:= x y)))
;		   ((eq? key=? flo:=) (lambda (x y) (flo:= x y)))
;		   (else key=?))
;	     (cond ((eq? key<? fix:<) (lambda (x y) (fix:< x y)))
;		   ((eq? key<? flo:<) (lambda (x y) (flo:< x y)))
;		   (else key<?)))
) ; end define

(define-public (rb-tree? t) (is-a? t <rb-tree>))

(define (guarantee-rb-tree tree procedure)
"Asserts that type of @var{tree} is <rb-tree>."
  (if (not (rb-tree? tree))
      (error "Wrong argument type. " tree "red/black tree" procedure))
)

(define-class <rb-tree-node> ()
  (key #:getter node-key #:setter set-node-key! #:init-keyword #:key)
  (value #:getter node-value #:setter set-node-value! #:init-keyword #:value)
  (up #:init-value #f #:setter set-node-up! #:getter node-up)
  (left #:init-value #f #:setter set-node-left! #:getter node-left)
  (right #:init-value #f #:setter set-node-right! #:getter node-right)
  (color #:init-value #f #:setter set-node-color! #:getter node-color)
) ; end define class

(define (make-node key value) (make <rb-tree-node> #:key key #:value value))

(define-public (node-pair node)
"Use the node as the key and value (node for a set)."
  (cons (node-key node) (node-value node)))

;;; The algorithms are left/right symmetric, so abstract "directions"
;;; permit code to be used for either symmetry:

(define (left? dir) (eq? 'LEFT dir))
(define (right? dir) (eq? 'RIGHT dir))

(define (b->dir left?)
"Convert predicate result to direction (symbol)."
  (if left? 'LEFT 'RIGHT))

(define (other-dir dir)
  (if (left? dir) 'RIGHT 'LEFT))

(define (get-link+ p d)
  (if (left? d)
      (node-left p)
      (node-right p)))

(define (set-link+! p d l)
  (if (left? d)
      (set-node-left! p l)
      (set-node-right! p l)))

(define (get-link- p d)
  (if (right? d)
      (node-left p)
      (node-right p)))

(define (set-link-! p d l)
  (if (right? d)
      (set-node-left! p l)
      (set-node-right! p l)))

(define (rotate+! tree x d)
  ;; Assumes (NOT (NOT (GET-LINK- X D))).
  (let ((y (get-link- x d)))
    (let ((beta (get-link+ y d)))
      (set-link-! x d beta)
      (if beta (set-node-up! beta x)))
    (let ((u (node-up x)))
      (set-node-up! y u)
      (cond ((not u)
	     (set-tree-root! tree y))
	    ((eq? x (get-link+ u d))
	     (set-link+! u d y))
	    (else
	     (set-link-! u d y))))
    (set-link+! y d x)
    (set-node-up! x y)))

(define (rotate-! tree x d)
  (rotate+! tree x (other-dir d)))

(define (red? n) (eq? 'RED (node-color n)))
(define (black? n) (eq? 'BLACK (node-color n)))
(define (red! n) (set-node-color! n 'RED))
(define (black! n) (set-node-color! n 'BLACK))

; disable interrupts to prevent structure corruption
(define (without-interrupts thunk)
  (if (defined? 'call-with-blocked-asyncs)
    (call-with-blocked-asyncs thunk)
; else try older deprecated API
    (begin
      (mask-signals)
      (thunk)
      (unmask-signals)
    ) ; end begin
  ) ; end if
) ; end define

(define (inserter! tree key value mutator!)
"@var{mutator!} is a function that operates on a (node, value), 
e.g. set-node-value!."
  (guarantee-rb-tree tree 'RB-TREE/INSERT!)
  (let ((key=? (tree-key=? tree))
	(key<? (tree-key<? tree)))
    (let loop ((x (tree-root tree)) (y #f) (d #f))
      (cond ((not x)
	     (let ((z (make-node key value)))
	       (without-interrupts
		(lambda ()
		  (set-node-up! z y)
		  (cond ((not y) (set-tree-root! tree z))
			((left? d) (set-node-left! y z))
			(else (set-node-right! y z)))
		  (red! z)
		  (insert-fixup! tree z)))))
	    ((key=? key (node-key x)) (mutator! x value))
	    ((key<? key (node-key x)) (loop (node-left x) x 'LEFT))
	    (else (loop (node-right x) x 'RIGHT))))))

(define-public (rb-tree/insert! tree key value)
"Associate @arg{value} with @arg{key}, creating a new entry or 
replacing/overwriting old one.  Returns no value."
  (inserter! tree key value set-node-value!)
) ; end define

(define-public (rb-tree/insert-if-new! tree key value)
"Associate @arg{value} with @arg{key} only if creating a new entry, else
keep old one.  Read that again: old entries are NOT overwritten.  
Returns a value if it was found in lookup, but not if it was newly added, 
otherwise returns void (unspecified)."
  (inserter! tree key value (lambda (n v) (node-value n)))
)

(define (insert-fixup! tree x)
  ;; Assumptions: X is red, and the only possible violation of the
  ;; tree properties is that (NODE-UP X) is also red.
  (let loop ((x x))
    (let ((u (node-up x)))
      (if (and u (red? u))
	  (let ((d (b->dir (eq? u (node-left (node-up u))))))
	    (let ((y (get-link- (node-up u) d)))
	      (if (and y (red? y))
		  ;; case 1
		  (begin
		    (black! u)
		    (black! y)
		    (red! (node-up u))
		    (loop (node-up u)))
		  (let ((x
			 (if (eq? x (get-link- u d))
			     ;; case 2
			     (begin
			       (rotate+! tree u d)
			       u)
			     x)))
		    ;; case 3
		    (let ((u (node-up x)))
		      (black! u )
		      (red! (node-up u))
		      (rotate-! tree (node-up u) d)))))))))
  (black! (tree-root tree)))

(define-public (alist->rb-tree alist key=? key<?)
"Important an assoc-list into a sorted map (red-black tree)."
  ;; Is there a more efficient way to do this?
  (let ((tree (make-rb-tree key=? key<?)))
    (do ((alist alist (cdr alist)))
	((null? alist))
      (rb-tree/insert! tree (caar alist) (cdar alist)))
    tree))

(define-public (rb-tree->stream tree)
"Converts a rb-tree to a stream, through alist.  "
  (list->stream (rb-tree->alist tree))
) ; end define

; TODO: stream->rb-tree

(define-public (rb-tree/delete! tree key)
"Dissociates key-value pair associated with @var{key}."
  (guarantee-rb-tree tree 'RB-TREE/DELETE!)
  (let ((key=? (tree-key=? tree))
	(key<? (tree-key<? tree)))
    (let loop ((x (tree-root tree)))
      (cond ((not x) (noop))
	    ((key=? key (node-key x)) (delete-node! tree x))
	    ((key<? key (node-key x)) (loop (node-left x)))
	    (else (loop (node-right x)))))))

(define (delete-node! tree z)
  (without-interrupts
   (lambda ()
     (let ((z
	    (if (and (node-left z) (node-right z))
		(let ((y (next-node z)))
		  (set-node-key! z (node-key y))
		  (set-node-value! z (node-value y))
		  y)
		z)))
       (let ((x (or (node-left z) (node-right z)))
	     (u (node-up z)))
	 (if x (set-node-up! x u))
	 (cond ((not u) (set-tree-root! tree x))
	       ((eq? z (node-left u)) (set-node-left! u x))
	       (else (set-node-right! u x)))
	 (if (black? z)
	     (delete-fixup! tree x u)))))))

(define (delete-fixup! tree x u)
  (let loop ((x x) (u u))
    (if (or (not u)
	    (and x (red? x)))
	(if x (black! x))
	(let ((d (b->dir (eq? x (node-left u)))))
	  (let ((w
		 (let ((w (get-link- u d)))
		   (if (red? w)
		       ;; case 1
		       (begin
			 (black! w)
			 (red! u )
			 (rotate+! tree u d)
			 (get-link- u d))
		       w)))
		(case-4
		 (lambda (w)
		   (set-node-color! w (node-color u))
		   (black! u)
		   (black! (get-link- w d))
		   (rotate+! tree u d)
		   (black! (tree-root tree)))))
	    (if (let ((n- (get-link- w d)))
		  (and n-
		       (red? n-)))
		(case-4 w)
		(let ((n+ (get-link+ w d)))
		  (if (or (not n+)
			  (black? n+))
		      ;; case 2
		      (begin
			(red! w)
			(loop u (node-up u)))
		      ;; case 3
		      (begin
			(black! n+)
			(red! w )
			(rotate-! tree w d)
			(case-4 (get-link- u d)))))))))))

(define (lookup-node tree key proc default)
  (let ((key=? (tree-key=? tree))
	(key<? (tree-key<? tree)))
    (let loop ((x (tree-root tree)))
      (cond ((not x) default)
	    ((key=? key (node-key x)) (proc x))
	    ((key<? key (node-key x)) (loop (node-left x)))
	    (else (loop (node-right x)))))))

(define-public (rb-tree/lookup-key tree key default)
"Return the key if it is associated in the tree to some value.  "
  (guarantee-rb-tree tree 'RB-TREE/LOOKUP)
  (lookup-node tree key node-key default)
)

(define-public (rb-tree/lookup tree key default)
"Return the value associated with @var{key}."
  (guarantee-rb-tree tree 'RB-TREE/LOOKUP)
  (lookup-node tree key node-value default)
)

(define-public (rb-tree/lookup-pair tree key default)
"Return the key-value pair associated with @var{key}."
  (guarantee-rb-tree tree 'RB-TREE/LOOKUP)
  (lookup-node tree key node-pair default)
)

(define-public (rb-tree/lookup-mutate! tree key proc-1 default)
"Alter the value associated with key, using procedure @var{proc-1}."
  (guarantee-rb-tree tree 'RB-TREE/LOOKUP-MUTATE!)
  (lookup-node tree key
    (lambda (x) (set-node-value! x (proc-1 (node-value x))))
    default)
)

(define (copy-subtree node up)
"This recursive copy preserves precise tree structure, is O(n)."
  (and node
    (let ((node* (make-node (node-key node) (node-value node))))
      (set-node-color! node* (node-color node))
      (set-node-up! node* up)
      (set-node-left! node* (copy-subtree (node-left node) node*))
      (set-node-right! node* (copy-subtree (node-right node) node*))
      node*)
  ) ; end and
) ; end define

(define-public (rb-tree/copy tree)
"Create a deep-copy of the tree nodes."
  (guarantee-rb-tree tree 'RB-TREE/COPY)
  (let ((result (make-rb-tree (tree-key=? tree) (tree-key<? tree))))
    (set-tree-root!
     result
     (copy-subtree (tree-root tree) #f)
;     (let loop ((node (tree-root tree)) (up #f))
;       (and node
;	    (let ((node* (make-node (node-key node) (node-value node))))
;	      (set-node-color! node* (node-color node))
;	      (set-node-up! node* up)
;	      (set-node-left! node* (loop (node-left node) node*))
;	      (set-node-right! node* (loop (node-right node) node*))
;	      node*))
;     ) ; end let
    ) ; end let
    result)
) ; end define

#!
"TODO: a filtered copy-if, whereby a predicate on the key-value pair is passed
Would like to reuse internal automatic tree-balancing without using insert!
Would remove! suffice?"
!#

(define (reduce-nodes node proc-2 default)
"Reduction visit of all nodes in the tree, O(n)."
  (if node
    (proc-2 (reduce-nodes (node-left node) proc-2 default)
      (reduce-nodes (node-right node) proc-2 default))
    default)
)

(define (reduce-visit tree proc-2 default)
"Reduction visit of all nodes in the tree, O(n)."
  (reduce-nodes (tree-root tree) proc-2 default)
;  (let loop ((node (tree-root tree)))
;    (if node
;	(proc-2 (loop (node-left node)) (loop (node-right node)))
;	default))
)

(define-public (rb-tree/height tree)
"Return the maximum depth of the tree, O(n)."
  (guarantee-rb-tree tree 'RB-TREE/HEIGHT)
  (reduce-visit tree (lambda (l r) (1+ (max l r))) 0)
)

(define-public (rb-tree/size tree)
"Return number of key-value pairs in tree, O(n)."
  (guarantee-rb-tree tree 'RB-TREE/SIZE)
  (reduce-visit tree (lambda (l r) (+ 1 l r)) 0)
)

(define-public (rb-tree/empty? tree)
"Are there any key-value pairs?"
  (guarantee-rb-tree tree 'RB-TREE/EMPTY?)
  (not (tree-root tree)))

(define-public (rb-tree/equal? x y value=?)
"Pair by pair comparison of two trees, not accounting for structural diffs."
  (guarantee-rb-tree x 'RB-TREE/EQUAL?)
  (guarantee-rb-tree y 'RB-TREE/EQUAL?)
  (let ((key=? (tree-key=? x)))
    (and (eq? key=? (tree-key=? y))
	 (let loop ((nx (min-node x)) (ny (min-node y)))
	   (if (not nx)
	       (not ny)
	       (and ny
		    (key=? (node-key nx) (node-key ny))
		    (value=? (node-value nx) (node-value ny))
		    (loop (next-node nx) (next-node ny))))))))

; returns nothing, maybe generalize like map-node or map-pairs?
; then apply, copy and treansform can share same code
(define (in-order-walk n thunk)
  (if n (begin
    (in-order-walk (node-left n) thunk)
    (thunk n)
    (in-order-walk (node-right n) thunk)
  ))
)

(define (pre-order-walk n thunk)
  (if n (begin
    (thunk n)
    (pre-order-walk (node-left n) thunk)
    (pre-order-walk (node-right n) thunk)
  ))
)

(define (post-order-walk n thunk)
  (if n (begin
    (post-order-walk (node-left n) thunk)
    (post-order-walk (node-right n) thunk)
    (thunk n)
  ))
)

; see also rb-tree/map-pairs
(define-public (rb-tree/for-each pair-proc tree)
"Apply a function to all key-value pairs, non destructive."
  (guarantee-rb-tree tree 'RB-TREE/FOR-EACH)
  (in-order-walk (tree-root tree) (lambda (n) (pair-proc (node-pair n))))
) ; endefine

(define-public (rb-tree/for-each-display-newline tree)
"Displays each key-value pair of rb-tree on its own line."
  (rb-tree/for-each (lambda (p) (display p) (newline)) tree)
) ; endefine

; TODO: reverse walks

(define (copy-insert-node node tree)
  (rb-tree/insert! tree (node-key node) (node-value node))
)

; untested! -- union (by key)
(define-public (rb-tree/merge x y combine-value)
"Merges two trees together, non-destructively.  
Apply combine-value on two values when their keys match."
  (guarantee-rb-tree x 'RB-TREE/MERGE)
  (guarantee-rb-tree y 'RB-TREE/MERGE)
  (let ((key=? (tree-key=? x))
        (key<? (tree-key<? x)))
    (and (eq? key=? (tree-key=? y))
      (eq? key<? (tree-key<? y))
      (let ((ret (make-rb-tree key=? key<?)))
	(let loop ((nx (min-node x)) (ny (min-node y)))
	  (cond
	    ((and nx ny)
              (cond
                ((key<? (node-key nx) (node-key ny))
                   (copy-insert-node nx ret)
                   (loop (next-node nx) ny))
                ((key=? (node-key nx) (node-key ny))
                   (rb-tree/insert! ret (node-key nx)
                     (combine-value (node-value nx) (node-value ny)))
                   (loop (next-node nx) (next-node ny)))
	        (else ; (key<? (node-key ny) (node-key nx))
                   (copy-insert-node ny ret)
                   (loop nx (next-node ny)))
              ) ; end cond
            ) ; end case
            (nx (in-order-walk nx (lambda (n) (copy-insert-node n ret))))
            (ny (in-order-walk ny (lambda (n) (copy-insert-node n ret))))
          ) ; end cond
        ) ; end let
        ret
      ) ; end let
    ) ; end and
  ) ; end let
) ; end define

; untested! -- intersection (by key)
(define-public (rb-tree/intersect x y combine-value)
"Creates intersection of two trees, non-destructively.  
Apply combine-value on two values when their keys match."
  (guarantee-rb-tree x 'RB-TREE/INTERSECT)
  (guarantee-rb-tree y 'RB-TREE/INTERSECT)
  (let ((key=? (tree-key=? x))
        (key<? (tree-key<? x)))
    (and (eq? key=? (tree-key=? y))
      (eq? key<? (tree-key<? y))
      (let ((ret (make-rb-tree key=? key<?)))
	(let loop ((nx (min-node x)) (ny (min-node y)))
	    (if (and nx ny)
              (cond
                ((key<? (node-key nx) (node-key ny))
                   (loop (next-node nx) ny))
                ((key=? (node-key nx) (node-key ny))
                   (rb-tree/insert! ret (node-key nx)
                     (combine-value (node-value nx) (node-value ny)))
                   (loop (next-node nx) (next-node ny)))
	        (else ; (key<? (node-key ny) (node-key nx))
                   (loop nx (next-node ny)))
              ) ; end cond
            ) ; end case
        ) ; end let
        ret
      ) ; end let
    ) ; end and
  ) ; end let
) ; end define

(define (rb-tree/map-nodes tree node-proc)
"Maps tree-leaves (nodes) into list, forward iterated."
  (guarantee-rb-tree tree 'RB-TREE/MAP-NODES)
  (let ((node (min-node tree)))
    (if node
	(let ((result (list (node-proc node))))
	  (let loop ((node (next-node node)) (prev result))
	    (if node
		(let ((pair (list (node-proc node))))
		  (set-cdr! prev pair)
		  (loop (next-node node) pair))))
	  result)
	'()))
) ; end define

; see also rb-tree/for-each
(define-public (rb-tree/map-pairs tree pair-proc)
"Map tree-leaves (key-value pairs) into list, forward iterated."
  (rb-tree/map-nodes tree (lambda (n) (pair-proc (node-pair n)))))

(define-public (rb-tree->alist tree)
"Converts tree into a non-sorted associative-list."
  (guarantee-rb-tree tree 'RB-TREE->ALIST)
  (rb-tree/map-nodes tree node-pair)
) ; end define

(define-method (write (obj <rb-tree>) port)
"Prints red-black tree as an associative list."
  (write (rb-tree->alist obj) port)
)

(define-public (rb-tree/key-list tree)
"Return list of keys (sorted)."
  (guarantee-rb-tree tree 'RB-TREE/KEY-LIST)
  (rb-tree/map-nodes tree node-key)
) ; end define

(define-public (rb-tree/value-list tree)
"Return list of values, in order of appearance by key."
  (guarantee-rb-tree tree 'RB-TREE/VALUE-LIST)
  (rb-tree/map-nodes tree node-value)
) ; end define

(define (min-node tree)
"Leftmost node of tree."
  (and (tree-root tree)
       (let loop ((x (tree-root tree)))
	 (if (node-left x)
	     (loop (node-left x))
	     x))))

(define (max-node tree)
"Rightmost node of tree."
  (and (tree-root tree)
       (let loop ((x (tree-root tree)))
	 (if (node-right x)
	     (loop (node-right x))
	     x))))

(define (next-node x)
"Node forward iteration."
  (if (node-right x)
       (let loop ((x (node-right x)))
	 (if (node-left x)
	     (loop (node-left x))
	     x))
      (let loop ((x x))
	(let ((y (node-up x)))
	  (if (and y (eq? x (node-right y)))
	      (loop y)
	      y)))))

; not used, defined for the hell of it
(define (prev-node x)
"Node reverse iteration."
  (if (node-left x)
       (let loop ((x (node-left x)))
	 (if (node-right x)
	     (loop (node-right x))
	     x))
      (let loop ((x x))
	(let ((y (node-up x)))
	  (if (and y (eq? x (node-left y)))
	      (loop y)
	      y)))))

(define (mm-find tree seek ret-proc root-default)
  (let ((node (seek tree)))
    (if node
	(ret-proc node)
	root-default))
)

(define (mm-del tree seek ret-proc root-default)
  (mm-find tree seek
    (lambda (node) 
      (let ((ret (ret-proc node)))
        (delete-node! tree node)
	ret)
    )
    root-default)
)

(define-public (rb-tree/min-key tree default)
  (guarantee-rb-tree tree 'RB-TREE/MIN-KEY)
  (mm-find tree min-node node-key default))

(define-public (rb-tree/min-value tree default)
  (guarantee-rb-tree tree 'RB-TREE/MIN-VALUE)
  (mm-find tree min-node node-value default))

(define-public (rb-tree/min-pair tree)
  (guarantee-rb-tree tree 'RB-TREE/MIN-PAIR)
  (mm-find tree min-node node-pair #f))

(define-public (rb-tree/delete-min! tree default)
  (guarantee-rb-tree tree 'RB-TREE/DELETE-MIN!)
  (mm-del tree min-node node-key default))

(define-public (rb-tree/delete-min-value! tree default)
  (guarantee-rb-tree tree 'RB-TREE/DELETE-MIN-VALUE!)
  (mm-del tree min-node node-value default))

(define-public (rb-tree/delete-min-pair! tree)
  (guarantee-rb-tree tree 'RB-TREE/DELETE-MIN-PAIR!)
  (mm-del tree min-node node-pair #f))

(define-public (rb-tree/max-key tree default)
  (guarantee-rb-tree tree 'RB-TREE/MAX-KEY)
  (mm-find tree max-node node-key default))

(define-public (rb-tree/max-value tree default)
  (guarantee-rb-tree tree 'RB-TREE/MAX-VALUE)
  (mm-find tree max-node node-value default))

(define-public (rb-tree/max-pair tree)
  (guarantee-rb-tree tree 'RB-TREE/MAX-PAIR)
  (mm-find tree max-node node-pair #f))

(define-public (rb-tree/delete-max! tree default)
  (guarantee-rb-tree tree 'RB-TREE/DELETE-MAX!)
  (mm-del tree max-node node-key default))

(define-public (rb-tree/delete-max-value! tree default)
  (guarantee-rb-tree tree 'RB-TREE/DELETE-MAX-VALUE!)
  (mm-del tree max-node node-value default))

(define-public (rb-tree/delete-max-pair! tree)
  (guarantee-rb-tree tree 'RB-TREE/DELETE-MAX-PAIR!)
  (mm-del tree max-node node-pair #f))

#!
"Increment counter by 1 or set to 1 if entry doesn't already exist at key."
!#
(define-public (rb-tree/increment! tree key)
  (let ((c (rb-tree/insert-if-new! tree key 1)))
    (if (not (unspecified? c)) ; then we found previous entry
      (rb-tree/lookup-mutate! tree key 1+ #f) ; increment
    ) ; end if
  ) ; end let
) ; define

