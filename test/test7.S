.section ".text"

sub sp, sp, #16
mov x0, sp
and x0, x0, #-16
mov sp, x0

