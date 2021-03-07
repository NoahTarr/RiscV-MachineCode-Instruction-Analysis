.globl merge

merge:
	addi sp, sp , -16 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	sw s0, 4(sp)
	sw s1, 8(sp)
	sw s2, 12(sp)
	
	#a0 -> empty array to store output
	#a1 -> constant unsorted array of values
	#a2 -> empty 16kB block
	mv s0, a0
	mv s1, a1
	mv s2, a2
	
	#copy unsorted array to empty array
	jal arrCopy #copy a1 to a0
	
	#use first 4kB of s2 to store copy of unsorted array
	mv a0, s2
	jal arrCopy #copy a1 to a0
	
	#use second 4kB of a2 to store copy of a1
	#li t0, 0x1000
	#add a0, s2, t0
	#jal arrCopy #copy a1 to a0
	
	#topDownSplitMerge(B, 0, n, A)
	mv a0, s2	#a0 = &s2
	li a1, 0	#iBegin
	li a2, 1024	#iEnd
	mv a3, s0	#a3 == &s0
	jal topDownSplitMerge
	
	#mv a0, s0
	#mv a1, s2
	#jal arrCopy #copy a1 to a0
	
	mv a0, s0
	mv a1, s1
	mv a2, s2
	
	lw s2, 12(sp)
	lw s1, 8(sp)
	lw s0, 4(sp)
	lw ra, 0(sp) # Restore return address register
	addi sp, sp, 16 # deallocate space for stack frame
	ret # return to calling point
	
	
	
#topDownSplitMerge(B, iBegin, iEnd, A)
topDownSplitMerge:
	addi sp, sp , -24 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	sw s0, 4(sp)
	sw s1, 8(sp)
	sw s2, 12(sp)
	sw s3, 16(sp)
	sw s4, 20(sp)
	
	mv s0, a0
	mv s1, a1
	mv s2, a2
	mv s3, a3
	
	#return if iEnd - iBegin - 1 <= 0
	sub t0, s2, s1
	addi t0, t0, -1
	blez t0, exittopDownSplitMerge
	
	#iMiddle(s4) = (iEnd + iBegin)/2
	add s4, s2, s1
	srli s4, s4, 1 
	
	#recursively sort both runs from A into B
	#Sort the left run
	#topDownSplitMerge(A, iBegin, iMiddle, B) == (s3, s1, s4, s0)
	mv a0, s3
	mv a1, s1
	mv a2, s4
	mv a3, s0
	jal topDownSplitMerge 
	#Sort the right run
	#topDownSplitMerge(A, iMiddle, iEnd, B) == (s3, s4, s2, s0)
	mv a0, s3
	mv a1, s4
	mv a2, s2
	mv a3, s0
	jal topDownSplitMerge 
	
	#merge resulting runs from B into A
	#topDownMerge(B, iBegin, iMiddle, iEnd, A) = (s0, s1, s4, s2, s3
	mv a0, s0
	mv a1, s1
	mv a2, s4
	mv a3, s2
	mv a4, s3
	jal topDownMerge
	
exittopDownSplitMerge:
	lw s4, 20(sp)
	lw s3, 16(sp)
	lw s2, 12(sp)
	lw s1, 8(sp)
	lw s0, 4(sp)
	lw ra, 0(sp) # Restore return address register
	addi sp, sp, 24 # deallocate space for stack frame
	ret # return to calling point



#Left source half is 	A[ iBegin:iMiddle-1]
#Right source half is 	A[iMiddle:iEnd-1]
#Result is 				B[ iBegin:iEnd-1]
#topDownMerge(A, iBegin, iMiddle, iEnd, B)
topDownMerge:
	addi sp, sp , -4 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	
	mv t0, a0
	mv t1, a1	#i
	mv t2, a2	#j
	#mv t3, a3
	mv t4, a4
	
	#for (k = iBegin; k < iEnd; k++)
	mv t3, t1	#k = i
	for: 
		#get A[i]
        slli t5, t1, 2	#t5 = i * 4
        add t5, t0, t5	#t5 = &A[i]
        lw t5, 0(t5)	#t5 = A[i]
		
		#get A[j]
        slli t6, t2, 2	#t6 = j * 4
        add t6, t0, t6	#t6 = &A[j]
        lw t6, 0(t6)	#t6 = A[j]
        
		#If left run head exists and is <= existing right run head
		#if (i < iMiddle && (j >= iEnd || A[i] <= A[j]))
		bgeu t1, a2, else	#i >= iMiddle -> else
		bgeu t2, a3, if		#j >= iEnd -> if
		bgtu t5, t6, else	#A[i] > A[j] -> else
		
		if:
			#get B[k]
        	slli t6, t3, 2	#t6 = k * 4
        	add t6, t4, t6	#t6 = &B[k]
        	sw t5, 0(t6)	#B[k] = A[i]
			addi t1, t1, 1	#i++
			b forNext
		else:
			#get B[k]
        	slli t5, t3, 2	#t5 = k * 4
        	add t5, t4, t5	#t5 = &B[k]
        	sw t6, 0(t5)	#B[k] = A[j]
			addi t2, t2, 1	#j++
		
		forNext:
			addi t3, t3, 1	#k++
			blt t3, a3, for
		
	
	lw ra, 0(sp) # Restore return address register
	addi sp, sp, 4 # deallocate space for stack frame
	ret # return to calling point
	
	
	
arrCopy:
	addi sp, sp , -4 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	
	mv t0, a0	#unsorted array(arrU)
	mv t1, a1	#empty array(arrE)
	li t2, 1024
	
	li t3, 0	#i
	forCopy:
		#get arrU[i]
        slli t4, t3, 2	#t4 = i * 4
        add t4, t1, t4	#t4 = &arrU[i]
        lw t4, 0(t4)	#t4 = arrU[i]
        
		#get arrE[i]
        slli t5, t3, 2	#t5 = i * 4
        add t5, t0, t5	#t5 = &arrE[i]
        
        sw t4, 0(t5)	#arrE[i] = arrU[i]
		addi t3, t3, 1	#i++
		blt t3, t2, forCopy
	
	mv a0, t0
	mv a1, t1
	lw ra, 0(sp) # Restore return address register
	addi sp, sp, 4 # deallocate space for stack frame
	ret # return to calling point