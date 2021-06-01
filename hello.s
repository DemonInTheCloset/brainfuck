# -------------------------------------------------------------
# source: https://gist.github.com/carloscarcamo/6833d19b726af698e62b
# Using Linux System calls for 64-bit
# to run:
#   gcc -c hello.s && ld hello.o && ./a.out
# o
#   gcc -nostdlib hello.s && ./a.out
#
# --------------------------------------------------------------
.intel_syntax noprefix
.global _start

.text

_start:
    # write (1, msj, 13)
    mov rax, 1              # system call 1 is write
    mov rdi, 1              # file handler 1 is stdout
    mov rsi, message        # address of string to output
    mov rdx, 13             # number of bytes
    syscall

    # exit(0)
    mov rax, 60             # system call 60 is exit
    xor rdi, rdi            # we want to return code 0
    syscall

message:
    .ascii "Hello, world\n"
