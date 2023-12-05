        lw      0       1       input   
        lw      0       2       pos1    
        lw      2       5       pos1    
        nand    1       0       3       
        nand    1       1       4       
        add     2       3       3       
        add     2       4       4       
        sw      5       3       0       
        sw      5       4       1       
        noop
        halt
input   .fill   20
pos1    .fill   1
mempos  .fill   none1
none1   .fill   0
none2   .fill   0

