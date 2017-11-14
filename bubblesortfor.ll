; ModuleID = 'program'

@arr = common global [20 x i64] zeroinitializer
@temp = common global i64 0
@i = common global i64 0
@j = common global i64 0
@string = internal constant [6 x i8] c"%d\22 \22\00"
@string.2 = internal constant [4 x i8] c"\22\22\0A\00"

define void @main() {
entry:
  %0 = call i64 @main.1()
  ret void
}

define internal i64 @main.1() {
entry:
  store i64 0, i64* @i
  br label %loop_header

loop_header:                                      ; preds = %loop_body, %entry
  %tmp = load i64, i64* @i
  %tmp1 = icmp slt i64 %tmp, 19
  br i1 %tmp1, label %loop_body, label %after_loop

loop_body:                                        ; preds = %loop_header
  %tmp2 = load i64, i64* @i
  %tmp3 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp2
  %tmp4 = load i64, i64* @i
  %tmp5 = sub i64 19, %tmp4
  store i64 %tmp5, i64* %tmp3
  %tmp6 = load i64, i64* @i
  %tmp7 = add i64 1, %tmp6
  store i64 %tmp7, i64* @i
  br label %loop_header

after_loop:                                       ; preds = %loop_header
  store i64 0, i64* @i
  br label %loop_header8

loop_header8:                                     ; preds = %after_loop15, %after_loop
  %tmp11 = load i64, i64* @i
  %tmp12 = icmp slt i64 %tmp11, 19
  br i1 %tmp12, label %loop_body9, label %after_loop10

loop_body9:                                       ; preds = %loop_header8
  %tmp16 = load i64, i64* @i
  %tmp17 = add i64 %tmp16, 1
  store i64 %tmp17, i64* @j
  br label %loop_header13

after_loop10:                                     ; preds = %loop_header8
  store i64 0, i64* @i
  br label %loop_header43

loop_header13:                                    ; preds = %mergeBlock, %loop_body9
  %tmp18 = load i64, i64* @j
  %tmp19 = icmp slt i64 %tmp18, 20
  br i1 %tmp19, label %loop_body14, label %after_loop15

loop_body14:                                      ; preds = %loop_header13
  %tmp20 = load i64, i64* @j
  %tmp21 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp20
  %tmp22 = load i64, i64* %tmp21
  %tmp23 = load i64, i64* @i
  %tmp24 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp23
  %tmp25 = load i64, i64* %tmp24
  %tmp26 = icmp sgt i64 %tmp25, %tmp22
  %zext = zext i1 %tmp26 to i64
  %tmp27 = icmp ne i64 %zext, 0
  br i1 %tmp27, label %ifBlock, label %mergeBlock

after_loop15:                                     ; preds = %loop_header13
  %tmp41 = load i64, i64* @i
  %tmp42 = add i64 1, %tmp41
  store i64 %tmp42, i64* @i
  br label %loop_header8

ifBlock:                                          ; preds = %loop_body14
  %tmp28 = load i64, i64* @i
  %tmp29 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp28
  %tmp30 = load i64, i64* %tmp29
  store i64 %tmp30, i64* @temp
  %tmp31 = load i64, i64* @i
  %tmp32 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp31
  %tmp33 = load i64, i64* @j
  %tmp34 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp33
  %tmp35 = load i64, i64* %tmp34
  store i64 %tmp35, i64* %tmp32
  %tmp36 = load i64, i64* @j
  %tmp37 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp36
  %tmp38 = load i64, i64* @temp
  store i64 %tmp38, i64* %tmp37
  br label %mergeBlock

mergeBlock:                                       ; preds = %loop_body14, %ifBlock
  %tmp39 = load i64, i64* @j
  %tmp40 = add i64 1, %tmp39
  store i64 %tmp40, i64* @j
  br label %loop_header13

loop_header43:                                    ; preds = %loop_body44, %after_loop10
  %tmp46 = load i64, i64* @i
  %tmp47 = icmp slt i64 %tmp46, 20
  br i1 %tmp47, label %loop_body44, label %after_loop45

loop_body44:                                      ; preds = %loop_header43
  %tmp48 = load i64, i64* @i
  %tmp49 = getelementptr inbounds [20 x i64], [20 x i64]* @arr, i64 0, i64 %tmp48
  %tmp50 = load i64, i64* %tmp49
  %printf = call i64 (...) @printf([6 x i8]* @string, i64 %tmp50)
  %tmp51 = load i64, i64* @i
  %tmp52 = add i64 1, %tmp51
  store i64 %tmp52, i64* @i
  br label %loop_header43

after_loop45:                                     ; preds = %loop_header43
  %printf53 = call i64 (...) @printf([4 x i8]* @string.2)
  ret i64 0
}

declare i64 @printf(...)
