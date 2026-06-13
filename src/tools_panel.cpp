#include "tools_panel.hpp"

#include <CLibUtilsQTR/DrawDebug.hpp>
#include <iterator>

namespace UI {

    void ClearDrawLines()
    {
        auto api = DebugAPI_IMPL::DebugAPI::GetSingleton();
        api->LinesToDraw.clear();
        api->Update();
    }

    void ApplyLogLevel(const int& a_level)
    {
        const auto level = static_cast<spdlog::level::level_enum>(Settings::NormalizeLogLevel(a_level));
        spdlog::set_level(level);
        spdlog::flush_on(level);
    }

    void RenderReadout()
    {
        const auto& state = Dynamics::GetPresetState();
        const auto& preview = Dynamics::GetPreviewState();
        GUI::Text("Current State: %s", state.stateName);
        GUI::Text("Current Preset: %s", state.applied ? VCD::PresetName(state.current) : "unknown");

        if (preview.active) {
            GUI::Text("Preview Preset: %s", VCD::PresetName(preview.preset));
        }
    }

    void RenderVisualization()
    {
        auto& settings = Settings::GetSettings();

        if (GUI::Checkbox("Draw Player Collision", &settings.drawCollision)) {
            ClearDrawLines();
        }

        GUI::Checkbox("Auto Draw While Editing", &settings.autoDrawPreview);
        Tooltip("Temporarily enables drawing when preset sliders are changed, then restores the previous draw setting when the editor closes.");

        GUI::SetNextItemWidth(260.0F);
        GUI::ColorEdit4("Line Color", settings.drawColor.data(), GUI::ImGuiColorEditFlags_DisplayRGB | GUI::ImGuiColorEditFlags_AlphaBar);

        GUI::SetNextItemWidth(260.0F);
        GUI::SliderFloat("Line Thickness", &settings.drawLineThickness, 0.25F, 5.0F);

        GUI::SetNextItemWidth(260.0F);
        GUI::SliderFloat("Preview Restore Delay", &settings.previewRestoreDelay, 0.0F, 10.0F);
        Tooltip("How long an inactive preset preview remains after closing the editor.");
    }

    void RenderLogging()
    {
        auto& settings = Settings::GetSettings();
        auto logLevel = Settings::NormalizeLogLevel(settings.logLevel);

        GUI::SetNextItemWidth(180.0F);
        if (GUI::Combo("Log Level", &logLevel, kLogLevels, static_cast<int>(std::size(kLogLevels)))) {
            settings.logLevel = logLevel;
            ApplyLogLevel(settings.logLevel);
        }
    }

    void __stdcall RenderToolsMenu()
    {
        constexpr auto defaultOpen = GUI::ImGuiTreeNodeFlags_DefaultOpen;

        if (GUI::CollapsingHeader("Current Dynamics", defaultOpen)) {
            RenderReadout();
        }

        GUI::Spacing();

        if (GUI::CollapsingHeader("Bump Visualization", defaultOpen)) {
            RenderVisualization();
        }

        GUI::Spacing();

        if (GUI::CollapsingHeader("Logging", defaultOpen)) {
            RenderLogging();
        }

        GUI::Spacing();
        if (CTAButton("Save Tools", Settings::IsToolsDirty())) {
            Settings::SaveTools();
        }
        Tooltip("Save Tools panel settings to disk.");
    }

}
