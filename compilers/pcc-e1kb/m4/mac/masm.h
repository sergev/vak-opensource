divert(-1)

define( `__n', 0)
define( `__getlab', `define(`__n',incr(__n))__n')
define( `__if', `define(`__endif',__getlab)$1 L.__endif')
define( `ifz', `__if(`uia')')
define( `ifi', `__if(`uza')')
define( `if1', `__if(`uza')')
define( `ifregz', `__if(`vim'),$1')
define( `ifreg1', `__if(`vzm'),$1')
define( `ifregi', `__if(`vzm'),$1')

define( `endif', `
L.__endif:')

define( `else', `define(`__else',__endif)define(`__endif',__getlab)uj L.__endif
L.__else:')

define( `loop', `define(`__begloop',__getlab)define(`__endloop',__getlab)
L.__begloop:')

define( `endloop', `uj L.__begloop
L.__endloop:')

define( `contloop', `uj L.__begloop')
define( `breakloop', `uj L.__endloop')

divert(0)
