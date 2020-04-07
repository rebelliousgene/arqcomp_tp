MOV AX, %001F //omite prompt
MOV [1], 150
MOV DX, 1
ADD CX, 12
SYS 2

