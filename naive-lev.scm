(define (lev a b)
  (cond ((= (length b) 0)
         (length a))
        ((= (length a) 0)
         (length b))
        ((eq? (car a) (car b))
         (lev (cdr a) (cdr b)))
        (else (let ((case0 (lev (cdr a) b))
                    (case1 (lev a (cdr b)))
                    (case2 (lev (cdr a) (cdr b))))
                (+ 1 (min case0 case1 case2))))))

(define (alev a b)
  (lev (string->list (symbol->string a))
       (string->list (symbol->string b))))