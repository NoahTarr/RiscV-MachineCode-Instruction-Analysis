.globl arrCompare

arrCompare:
	addi sp, sp , -4 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	
	#mv t0, a0	#array A
	#mv t1, a1	#array B
	li t0, 1024
	
	li t1, 0	#i
	for:
        slli t2, t1, 2	#t2 = i * 4
        
		#get A[i]
        add t3, a0, t2	#t2 = &arrU[i]
        lw t3, 0(t3)	#t3 = arrU[i]
        
		#get B[i]
        add t4, a1, t2	#t4 = &arrU[i]
        lw t4, 0(t4)	#t5 = arrU[i]
        
        bne t3, t4, notEqual	#A[i] != B[i]
        addi t1, t1, 1		#i++
        bge t1, t0, equal	#i >= 1024
        b for
        
	notEqual:
		li a0, 0	#Output false for noequal
		mv a1, t1	#differing index
		b exit
		
	equal:
		li a0, 1	#Output true for equal
	
exit:
	lw ra, 0(sp) # Restore return address register
	addi sp, sp, 4 # deallocate space for stack frame
	ret # return to calling point