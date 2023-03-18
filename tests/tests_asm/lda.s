 lda #$01
 lda $0F
 lda $010F
 
 .org $0F
 .byte $02
 
 .org $010F
 .byte $03
 
 .org $FFFC
 .word $0000
 .word $0000
