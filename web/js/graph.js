/**
 * 图谱可视化模块
 * 使用D3.js渲染交互式知识图谱
 */

const GraphVisualization = (function() {
    'use strict';

    // D3 相关变量
    let svg, g, simulation, link, node, labels;
    let width, height;
    let transform = d3.zoomIdentity;
    
    // 状态变量
    let selectedNodeId = null;
    let highlightedNodes = new Set();
    let highlightedLinks = new Set();
    let currentData = { nodes: [], links: [] };
    
    // 配置选项
    const config = {
        nodeRadius: {
            module: 35,
            topic: 25,
            concept: 15
        },
        colors: {
            module: '#f78166',
            topic: '#a371f7',
            concept: '#3fb950',
            link: '#30363d',
            linkHighlight: '#58a6ff',
            text: '#e6edf3'
        },
        forces: {
            charge: -300,
            linkDistance: 100,
            collide: 50,
            center: 0.05
        }
    };

    /**
     * 初始化图谱
     */
    function init() {
        const container = document.getElementById('graph-svg');
        width = container.clientWidth;
        height = container.clientHeight;

        // 创建SVG
        svg = d3.select('#graph-svg')
            .append('svg')
            .attr('width', width)
            .attr('height', height)
            .attr('viewBox', [0, 0, width, height]);

        // 添加缩放行为
        const zoom = d3.zoom()
            .scaleExtent([0.1, 4])
            .on('zoom', (event) => {
                transform = event.transform;
                g.attr('transform', transform);
            });

        svg.call(zoom);

        // 创建主组
        g = svg.append('g');

        // 创建箭头标记
        createArrowMarkers();

        // 加载数据
        loadData();

        // 绑定控制按钮事件
        bindControlEvents();

        // 窗口大小改变时重新调整
        window.addEventListener('resize', handleResize);
    }

    /**
     * 创建箭头标记
     */
    function createArrowMarkers() {
        const defs = svg.append('defs');
        
        // 前置知识箭头
        defs.append('marker')
            .attr('id', 'arrow-prerequisite')
            .attr('viewBox', '0 -5 10 10')
            .attr('refX', 25)
            .attr('refY', 0)
            .attr('markerWidth', 6)
            .attr('markerHeight', 6)
            .attr('orient', 'auto')
            .append('path')
            .attr('d', 'M0,-5L10,0L0,5')
            .attr('fill', '#58a6ff');

        // 相关概念箭头
        defs.append('marker')
            .attr('id', 'arrow-related')
            .attr('viewBox', '0 -5 10 10')
            .attr('refX', 25)
            .attr('refY', 0)
            .attr('markerWidth', 6)
            .attr('markerHeight', 6)
            .attr('orient', 'auto')
            .append('path')
            .attr('d', 'M0,-5L10,0L0,5')
            .attr('fill', '#7d8590');
    }

    /**
     * 加载数据
     */
    function loadData() {
        const data = DataLoader.getData();
        currentData = {
            nodes: data.nodes.map(n => ({ ...n })),
            links: data.links.map(l => ({ ...l }))
        };

        render();
    }

    /**
     * 渲染图谱
     */
    function render() {
        // 清除现有内容
        g.selectAll('*').remove();

        // 创建力导向模拟
        simulation = d3.forceSimulation(currentData.nodes)
            .force('link', d3.forceLink(currentData.links)
                .id(d => d.id)
                .distance(d => {
                    if (d.type === 'prerequisite') return 120;
                    if (d.type === 'contains') return 80;
                    return 100;
                }))
            .force('charge', d3.forceManyBody()
                .strength(d => {
                    if (d.type === 'module') return -500;
                    if (d.type === 'topic') return -300;
                    return -150;
                }))
            .force('center', d3.forceCenter(width / 2, height / 2))
            .force('collide', d3.forceCollide()
                .radius(d => getNodeRadius(d) + 10));

        // 绘制连线
        link = g.append('g')
            .attr('class', 'links')
            .selectAll('line')
            .data(currentData.links)
            .join('line')
            .attr('class', d => `link ${d.type}`)
            .attr('stroke-width', d => d.type === 'prerequisite' ? 2 : 1.5)
            .attr('marker-end', d => {
                if (d.type === 'prerequisite') return 'url(#arrow-prerequisite)';
                if (d.type === 'related') return 'url(#arrow-related)';
                return null;
            });

        // 绘制节点
        node = g.append('g')
            .attr('class', 'nodes')
            .selectAll('g')
            .data(currentData.nodes)
            .join('g')
            .attr('class', d => `node ${d.type}`)
            .attr('data-id', d => d.id)
            .call(d3.drag()
                .on('start', dragstarted)
                .on('drag', dragged)
                .on('end', dragended));

        // 添加节点圆形
        node.append('circle')
            .attr('class', 'node-circle')
            .attr('r', d => getNodeRadius(d))
            .attr('fill', d => DataLoader.getNodeColor(d))
            .attr('stroke', d => d3.rgb(DataLoader.getNodeColor(d)).brighter(0.5))
            .attr('stroke-width', 2);

        // 添加节点图标/文字
        node.append('text')
            .attr('class', 'node-icon')
            .attr('text-anchor', 'middle')
            .attr('dy', d => d.type === 'module' ? '0.35em' : '-0.2em')
            .attr('font-family', 'Font Awesome 6 Free')
            .attr('font-weight', '900')
            .attr('font-size', d => d.type === 'module' ? '16px' : '12px')
            .attr('fill', 'white')
            .text(d => getNodeIcon(d));

        // 添加节点标签
        labels = g.append('g')
            .attr('class', 'labels')
            .selectAll('text')
            .data(currentData.nodes)
            .join('text')
            .attr('class', 'node-label')
            .attr('text-anchor', 'middle')
            .attr('dy', d => getNodeRadius(d) + 15)
            .text(d => d.name)
            .style('font-size', d => d.type === 'module' ? '14px' : '12px')
            .style('font-weight', d => d.type === 'module' ? '600' : '400')
            .style('opacity', d => d.type === 'concept' ? 0.8 : 1);

        // 绑定节点事件
        node.on('click', handleNodeClick)
            .on('dblclick', handleNodeDoubleClick)
            .on('mouseenter', handleNodeMouseEnter)
            .on('mouseleave', handleNodeMouseLeave);

        // 开始模拟
        simulation.on('tick', ticked);
    }

    /**
     * 每帧更新位置
     */
    function ticked() {
        link
            .attr('x1', d => d.source.x)
            .attr('y1', d => d.source.y)
            .attr('x2', d => d.target.x)
            .attr('y2', d => d.target.y);

        node
            .attr('transform', d => `translate(${d.x},${d.y})`);

        labels
            .attr('x', d => d.x)
            .attr('y', d => d.y);
    }

    /**
     * 获取节点半径
     */
    function getNodeRadius(d) {
        return config.nodeRadius[d.type] || 15;
    }

    /**
     * 获取节点图标
     */
    function getNodeIcon(d) {
        const icons = {
            module: '\uf5fc',  // project-diagram
            topic: '\uf02d',   // book
            concept: '\uf0eb'  // lightbulb
        };
        return icons[d.type] || '\uf111';
    }

    /**
     * 拖拽开始
     */
    function dragstarted(event, d) {
        if (!event.active) simulation.alphaTarget(0.3).restart();
        d.fx = d.x;
        d.fy = d.y;
    }

    /**
     * 拖拽中
     */
    function dragged(event, d) {
        d.fx = event.x;
        d.fy = event.y;
    }

    /**
     * 拖拽结束
     */
    function dragended(event, d) {
        if (!event.active) simulation.alphaTarget(0);
        d.fx = null;
        d.fy = null;
    }

    /**
     * 处理节点点击
     */
    function handleNodeClick(event, d) {
        event.stopPropagation();
        
        // 更新选中状态
        selectedNodeId = d.id;
        
        // 更新视觉状态
        node.classed('selected', n => n.id === d.id);
        
        // 显示详情弹窗
        showNodeModal(d);
        
        // 高亮相关节点和连线
        highlightConnections(d);
    }

    /**
     * 处理节点双击
     */
    function handleNodeDoubleClick(event, d) {
        event.stopPropagation();
        
        // 展开/折叠子节点
        toggleChildren(d);
    }

    /**
     * 处理鼠标进入
     */
    function handleNodeMouseEnter(event, d) {
        // 显示工具提示
        showTooltip(event, d);
    }

    /**
     * 处理鼠标离开
     */
    function handleNodeMouseLeave(event, d) {
        // 隐藏工具提示
        hideTooltip();
    }

    /**
     * 显示工具提示
     */
    function showTooltip(event, d) {
        // 移除现有工具提示
        d3.selectAll('.tooltip').remove();
        
        const tooltip = d3.select('body')
            .append('div')
            .attr('class', 'tooltip')
            .style('position', 'absolute')
            .style('background', 'var(--bg-secondary)')
            .style('border', '1px solid var(--border-color)')
            .style('border-radius', '6px')
            .style('padding', '10px')
            .style('font-size', '12px')
            .style('pointer-events', 'none')
            .style('z-index', '3000')
            .style('box-shadow', 'var(--shadow-md)');
        
        tooltip.html(`
            <strong>${d.name}</strong><br>
            <span style="color: var(--text-secondary)">类型: ${d.type}</span><br>
            <span style="color: var(--text-secondary)">难度: ${d.difficulty || 'N/A'}</span>
        `);
        
        tooltip
            .style('left', (event.pageX + 10) + 'px')
            .style('top', (event.pageY - 10) + 'px');
    }

    /**
     * 隐藏工具提示
     */
    function hideTooltip() {
        d3.selectAll('.tooltip').remove();
    }

    /**
     * 高亮连接
     */
    function highlightConnections(d) {
        // 获取相关节点和连线
        const neighbors = new Set();
        const connectedLinks = new Set();
        
        currentData.links.forEach(l => {
            const sourceId = typeof l.source === 'object' ? l.source.id : l.source;
            const targetId = typeof l.target === 'object' ? l.target.id : l.target;
            
            if (sourceId === d.id) {
                neighbors.add(targetId);
                connectedLinks.add(l);
            } else if (targetId === d.id) {
                neighbors.add(sourceId);
                connectedLinks.add(l);
            }
        });
        
        neighbors.add(d.id);
        
        // 更新视觉状态
        node.style('opacity', n => neighbors.has(n.id) ? 1 : 0.3);
        link.style('opacity', l => connectedLinks.has(l) ? 1 : 0.1);
        labels.style('opacity', n => neighbors.has(n.id) ? 1 : 0.3);
    }

    /**
     * 清除高亮
     */
    function clearHighlight() {
        node.style('opacity', 1);
        link.style('opacity', 1);
        labels.style('opacity', d => d.type === 'concept' ? 0.8 : 1);
        selectedNodeId = null;
        node.classed('selected', false);
    }

    /**
     * 展开/折叠子节点
     */
    function toggleChildren(d) {
        // 查找子节点
        const children = [];
        currentData.links.forEach(l => {
            const sourceId = typeof l.source === 'object' ? l.source.id : l.source;
            if (sourceId === d.id && l.type === 'contains') {
                const targetId = typeof l.target === 'object' ? l.target.id : l.target;
                const child = currentData.nodes.find(n => n.id === targetId);
                if (child) children.push(child);
            }
        });
        
        if (children.length === 0) return;
        
        // 切换可见性
        const allHidden = children.every(c => c.hidden);
        children.forEach(child => {
            child.hidden = !allHidden;
        });
        
        // 重新渲染
        render();
    }

    /**
     * 显示节点详情弹窗
     */
    function showNodeModal(d) {
        const modal = document.getElementById('node-modal');
        const overlay = document.getElementById('modal-overlay');
        
        // 填充数据
        document.getElementById('modal-title').textContent = d.name;
        document.getElementById('modal-type').textContent = d.type;
        document.getElementById('modal-difficulty').textContent = d.difficulty || 'N/A';
        document.getElementById('modal-time').textContent = d.estimatedTime ? `${d.estimatedTime}小时` : 'N/A';
        document.getElementById('modal-module').textContent = d.module || 'N/A';
        document.getElementById('modal-description').textContent = d.description || '暂无描述';
        
        // 前置知识列表
        const prereqList = document.getElementById('modal-prerequisites');
        prereqList.innerHTML = '';
        
        const prerequisites = DataLoader.getNeighbors(d.id)
            .filter(n => n.relation === 'prerequisite')
            .map(n => n.node);
        
        if (prerequisites.length === 0) {
            prereqList.innerHTML = '<li>无前置知识</li>';
        } else {
            prerequisites.forEach(p => {
                const li = document.createElement('li');
                li.innerHTML = `<a href="#" data-node-id="${p.id}">${p.name}</a>`;
                li.querySelector('a').addEventListener('click', (e) => {
                    e.preventDefault();
                    focusNode(p.id);
                    closeModal();
                });
                prereqList.appendChild(li);
            });
        }
        
        // 相关资源
        const resourceList = document.getElementById('modal-resources');
        resourceList.innerHTML = '';
        
        if (d.resources && d.resources.length > 0) {
            d.resources.forEach(r => {
                const li = document.createElement('li');
                li.innerHTML = `<a href="${r.url}" target="_blank">${r.name}</a>`;
                resourceList.appendChild(li);
            });
        } else {
            resourceList.innerHTML = '<li>暂无资源链接</li>';
        }
        
        // 显示弹窗
        modal.classList.add('active');
        overlay.classList.add('active');
        
        // 绑定关闭事件
        document.getElementById('modal-close').onclick = closeModal;
        document.getElementById('view-details').onclick = () => {
            if (d.filePath) {
                window.open(d.filePath, '_blank');
            }
        };
        document.getElementById('plan-learning').onclick = () => {
            closeModal();
            openLearningPathPanel(d.id);
        };
    }

    /**
     * 关闭弹窗
     */
    function closeModal() {
        document.querySelectorAll('.modal.active').forEach(m => m.classList.remove('active'));
        document.getElementById('modal-overlay').classList.remove('active');
    }

    /**
     * 打开学习路径面板
     */
    function openLearningPathPanel(nodeId) {
        const panel = document.getElementById('learning-path-panel');
        panel.classList.remove('hidden');
        
        // 设置目标节点
        const select = document.getElementById('target-node');
        select.value = nodeId;
    }

    /**
     * 聚焦节点
     */
    function focusNode(nodeId) {
        const targetNode = currentData.nodes.find(n => n.id === nodeId);
        if (!targetNode) return;
        
        // 计算缩放和位置
        const scale = 1.5;
        const x = -targetNode.x * scale + width / 2;
        const y = -targetNode.y * scale + height / 2;
        
        // 平滑过渡
        svg.transition()
            .duration(750)
            .call(
                d3.zoom().transform,
                d3.zoomIdentity.translate(x, y).scale(scale)
            );
        
        // 高亮节点
        handleNodeClick({ stopPropagation: () => {} }, targetNode);
    }

    /**
     * 缩放控制
     */
    function zoomIn() {
        svg.transition().call(
            d3.zoom().scaleBy,
            1.3
        );
    }

    function zoomOut() {
        svg.transition().call(
            d3.zoom().scaleBy,
            1 / 1.3
        );
    }

    function zoomFit() {
        const bounds = g.node().getBBox();
        const fullWidth = width;
        const fullHeight = height;
        const widthRatio = fullWidth / bounds.width;
        const heightRatio = fullHeight / bounds.height;
        const minRatio = Math.min(widthRatio, heightRatio) * 0.9;
        
        const transform = d3.zoomIdentity
            .translate(
                fullWidth / 2 - (bounds.x + bounds.width / 2) * minRatio,
                fullHeight / 2 - (bounds.y + bounds.height / 2) * minRatio
            )
            .scale(minRatio);
        
        svg.transition().call(
            d3.zoom().transform,
            transform
        );
    }

    function resetView() {
        svg.transition().call(
            d3.zoom().transform,
            d3.zoomIdentity
        );
        clearHighlight();
    }

    /**
     * 处理窗口大小改变
     */
    function handleResize() {
        const container = document.getElementById('graph-svg');
        width = container.clientWidth;
        height = container.clientHeight;
        
        svg.attr('width', width).attr('height', height);
        simulation.force('center', d3.forceCenter(width / 2, height / 2));
        simulation.alpha(0.3).restart();
    }

    /**
     * 绑定控制按钮事件
     */
    function bindControlEvents() {
        document.getElementById('zoom-in').addEventListener('click', zoomIn);
        document.getElementById('zoom-out').addEventListener('click', zoomOut);
        document.getElementById('zoom-fit').addEventListener('click', zoomFit);
        document.getElementById('reset-view').addEventListener('click', resetView);
        
        // 点击背景清除高亮
        svg.on('click', () => {
            clearHighlight();
        });
    }

    /**
     * 过滤节点
     */
    function filterNodes(types, difficulties) {
        let filteredNodes = [...DataLoader.getData().nodes];
        
        if (types && types.length > 0) {
            filteredNodes = filteredNodes.filter(n => types.includes(n.type));
        }
        
        if (difficulties && difficulties.length > 0) {
            filteredNodes = filteredNodes.filter(n => difficulties.includes(n.difficulty));
        }
        
        const nodeIds = new Set(filteredNodes.map(n => n.id));
        const filteredLinks = DataLoader.getData().links.filter(
            l => nodeIds.has(typeof l.source === 'object' ? l.source.id : l.source) &&
                 nodeIds.has(typeof l.target === 'object' ? l.target.id : l.target)
        );
        
        currentData = {
            nodes: filteredNodes,
            links: filteredLinks
        };
        
        render();
    }

    /**
     * 高亮路径
     */
    function highlightPath(pathNodeIds) {
        const pathSet = new Set(pathNodeIds);
        
        node.classed('highlighted', d => pathSet.has(d.id));
        
        // 高亮路径上的连线
        link.style('stroke', l => {
            const sourceId = typeof l.source === 'object' ? l.source.id : l.source;
            const targetId = typeof l.target === 'object' ? l.target.id : l.target;
            return (pathSet.has(sourceId) && pathSet.has(targetId)) ? 
                config.colors.linkHighlight : config.colors.link;
        }).style('stroke-width', l => {
            const sourceId = typeof l.source === 'object' ? l.source.id : l.source;
            const targetId = typeof l.target === 'object' ? l.target.id : l.target;
            return (pathSet.has(sourceId) && pathSet.has(targetId)) ? 3 : 1.5;
        });
        
        // 降低其他节点透明度
        node.style('opacity', d => pathSet.has(d.id) ? 1 : 0.2);
        link.style('opacity', l => {
            const sourceId = typeof l.source === 'object' ? l.source.id : l.source;
            const targetId = typeof l.target === 'object' ? l.target.id : l.target;
            return (pathSet.has(sourceId) && pathSet.has(targetId)) ? 1 : 0.1;
        });
        labels.style('opacity', d => pathSet.has(d.id) ? 1 : 0.2);
    }

    // 公共API
    return {
        init,
        focusNode,
        zoomIn,
        zoomOut,
        zoomFit,
        resetView,
        filterNodes,
        highlightPath,
        clearHighlight,
        get selectedNodeId() { return selectedNodeId; }
    };
})();

// 如果在Node.js环境中导出
if (typeof module !== 'undefined' && module.exports) {
    module.exports = GraphVisualization;
}
