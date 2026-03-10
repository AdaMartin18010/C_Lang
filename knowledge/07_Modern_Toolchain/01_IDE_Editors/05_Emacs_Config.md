# Emacs C语言开发配置指南

> **层级定位**: 07 Modern Toolchain / 01 IDE Editors
> **难度级别**: L3 进阶
> **预估学习时间**: 6-8小时

---

## 📋 简介

Emacs是历史悠久的编辑器，功能极其强大。

---

## 🚀 快速配置

### 安装

```bash
# macOS
brew install emacs

# Linux
sudo apt-get install emacs

# Windows
# 下载: https://www.gnu.org/software/emacs/
```

### 基础C配置

**~/.emacs** 或 **~/.emacs.d/init.el**:

```elisp
;; 包管理
(require 'package)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/"))
(package-initialize)

;; 安装use-package
(unless (package-installed-p 'use-package)
  (package-refresh-contents)
  (package-install 'use-package))

;; C模式配置
(use-package cc-mode
  :hook (c-mode . (lambda ()
                    (setq tab-width 4)
                    (setq indent-tabs-mode nil))))

;; LSP
(use-package lsp-mode
  :hook (c-mode . lsp)
  :commands lsp)

(use-package lsp-ui
  :commands lsp-ui-mode)

;; 补全
(use-package company
  :hook (after-init . global-company-mode))
```

---

## 📚 常用快捷键

| 快捷键 | 功能 |
|:-------|:-----|
| `C-x C-f` | 打开文件 |
| `C-x C-s` | 保存 |
| `C-g` | 取消 |
| `M-x` | 执行命令 |
| `C-x b` | 切换缓冲区 |

---

**← [返回工具链主页](../README.md)**
