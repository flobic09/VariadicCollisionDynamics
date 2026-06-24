#include "menu.hpp"

namespace UI {

    void __stdcall RenderDynamicsMenu();
    void __stdcall RenderToolsMenu();
    void __stdcall RenderPoseFixesMenu();

    void Register()
    {
        if (!SKSEMenuFramework::IsInstalled()) {
            return;
        }

        SKSEMenuFramework::SetSection("Variadic Collisions");
        SKSEMenuFramework::AddSectionItem("Dynamics", RenderDynamicsMenu);
        SKSEMenuFramework::AddSectionItem("Tools", RenderToolsMenu);
        SKSEMenuFramework::AddSectionItem("Pose Fixes", RenderPoseFixesMenu);
    }

}
