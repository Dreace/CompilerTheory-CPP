编译原理基本算法的 C++ 实现，另有 TypeScript 实现版本可以看这里 https://github.com/Dreace/CompilerTheory-TypeScript。

## 已完成

- [x] 词法分析
- [x] LL1 文法分析

## 运行

### 词法分析

输入文件 `input.txt`：

```c++
int ans = 0;
for(int i = 1; i < 100; ++i) {
    ans += i;
}
printf("ans = %d", ans);
```

编译 `lexical_analysis` 目录下文件后，执行 `./Lexical < input.txt` 得到输出：

```
int => KEY_WORD
ans => IDENTIFIER
= => OPERATOR
0 => NUMBER
; => DELIMITER
for => KEY_WORD
( => DELIMITER
int => KEY_WORD
i => IDENTIFIER
= => OPERATOR
1 => NUMBER
; => DELIMITER
i => IDENTIFIER
< => OPERATOR
100 => NUMBER
; => DELIMITER
++ => OPERATOR
i => IDENTIFIER
) => DELIMITER
{ => DELIMITER
ans => IDENTIFIER
+= => OPERATOR
i => IDENTIFIER
; => DELIMITER
} => DELIMITER
printf => IDENTIFIER
( => DELIMITER
ans = %d => STRING
, => DELIMITER
ans => IDENTIFIER
) => DELIMITER
; => DELIMITER
```

### LL(1) 文法分析

输入文件 `input.txt`：

```
S->TA
A->+TA|$
T->FB
B->*FB|$
F->(S)|i~
(i+i)*(i+i)#~
```

文法部分和输入串部分必须以 `~` 结尾并且 `~` 前不能有换行或空格，文法开始符号只能是 `S`，输入串结尾必须有 `#`。

编译 `ll1` 目录下文件后，执行 `./LL1 < input.txt` 得到输出：

```
$:$ 
(:( 
):
*:* 
+:+ 
A:$ + 
B:$ * 
F:( i 
S:( i 
T:( i 
i:i 
----------
$:$ 
(S):( 
*FB:* 
+TA:+ 
FB:( i 
TA:( i 
i:i 
----------
A:# ) 
B:# ) + 
F:# ) * + 
S:# ) 
T:# ) + 
----------
A: (#,$) (),$) (+,+TA) 
B: (#,$) (),$) (*,*FB) (+,$) 
F: ((,(S)) (i,i) 
S: ((,TA) (i,TA) 
T: ((,FB) (i,FB) 
----------
0	#S      (i+i)*(i+i)#	S->TA
1	#AT     (i+i)*(i+i)#	T->FB
2	#ABF    (i+i)*(i+i)#	F->(S)
3	#AB)S(  (i+i)*(i+i)#	( 匹配
4	#AB)S    i+i)*(i+i)#	S->TA
5	#AB)AT   i+i)*(i+i)#	T->FB
6	#AB)ABF  i+i)*(i+i)#	F->i
7	#AB)ABi  i+i)*(i+i)#	i 匹配
8	#AB)AB    +i)*(i+i)#	B->$
9	#AB)A     +i)*(i+i)#	A->+TA
10	#AB)AT+   +i)*(i+i)#	+ 匹配
11	#AB)AT     i)*(i+i)#	T->FB
12	#AB)ABF    i)*(i+i)#	F->i
13	#AB)ABi    i)*(i+i)#	i 匹配
14	#AB)AB      )*(i+i)#	B->$
15	#AB)A       )*(i+i)#	A->$
16	#AB)        )*(i+i)#	) 匹配
17	#AB          *(i+i)#	B->*FB
18	#ABF*        *(i+i)#	* 匹配
19	#ABF          (i+i)#	F->(S)
20	#AB)S(        (i+i)#	( 匹配
21	#AB)S          i+i)#	S->TA
22	#AB)AT         i+i)#	T->FB
23	#AB)ABF        i+i)#	F->i
24	#AB)ABi        i+i)#	i 匹配
25	#AB)AB          +i)#	B->$
26	#AB)A           +i)#	A->+TA
27	#AB)AT+         +i)#	+ 匹配
28	#AB)AT           i)#	T->FB
29	#AB)ABF          i)#	F->i
30	#AB)ABi          i)#	i 匹配
31	#AB)AB            )#	B->$
32	#AB)A             )#	A->$
33	#AB)              )#	) 匹配
34	#AB                #	B->$
35	#A                 #	A->$
36	#                  #	分析成功
```

依次输出符号的 FIRST 集合、符号串的 FIRST 集合、FOLLOW 集合、预测分析表、解析过程（包括分析栈、剩余输入串、动作）。