/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 899
 * Language: c
 * Block ID: c498029f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Chrome站点隔离实现

class SiteIsolationPolicy {
public:
    // 判断是否需要为站点创建独立进程
    static bool ShouldIsolateSite(const GURL& url) {
        // 检查是否为跨站点导航
        if (IsCrossSiteNavigation(url)) {
            return true;
        }

        // 检查是否为高风险站点
        if (IsHighRiskSite(url)) {
            return true;
        }

        // 检查站点是否启用了隔离策略
        return GetIsolationPolicyForSite(url) == ISOLATION_REQUIRED;
    }

    // 进程隔离检查
    static bool CanAccessDataInProcess(const GURL& site_url,
                                        RenderProcessHost* process) {
        // 验证进程是否有权访问该站点的数据
        return process->IsLockedToSite(site_url);
    }
};

// 渲染进程锁定到特定站点
void RenderProcessHostImpl::LockToSite(const GURL& site_url) {
    // 设置进程只能访问该站点的cookie、storage等
    site_isolation_policy_->LockProcessToSite(this, site_url);

    // 通知子进程锁定状态
    Send(new ChildProcessMsg_SetSiteLock(site_url));
}
