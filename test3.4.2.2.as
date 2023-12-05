        lw      0       1       input           
        lw      0       4       subAdr          
        jalr    4       7                       
        halt
sub4n   lw      0       6       pos1            
        sw      5       7       stack           
        add     5       6       5               
        sw      5       1       stack           
        add     5       6       5               
        add     1       1       1               
        add     1       1       3               
        lw      0       6       neg1            
        add     5       6       5               
        lw      5       1       stack           
        add     5       6       5               
        lw      5       7       stack           
        jalr    7       4                       
pos1    .fill   1
neg1    .fill   -1
subAdr  .fill   sub4n                           
input   .fill   10                              
stack   .fill   0                               
stack1  .fill   0       