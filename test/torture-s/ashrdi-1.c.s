	.text
	.file	"/b/build/slave/linux/build/src/src/work/gcc/gcc/testsuite/gcc.c-torture/execute/ashrdi-1.c"
	.section	.text.main,"ax",@progbits
	.hidden	main
	.globl	main
	.type	main,@function
main:                                   # @main
	.result 	i32
	.local  	i64, i32, i32
# BB#0:                                 # %entry
	i64.const	$0=, 0
	i32.const	$2=, zext
.LBB0_1:                                # %for.body
                                        # =>This Inner Loop Header: Depth=1
	block
	block
	loop                            # label2:
	i64.const	$push20=, 8526495107234113920
	i64.shr_u	$push0=, $pop20, $0
	i64.load	$push1=, 0($2)
	i64.ne  	$push2=, $pop0, $pop1
	br_if   	2, $pop2        # 2: down to label1
# BB#2:                                 # %for.cond
                                        #   in Loop: Header=BB0_1 Depth=1
	i64.const	$push19=, 1
	i64.add 	$0=, $0, $pop19
	i32.const	$push18=, 8
	i32.add 	$2=, $2, $pop18
	i64.const	$push17=, 63
	i64.le_s	$push3=, $0, $pop17
	br_if   	0, $pop3        # 0: up to label2
# BB#3:
	end_loop                        # label3:
	i64.const	$0=, 0
	i32.const	$2=, sext
.LBB0_4:                                # %for.body4
                                        # =>This Inner Loop Header: Depth=1
	loop                            # label4:
	i64.const	$push24=, -8152436031399644656
	i64.shr_s	$push4=, $pop24, $0
	i64.load	$push5=, 0($2)
	i64.ne  	$push6=, $pop4, $pop5
	br_if   	2, $pop6        # 2: down to label1
# BB#5:                                 # %for.cond2
                                        #   in Loop: Header=BB0_4 Depth=1
	i64.const	$push23=, 1
	i64.add 	$0=, $0, $pop23
	i32.const	$push22=, 8
	i32.add 	$2=, $2, $pop22
	i64.const	$push21=, 63
	i64.le_s	$push7=, $0, $pop21
	br_if   	0, $pop7        # 0: up to label4
# BB#6:
	end_loop                        # label5:
	i32.const	$2=, 0
	i32.const	$1=, zext
.LBB0_7:                                # %for.body16
                                        # =>This Inner Loop Header: Depth=1
	loop                            # label6:
	i64.const	$push28=, 8526495107234113920
	i64.call	$push8=, constant_shift@FUNCTION, $pop28, $2
	i64.load	$push9=, 0($1)
	i64.ne  	$push10=, $pop8, $pop9
	br_if   	3, $pop10       # 3: down to label0
# BB#8:                                 # %for.cond14
                                        #   in Loop: Header=BB0_7 Depth=1
	i32.const	$push27=, 1
	i32.add 	$2=, $2, $pop27
	i32.const	$push26=, 8
	i32.add 	$1=, $1, $pop26
	i32.const	$push25=, 63
	i32.le_s	$push11=, $2, $pop25
	br_if   	0, $pop11       # 0: up to label6
# BB#9:
	end_loop                        # label7:
	i32.const	$2=, 0
	i32.const	$1=, sext
.LBB0_10:                               # %for.body28
                                        # =>This Inner Loop Header: Depth=1
	block
	loop                            # label9:
	i64.const	$push32=, -8152436031399644656
	i64.call	$push12=, constant_shift@FUNCTION, $pop32, $2
	i64.load	$push13=, 0($1)
	i64.ne  	$push14=, $pop12, $pop13
	br_if   	2, $pop14       # 2: down to label8
# BB#11:                                # %for.cond26
                                        #   in Loop: Header=BB0_10 Depth=1
	i32.const	$push31=, 1
	i32.add 	$2=, $2, $pop31
	i32.const	$push30=, 8
	i32.add 	$1=, $1, $pop30
	i32.const	$push29=, 63
	i32.le_s	$push15=, $2, $pop29
	br_if   	0, $pop15       # 0: up to label9
# BB#12:                                # %for.end37
	end_loop                        # label10:
	i32.const	$push16=, 0
	call    	exit@FUNCTION, $pop16
	unreachable
.LBB0_13:                               # %if.then33
	end_block                       # label8:
	call    	abort@FUNCTION
	unreachable
.LBB0_14:                               # %if.then
	end_block                       # label1:
	call    	abort@FUNCTION
	unreachable
.LBB0_15:                               # %if.then21
	end_block                       # label0:
	call    	abort@FUNCTION
	unreachable
	.endfunc
.Lfunc_end0:
	.size	main, .Lfunc_end0-main

	.section	.text.constant_shift,"ax",@progbits
	.type	constant_shift,@function
constant_shift:                         # @constant_shift
	.param  	i64, i32
	.result 	i64
# BB#0:                                 # %entry
	block
	i32.const	$push0=, 63
	i32.gt_u	$push1=, $1, $pop0
	br_if   	0, $pop1        # 0: down to label11
# BB#1:                                 # %entry
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
	tableswitch	$1, 1, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 # 1: down to label74
                                        # 0: down to label75
                                        # 2: down to label73
                                        # 3: down to label72
                                        # 4: down to label71
                                        # 5: down to label70
                                        # 6: down to label69
                                        # 7: down to label68
                                        # 8: down to label67
                                        # 9: down to label66
                                        # 10: down to label65
                                        # 11: down to label64
                                        # 12: down to label63
                                        # 13: down to label62
                                        # 14: down to label61
                                        # 15: down to label60
                                        # 16: down to label59
                                        # 17: down to label58
                                        # 18: down to label57
                                        # 19: down to label56
                                        # 20: down to label55
                                        # 21: down to label54
                                        # 22: down to label53
                                        # 23: down to label52
                                        # 24: down to label51
                                        # 25: down to label50
                                        # 26: down to label49
                                        # 27: down to label48
                                        # 28: down to label47
                                        # 29: down to label46
                                        # 30: down to label45
                                        # 31: down to label44
                                        # 32: down to label43
                                        # 33: down to label42
                                        # 34: down to label41
                                        # 35: down to label40
                                        # 36: down to label39
                                        # 37: down to label38
                                        # 38: down to label37
                                        # 39: down to label36
                                        # 40: down to label35
                                        # 41: down to label34
                                        # 42: down to label33
                                        # 43: down to label32
                                        # 44: down to label31
                                        # 45: down to label30
                                        # 46: down to label29
                                        # 47: down to label28
                                        # 48: down to label27
                                        # 49: down to label26
                                        # 50: down to label25
                                        # 51: down to label24
                                        # 52: down to label23
                                        # 53: down to label22
                                        # 54: down to label21
                                        # 55: down to label20
                                        # 56: down to label19
                                        # 57: down to label18
                                        # 58: down to label17
                                        # 59: down to label16
                                        # 60: down to label15
                                        # 61: down to label14
                                        # 62: down to label13
                                        # 63: down to label12
.LBB1_2:                                # %sw.bb1
	end_block                       # label75:
	i64.const	$push64=, 1
	i64.shr_s	$0=, $0, $pop64
.LBB1_3:                                # %sw.epilog
	end_block                       # label74:
	return  	$0
.LBB1_4:                                # %sw.bb3
	end_block                       # label73:
	i64.const	$push63=, 2
	i64.shr_s	$0=, $0, $pop63
	return  	$0
.LBB1_5:                                # %sw.bb5
	end_block                       # label72:
	i64.const	$push62=, 3
	i64.shr_s	$0=, $0, $pop62
	return  	$0
.LBB1_6:                                # %sw.bb7
	end_block                       # label71:
	i64.const	$push61=, 4
	i64.shr_s	$0=, $0, $pop61
	return  	$0
.LBB1_7:                                # %sw.bb9
	end_block                       # label70:
	i64.const	$push60=, 5
	i64.shr_s	$0=, $0, $pop60
	return  	$0
.LBB1_8:                                # %sw.bb11
	end_block                       # label69:
	i64.const	$push59=, 6
	i64.shr_s	$0=, $0, $pop59
	return  	$0
.LBB1_9:                                # %sw.bb13
	end_block                       # label68:
	i64.const	$push58=, 7
	i64.shr_s	$0=, $0, $pop58
	return  	$0
.LBB1_10:                               # %sw.bb15
	end_block                       # label67:
	i64.const	$push57=, 8
	i64.shr_s	$0=, $0, $pop57
	return  	$0
.LBB1_11:                               # %sw.bb17
	end_block                       # label66:
	i64.const	$push56=, 9
	i64.shr_s	$0=, $0, $pop56
	return  	$0
.LBB1_12:                               # %sw.bb19
	end_block                       # label65:
	i64.const	$push55=, 10
	i64.shr_s	$0=, $0, $pop55
	return  	$0
.LBB1_13:                               # %sw.bb21
	end_block                       # label64:
	i64.const	$push54=, 11
	i64.shr_s	$0=, $0, $pop54
	return  	$0
.LBB1_14:                               # %sw.bb23
	end_block                       # label63:
	i64.const	$push53=, 12
	i64.shr_s	$0=, $0, $pop53
	return  	$0
.LBB1_15:                               # %sw.bb25
	end_block                       # label62:
	i64.const	$push52=, 13
	i64.shr_s	$0=, $0, $pop52
	return  	$0
.LBB1_16:                               # %sw.bb27
	end_block                       # label61:
	i64.const	$push51=, 14
	i64.shr_s	$0=, $0, $pop51
	return  	$0
.LBB1_17:                               # %sw.bb29
	end_block                       # label60:
	i64.const	$push50=, 15
	i64.shr_s	$0=, $0, $pop50
	return  	$0
.LBB1_18:                               # %sw.bb31
	end_block                       # label59:
	i64.const	$push49=, 16
	i64.shr_s	$0=, $0, $pop49
	return  	$0
.LBB1_19:                               # %sw.bb33
	end_block                       # label58:
	i64.const	$push48=, 17
	i64.shr_s	$0=, $0, $pop48
	return  	$0
.LBB1_20:                               # %sw.bb35
	end_block                       # label57:
	i64.const	$push47=, 18
	i64.shr_s	$0=, $0, $pop47
	return  	$0
.LBB1_21:                               # %sw.bb37
	end_block                       # label56:
	i64.const	$push46=, 19
	i64.shr_s	$0=, $0, $pop46
	return  	$0
.LBB1_22:                               # %sw.bb39
	end_block                       # label55:
	i64.const	$push45=, 20
	i64.shr_s	$0=, $0, $pop45
	return  	$0
.LBB1_23:                               # %sw.bb41
	end_block                       # label54:
	i64.const	$push44=, 21
	i64.shr_s	$0=, $0, $pop44
	return  	$0
.LBB1_24:                               # %sw.bb43
	end_block                       # label53:
	i64.const	$push43=, 22
	i64.shr_s	$0=, $0, $pop43
	return  	$0
.LBB1_25:                               # %sw.bb45
	end_block                       # label52:
	i64.const	$push42=, 23
	i64.shr_s	$0=, $0, $pop42
	return  	$0
.LBB1_26:                               # %sw.bb47
	end_block                       # label51:
	i64.const	$push41=, 24
	i64.shr_s	$0=, $0, $pop41
	return  	$0
.LBB1_27:                               # %sw.bb49
	end_block                       # label50:
	i64.const	$push40=, 25
	i64.shr_s	$0=, $0, $pop40
	return  	$0
.LBB1_28:                               # %sw.bb51
	end_block                       # label49:
	i64.const	$push39=, 26
	i64.shr_s	$0=, $0, $pop39
	return  	$0
.LBB1_29:                               # %sw.bb53
	end_block                       # label48:
	i64.const	$push38=, 27
	i64.shr_s	$0=, $0, $pop38
	return  	$0
.LBB1_30:                               # %sw.bb55
	end_block                       # label47:
	i64.const	$push37=, 28
	i64.shr_s	$0=, $0, $pop37
	return  	$0
.LBB1_31:                               # %sw.bb57
	end_block                       # label46:
	i64.const	$push36=, 29
	i64.shr_s	$0=, $0, $pop36
	return  	$0
.LBB1_32:                               # %sw.bb59
	end_block                       # label45:
	i64.const	$push35=, 30
	i64.shr_s	$0=, $0, $pop35
	return  	$0
.LBB1_33:                               # %sw.bb61
	end_block                       # label44:
	i64.const	$push34=, 31
	i64.shr_s	$0=, $0, $pop34
	return  	$0
.LBB1_34:                               # %sw.bb63
	end_block                       # label43:
	i64.const	$push33=, 32
	i64.shr_s	$0=, $0, $pop33
	return  	$0
.LBB1_35:                               # %sw.bb65
	end_block                       # label42:
	i64.const	$push32=, 33
	i64.shr_s	$0=, $0, $pop32
	return  	$0
.LBB1_36:                               # %sw.bb67
	end_block                       # label41:
	i64.const	$push31=, 34
	i64.shr_s	$0=, $0, $pop31
	return  	$0
.LBB1_37:                               # %sw.bb69
	end_block                       # label40:
	i64.const	$push30=, 35
	i64.shr_s	$0=, $0, $pop30
	return  	$0
.LBB1_38:                               # %sw.bb71
	end_block                       # label39:
	i64.const	$push29=, 36
	i64.shr_s	$0=, $0, $pop29
	return  	$0
.LBB1_39:                               # %sw.bb73
	end_block                       # label38:
	i64.const	$push28=, 37
	i64.shr_s	$0=, $0, $pop28
	return  	$0
.LBB1_40:                               # %sw.bb75
	end_block                       # label37:
	i64.const	$push27=, 38
	i64.shr_s	$0=, $0, $pop27
	return  	$0
.LBB1_41:                               # %sw.bb77
	end_block                       # label36:
	i64.const	$push26=, 39
	i64.shr_s	$0=, $0, $pop26
	return  	$0
.LBB1_42:                               # %sw.bb79
	end_block                       # label35:
	i64.const	$push25=, 40
	i64.shr_s	$0=, $0, $pop25
	return  	$0
.LBB1_43:                               # %sw.bb81
	end_block                       # label34:
	i64.const	$push24=, 41
	i64.shr_s	$0=, $0, $pop24
	return  	$0
.LBB1_44:                               # %sw.bb83
	end_block                       # label33:
	i64.const	$push23=, 42
	i64.shr_s	$0=, $0, $pop23
	return  	$0
.LBB1_45:                               # %sw.bb85
	end_block                       # label32:
	i64.const	$push22=, 43
	i64.shr_s	$0=, $0, $pop22
	return  	$0
.LBB1_46:                               # %sw.bb87
	end_block                       # label31:
	i64.const	$push21=, 44
	i64.shr_s	$0=, $0, $pop21
	return  	$0
.LBB1_47:                               # %sw.bb89
	end_block                       # label30:
	i64.const	$push20=, 45
	i64.shr_s	$0=, $0, $pop20
	return  	$0
.LBB1_48:                               # %sw.bb91
	end_block                       # label29:
	i64.const	$push19=, 46
	i64.shr_s	$0=, $0, $pop19
	return  	$0
.LBB1_49:                               # %sw.bb93
	end_block                       # label28:
	i64.const	$push18=, 47
	i64.shr_s	$0=, $0, $pop18
	return  	$0
.LBB1_50:                               # %sw.bb95
	end_block                       # label27:
	i64.const	$push17=, 48
	i64.shr_s	$0=, $0, $pop17
	return  	$0
.LBB1_51:                               # %sw.bb97
	end_block                       # label26:
	i64.const	$push16=, 49
	i64.shr_s	$0=, $0, $pop16
	return  	$0
.LBB1_52:                               # %sw.bb99
	end_block                       # label25:
	i64.const	$push15=, 50
	i64.shr_s	$0=, $0, $pop15
	return  	$0
.LBB1_53:                               # %sw.bb101
	end_block                       # label24:
	i64.const	$push14=, 51
	i64.shr_s	$0=, $0, $pop14
	return  	$0
.LBB1_54:                               # %sw.bb103
	end_block                       # label23:
	i64.const	$push13=, 52
	i64.shr_s	$0=, $0, $pop13
	return  	$0
.LBB1_55:                               # %sw.bb105
	end_block                       # label22:
	i64.const	$push12=, 53
	i64.shr_s	$0=, $0, $pop12
	return  	$0
.LBB1_56:                               # %sw.bb107
	end_block                       # label21:
	i64.const	$push11=, 54
	i64.shr_s	$0=, $0, $pop11
	return  	$0
.LBB1_57:                               # %sw.bb109
	end_block                       # label20:
	i64.const	$push10=, 55
	i64.shr_s	$0=, $0, $pop10
	return  	$0
.LBB1_58:                               # %sw.bb111
	end_block                       # label19:
	i64.const	$push9=, 56
	i64.shr_s	$0=, $0, $pop9
	return  	$0
.LBB1_59:                               # %sw.bb113
	end_block                       # label18:
	i64.const	$push8=, 57
	i64.shr_s	$0=, $0, $pop8
	return  	$0
.LBB1_60:                               # %sw.bb115
	end_block                       # label17:
	i64.const	$push7=, 58
	i64.shr_s	$0=, $0, $pop7
	return  	$0
.LBB1_61:                               # %sw.bb117
	end_block                       # label16:
	i64.const	$push6=, 59
	i64.shr_s	$0=, $0, $pop6
	return  	$0
.LBB1_62:                               # %sw.bb119
	end_block                       # label15:
	i64.const	$push5=, 60
	i64.shr_s	$0=, $0, $pop5
	return  	$0
.LBB1_63:                               # %sw.bb121
	end_block                       # label14:
	i64.const	$push4=, 61
	i64.shr_s	$0=, $0, $pop4
	return  	$0
.LBB1_64:                               # %sw.bb123
	end_block                       # label13:
	i64.const	$push3=, 62
	i64.shr_s	$0=, $0, $pop3
	return  	$0
.LBB1_65:                               # %sw.bb125
	end_block                       # label12:
	i64.const	$push2=, 63
	i64.shr_s	$0=, $0, $pop2
	return  	$0
.LBB1_66:                               # %sw.default
	end_block                       # label11:
	call    	abort@FUNCTION
	unreachable
	.endfunc
.Lfunc_end1:
	.size	constant_shift, .Lfunc_end1-constant_shift

	.type	zext,@object            # @zext
	.section	.rodata.zext,"a",@progbits
	.p2align	4
zext:
	.int64	8526495107234113920     # 0x7654321fedcba980
	.int64	4263247553617056960     # 0x3b2a190ff6e5d4c0
	.int64	2131623776808528480     # 0x1d950c87fb72ea60
	.int64	1065811888404264240     # 0xeca8643fdb97530
	.int64	532905944202132120      # 0x7654321fedcba98
	.int64	266452972101066060      # 0x3b2a190ff6e5d4c
	.int64	133226486050533030      # 0x1d950c87fb72ea6
	.int64	66613243025266515       # 0xeca8643fdb9753
	.int64	33306621512633257       # 0x7654321fedcba9
	.int64	16653310756316628       # 0x3b2a190ff6e5d4
	.int64	8326655378158314        # 0x1d950c87fb72ea
	.int64	4163327689079157        # 0xeca8643fdb975
	.int64	2081663844539578        # 0x7654321fedcba
	.int64	1040831922269789        # 0x3b2a190ff6e5d
	.int64	520415961134894         # 0x1d950c87fb72e
	.int64	260207980567447         # 0xeca8643fdb97
	.int64	130103990283723         # 0x7654321fedcb
	.int64	65051995141861          # 0x3b2a190ff6e5
	.int64	32525997570930          # 0x1d950c87fb72
	.int64	16262998785465          # 0xeca8643fdb9
	.int64	8131499392732           # 0x7654321fedc
	.int64	4065749696366           # 0x3b2a190ff6e
	.int64	2032874848183           # 0x1d950c87fb7
	.int64	1016437424091           # 0xeca8643fdb
	.int64	508218712045            # 0x7654321fed
	.int64	254109356022            # 0x3b2a190ff6
	.int64	127054678011            # 0x1d950c87fb
	.int64	63527339005             # 0xeca8643fd
	.int64	31763669502             # 0x7654321fe
	.int64	15881834751             # 0x3b2a190ff
	.int64	7940917375              # 0x1d950c87f
	.int64	3970458687              # 0xeca8643f
	.int64	1985229343              # 0x7654321f
	.int64	992614671               # 0x3b2a190f
	.int64	496307335               # 0x1d950c87
	.int64	248153667               # 0xeca8643
	.int64	124076833               # 0x7654321
	.int64	62038416                # 0x3b2a190
	.int64	31019208                # 0x1d950c8
	.int64	15509604                # 0xeca864
	.int64	7754802                 # 0x765432
	.int64	3877401                 # 0x3b2a19
	.int64	1938700                 # 0x1d950c
	.int64	969350                  # 0xeca86
	.int64	484675                  # 0x76543
	.int64	242337                  # 0x3b2a1
	.int64	121168                  # 0x1d950
	.int64	60584                   # 0xeca8
	.int64	30292                   # 0x7654
	.int64	15146                   # 0x3b2a
	.int64	7573                    # 0x1d95
	.int64	3786                    # 0xeca
	.int64	1893                    # 0x765
	.int64	946                     # 0x3b2
	.int64	473                     # 0x1d9
	.int64	236                     # 0xec
	.int64	118                     # 0x76
	.int64	59                      # 0x3b
	.int64	29                      # 0x1d
	.int64	14                      # 0xe
	.int64	7                       # 0x7
	.int64	3                       # 0x3
	.int64	1                       # 0x1
	.int64	0                       # 0x0
	.size	zext, 512

	.type	sext,@object            # @sext
	.section	.rodata.sext,"a",@progbits
	.p2align	4
sext:
	.int64	-8152436031399644656    # 0x8edcba9f76543210
	.int64	-4076218015699822328    # 0xc76e5d4fbb2a1908
	.int64	-2038109007849911164    # 0xe3b72ea7dd950c84
	.int64	-1019054503924955582    # 0xf1db9753eeca8642
	.int64	-509527251962477791     # 0xf8edcba9f7654321
	.int64	-254763625981238896     # 0xfc76e5d4fbb2a190
	.int64	-127381812990619448     # 0xfe3b72ea7dd950c8
	.int64	-63690906495309724      # 0xff1db9753eeca864
	.int64	-31845453247654862      # 0xff8edcba9f765432
	.int64	-15922726623827431      # 0xffc76e5d4fbb2a19
	.int64	-7961363311913716       # 0xffe3b72ea7dd950c
	.int64	-3980681655956858       # 0xfff1db9753eeca86
	.int64	-1990340827978429       # 0xfff8edcba9f76543
	.int64	-995170413989215        # 0xfffc76e5d4fbb2a1
	.int64	-497585206994608        # 0xfffe3b72ea7dd950
	.int64	-248792603497304        # 0xffff1db9753eeca8
	.int64	-124396301748652        # 0xffff8edcba9f7654
	.int64	-62198150874326         # 0xffffc76e5d4fbb2a
	.int64	-31099075437163         # 0xffffe3b72ea7dd95
	.int64	-15549537718582         # 0xfffff1db9753eeca
	.int64	-7774768859291          # 0xfffff8edcba9f765
	.int64	-3887384429646          # 0xfffffc76e5d4fbb2
	.int64	-1943692214823          # 0xfffffe3b72ea7dd9
	.int64	-971846107412           # 0xffffff1db9753eec
	.int64	-485923053706           # 0xffffff8edcba9f76
	.int64	-242961526853           # 0xffffffc76e5d4fbb
	.int64	-121480763427           # 0xffffffe3b72ea7dd
	.int64	-60740381714            # 0xfffffff1db9753ee
	.int64	-30370190857            # 0xfffffff8edcba9f7
	.int64	-15185095429            # 0xfffffffc76e5d4fb
	.int64	-7592547715             # 0xfffffffe3b72ea7d
	.int64	-3796273858             # 0xffffffff1db9753e
	.int64	-1898136929             # 0xffffffff8edcba9f
	.int64	-949068465              # 0xffffffffc76e5d4f
	.int64	-474534233              # 0xffffffffe3b72ea7
	.int64	-237267117              # 0xfffffffff1db9753
	.int64	-118633559              # 0xfffffffff8edcba9
	.int64	-59316780               # 0xfffffffffc76e5d4
	.int64	-29658390               # 0xfffffffffe3b72ea
	.int64	-14829195               # 0xffffffffff1db975
	.int64	-7414598                # 0xffffffffff8edcba
	.int64	-3707299                # 0xffffffffffc76e5d
	.int64	-1853650                # 0xffffffffffe3b72e
	.int64	-926825                 # 0xfffffffffff1db97
	.int64	-463413                 # 0xfffffffffff8edcb
	.int64	-231707                 # 0xfffffffffffc76e5
	.int64	-115854                 # 0xfffffffffffe3b72
	.int64	-57927                  # 0xffffffffffff1db9
	.int64	-28964                  # 0xffffffffffff8edc
	.int64	-14482                  # 0xffffffffffffc76e
	.int64	-7241                   # 0xffffffffffffe3b7
	.int64	-3621                   # 0xfffffffffffff1db
	.int64	-1811                   # 0xfffffffffffff8ed
	.int64	-906                    # 0xfffffffffffffc76
	.int64	-453                    # 0xfffffffffffffe3b
	.int64	-227                    # 0xffffffffffffff1d
	.int64	-114                    # 0xffffffffffffff8e
	.int64	-57                     # 0xffffffffffffffc7
	.int64	-29                     # 0xffffffffffffffe3
	.int64	-15                     # 0xfffffffffffffff1
	.int64	-8                      # 0xfffffffffffffff8
	.int64	-4                      # 0xfffffffffffffffc
	.int64	-2                      # 0xfffffffffffffffe
	.int64	-1                      # 0xffffffffffffffff
	.size	sext, 512


	.ident	"clang version 3.9.0 "
