/*#include "DockLayout.h"
#include "../../imgui/imgui.h"




bool DockLayout::s_FirstRun = true;

void DockLayout::Begin()
{
    // 1) 全屏无边框窗口承载 DockSpace
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags flags = 
          ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("##MainDockWindow", nullptr, flags);
    ImGui::PopStyleVar();

    // 2) 创建 DockSpace 区域
    ImGuiID dock_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dock_id, ImVec2(0,0), ImGuiDockNodeFlags_PassthruCentralNode);

    // 3) 第一次运行时设置默认布局
    if (s_FirstRun) {
        SetupDefaultLayout(dock_id, vp);
        s_FirstRun = false;
    }

    ImGui::End();
}

void DockLayout::SetupDefaultLayout(ImGuiID dockspace_id, const ImGuiViewport* viewport)
{
    // 先清空已有节点
    ImGui::DockBuilderRemoveNode(dockspace_id);
    // 添加一个新的 DockSpace 节点
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

    // 拆分： 20% 左 | 剩余 80%
    ImGuiID left_id, right_main_id;
    ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, &left_id, &right_main_id);

    // 在右半区再拆： 75% 中心 | 25% 右
    ImGuiID center_id, right_id;
    ImGui::DockBuilderSplitNode(right_main_id, ImGuiDir_Left, 0.75f, &center_id, &right_id);

    // 在左区停靠面板
    ImGui::DockBuilderDockWindow("Control Panel", left_id);
    ImGui::DockBuilderDockWindow("LOD Control",   left_id);

    // 在中区停靠渲染视图
    ImGui::DockBuilderDockWindow("Frame Buffer View", center_id);

    // 在右区停靠属性面板
    ImGui::DockBuilderDockWindow("Transform",       right_id);
    ImGui::DockBuilderDockWindow("Light Settings",  right_id);
    ImGui::DockBuilderDockWindow("Camera Control",  right_id);

    // 完成布局
    ImGui::DockBuilderFinish(dockspace_id);
}
*/
