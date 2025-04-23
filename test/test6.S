.section ".text"

sub sp, sp, #16
mov x0, #15
mvn x0, x0
and sp, sp, x0

