#assume always 4kb of 32-bit unsigned numbers
#e.g. arrUns always 1024 values in length
#.data
#arrLen: 
#	.word 1024
	
.globl insert

insert:
	addi sp, sp , -16 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	sw s0, 4(sp)
	sw s1, 8(sp)
	sw s2, 12(sp)
	
	jal arrCopy
	
	mv s0, a0	#output address
	
	li s2, 0	#i
	whilei:
		addi s2, s2, 1	#i++
		
		#exit if i == len(arr); len(arr) := 1024
		li t0, 1024
		beq s2, t0, exit
		
		mv s3, s2	#j = i
		whilej:
			#get arrUns[j]
        	slli t0, s3, 2	#t0 = j * 4
        	add t0, s0, t0	#t0 = &arr[j]
        	lw t1, 0(t0)	#t1 = arr[j]
        	
			#get arrUns[j-1]
			addi t2, s3, -1	#t2 = j - 1
        	slli t2, t2, 2	#t2 = (j-1) * 4
        	add t2, s0, t2	#t2 = &arr[j-1]
        	lw t3, 0(t2)	#t3 = arr[j-1]
        	
        	#break if arr[j-1] <= arr[j]
        	ble t3, t1, whilei
        	
        	#swap(arr[j], arr[j-1])
        	sw t3, 0(t0)	#&arr[j] = arr[j-1]
        	sw t1, 0(t2)	#&arr[j-1] = arr[j]
        	
        	addi s3, s3, -1	#j--
        	beq s3, zero whilei
        	beq zero, zero whilej
	
	
	
exit:
	mv a0, s0
	lw s2, 12(sp)
	lw s1, 8(sp)
	lw s0, 4(sp)
	lw ra, 0(sp) # Restore return address register
	addi sp, sp, 16 # deallocate space for stack frame
	ret # return to calling point
	
	
	
arrCopy:
	addi sp, sp , -4 # allocate space for stack frame
	sw ra, 0(sp) # save the return address (ra register) 
	
	mv t0, a0	#unsorted array(arrU)
	mv t1, a1	#empty array(arrE)
	li t2, 1024
	
	li t3, 0	#i
	for:
		#get arrU[i]
        slli t4, t3, 2	#t4 = i * 4
        add t4, t1, t4	#t4 = &arrU[i]
        lw t4, 0(t4)	#t4 = arrU[i]
        
		#get arrE[i]
        slli t5, t3, 2	#t5 = i * 4
        add t5, t0, t5	#t5 = &arrE[i]
        
        sw t4, 0(t5)	#arrE[i] = arrU[i]
		addi t3, t3, 1	#i++
		blt t3, t2, for
	
	mv a0, t0
	mv a1, t1
	lw ra, 0(sp) # Restore return address register
	addi sp, sp, 4 # deallocate space for stack frame
	ret # return to calling point
