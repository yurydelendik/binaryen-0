	.text
	.file	"/b/build/slave/linux/build/src/src/work/gcc/gcc/testsuite/gcc.c-torture/execute/pr48809.c"
	.section	.text.foo,"ax",@progbits
	.hidden	foo
	.globl	foo
	.type	foo,@function
foo:                                    # @foo
	.param  	i32
	.result 	i32
	.local  	i32
# BB#0:                                 # %entry
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	block
	i32.const	$push0=, 32
	i32.gt_u	$push1=, $0, $pop0
	br_if   	0, $pop1        # 0: down to label31
# BB#1:                                 # %entry
	block
	tableswitch	$0, 0, 0, 4, 3, 2, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 3, 20, 21, 22, 23, 24, 25, 32, 26, 27, 28, 29, 30, 31 # 0: down to label32
                                        # 4: down to label28
                                        # 3: down to label29
                                        # 2: down to label30
                                        # 5: down to label27
                                        # 6: down to label26
                                        # 7: down to label25
                                        # 8: down to label24
                                        # 9: down to label23
                                        # 10: down to label22
                                        # 11: down to label21
                                        # 12: down to label20
                                        # 13: down to label19
                                        # 14: down to label18
                                        # 15: down to label17
                                        # 16: down to label16
                                        # 17: down to label15
                                        # 18: down to label14
                                        # 19: down to label13
                                        # 20: down to label12
                                        # 21: down to label11
                                        # 22: down to label10
                                        # 23: down to label9
                                        # 24: down to label8
                                        # 25: down to label7
                                        # 32: down to label0
                                        # 26: down to label6
                                        # 27: down to label5
                                        # 28: down to label4
                                        # 29: down to label3
                                        # 30: down to label2
                                        # 31: down to label1
.LBB0_2:                                # %sw.bb
	end_block                       # label32:
	i32.const	$0=, 1
	return  	$0
.LBB0_3:                                # %entry
	end_block                       # label31:
	i32.const	$push2=, -62
	i32.eq  	$push3=, $0, $pop2
	br_if   	0, $pop3        # 0: down to label30
# BB#4:                                 # %entry
	i32.const	$push4=, 98
	i32.eq  	$1=, $0, $pop4
	i32.const	$0=, 0
	i32.const	$push5=, 0
	i32.eq  	$push6=, $1, $pop5
	br_if   	1, $pop6        # 1: down to label29
# BB#5:                                 # %sw.bb33
	i32.const	$0=, 18
	return  	$0
.LBB0_6:                                # %sw.bb34
	end_block                       # label30:
	i32.const	$0=, 19
.LBB0_7:                                # %sw.epilog
	end_block                       # label29:
	return  	$0
.LBB0_8:                                # %sw.bb1
	end_block                       # label28:
	i32.const	$0=, 7
	return  	$0
.LBB0_9:                                # %sw.bb4
	end_block                       # label27:
	i32.const	$0=, 5
	return  	$0
.LBB0_10:                               # %sw.bb5
	end_block                       # label26:
	i32.const	$0=, 17
	return  	$0
.LBB0_11:                               # %sw.bb6
	end_block                       # label25:
	i32.const	$0=, 31
	return  	$0
.LBB0_12:                               # %sw.bb7
	end_block                       # label24:
	i32.const	$0=, 8
	return  	$0
.LBB0_13:                               # %sw.bb8
	end_block                       # label23:
	i32.const	$0=, 28
	return  	$0
.LBB0_14:                               # %sw.bb9
	end_block                       # label22:
	i32.const	$0=, 16
	return  	$0
.LBB0_15:                               # %sw.bb10
	end_block                       # label21:
	i32.const	$0=, 31
	return  	$0
.LBB0_16:                               # %sw.bb11
	end_block                       # label20:
	i32.const	$0=, 12
	return  	$0
.LBB0_17:                               # %sw.bb12
	end_block                       # label19:
	i32.const	$0=, 15
	return  	$0
.LBB0_18:                               # %sw.bb13
	end_block                       # label18:
	i32.const	$0=, 111
	return  	$0
.LBB0_19:                               # %sw.bb14
	end_block                       # label17:
	i32.const	$0=, 17
	return  	$0
.LBB0_20:                               # %sw.bb15
	end_block                       # label16:
	i32.const	$0=, 10
	return  	$0
.LBB0_21:                               # %sw.bb16
	end_block                       # label15:
	i32.const	$0=, 31
	return  	$0
.LBB0_22:                               # %sw.bb17
	end_block                       # label14:
	i32.const	$0=, 7
	return  	$0
.LBB0_23:                               # %sw.bb18
	end_block                       # label13:
	i32.const	$0=, 2
	return  	$0
.LBB0_24:                               # %sw.bb20
	end_block                       # label12:
	i32.const	$0=, 5
	return  	$0
.LBB0_25:                               # %sw.bb21
	end_block                       # label11:
	i32.const	$0=, 107
	return  	$0
.LBB0_26:                               # %sw.bb22
	end_block                       # label10:
	i32.const	$0=, 31
	return  	$0
.LBB0_27:                               # %sw.bb23
	end_block                       # label9:
	i32.const	$0=, 8
	return  	$0
.LBB0_28:                               # %sw.bb24
	end_block                       # label8:
	i32.const	$0=, 28
	return  	$0
.LBB0_29:                               # %sw.bb25
	end_block                       # label7:
	i32.const	$0=, 106
	return  	$0
.LBB0_30:                               # %sw.bb27
	end_block                       # label6:
	i32.const	$0=, 102
	return  	$0
.LBB0_31:                               # %sw.bb28
	end_block                       # label5:
	i32.const	$0=, 105
	return  	$0
.LBB0_32:                               # %sw.bb29
	end_block                       # label4:
	i32.const	$0=, 111
	return  	$0
.LBB0_33:                               # %sw.bb30
	end_block                       # label3:
	i32.const	$0=, 17
	return  	$0
.LBB0_34:                               # %sw.bb31
	end_block                       # label2:
	i32.const	$0=, 10
	return  	$0
.LBB0_35:                               # %sw.bb32
	end_block                       # label1:
	i32.const	$0=, 31
	return  	$0
.LBB0_36:                               # %sw.bb26
	end_block                       # label0:
	i32.const	$0=, 31
	return  	$0
	.endfunc
.Lfunc_end0:
	.size	foo, .Lfunc_end0-foo

	.section	.text.main,"ax",@progbits
	.hidden	main
	.globl	main
	.type	main,@function
main:                                   # @main
	.result 	i32
# BB#0:                                 # %if.end25
	i32.const	$push0=, 0
	return  	$pop0
	.endfunc
.Lfunc_end1:
	.size	main, .Lfunc_end1-main


	.ident	"clang version 3.9.0 "
