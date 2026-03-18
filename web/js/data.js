/**
 * 数据管理模块
 * 负责加载、处理和缓存知识图谱数据
 */

const DataLoader = (function() {
    'use strict';

    // 私有变量
    let graphData = null;
    let nodeMap = new Map();
    let searchIndex = [];

    // 节点颜色配置
    const nodeColors = {
        module: '#f78166',
        topic: '#a371f7',
        concept: '#3fb950',
        beginner: '#3fb950',
        intermediate: '#d29922',
        advanced: '#f78166',
        expert: '#f85149'
    };

    // 模块配置
    const moduleConfig = {
        'L1': { name: '核心知识体系', color: '#f78166', icon: '\uf5fc' },
        'L2': { name: '形式语义与物理', color: '#a371f7', icon: '\uf0e8' },
        'L3': { name: '系统技术领域', color: '#3fb950', icon: '\uf233' },
        'L4': { name: '工业场景', color: '#58a6ff', icon: '\uf275' },
        'L5': { name: '深层结构与元物理', color: '#f0883e', icon: '\uf0ac' },
        'L6': { name: '思维表达', color: '#d29922', icon: '\uf0eb' },
        'L7': { name: '现代工具链', color: '#79c0ff', icon: '\uf7d9' }
    };

    /**
     * 加载知识图谱数据
     * @returns {Promise} 加载完成的Promise
     */
    async function load() {
        try {
            const response = await fetch('data/knowledge_graph.json');
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            graphData = await response.json();
            
            // 构建节点映射
            buildNodeMap();
            
            // 构建搜索索引
            buildSearchIndex();
            
            console.log(`数据加载完成: ${graphData.nodes.length} 节点, ${graphData.links.length} 关系`);
            return graphData;
        } catch (error) {
            console.error('数据加载失败:', error);
            // 使用内置的示例数据作为后备
            loadFallbackData();
            return graphData;
        }
    }

    /**
     * 构建节点映射表
     */
    function buildNodeMap() {
        nodeMap.clear();
        graphData.nodes.forEach(node => {
            nodeMap.set(node.id, node);
        });
    }

    /**
     * 构建搜索索引
     */
    function buildSearchIndex() {
        searchIndex = graphData.nodes.map(node => ({
            id: node.id,
            name: node.name,
            type: node.type,
            module: node.module,
            keywords: node.keywords || [],
            searchText: `${node.name} ${node.description || ''} ${(node.keywords || []).join(' ')}`.toLowerCase()
        }));
    }

    /**
     * 获取完整数据
     * @returns {Object} 图谱数据
     */
    function getData() {
        return graphData;
    }

    /**
     * 获取节点
     * @param {string} id - 节点ID
     * @returns {Object|null} 节点对象
     */
    function getNode(id) {
        return nodeMap.get(id) || null;
    }

    /**
     * 获取节点的邻居
     * @param {string} id - 节点ID
     * @returns {Array} 邻居节点数组
     */
    function getNeighbors(id) {
        const neighbors = [];
        graphData.links.forEach(link => {
            const sourceId = typeof link.source === 'object' ? link.source.id : link.source;
            const targetId = typeof link.target === 'object' ? link.target.id : link.target;
            
            if (sourceId === id) {
                neighbors.push({ node: nodeMap.get(targetId), relation: link.type });
            } else if (targetId === id) {
                neighbors.push({ node: nodeMap.get(sourceId), relation: link.type });
            }
        });
        return neighbors;
    }

    /**
     * 搜索节点
     * @param {string} query - 搜索关键词
     * @param {number} limit - 返回结果数量限制
     * @returns {Array} 匹配的节点数组
     */
    function search(query, limit = 10) {
        if (!query || query.trim().length === 0) {
            return [];
        }
        
        const lowerQuery = query.toLowerCase().trim();
        const results = [];
        
        searchIndex.forEach(item => {
            let score = 0;
            
            // 精确匹配名称得分最高
            if (item.name.toLowerCase() === lowerQuery) {
                score = 100;
            } else if (item.name.toLowerCase().startsWith(lowerQuery)) {
                score = 80;
            } else if (item.name.toLowerCase().includes(lowerQuery)) {
                score = 60;
            } else if (item.searchText.includes(lowerQuery)) {
                score = 40;
            }
            
            // 关键词匹配加分
            if (item.keywords.some(k => k.toLowerCase().includes(lowerQuery))) {
                score += 20;
            }
            
            if (score > 0) {
                results.push({ ...item, score });
            }
        });
        
        // 按得分排序并返回限制数量
        return results
            .sort((a, b) => b.score - a.score)
            .slice(0, limit)
            .map(item => nodeMap.get(item.id));
    }

    /**
     * 按类型过滤节点
     * @param {Array} types - 类型数组
     * @returns {Array} 过滤后的节点数组
     */
    function filterByType(types) {
        return graphData.nodes.filter(node => types.includes(node.type));
    }

    /**
     * 按难度过滤节点
     * @param {Array} difficulties - 难度数组
     * @returns {Array} 过滤后的节点数组
     */
    function filterByDifficulty(difficulties) {
        return graphData.nodes.filter(node => difficulties.includes(node.difficulty));
    }

    /**
     * 按模块获取节点
     * @param {string} moduleId - 模块ID
     * @returns {Array} 节点数组
     */
    function getNodesByModule(moduleId) {
        return graphData.nodes.filter(node => node.module === moduleId);
    }

    /**
     * 获取模块列表
     * @returns {Array} 模块节点数组
     */
    function getModules() {
        return graphData.nodes.filter(node => node.type === 'module');
    }

    /**
     * 计算最短路径（使用Dijkstra算法）
     * @param {string} startId - 起始节点ID
     * @param {string} endId - 目标节点ID
     * @returns {Array} 路径节点数组
     */
    function findShortestPath(startId, endId) {
        const distances = new Map();
        const previous = new Map();
        const unvisited = new Set();
        
        // 初始化
        graphData.nodes.forEach(node => {
            distances.set(node.id, node.id === startId ? 0 : Infinity);
            unvisited.add(node.id);
        });
        
        while (unvisited.size > 0) {
            // 找到距离最小的未访问节点
            let currentId = null;
            let minDistance = Infinity;
            
            unvisited.forEach(id => {
                const dist = distances.get(id);
                if (dist < minDistance) {
                    minDistance = dist;
                    currentId = id;
                }
            });
            
            if (currentId === null || minDistance === Infinity) break;
            if (currentId === endId) break;
            
            unvisited.delete(currentId);
            
            // 更新邻居距离
            const neighbors = getNeighbors(currentId);
            neighbors.forEach(({ node }) => {
                if (unvisited.has(node.id)) {
                    const newDist = distances.get(currentId) + 1;
                    if (newDist < distances.get(node.id)) {
                        distances.set(node.id, newDist);
                        previous.set(node.id, currentId);
                    }
                }
            });
        }
        
        // 重建路径
        const path = [];
        let currentId = endId;
        
        while (currentId !== undefined) {
            path.unshift(nodeMap.get(currentId));
            currentId = previous.get(currentId);
        }
        
        return path[0]?.id === startId ? path : [];
    }

    /**
     * 获取学习路径（考虑难度）
     * @param {string} targetId - 目标节点ID
     * @param {string} pathType - 路径类型: shortest|beginner|comprehensive
     * @returns {Array} 学习路径节点数组
     */
    function getLearningPath(targetId, pathType = 'shortest') {
        const target = nodeMap.get(targetId);
        if (!target) return [];
        
        // 查找所有前置知识
        const prerequisites = new Set();
        const visited = new Set();
        
        function collectPrerequisites(nodeId) {
            if (visited.has(nodeId)) return;
            visited.add(nodeId);
            
            graphData.links.forEach(link => {
                const sourceId = typeof link.source === 'object' ? link.source.id : link.source;
                const targetId = typeof link.target === 'object' ? link.target.id : link.target;
                
                if (targetId === nodeId && link.type === 'prerequisite') {
                    prerequisites.add(sourceId);
                    collectPrerequisites(sourceId);
                }
            });
        }
        
        collectPrerequisites(targetId);
        
        // 根据路径类型排序
        const pathNodes = Array.from(prerequisites).map(id => nodeMap.get(id));
        
        if (pathType === 'beginner') {
            // 初级路线：按难度从低到高
            pathNodes.sort((a, b) => {
                const diffOrder = { beginner: 0, intermediate: 1, advanced: 2, expert: 3 };
                return diffOrder[a.difficulty] - diffOrder[b.difficulty];
            });
        } else if (pathType === 'comprehensive') {
            // 全面路线：按模块分组
            const moduleOrder = ['L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7'];
            pathNodes.sort((a, b) => {
                const moduleDiff = moduleOrder.indexOf(a.module) - moduleOrder.indexOf(b.module);
                if (moduleDiff !== 0) return moduleDiff;
                const diffOrder = { beginner: 0, intermediate: 1, advanced: 2, expert: 3 };
                return diffOrder[a.difficulty] - diffOrder[b.difficulty];
            });
        }
        
        // 添加目标节点
        pathNodes.push(target);
        
        return pathNodes;
    }

    /**
     * 获取节点颜色
     * @param {Object} node - 节点对象
     * @returns {string} 颜色值
     */
    function getNodeColor(node) {
        if (node.color) return node.color;
        if (node.type === 'module' && moduleConfig[node.module]) {
            return moduleConfig[node.module].color;
        }
        return nodeColors[node.type] || nodeColors[node.difficulty] || '#7d8590';
    }

    /**
     * 使用后备数据（当JSON加载失败时）
     */
    function loadFallbackData() {
        console.log('使用内置示例数据');
        
        // 创建基本的示例数据
        const nodes = [
            // 模块节点
            { id: 'L1', name: '核心知识体系', type: 'module', module: 'L1', difficulty: 'beginner', description: 'C语言核心语法、指针、内存管理、标准库等基础内容' },
            { id: 'L2', name: '形式语义与物理', type: 'module', module: 'L2', difficulty: 'advanced', description: '数字逻辑、ISA架构、编译映射、形式化方法' },
            { id: 'L3', name: '系统技术领域', type: 'module', module: 'L3', difficulty: 'advanced', description: '虚拟机、编解码、安全启动、分布式系统等技术' },
            { id: 'L4', name: '工业场景', type: 'module', module: 'L4', difficulty: 'expert', description: '汽车ABS、5G基带、量子计算等工业应用' },
            { id: 'L5', name: '深层结构与元物理', type: 'module', module: 'L5', difficulty: 'expert', description: '形式语义、计算理论、图灵机、验证方法' },
            { id: 'L6', name: '思维表达', type: 'module', module: 'L6', difficulty: 'intermediate', description: '决策树、思维导图、学习路径、知识图谱' },
            { id: 'L7', name: '现代工具链', type: 'module', module: 'L7', difficulty: 'intermediate', description: 'IDE、构建系统、CI/CD、代码质量工具' }
        ];
        
        // 添加一些示例主题和概念节点
        const sampleTopics = [
            { id: 'L1-1', name: '语法基础', type: 'topic', module: 'L1', difficulty: 'beginner', description: 'C语言基本语法元素' },
            { id: 'L1-2', name: '指针深度', type: 'topic', module: 'L1', difficulty: 'intermediate', description: 'C语言指针详解' },
            { id: 'L1-3', name: '内存管理', type: 'topic', module: 'L1', difficulty: 'intermediate', description: '动态内存分配与管理' },
            { id: 'L2-1', name: '数字逻辑', type: 'topic', module: 'L2', difficulty: 'advanced', description: '布尔代数与门电路' },
            { id: 'L2-2', name: 'ISA架构', type: 'topic', module: 'L2', difficulty: 'advanced', description: '指令集架构' },
            { id: 'L3-1', name: '并发编程', type: 'topic', module: 'L3', difficulty: 'advanced', description: '多线程与同步' },
            { id: 'L4-1', name: '嵌入式系统', type: 'topic', module: 'L4', difficulty: 'expert', description: '嵌入式开发' },
            { id: 'L5-1', name: '图灵机', type: 'topic', module: 'L5', difficulty: 'expert', description: '计算理论基础' },
            { id: 'L6-1', name: '决策树', type: 'topic', module: 'L6', difficulty: 'intermediate', description: '问题诊断决策树' },
            { id: 'L7-1', name: 'CMake', type: 'topic', module: 'L7', difficulty: 'intermediate', description: '现代CMake构建系统' }
        ];
        
        nodes.push(...sampleTopics);
        
        // 创建关系
        const links = [
            { source: 'L1', target: 'L1-1', type: 'contains' },
            { source: 'L1', target: 'L1-2', type: 'contains' },
            { source: 'L1', target: 'L1-3', type: 'contains' },
            { source: 'L2', target: 'L2-1', type: 'contains' },
            { source: 'L2', target: 'L2-2', type: 'contains' },
            { source: 'L3', target: 'L3-1', type: 'contains' },
            { source: 'L4', target: 'L4-1', type: 'contains' },
            { source: 'L5', target: 'L5-1', type: 'contains' },
            { source: 'L6', target: 'L6-1', type: 'contains' },
            { source: 'L7', target: 'L7-1', type: 'contains' },
            { source: 'L1-1', target: 'L1-2', type: 'prerequisite' },
            { source: 'L1-2', target: 'L1-3', type: 'prerequisite' },
            { source: 'L1', target: 'L2', type: 'prerequisite' },
            { source: 'L2', target: 'L3', type: 'prerequisite' },
            { source: 'L3', target: 'L4', type: 'prerequisite' }
        ];
        
        graphData = { nodes, links, metadata: { version: '1.0', fallback: true } };
        buildNodeMap();
        buildSearchIndex();
    }

    // 公共API
    return {
        load,
        getData,
        getNode,
        getNeighbors,
        search,
        filterByType,
        filterByDifficulty,
        getNodesByModule,
        getModules,
        findShortestPath,
        getLearningPath,
        getNodeColor,
        get moduleConfig() { return moduleConfig; }
    };
})();

// 如果在Node.js环境中导出
if (typeof module !== 'undefined' && module.exports) {
    module.exports = DataLoader;
}
