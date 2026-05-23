; =============================================================================
; MajesticOS Bootloader
; A retro-style operating system
; Loaded by BIOS at 0x7C00
; =============================================================================

[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET equ 0x1000   ; Memory offset where kernel will be loaded

start:
    ; Disable interrupts, set up segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Save boot drive
    mov [BOOT_DRIVE], dl

    ; Print boot message
    mov si, MSG_BOOT
    call print_string

    ; Load kernel from disk
    call load_kernel

    ; Switch to 32-bit protected mode
    call switch_to_pm

    ; Should never reach here
    jmp $

; =============================================================================
; Load Kernel from Disk
; =============================================================================
load_kernel:
    mov si, MSG_LOAD_KERNEL
    call print_string

    mov bx, KERNEL_OFFSET   ; ES:BX = destination
    mov dh, 32              ; Load 32 sectors
    mov dl, [BOOT_DRIVE]    ; Drive number
    call disk_load
    ret

; =============================================================================
; Disk Load Routine
; =============================================================================
disk_load:
    push dx
    mov ah, 0x02            ; BIOS read sectors
    mov al, dh              ; Number of sectors
    mov ch, 0x00            ; Cylinder 0
    mov cl, 0x02            ; Start from sector 2 (sector 1 = bootloader)
    mov dh, 0x00            ; Head 0
    int 0x13

    jc disk_error
    pop dx
    cmp dh, al
    jne sector_error
    ret

disk_error:
    mov si, MSG_DISK_ERROR
    call print_string
    jmp $

sector_error:
    mov si, MSG_SECTOR_ERROR
    call print_string
    jmp $

; =============================================================================
; Print String (16-bit real mode)
; =============================================================================
print_string:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

; =============================================================================
; Switch to 32-bit Protected Mode
; =============================================================================
switch_to_pm:
    cli
    lgdt [gdt_descriptor]

    ; Set PE bit in CR0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to flush pipeline and enter 32-bit mode
    jmp CODE_SEG:init_pm

[BITS 32]
init_pm:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel
    call KERNEL_OFFSET

    jmp $

; =============================================================================
; GDT (Global Descriptor Table)
; =============================================================================
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0

gdt_code:
    ; Code segment descriptor
    dw 0xffff       ; Limit (bits 0-15)
    dw 0x0000       ; Base (bits 0-15)
    db 0x00         ; Base (bits 16-23)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x00         ; Base (bits 24-31)

gdt_data:
    ; Data segment descriptor
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; =============================================================================
; Data
; =============================================================================
BOOT_DRIVE    db 0
MSG_BOOT         db "MajesticOS Bootloader v1.0", 13, 10, 0
MSG_LOAD_KERNEL  db "Loading kernel...", 13, 10, 0
MSG_DISK_ERROR   db "ERROR: Disk read failed!", 13, 10, 0
MSG_SECTOR_ERROR db "ERROR: Sector count mismatch!", 13, 10, 0

; =============================================================================
; Boot sector padding and signature
; =============================================================================
times 510-($-$$) db 0
dw 0xAA55
