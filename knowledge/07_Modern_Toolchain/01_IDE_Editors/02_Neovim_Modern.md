# Neovim 现代C语言开发配置指南

> **层级定位**: 07 Modern Toolchain / 01 IDE Editors
> **难度级别**: L3 进阶
> **预估学习时间**: 8-12小时

---

## 📋 为什么选择Neovim

### 核心优势

| 特性 | 说明 |
|:-----|:-----|
| **极致性能** | 启动快，内存占用极低 |
| **完全可定制** | Lua配置，无限可能 |
| **现代架构** | 内置LSP、Tree-sitter |
| **终端原生** | 服务器开发首选 |
| **生态丰富** | 活跃的插件社区 |

### 适用人群

- 追求键盘效率的开发者
- 服务器/远程开发场景
- 喜欢终端环境的用户
- 愿意投入时间配置的用户

---

## 🚀 快速开始

### 安装Neovim

```bash
# macOS
brew install neovim

# Linux (Ubuntu)
sudo apt-get install neovim

# Windows (scoop)
scoop install neovim
```

### 基础配置结构

```
~/.config/nvim/
├── init.lua              # 主配置文件
├── lua/
│   ├── plugins.lua       # 插件管理
│   ├── lsp.lua           # LSP配置
│   └── options.lua       # 编辑器选项
└── plugin/               # 插件安装目录
```

### 最小可用配置

**~/.config/nvim/init.lua**:

```lua
-- 基础设置
vim.opt.number = true
vim.opt.tabstop = 4
vim.opt.shiftwidth = 4
vim.opt.expandtab = true

-- 插件管理器 (lazy.nvim)
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.loop.fs_stat(lazypath) then
  vim.fn.system({
    "git", "clone", "--filter=blob:none",
    "https://github.com/folke/lazy.nvim.git",
    "--branch=stable", lazypath,
  })
end
vim.opt.rtp:prepend(lazypath)

-- 插件列表
require("lazy").setup({
  -- 语法高亮
  { "nvim-treesitter/nvim-treesitter", build = ":TSUpdate" },

  -- LSP
  { "neovim/nvim-lspconfig" },

  -- 自动补全
  { "hrsh7th/nvim-cmp" },
  { "hrsh7th/cmp-nvim-lsp" },
})

-- LSP配置
local lspconfig = require('lspconfig')
lspconfig.clangd.setup({})
```

---

## 📚 完整配置方案

### 推荐方案: LazyVim (新手友好)

**安装**:

```bash
# 备份旧配置
mv ~/.config/nvim ~/.config/nvim.bak

# 安装LazyVim
git clone https://github.com/LazyVim/starter ~/.config/nvim
rm -rf ~/.config/nvim/.git
```

**C开发增强**:

创建 `~/.config/nvim/lua/plugins/c-cpp.lua`:

```lua
return {
  -- 确保安装C解析器
  {
    "nvim-treesitter/nvim-treesitter",
    opts = function(_, opts)
      vim.list_extend(opts.ensure_installed, { "c", "cpp" })
    end,
  },

  -- clangd配置
  {
    "neovim/nvim-lspconfig",
    opts = {
      servers = {
        clangd = {
          cmd = {
            "clangd",
            "--background-index",
            "--clang-tidy",
            "--header-insertion=iwyu",
            "--completion-style=bundled",
            "--pch-storage=memory",
          },
        },
      },
    },
  },

  -- 调试适配器
  {
    "mfussenegger/nvim-dap",
    dependencies = {
      "rcarriga/nvim-dap-ui",
    },
    config = function()
      local dap = require("dap")

      -- GDB配置
      dap.adapters.gdb = {
        type = "executable",
        command = "gdb",
        args = { "-i", "dap" },
      }

      dap.configurations.c = {
        {
          name = "Launch",
          type = "gdb",
          request = "launch",
          program = function()
            return vim.fn.input("Path to executable: ", vim.fn.getcwd() .. "/", "file")
          end,
          cwd = "${workspaceFolder}",
        },
      }
    end,
  },
}
```

### 进阶方案: 自定义配置

**完整init.lua示例**:

```lua
-- ============================================
-- Neovim C开发完整配置
-- ============================================

-- 基础选项
local opt = vim.opt
opt.number = true
opt.relativenumber = true
opt.tabstop = 4
opt.shiftwidth = 4
opt.expandtab = true
opt.smartindent = true
opt.wrap = false
opt.cursorline = true
opt.termguicolors = true

-- 键位映射
vim.g.mapleader = " "

-- 插件管理
require("plugins")
require("lsp")
require("cmp_config")
```

**lua/plugins.lua**:

```lua
-- 插件管理器引导
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.loop.fs_stat(lazypath) then
  vim.fn.system({
    "git", "clone", "--filter=blob:none",
    "https://github.com/folke/lazy.nvim.git",
    "--branch=stable", lazypath,
  })
end
vim.opt.rtp:prepend(lazypath)

-- 插件列表
require("lazy").setup({
  -- 主题
  { "catppuccin/nvim", name = "catppuccin", priority = 1000 },

  -- 语法树
  {
    "nvim-treesitter/nvim-treesitter",
    build = ":TSUpdate",
    config = function()
      require("nvim-treesitter.configs").setup({
        ensure_installed = { "c", "cpp", "lua", "vim" },
        highlight = { enable = true },
        indent = { enable = true },
      })
    end,
  },

  -- LSP
  { "neovim/nvim-lspconfig" },
  { "williamboman/mason.nvim" },
  { "williamboman/mason-lspconfig.nvim" },

  -- 自动补全
  { "hrsh7th/nvim-cmp" },
  { "hrsh7th/cmp-nvim-lsp" },
  { "hrsh7th/cmp-buffer" },
  { "hrsh7th/cmp-path" },
  { "L3MON4D3/LuaSnip" },
  { "saadparwaiz1/cmp_luasnip" },

  -- 文件浏览器
  {
    "nvim-tree/nvim-tree.lua",
    dependencies = { "nvim-tree/nvim-web-devicons" },
    config = true,
  },

  -- 模糊查找
  {
    "nvim-telescope/telescope.nvim",
    dependencies = { "nvim-lua/plenary.nvim" },
  },

  -- 状态栏
  {
    "nvim-lualine/lualine.nvim",
    config = function()
      require("lualine").setup({
        options = { theme = "catppuccin" },
      })
    end,
  },
})

-- 加载主题
vim.cmd.colorscheme "catppuccin-mocha"
```

**lua/lsp.lua**:

```lua
-- LSP配置
require("mason").setup()
require("mason-lspconfig").setup({
  ensure_installed = { "clangd" },
})

local lspconfig = require("lspconfig")
local capabilities = require("cmp_nvim_lsp").default_capabilities()

-- clangd配置
lspconfig.clangd.setup({
  capabilities = capabilities,
  cmd = {
    "clangd",
    "--background-index",
    "--clang-tidy",
    "--header-insertion=iwyu",
    "--completion-style=bundled",
    "--pch-storage=memory",
    "--cross-file-rename",
  },
  on_attach = function(client, bufnr)
    -- 键位映射
    local opts = { buffer = bufnr, silent = true }
    vim.keymap.set("n", "gd", vim.lsp.buf.definition, opts)
    vim.keymap.set("n", "gr", vim.lsp.buf.references, opts)
    vim.keymap.set("n", "K", vim.lsp.buf.hover, opts)
    vim.keymap.set("n", "<leader>rn", vim.lsp.buf.rename, opts)
    vim.keymap.set("n", "<leader>ca", vim.lsp.buf.code_action, opts)
  end,
})
```

**lua/cmp_config.lua**:

```lua
-- 自动补全配置
local cmp = require("cmp")
local luasnip = require("luasnip")

cmp.setup({
  snippet = {
    expand = function(args)
      luasnip.lsp_expand(args.body)
    end,
  },
  mapping = cmp.mapping.preset.insert({
    ["<C-b>"] = cmp.mapping.scroll_docs(-4),
    ["<C-f>"] = cmp.mapping.scroll_docs(4),
    ["<C-Space>"] = cmp.mapping.complete(),
    ["<CR>"] = cmp.mapping.confirm({ select = true }),
    ["<Tab>"] = cmp.mapping(function(fallback)
      if cmp.visible() then
        cmp.select_next_item()
      elseif luasnip.expand_or_jumpable() then
        luasnip.expand_or_jump()
      else
        fallback()
      end
    end, { "i", "s" }),
  }),
  sources = cmp.config.sources({
    { name = "nvim_lsp" },
    { name = "luasnip" },
  }, {
    { name = "buffer" },
    { name = "path" },
  }),
})
```

---

## ⌨️ 常用快捷键

### 基础操作

| 快捷键 | 功能 |
|:-------|:-----|
| `Space` | Leader键 |
| `<Space>e` | 打开文件树 |
| `<Space>ff` | 模糊查找文件 |
| `<Space>fg` | 全局搜索 |
| `<Space>fb` | 查找缓冲区 |

### LSP相关

| 快捷键 | 功能 |
|:-------|:-----|
| `gd` | 跳转到定义 |
| `gr` | 查找引用 |
| `K` | 显示文档 |
| `<Space>rn` | 重命名 |
| `<Space>ca` | 代码操作 |
| `<Space>f` | 格式化 |

### 调试

| 快捷键 | 功能 |
|:-------|:-----|
| `<F5>` | 开始调试 |
| `<F9>` | 切换断点 |
| `<F10>` | 单步跳过 |
| `<F11>` | 单步进入 |

---

## 🔧 常见问题

### Q1: LSP无法启动

**检查**:

```bash
which clangd
clangd --version
```

**解决**:

```lua
-- 指定clangd路径
cmd = { "/usr/local/bin/clangd", "--background-index" }
```

### Q2: 补全不工作

**检查compile_commands.json**:

```bash
# 使用bear生成
bear -- make

# 或使用cmake
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .
```

### Q3: 主题不显示

**解决**:

```lua
-- 确保在插件加载后设置
vim.opt.termguicolors = true
vim.cmd.colorscheme "catppuccin-mocha"
```

---

## ✅ 验证清单

- [ ] Neovim安装完成 (`nvim --version`)
- [ ] 插件管理器安装
- [ ] LSP服务器安装 (`:Mason`)
- [ ] clangd配置正确
- [ ] 自动补全工作
- [ ] 代码跳转正常
- [ ] 调试器配置完成

---

**← [返回工具链主页](../README.md)**
