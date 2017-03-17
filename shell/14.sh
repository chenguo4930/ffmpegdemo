#!/bin/bash

##标准输入输出
#0 STDIN 
#1 STDOUT 标准输出
#2 STDERR 标准错误

#永久重定向  1指向test7，7指向test9，&7就代表test9
exec 1>test7
exec 2>test8

#自定义输出
exec 7>test9

echo "cc zz" >&7
echo "cc yy"

ls -a ./ff



