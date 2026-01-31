# 常用git命令

## 📌 写在前面
如果此文档仍有遗漏，可使用 git help <command> 获取更详细的帮助信息。

# 基础操作
```bash
git status                     # 查看状态
git log --oneline              # 简洁查看提交历史
git diff                       # 查看未暂存的修改
git diff --staged              # 查看已暂存的修改
git add <file>                 # 添加文件到暂存区
```

## 暂存文件&储藏文件
```bash
git add .                     # 添加所有文件
git rm <file>                 # 删除文件并暂存
git mv <old> <new>            # 重命名文件并暂存
git stash                     # 暂存当前修改
git stash list                # 查看储藏列表
git stash pop                 # 恢复并删除最近储藏
git stash apply               # 恢复但不删除
git stash drop                # 删除最近储藏
```

## 分支管理
```bash
git branch                        # 查看本地分支
git branch <name>                 # 创建新分支
git checkout <branch>             # 切换分支
git checkout -b <branch>          # 创建并切换分支
git merge <branch>                # 合并分支
git branch -d <branch>            # 删除分支
git branch -D <branch>            # 强制删除分支
git push origin --delete <branch> # 删除远程分支
```

## git记录管理
```bash
git checkout -- <file>                # 撤销对文件的修改
git checkout <commit-hash> -- <file>  # 恢复指定提交的文件
git reset <file>                      # 取消暂存文件
git checkout <commit-hash>            # 回到指定提交（分离头指针状态）
git reset HEAD~1                      # 撤销最近一次提交（保留更改）
git reset --hard HEAD~1               # 彻底撤销最近一次提交
git commit --amend                    # 修改最近一次提交信息
git rebase -i HEAD~3                  # 交互式合并最近3次提交
git cherry-pick <commit-hash>         # 应用指定提交
git clean -fd                         # 删除未跟踪的文件和目录
```

## 提交到远程
```bash
git pull                       # 重要！拉取远程更新（fetch+merge）
git push origin <branch>       # 推送到远程分支
git push -u origin <branch>    # 设置上游并推送
git fetch                      # 仅获取远程更新
git pull --rebase              # 拉取并变基
```

## 强制覆盖（一般不建议）
```bash
git push --force              # 强制推送（覆盖远程）
git push --force-with-lease   # 更安全的强制推送
git reset --hard origin/main  # 强制本地与远程同步
```

## 远程
```bash
git remote add <name> <url>          # 添加远程仓库
git remote -v                        # 查看远程仓库
git remote remove <name>             # 删除远程仓库
git remote rename <old> <new>        # 重命名远程仓库
git remote show <name>               # 查看远程仓库详情
git remote set-url <name> <newurl>   # 修改远程仓库地址
```

## 查看日志
```bash
git log                       # 详细日志
git log --oneline             # 简洁日志
git log --graph               # 图形化显示分支
git log --stat                # 显示文件改动统计
git log -p                    # 显示具体改动内容
git log --since="2024-01-01"  # 按时间筛选
git log --author="name"       # 按作者筛选
git log -S "function"         # 搜索代码改动
```
---