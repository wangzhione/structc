# Bash 烂笔头

## 0. 起手式

```Bash
#!/bin/bash

# nounset : 使用未声明变量时, 使脚本退出
set -o nounset
# errexit : 当命令运行出错时, 使脚本退出
set -o errexit
```

    也可以用简写

    set -o nounset 快捷写法 set -u 
    set -o errexit 快捷写法 set -e, 更简单点 set -ue. 

    更多 set 操作, 可以继续查资料手册

[The Set Builtin](https://www.gnu.org/software/bash/manual/html_node/The-Set-Builtin.html)

## 1. 变量

### 1.1 变量命名

* 命名只能使用英文字母[A-Za-z], 数字[0-9]和下划线[_], 且不能以数字开头
* 不能使用 bash 里的关键字 (help 命令可查看保留关键字)
* [可选] 环境变量大写, 普通变量小写

### 1.2 变量声明

    [key]=[value]

    变量声明 = 两侧要紧贴, 不能有空白相关字符. 

    原因是: 当有空白时候, Shell 解释器会把空白分隔部分当做单独命令去解析 ~

#### 1.2.1 环境变量声明

    环境变量是对当前 shell 会话内所有的程序或脚本都可见的变量

```bash
# nginx config
export NGINX_HOME=/usr/local/nginx
```

#### 1.2.2 普通变量声明

    当前的程序或脚本都可见的变量

```bash
hoge="你真了不得"
```

#### 1.2.3 只读变量声明

```bash
readonly threshold=0.0000005
```

    或者这么声明

```bash
readonly hoge
```

    防止变量被篡改

#### 1.2.4 局部变量声明

    名词可能有多个, 也可以叫局部函数变量, 仅仅在函数内部有效.

    可以有效防止变量污染 ~

```bash
#!/bin/bash

# errexit : 当命令运行出错时, 使脚本退出
# nounset : 使用未声明变量时, 使脚本退出
set -o errexit
set -o nounset

hello () {
    local piyo="猴哥猴哥"
    echo ${piyo}
}

hello

echo ${piyo}
```

    输出

```bash
$ bash 1.2.4.sh 
> 猴哥猴哥
> 1.2.4.sh: 行 15: piyo：未绑定的变量
```

    业务复杂的时候, 尝试用结构思维, 拆分为多个流程(函数)!
    
    如果业务超级复杂, 请学会松手 ~ 

### 1.3 使用变量

    记住 ${variable} !
    记住 ${variable} !
    记住 ${variable} !

    对于 array string ... 类型变量声明和使用, 可以自行温习

[数组](https://wangdoc.com/bash/array.html)
[字符串操作](https://wangdoc.com/bash/string.html)

### 1.4 删除变量

```bash
unset hoge
```

## 2. 条件分支 if [[ ]]

    用 if [[ ]] 替代 if [ ] !
    用 if [[ ]] 替代 if [ ] !
    用 if [[ ]] 替代 if [ ] !

    Shell 相关判断语法很多, 例如

```bash
# 写法一 : expression 是一个表达式 
#        : 这个表达式为真, test 命令执行成功返回 0 ※
#        : 这个表达式为伪, test 命令执行失败返回 1 ※
test expression

# 写法二 :
#        : [ ] 是 test 简写
#        : [ ] 与 expression 表达式之间必须有空格
[ expression ]

# 写法三 :
#        : [[ ]] 是 [ ] 升级版本, 并且支持正则表达式
[[ expression ]]

# 写法四 : 
#        : 算术判断, 如果算术计算结果是非零值, 则判断条件为真
#        : test 0 返回真, (( 0 )) 返回假
#        : test 1 返回真, (( 1 )) 返回真, 注意二者区别
#        : 可以用 > < = 替代 -gt -lt -eq ...
(( expression ))
```
    
    对于这千千万语法, 只需要记住和使用 [[  ]] 就好! 

    后面几个小节, 罗列常用的并且好写的代码, 用于各种场景判断操作 ~ 

### 2.1 integer 判断

    [[ integer1 -eq integer2 ]]: 如果 integer1 等于 integer2, 则为真
    [[ integer1 -ne integer2 ]]: 如果 integer1 不等于 integer2, 则为真
    [[ integer1 -le integer2 ]]: 如果 integer1 小于或等于 integer2, 则为真
    [[ integer1 -lt integer2 ]]: 如果 integer1 小于 integer2, 则为真
    [[ integer1 -ge integer2 ]]: 如果 integer1 大于或等于 integer2, 则为真
    [[ integer1 -gt integer2 ]]: 如果 integer1 大于 integer2, 则为真

### 2.2 number 判断

    方法很多, 这里引述一种, 借助计算器 bc 指令

    [[ $(echo "${number1} == ${number2}" | bc) -eq 1 ]] 等价于 number1 == number2
    [[ $(echo "${number1} != ${number2}" | bc) -eq 1 ]] 等价于 number1 != number2
    [[ $(echo "${number1} <= ${number2}" | bc) -eq 1 ]] 等价于 number1 <= number2
    [[ $(echo "${number1} <  ${number2}" | bc) -eq 1 ]] 等价于 number1 <  number2
    [[ $(echo "${number1} >= ${number2}" | bc) -eq 1 ]] 等价于 number1 >= number2
    [[ $(echo "${number1} >  ${number2}" | bc) -eq 1 ]] 等价于 number1 >  number2

### 2.3 string 判断

    [[ string ]]   : 如果 string 不为空(长度大于 0), 则为真
    [[ -n string ]]: 如果字符串 string 的长度大于零, 则为真
    [[ -z string ]]: 如果字符串 string 的长度为零, 则为真
    
    [[ string1 == string2 ]]: 如果 string1 和 string2 相同, 则为真
    [[ string1 != string2 ]]: 如果 string1 和 string2 不相同, 则为真
    [[ string1 >  string2 ]]: 如果按照字典顺序 string1 排列在 string2 之后, 则为真
    [[ string1 <  string2 ]]: 如果按照字典顺序 string1 排列在 string2 之前, 则为真

    注意, 字符串判断情况下, 变量放在双引号之中! 
    如 [[ -n "${variable}" ]] 可以避免字符串为空的时语法错误.
    如 [[ "me too" == "me too" ]] 避免出现 [[ me too == me too ]] 语法解释错误

### 2.4 regex 判断

    [[ string =~ regex ]]: 如果 string 匹配 regex 正则表达式, 则为真

```bash
#
# 举个是否为邮箱的判断案例
#

mail="ccp@gmail.com"

if [[ "${mail}" =~ ^([A-Za-z0-9_])+\@([A-Za-z0-9_])+\.([A-Za-z]{2,4})$ ]]
then
    echo "${mail} true"
else
    echo "${mail} false"
fi
```

    [[ "${variable}" =~ regex ]] 判断比较特殊, 这里 regex 正则表达式不加 "" !

### 2.5 file 判断

    [[ -a file ]]: 如果 file 存在, 则为真
    [[ -b file ]]: 如果 file 存在并且是一个块(设备)文件, 则为真
    [[ -c file ]]: 如果 file 存在并且是一个字符(设备)文件, 则为真
    [[ -d file ]]: 如果 file 存在并且是一个目录, 则为真
    [[ -e file ]]: 如果 file 存在, 则为真
    [[ -f file ]]: 如果 file 存在并且是一个普通文件, 则为真
    [[ -g file ]]: 如果 file 存在并且设置了组 ID, 则为真
    [[ -G file ]]: 如果 file 存在并且属于有效的组 ID, 则为真
    [[ -h file ]]: 如果 file 存在并且是符号链接, 则为真
    [[ -k file ]]: 如果 file 存在并且设置了它的粘滞位(sticky bit), 则为真
    [[ -L file ]]: 如果 file 存在并且是一个符号链接, 则为真
    [[ -N file ]]: 如果 file 存在并且自上次读取后已被修改, 则为真
    [[ -O file ]]: 如果 file 存在并且属于有效的用户 ID, 则为真
    [[ -p file ]]: 如果 file 存在并且是一个命名管道, 则为真
    [[ -r file ]]: 如果 file 存在并且可读(当前用户有可读权限), 则为真
    [[ -s file ]]: 如果 file 存在且其长度大于零, 则为真
    [[ -S file ]]: 如果 file 存在且是一个网络 socket, 则为真
    [[ -u file ]]: 如果 file 存在并且设置了 setuid 位, 则为真
    [[ -w file ]]: 如果 file 存在并且可写(当前用户拥有可写权限), 则为真
    [[ -x file ]]: 如果 file 存在并且可执行(有效用户有执行/搜索权限), 则为真

    [[ file1 -nt file2 ]]: 如果 file1 比 file2 的更新时间最近, 
                           或者 file1 存在而 file2 不存在, 则为真
    [[ file1 -ot file2 ]]: 如果 file1 比 file2 的更新时间更旧, 
                           或者 file2 存在而 file1 不存在, 则为真
    [[ file1 -ef file2 ]]: 如果 file1 和 file2 引用相同的设备和 inode 编号, 则为真

    其中 [[ ]] 技巧也可以拓展到 switch case 中, 需要用到的时候, 再查相关表也容易.

## 3. 调试

    抛开调试基本功, 打点.  

    我们也介绍几种 Bash Shell 本身提供协助调试的相关功能

```bash
#  ### 常用 Shell 脚本调试选项  
#
#  -n (noexec 或 no ecxecution) 
#     语法检查模式, 读取脚本并检查语法错误, 但不执行
#
#  -x (xtrace 或 execution trace) 
#     跟踪模式, 识别语法错误和逻辑错误, 显示所有执行的命令, 参数和结果
#
#  -v (verbose) 
#     详细模式, 将所有执行过的脚本命令打印到标准输出
#
#
#  ### 执行调试的方法
#
#  1. 在命令行提供参数, 调试整个脚本. 例如 bash -x script.sh
#  2. 脚本开头提供参数, 调试整个脚本. 例如 #!/bin/bash -x
#  3. 在脚本中用 set 命令对特定部分进行调试. 例如 set -x 启用调试和 set +x 禁用调试
#
#  
#  ### set 命令  
#
#  - 使用内置命令set可以调试Shell脚本的指定部分 
#  - 启用调试："set -<选项>"
#  - 禁用调试："set +<选项>"
#  
#  -n    noexec    检查语法  
#  -x    xtrace    调试模式  
#  -v    verbose   verbose 模式  
#  -e    errexit   如果命令运行失败, 脚本退出执行 
#  -u    nounset   如果存在未声明的变量, 脚本退出执行 
```

    总结, -n 检查语法. -x 进入调试模式, 显示 命令执行 和 输出结果 对照 ~

## 4. date 日期命令

    date 命令常用作操作时间戳和时间串场景.

```bash
# 得到当前系统环境的时间串
$ date
> 2020年 06月 26日 星期五 21:51:28 CST

# 得到当前系统环境时间戳
# 从 1970 年 1 月 1 日 00:00:00 UTC 到目前为止的秒数
$ date +%s
> 1593179500

# 时间戳转时间串
$ date +"%Y-%m-%d %H:%M:%S" -d@1593179500
> 2020-06-26 21:51:40

# 时间串转时间戳
$ date +%s -d"2020-06-26 21:51:40"
> 1593179500
```

    欢迎找规律, 更多详细介绍请翻阅 man date
