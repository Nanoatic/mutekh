
     m4_define(`m4_forloop',
            `m4_pushdef(`$1', `$2')m4__forloop(`$1', `$2', `$3', `$4')m4_popdef(`$1')')
     m4_define(`m4__forloop',
            `$4`'m4_ifelse($1, `$3', ,
                   `m4_define(`$1', m4_incr($1))m4__forloop(`$1', `$2', `$3', `$4')')')

     m4_define(m4_concat, $1$2$3)

