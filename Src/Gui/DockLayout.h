#pragma once
#include "../../imgui/imgui.h"

/// DockLayout 负责全局 DockSpace 的创建和初次布局
class DockLayout {
public:
    /// 在主循环每帧开始前调用，创建/显示全屏 DockSpace
    static void Begin();

private:
    /// 第一次运行时构建默认停靠布局
    static void SetupDefaultLayout(ImGuiID dockspace_id, const ImGuiViewport* viewport);

    /// 标记是否已完成第一次布局
    static bool s_FirstRun;
};
