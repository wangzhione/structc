# Git 烂笔头

## git branch

```Bash
# 清理本地冗余分支
git branch | grep -v -E 'master$|preview$|staging$' | xargs git branch -D

# 清理远端冗余分支
# git branch -r | grep -v -E 'master$|preview$|staging$' | sed 's/origin\///g' | xargs -I {} echo 'git push origin :{}' | more
git branch -r | grep -v -E 'master$|preview$|staging$' | sed 's/origin\///g' | xargs -I {} git push origin :{}

# 本地分支类比远端分支重新整理, 删除在本地有但在远程库中已经不存在的分支
git remote prune origin
```

## git tag

Git 可以给仓库历史中的某一个提交打上标签, **以示重要**. 比较有代表性的是人们会使用这个功能来**标记发布结点**(v1.0.0, v2.0.0 等等)

**列出标签**

```Bash
# 列出所有
git tag

# 模糊匹配 -list
git tag -l "v2.0.2"
```

**创建标签**

```Bash
git tag -a v2.0.2 -m "乌拉"
```

其中 git show 可以看到标签信息对应的提交信息

```Bash
git show v2.0.2
```

**共享标签**

共享标签相当于把本地标签推送到远端

```Bash
# 推送指定版本标签
git push origin v2.0.2

# 全部推送
git push origin --tags
```

**删除标签**

```Bash
# 本地删除
git tag -d vZ.卍

# 远端删除
git push origin --delete vZ.卍
```

## git reset

**强制覆盖到本地**

```Bash
# 1. 将远程仓库最新代码下载到本地，但是不和本地仓库代码合并
# git fetch --all
# 2. 强制清空本地仓库 master 分支下面的暂存区的代码
# git reset --hard origin/master
# 3. 更新合并远程本地代码
# git pull

git fetch --all
git reset --hard origin/master
git pull
```

**强制推送到远端**

**做好 backup**, 当然也可以选择下一个删库跑路的*传说哥*

```Bash
# 回退到上个版本
git reset --hard HEAD^

# 回退到前 2 次提交之前, 以此类推到第 n 次
git reset --hard HEAD~2

# 回退到指定 commid id
git reset --hard {commit id}

# 强制推送到远端 !!!
git push origin HEAD --force
```

## git config

**查看**

```Bash

# git config 配置操作指令
git config

# 查看系统 git config
git config --system --list

# 查看当前用户 global 全局配置
git config --global  --list

# 查看当前仓库配置信息
git config --local  --list
```

**常见操作**

```Bash
git config --global user.name "[user name]"
git config --global user.email  "[user email]"
```

行尾 autocrlf 格式化

```Bash
git config --global core.autocrlf input

# core.autocrlf true  : Git 在你提交时自动地把行结束符 CRLF 转换成 LF, 而在签出代码时把 LF 转换成 CRLF
# core.autocrlf input : Git 在你提交时把 CRLF 转换成 LF, 签出时不转换
# core.autocrlf false : false 关闭这项功能, 提交和签出都不额外操作.
```

## git connect github

**生成 ssh 公钥和本地私钥**

```Bash
ssh-keygen -t rsa -C "[user email]"

# ssh-keygen 支持 -t 选项指定加密算法, -C 选项指定注释, -f 选项指定密钥文件路径
```

找到生成 *.pub 公钥, 放入 github 中. 
然后 { copy pub -> [Account settings] -> [SSH Keys] -> [Ctrl + V] } 
随后 ssh -T git@github.com 测试, 想办法看到 **You've successfully**

```Bash
ssh -T git@github.com

# Hi {user name}! You've successfully authenticated, but GitHub does not provide shell access.
```

## 参考手册

🙏 [Pro Git](https://git-scm.com/book/zh/v2)

