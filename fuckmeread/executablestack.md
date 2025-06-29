## 发现在linux环境下cmake会跳一个警告


`CMakeFiles/boot.dir/start.S.o: missing .note.GNU-stack section implies executable stack `

这个警告的意思是 */boot/start.s* 文件没有指定栈权限,或许将默认的为栈加入可执行的权限,但这样做可能会使程序有安全性漏洞.消这个警告的方法就是指明这个.s文件的权限

`.section .note.GNU-stack,"",%progbits`
**这条指令显式的指出不需要可执行权限**


`.section .note.GNU-stack,"x",%progbits`
**这条指令显式的指出需要可执行权限**

### 我实际测试发现不需要可执行权限 所以选择第一种