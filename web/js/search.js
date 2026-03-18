/**
 * 搜索模块
 * 提供全文搜索、过滤和自动完成功能
 */

const SearchManager = (function() {
    'use strict';

    // DOM 元素引用
    let searchInput;
    let suggestionsContainer;
    
    // 状态变量
    let currentHighlight = -1;
    let searchResults = [];
    let debounceTimer = null;

    /**
     * 初始化搜索功能
     */
    function init() {
        searchInput = document.getElementById('search-input');
        suggestionsContainer = document.getElementById('search-suggestions');

        // 绑定输入事件
        searchInput.addEventListener('input', handleInput);
        searchInput.addEventListener('keydown', handleKeyDown);
        searchInput.addEventListener('focus', handleFocus);
        
        // 点击外部关闭建议
        document.addEventListener('click', handleDocumentClick);
        
        // 绑定过滤器事件
        bindFilterEvents();
        
        // 绑定学习路径事件
        bindLearningPathEvents();
        
        // 绑定快捷键
        bindShortcuts();
    }

    /**
     * 处理输入事件
     */
    function handleInput(e) {
        const query = e.target.value.trim();
        
        // 清除之前的防抖定时器
        if (debounceTimer) {
            clearTimeout(debounceTimer);
        }
        
        if (query.length === 0) {
            hideSuggestions();
            return;
        }
        
        // 防抖处理
        debounceTimer = setTimeout(() => {
            performSearch(query);
        }, 150);
    }

    /**
     * 执行搜索
     */
    function performSearch(query) {
        searchResults = DataLoader.search(query, 10);
        renderSuggestions(searchResults);
    }

    /**
     * 渲染搜索建议
     */
    function renderSuggestions(results) {
        if (results.length === 0) {
            suggestionsContainer.innerHTML = `
                <div class="suggestion-item no-results">
                    <i class="fas fa-search"></i>
                    <span>未找到匹配结果</span>
                </div>
            `;
            suggestionsContainer.classList.add('active');
            return;
        }
        
        suggestionsContainer.innerHTML = results.map((node, index) => `
            <div class="suggestion-item" data-index="${index}" data-node-id="${node.id}">
                <i class="fas ${getIconForType(node.type)}"></i>
                <span>${highlightMatch(node.name, searchInput.value)}</span>
                <span class="suggestion-type">${getTypeLabel(node.type)}</span>
            </div>
        `).join('');
        
        // 绑定点击事件
        suggestionsContainer.querySelectorAll('.suggestion-item').forEach(item => {
            item.addEventListener('click', () => {
                const nodeId = item.dataset.nodeId;
                selectResult(nodeId);
            });
        });
        
        suggestionsContainer.classList.add('active');
        currentHighlight = -1;
    }

    /**
     * 高亮匹配文本
     */
    function highlightMatch(text, query) {
        const regex = new RegExp(`(${escapeRegex(query)})`, 'gi');
        return text.replace(regex, '<strong style="color: var(--text-link);">$1</strong>');
    }

    /**
     * 转义正则特殊字符
     */
    function escapeRegex(string) {
        return string.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    }

    /**
     * 获取类型图标
     */
    function getIconForType(type) {
        const icons = {
            module: 'fa-layer-group',
            topic: 'fa-book',
            concept: 'fa-lightbulb'
        };
        return icons[type] || 'fa-circle';
    }

    /**
     * 获取类型标签
     */
    function getTypeLabel(type) {
        const labels = {
            module: '模块',
            topic: '主题',
            concept: '概念'
        };
        return labels[type] || type;
    }

    /**
     * 处理键盘事件
     */
    function handleKeyDown(e) {
        const items = suggestionsContainer.querySelectorAll('.suggestion-item:not(.no-results)');
        
        switch (e.key) {
            case 'ArrowDown':
                e.preventDefault();
                currentHighlight = Math.min(currentHighlight + 1, items.length - 1);
                updateHighlight(items);
                break;
                
            case 'ArrowUp':
                e.preventDefault();
                currentHighlight = Math.max(currentHighlight - 1, -1);
                updateHighlight(items);
                break;
                
            case 'Enter':
                e.preventDefault();
                if (currentHighlight >= 0 && items[currentHighlight]) {
                    const nodeId = items[currentHighlight].dataset.nodeId;
                    selectResult(nodeId);
                } else if (searchResults.length > 0) {
                    selectResult(searchResults[0].id);
                }
                break;
                
            case 'Escape':
                hideSuggestions();
                searchInput.blur();
                break;
        }
    }

    /**
     * 更新高亮状态
     */
    function updateHighlight(items) {
        items.forEach((item, index) => {
            item.classList.toggle('highlighted', index === currentHighlight);
        });
        
        // 滚动到可见区域
        if (currentHighlight >= 0 && items[currentHighlight]) {
            items[currentHighlight].scrollIntoView({ block: 'nearest' });
        }
    }

    /**
     * 选择搜索结果
     */
    function selectResult(nodeId) {
        hideSuggestions();
        searchInput.value = '';
        
        // 聚焦到图谱中的节点
        GraphVisualization.focusNode(nodeId);
        
        // 添加到最近搜索（可以实现历史记录功能）
        addToRecentSearches(nodeId);
    }

    /**
     * 添加到最近搜索
     */
    function addToRecentSearches(nodeId) {
        const node = DataLoader.getNode(nodeId);
        if (!node) return;
        
        let recent = JSON.parse(localStorage.getItem('recentSearches') || '[]');
        recent = recent.filter(r => r.id !== nodeId);
        recent.unshift({ id: nodeId, name: node.name, timestamp: Date.now() });
        recent = recent.slice(0, 10); // 只保留10条
        
        localStorage.setItem('recentSearches', JSON.stringify(recent));
    }

    /**
     * 处理输入框获得焦点
     */
    function handleFocus() {
        if (searchInput.value.trim().length > 0) {
            suggestionsContainer.classList.add('active');
        }
    }

    /**
     * 处理文档点击
     */
    function handleDocumentClick(e) {
        if (!searchInput.contains(e.target) && !suggestionsContainer.contains(e.target)) {
            hideSuggestions();
        }
    }

    /**
     * 隐藏建议列表
     */
    function hideSuggestions() {
        suggestionsContainer.classList.remove('active');
        currentHighlight = -1;
    }

    /**
     * 绑定过滤器事件
     */
    function bindFilterEvents() {
        const filterToggle = document.getElementById('filter-toggle');
        const filterPanel = document.getElementById('filter-panel');
        const resetBtn = document.getElementById('reset-filters');
        const applyBtn = document.getElementById('apply-filters');
        
        // 切换过滤器面板
        if (filterToggle) {
            filterToggle.addEventListener('click', () => {
                filterPanel.classList.toggle('collapsed');
                const icon = filterToggle.querySelector('i');
                icon.classList.toggle('fa-chevron-up');
                icon.classList.toggle('fa-chevron-down');
            });
        }
        
        // 重置过滤器
        if (resetBtn) {
            resetBtn.addEventListener('click', () => {
                document.querySelectorAll('[data-filter]').forEach(checkbox => {
                    checkbox.checked = true;
                });
                applyFilters();
            });
        }
        
        // 应用过滤器
        if (applyBtn) {
            applyBtn.addEventListener('click', applyFilters);
        }
        
        // 实时过滤（可选）
        document.querySelectorAll('[data-filter]').forEach(checkbox => {
            checkbox.addEventListener('change', debounce(applyFilters, 300));
        });
    }

    /**
     * 应用过滤器
     */
    function applyFilters() {
        const typeFilters = Array.from(document.querySelectorAll('[data-filter="type"]:checked'))
            .map(cb => cb.value);
        const difficultyFilters = Array.from(document.querySelectorAll('[data-filter="difficulty"]:checked'))
            .map(cb => cb.value);
        
        GraphVisualization.filterNodes(typeFilters, difficultyFilters);
    }

    /**
     * 防抖函数
     */
    function debounce(func, wait) {
        let timeout;
        return function executedFunction(...args) {
            const later = () => {
                clearTimeout(timeout);
                func(...args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    }

    /**
     * 绑定学习路径事件
     */
    function bindLearningPathEvents() {
        const openBtn = document.getElementById('plan-learning');
        const closeBtn = document.getElementById('close-learning-path');
        const generateBtn = document.getElementById('generate-path');
        const panel = document.getElementById('learning-path-panel');
        const targetSelect = document.getElementById('target-node');
        
        // 填充目标节点选择器
        const populateTargetSelect = () => {
            const nodes = DataLoader.getData().nodes.filter(n => n.type !== 'module');
            targetSelect.innerHTML = '<option value="">-- 选择学习目标 --</option>' +
                nodes.map(n => `<option value="${n.id}">${n.name}</option>`).join('');
        };
        
        // 初始填充
        populateTargetSelect();
        
        // 关闭面板
        if (closeBtn) {
            closeBtn.addEventListener('click', () => {
                panel.classList.add('hidden');
                GraphVisualization.clearHighlight();
            });
        }
        
        // 生成学习路径
        if (generateBtn) {
            generateBtn.addEventListener('click', generateLearningPath);
        }
    }

    /**
     * 生成学习路径
     */
    function generateLearningPath() {
        const targetId = document.getElementById('target-node').value;
        const pathType = document.querySelector('input[name="path-type"]:checked')?.value || 'shortest';
        
        if (!targetId) {
            alert('请选择一个学习目标');
            return;
        }
        
        const path = DataLoader.getLearningPath(targetId, pathType);
        renderLearningPath(path);
        
        // 在图谱中高亮路径
        const pathIds = path.map(n => n.id);
        GraphVisualization.highlightPath(pathIds);
    }

    /**
     * 渲染学习路径
     */
    function renderLearningPath(path) {
        const resultContainer = document.getElementById('path-result');
        
        if (path.length === 0) {
            resultContainer.innerHTML = '<p class="text-muted">无法生成学习路径</p>';
            return;
        }
        
        const totalTime = path.reduce((sum, node) => sum + (node.estimatedTime || 0), 0);
        
        resultContainer.innerHTML = `
            <div class="path-summary">
                <p><i class="fas fa-clock"></i> 预计总学习时间: <strong>${totalTime}小时</strong></p>
                <p><i class="fas fa-list-ol"></i> 共 <strong>${path.length}</strong> 个学习节点</p>
            </div>
            <div class="path-steps">
                ${path.map((node, index) => `
                    <div class="path-step" data-node-id="${node.id}">
                        <div class="path-step-number">${index + 1}</div>
                        <div class="path-step-content">
                            <div class="path-step-title">${node.name}</div>
                            <div class="path-step-meta">
                                <span class="badge ${node.difficulty}">${getDifficultyLabel(node.difficulty)}</span>
                                ${node.estimatedTime ? `<span>${node.estimatedTime}小时</span>` : ''}
                            </div>
                        </div>
                    </div>
                `).join('')}
            </div>
        `;
        
        // 绑定点击事件
        resultContainer.querySelectorAll('.path-step').forEach(step => {
            step.addEventListener('click', () => {
                const nodeId = step.dataset.nodeId;
                GraphVisualization.focusNode(nodeId);
            });
        });
    }

    /**
     * 获取难度标签
     */
    function getDifficultyLabel(difficulty) {
        const labels = {
            beginner: '初级',
            intermediate: '中级',
            advanced: '高级',
            expert: '专家'
        };
        return labels[difficulty] || difficulty;
    }

    /**
     * 绑定快捷键
     */
    function bindShortcuts() {
        document.addEventListener('keydown', (e) => {
            // Ctrl/Cmd + F 聚焦搜索框
            if ((e.ctrlKey || e.metaKey) && e.key === 'f') {
                e.preventDefault();
                searchInput.focus();
            }
            
            // Ctrl/Cmd + 0 重置视图
            if ((e.ctrlKey || e.metaKey) && e.key === '0') {
                e.preventDefault();
                GraphVisualization.resetView();
            }
            
            // ESC 关闭弹窗
            if (e.key === 'Escape') {
                const activeModal = document.querySelector('.modal.active');
                if (activeModal) {
                    activeModal.classList.remove('active');
                    document.getElementById('modal-overlay').classList.remove('active');
                }
                
                const learningPanel = document.getElementById('learning-path-panel');
                if (!learningPanel.classList.contains('hidden')) {
                    learningPanel.classList.add('hidden');
                    GraphVisualization.clearHighlight();
                }
            }
        });
    }

    /**
     * 高级搜索功能
     * @param {Object} options - 搜索选项
     * @returns {Array} 搜索结果
     */
    function advancedSearch(options) {
        const { query, types, difficulties, module, tags } = options;
        
        let results = DataLoader.getData().nodes;
        
        // 文本搜索
        if (query) {
            const lowerQuery = query.toLowerCase();
            results = results.filter(n => 
                n.name.toLowerCase().includes(lowerQuery) ||
                (n.description && n.description.toLowerCase().includes(lowerQuery)) ||
                (n.keywords && n.keywords.some(k => k.toLowerCase().includes(lowerQuery)))
            );
        }
        
        // 类型过滤
        if (types && types.length > 0) {
            results = results.filter(n => types.includes(n.type));
        }
        
        // 难度过滤
        if (difficulties && difficulties.length > 0) {
            results = results.filter(n => difficulties.includes(n.difficulty));
        }
        
        // 模块过滤
        if (module) {
            results = results.filter(n => n.module === module);
        }
        
        // 标签过滤
        if (tags && tags.length > 0) {
            results = results.filter(n => 
                n.tags && tags.some(t => n.tags.includes(t))
            );
        }
        
        return results;
    }

    /**
     * 获取搜索历史
     * @returns {Array} 搜索历史
     */
    function getSearchHistory() {
        return JSON.parse(localStorage.getItem('recentSearches') || '[]');
    }

    /**
     * 清除搜索历史
     */
    function clearSearchHistory() {
        localStorage.removeItem('recentSearches');
    }

    // 公共API
    return {
        init,
        search: performSearch,
        advancedSearch,
        getSearchHistory,
        clearSearchHistory,
        hideSuggestions
    };
})();

// 如果在Node.js环境中导出
if (typeof module !== 'undefined' && module.exports) {
    module.exports = SearchManager;
}
