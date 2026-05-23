; =============================================================================
; MajesticOS Kernel Entry Point
; Bridges bootloader (assembly) to C kernel
; =============================================================================

[BITS 32]

; External C function
extern kernel_main

global _start

_start:
    call kernel_main
    ; If kernel_main returns, halt forever
    hlt
    jmp $
