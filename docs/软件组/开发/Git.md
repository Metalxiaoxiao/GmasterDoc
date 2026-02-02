# Git

## 📌 写在前面

本文档参考2025届老登邹绎达组长编写的《git workflow》，特此感谢。
这是一份全面的 Git 学习指南，涵盖从基础概念到实际工作流的所有内容。本文档既可作为学习资料，也可作为工作中的参考手册。

遇到问题时，可使用 `git help <command>` 获取更详细的帮助信息。

---

## 第一部分：核心概念与基础

### 什么是版本控制？

**版本控制就是一种记录和管理文件变化历史的系统**。在软件开发中，我们经常需要修改代码，而这些修改往往涉及多个文件。如果没有版本控制系统，我们很难追踪谁做了什么改动、什么时候做的、为什么要做这些改动。更糟糕的是，当我们发现某个改动导致了 bug 时，我们几乎无法快速恢复到之前的状态。

**Git** 是当今最流行的分布式版本控制系统，它允许我们：
-  记录每一次代码改动
-  随时回到任何历史版本
-  创建多个分支进行并行开发
-  与团队成员协作开发
-  追踪谁做了什么改动

### 工作区、暂存区和版本库

在使用 Git 前，必须理解三个核心区域的概念：

```
本地仓库（local）
    ↓ git push / pull
远程 Fork 仓库（origin）
    ↓ Pull Request
远程上游仓库（upstream）
```

这三个区域的关系是 Git 工作流的核心。理解了这个概念，你就能理解为什么需要 `git add`、`git commit` 等命令。

### 分支 (Branch)

分支是指向某个特定提交的"指针"。主分支（通常叫 `main` 或 `master`）是项目的主线，而其他分支则允许你在不影响主线的情况下进行独立的开发。

**为什么需要分支？**

想象一个场景：你和你的团队在开发一个项目。你需要开发新功能 A，而你的同事需要开发功能 B。如果你们都在同一个分支上工作，你们的改动会互相干扰。分支解决了这个问题——允许多人在不同的分支上独立工作，最后再将各自的改动合并回主分支。

### 提交 (Commit)

每一次 `git commit` 都会创建一个快照，记录当前项目的完整状态。每个 commit 都包含：
- 作者信息
- 提交时间
- 提交信息（描述这次改动做了什么）
- 该次改动具体修改了哪些文件
- 唯一的 commit hash（用于标识这个提交）

### 三个仓库概念（在团队协作中）

在团队开发中，通常存在三个仓库：

```
第 1 层：本地仓库（local）
    - 你本机上的完整 Git 仓库
    - 包含所有分支和提交历史

        ↓ git push / pull

第 2 层：远程 Fork 仓库（origin）
    - 你在 GitHub 上的个人复制仓库
    - 用于保存你的工作成果

        ↓ Pull Request

第 3 层：远程上游仓库（upstream）
    - 原始的、官方的项目仓库
    - 所有贡献者的代码最终都会合并到这里
```

---
## 第二部分：工作流
工作流概述
根据项目类型的不同，我们采用两种不同的工作流：GSRL 库贡献工作流和项目仓库开发工作流

---

### GSRL 库贡献工作流
采用Git Flow工作流, 基于fork+pull request的协作模式。
可以使用两种fork仓库：
1. （推荐）使用GMasterEmbeddedGroup组织下的GSRL fork仓库进行开发，便于组内协作
和代码管理。
2. 在个人GitHub账号下创建GSRL fork仓库进行开发。

工作流图解
```
你的本地仓库 (local)
    ↑↓ git push / pull
你的 Fork 仓库 (origin)
    ↑ Pull Request
GSRL 上游仓库 (upstream)
    ↓ 代码评审通过后合并
```

#### 1. 创建fork仓库
fork仓库是原始仓库的副本，所有开发工作均在fork仓库中进行，完成后通过pull request将代码
合并回原始仓库。
你对fork仓库拥有完全的读写权限，fork仓库不会影响原始仓库的代码（pull request前），其在
保护了原始仓库的完整性的同时，允许你自由地进行开发和实验。
以下操作二选一：
1. 访问GMasterEmbeddedGroup组织下的GSRL仓库(fork)
2. 访问GSRL仓库，点击右上角的 Fork 按钮，将仓库fork到个人GitHub账号下。
#### 2. 克隆fork仓库（及其子模块）到本地
在非中文工作路径下打开终端并执行以下命令：
git clone --recurse-submodules <fork仓库地址>
cd GSRL
或者，分布克隆仓库和子模块：
git clone <fork仓库地址>
cd GSRL
git submodule update --init --recursive
记得在执行完上述命令后，命令提示符（终端输入光标前的部分）会显示当前所在的分支名称，
例如：main
若没有显示，检查当前命令提示符路径是否在GSRL仓库根目录下（是否执行了上述命令中的cd
GSRL），是否使用的是Git Bash。
#### 3. 添加上游仓库(upstream)
为了使得 git pull , git push 等命令能够正确工作，需要配置git的远程仓库地址。
在克隆仓库时，会默认添加克隆仓库(fork仓库)的地址为远程仓库地址，命名为 origin 。
可以通过以下命令查看当前远程仓库地址：
```bash
git remote -v
```
示例输出：
```bash
$ git remote -v
origin https://github.com/xxx/GSRL.git (fetch)
origin https://github.com/xxx/GSRL.git (push)
```
对于fork仓库， origin 指向的是原始仓库的副本（个人/组织的fork仓库），为了方便后续同步
最新代码，我们需要添加原始的上游仓库地址，命名为 upstream 。
执行以下命令添加上游仓库地址：
```bash
git remote add upstream https://github.com/popchutty/GSRL.git
```
可以再次执行 git remote -v 命令查看当前远程仓库地址，输出应包含 origin (你的fork)
和 upstream (原作者的仓库)
一个典型的基于fork仓库的开发环境中一共存在以下三个仓库：
• 本地仓库(local)：存储在你电脑上的仓库
• 远程fork仓库(origin)：存储在你的GitHub账号下的fork仓库
• 远程上游仓库(upstream)：存储在原作者账号下的原始仓库

#### 4. 创建和切换分支
永远不要直接在main分支上开发，必须新建分支。
（可选）确保在main分支上并习惯性的同步一下上游最新代码：
git switch main
git pull upstream main
创建并切换到新分支：
```bash
git switch --create <your-branch-name>
```
 或简写:

```bash
git switch -c <your-branch-name>
```
<your-branch-name> 请根据以下规范命名：

- 功能开发分支：dev/模块名-简要描述（如：dev/device-motor）
- 缺陷修复分支：fix/模块名-简要描述（如：fix/driver-can）
- 文档更新分支：docs/文档名-简要描述（如：docs/motor-api-update）
- 架构重构分支：refactor/模块名-简要描述（如：refactor/lib-dsp）
发布分支到远程fork仓库(origin)：

推送本地的分支到远程fork仓库(origin)，并同时设置fork上远程分支为本地分支的上游分支

```bash
git push --set-upstream origin <your-branch-name>
```
 或简写:

```bash
git push -u origin <your-branch-name>
```

#### 5. 开发和提交代码
在新建的分支上进行代码开发，以下是常用Git命令：
- 暂存所有修改的文件
```bash
git add .
```
- 提交代码，-m后跟提交信息
```bash
git commit -m "简要描述本次提交的内容"
```
- 或使用等效长参数 --message：
```bash
git commit --message "简要描述本次提交的内容"
```
- 推送代码到远程fork仓库(origin)
```bash
git push origin <your-branch-name>
```
 或简写(若已通过git push -u设置好上游分支):
```bash
git push
```
#### 6. 合并前准备：同步上游与整理提交
在提交Pull Request(PR)前，再次确保代码是基于上游最新代码开发的，并整理提交记录。

假设在你开发的过程中，上游原始仓库已经有了其他人的新的提交/合并
拉取上游main分支(upstream/main)的最新代码到本地main分支(main)：
1. 切换到本地main分支
git switch main
2. 拉取上游最新代码到本地main分支
git pull upstream main
这一步理论不应该出现任何冲突，若出现，请及时联系管理者
顺手更新一下远程fork仓库的main分支(origin/main)：
git push origin main
 或简写(若已通过**git push -u**设置好上游分支):
git push
接下来变基并整理提交历史：
3. 切换回你的开发分支
```bash
git switch <your-branch-name>
```
4. 交互式变基到最新的main分支
```bash
git rebase -i main
```

此时会打开一个文本编辑器(git默认文本编辑器)，列出你在该分支上的所有提交记录。按照提示修改提交记录，例如合并多个零碎提交为一个有意义的提交，修改提交信息等。

比如，将多个小的修复提交的**pick**改为**squash**，将琐碎的“fix typo”合并成一个大的Commit。

提示：如果你没有配置过git的默认文本编辑器，git大概率会使用**Vim**作为文本编辑器，建议将git默认文本编辑器配置为你熟悉的编辑器，如**VSCode**.

>如果已经不慎进入了Vim编辑器，以下是一些基本操作：
>- 按下**i** 键进入编辑模式，此时左下角显示 **-- INSERT --** ，使用方向键移动光标，输入文本进行编辑
>- 编辑完成后，按下 **Esc** 键退出编辑模式
>- 依次进行以下操作：输入 **:wq** ，然后按下**Enter** 来保存并退出Vim编辑器

若在交互式变基过程中遇到冲突，git会提示你哪些文件存在冲突，请手动打开这些文件，解决冲突并保存文件后，执行以下命令继续变基：
- 暂存解决冲突后的文件(<conflicted-file>替换为实际冲突的文件名)
```bash
git add <conflicted-file>
```
- 继续变基
```bash
git rebase --continue
```
完成变基后，使用带保护的强制推送将你的开发分支推送到远程fork仓库(origin)：
```bash
git push --force-with-lease origin <your-branch-name>
```
 或简写(若已通过git push -u设置好上游分支):
```
git push --force-with-lease
```
#### 创建Pull Request请求合并分支
在GitHub上打开你的fork仓库页面，切换到你刚刚推送的分支 <your-branch-name> ，点
击 Compare & pull request 按钮，填写PR标题和描述后，点击 Create pull request 按钮提交
PR请求。
#### 代码评审与收尾工作
PR创建后，等待代码评审人员的审核和反馈，若有修改建议，请在本地分支进行修改并提交，
再重新推送到远程fork仓库(origin)，PR会自动更新。
具体步骤为：
- 切换到你的分支
```bash
git switch <your-branch-name>
```
- 根据反馈意见在本地分支进行修改
- 暂存修改
```bash
git add .
```
- 提交修改
```bash
git commit -m "根据反馈意见修改后的提交信息"
```
- 推送修改到远程fork仓库(origin)
```bash
git push origin <your-branch-name>
```
- 或简写(若已通过git push -u设置好上游分支):
```bash
git push
```
PR审核通过后，恭喜你，你的代码被成功合并到上游仓库的主分支中了，接下来可以清理(可
选)本地工作现场以准备下一个开发任务：
- 切换回main分支
git switch main
- 拉取上游最新代码到本地main分支
git pull upstream main
- 顺手更新一下远程fork仓库的main分支(origin/main)
```bash
git push origin main
```
 或简写(若已通过git push -u设置好上游分支):
```bash
git push
```
- 删除远程fork仓库(origin)的分支(可选，若确认该分支不再使用)
git push origin --delete <your-branch-name>
- 更新本地仓库，并裁剪无用的远程分支引用
git pull --prune
- 删除本地开发分支(可选，若确认该分支不再使用)
git branch --delete <your-branch-name>
 或简写:
git branch -d <your-branch-name>

#### 总结
通过以上步骤，你已经完成了基于Git Flow工作流的GSRL库开发全过程。
初次执行如果觉得难以适应和理解，记住以下要点：
1. 整个工作流中存在的三个仓库及他们在命令中的名字：
• 本地仓库: local
• 远程fork仓库(复制仓库): origin
• 远程上游仓库(原作者仓库): upstream
2. 几乎所有命令干的事情就是：在这几个仓库之间进行拉取、推送、合并等同步操作。

### 项目仓库开发
一般战队每年一个机器人会对应一个项目仓库，命名为： 年份-兵种名称 ，仓库存放在GitHub上GMasterEmbeddedGroup组织下，一般是私有仓库。

由于项目仓库开发成员较少（一般1-2名运动控制算法组成员），且项目生命周期较短，因此项目仓库采用简化的Git工作流。工作流详细步骤
#### 1. 克隆项目仓库（及其子模块）到本地
分两种情况：
1. 组织中已有该项目仓库：见1.1.2克隆fork仓库（及其子模块）到本地，将<fork仓库地址>替
换为组织中的项目仓库地址即可。
2. 组织中没有该项目仓库（为新建项目）：联系管理员创建空的项目仓库后，按以下步骤新建
项目：
- 若为基于GSRL库开发的项目：

    a. 参照克隆fork仓库（及其子模块）[2. 克隆fork仓库（及其子模块）到本地](#2-克隆fork仓库及其子模块到本地)到本地中的命令，将<fork仓库地址>替换为GSRL原始仓库地址： https://github.com/popchutty/GSRL.git

    b. 在终端中进入项目根目录（确保命令提示符路径为GSRL结尾），执行以下命令删除掉本地的Git仓库：
    ```bash
    rm -rf .git
    ```

    c. 按照GitHub上空仓库中的提示（推送新仓库），将本地代码推送到新建的项目仓库中。

- 创建和切换分支
参照[4.创建和切换分支](#4-创建和切换分支)，跳过同步上游代码的步骤。
- 开发和提交代码
参照[5.开发和提交代码](#5-开发和提交代码)。
- 合并到主分支
在项目仓库中，直接将开发分支合并到main分支，有两种方式：
1. (建议)通过GitHub网页界面发起Pull Request，经过代码评审后合并。
2. 通过命令行完成合并：

    - 切换到main分支
    ```bash
    git switch main
    ```
    - 合并开发分支到main分支
    ```bash
    git merge <your-branch-name>
    ```
    - 若有冲突，手动解决冲突后，执行以下命令完成合并
    ```bash
    git add <conflicted-file>
    git commit
    ```
    - 推送main分支到远程仓库
    ```bash
    git push origin main
    ```
    或简写(若已通过git push -u设置好上游分支):
    ```bash
    git push
    ```
#### 5. 合并后收尾
参照[代码评审与收尾工作](#代码评审与收尾工作)，跳过与上游仓库相关的操作。


---

## 第三部分：常用git命令
VS Code 的图形化界面上手轻松、体验友好；而命令行在熟悉之后往往能带来更高的工作效率。两种方式各有优势，大家可以根据自己的习惯自由选择。下面整理了一些常用的 Git 命令，希望能为倾向使用命令行的同学提供参考。

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