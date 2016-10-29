#! /usr/local/bin/apl --script --

⎕ARG    ⍝ show command line options

⎕IO←0
⎕PP←18
⍉3 17⍴ (⍳17) , (17 / ⊂'!=') , !⍳17

)OFF    ⍝ leave the interpreter
