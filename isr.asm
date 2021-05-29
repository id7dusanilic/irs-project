;--------------------------------------------------------------------------------------
                .cdecls C,LIST,"msp430.h"           ; Include device header file

;--------------------------------------------------------------------------------------
                .ref    timing_in_progress          ; Timing in progress flag
                .ref    low_count                   ; Button low state count
                .ref    high_count                  ; Button high state count
;--------------------------------------------------------------------------------------

;--------------------------------------------------------------------------------------
; Interrupt Service Routines
;--------------------------------------------------------------------------------------
                .text                               ; Assemble into program memory.

;--------------------------------------------------------------------------------------
; PORT2 Interrupt Service Routine
;
; If a falling edge occurred on button S1, starts the timer for debouncing
;--------------------------------------------------------------------------------------
PORT2_ISR       bit.b   #BIT1, &P2IFG               ; Checking if the event was on
                                                    ; button S1
                jz      port2_isr_ret
                bic.b   #BIT1, &P2IE                ; Disabling interrupts on P2.1
                bic.b   #BIT1, &P2IFG               ; Clearing the flag
                bis.w   #MC__UP, &TA0CTL            ; Starting TA0 in up mode
port2_isr_ret   reti

;--------------------------------------------------------------------------------------
; Timer A0 CCR0 Interrupt service routine
;
; Debounces button S1. If the button is pressed, timing process is started,
; and previous (if running) is stopped.
;--------------------------------------------------------------------------------------
TA0CCR0_ISR     bic.w   #0x0030, &TA0CTL            ; Stopping TA0
                bis.w   #TACLR, &TA0CTL             ; Reseting TA0
                bit.b   #BIT1, &P2IN                ; Testing if button S1 is pressed
                jnz     not_pressed
pressed         bit.w   #BIT1, &timing_in_progress  ; If there was a timing session
                jz      start_timing                ; running when pressing the button
                                                    ; first end the current session
                bic.w   #0x0030, &TA1CTL            ; Stopping TA1
                bis.w   #TACLR, &TA1CTL             ; Reseting TA1
                mov.w   #0x0000, &low_count         ; Reseting low count
                mov.w   #0x0000, &high_count        ; Reseting high count
start_timing    bis.w   #MC__UP, &TA1CTL            ; Starting TA1 in up mode
                bis.w   #BIT1, &timing_in_progress  ; New timing session starting
                bic.b   #BIT7, &P4OUT               ; Turning the LED2 off to signal
                                                    ; that there is a code in progress
                jmp     ta0ccr0_isr_ret
not_pressed     bic.b   #BIT1, &P2IFG               ; Clearing the flag
                bis.b   #BIT1, &P2IE                ; Enabling interrupts on P2.1
ta0ccr0_isr_ret reti

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
                .sect   PORT2_VECTOR                ; PORT2 Vector
                .short  PORT2_ISR
                .sect   TIMER0_A0_VECTOR            ; TA0CCR0 Vector
                .short  TA0CCR0_ISR
