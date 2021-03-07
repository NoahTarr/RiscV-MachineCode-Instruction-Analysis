.globl bsearch

bsearch:
	addi sp, sp , -4 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	
	#a0 = A (array begin search)
	#a1 = K (Value being searched for)
	li t0, 0		#L, Left Ptr
	li t1, 1023		#R, Right Ptr
	
	doWhile:
		#if L > R -> return -1
		bgt t0, t1, notEqual
		
		add t2, t0, t1	#m = L + R
		srli t2, t2, 1	#m = floor(m/2)
		
		#get A[m]
        slli t3, t2, 2	#t3 = m * 4
        add t3, a0, t3	#t3 = &A[m]
        lw t3, 0(t3)	#t3 = A[m]
        
        #if A[m] == K -> return m
        beq t3, a1, equal
        
        #if A[m] < T -> L := m + 1
        bgt t3, a1, greaterThanKey
        	addi t0, t2, 1	#L := m + 1
        	b doWhile
        	
        #if A[m] > T -> R := m - 1
        greaterThanKey:
        	addi t1, t2, -1	#R := m - 1
        	b doWhile
	
	equal:
		mv a0, t2	#return m(index of value)
        b exit
        
    notEqual:
    	li a0, -1	#return -1 b/c value not found
	
exit:
	addi sp, sp, 4 # deallocate space for stack frame
	ret # return to calling point