#pragma once
#include <vector>
#include <string>
#include <ImGui/imgui.h>

bool editButtonsMode = false;

const char* ButtonActions[] = {
    "None",
    "KillAura",
    "MobAura",
    "NoClip",
    "FullBright",
    "Xray",
    "NoSlowDown",
    "FastFall",
    "AutoClicker",
    "Sprint",
    "AutoClickBuild",
    "AntiKnockback",
    "Step",
    "LadderFly"
};

// Вставить сюда
constexpr int ButtonActionCount =
    sizeof(ButtonActions) / sizeof(ButtonActions[0]);

const char* GetButtonActionName(int action) {
    if (action < 0 || action >= ButtonActionCount) {
        return ButtonActions[0];
    }

    return ButtonActions[action];
}

// Ниже уже существующая функция
void ExecuteButtonAction(int action) {
    switch (action) {
        case 1:  killAura = !killAura; break;
        case 2:  mobAura = !mobAura; break;
        // ...
    }
}
    
    
void ExecuteButtonAction(int action) {
    switch (action) {
        case 1:  killAura = !killAura; break;
        case 2:  mobAura = !mobAura; break;
        case 3:  noclip = !noclip; break;
        case 4:  fullBright = !fullBright; break;
        case 5:  xrayDefault = !xrayDefault; rebuildChunkGeometry = true; break;
        case 6:  noSlowDown = !noSlowDown; break;
        case 7:  fastFall = !fastFall; break;
        case 8:  autoClicker = !autoClicker; break;
        case 9:  alwaysSprinting = !alwaysSprinting; break;
        case 10: fastBridge = !fastBridge; break;
        case 11: antiKnockback = !antiKnockback; break;
        case 12: step = !step; break;
        case 13: ladderFly = !ladderFly; break;
    }
}

bool IsButtonActionOn(int action) {
    switch (action) {
        case 1:  return killAura;
        case 2:  return mobAura;
        case 3:  return noclip;
        case 4:  return fullBright;
        case 5:  return xrayDefault;
        case 6:  return noSlowDown;
        case 7:  return fastFall;
        case 8:  return autoClicker;
        case 9:  return alwaysSprinting;
        case 10: return fastBridge;
        case 11: return antiKnockback;
        case 12: return step;
        case 13: return ladderFly;
    }
    return false;
}


void RenderCustomButtons() {
    if (inMenu) {
        if (!editButtonsMode) return;
        for (size_t i = 0; i < customButtons.size(); i++) {
            CustomButton& btn = customButtons[i];
            if (!btn.enabled) continue;
            ImGui::SetNextWindowPos(ImVec2(btn.x, btn.y), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(btn.w, btn.h), ImGuiCond_Always);
            std::string windowName = "Edit##btn_" + std::to_string(i);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.70f, 0.00f, 1.00f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));
            ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            btn.x = pos.x; btn.y = pos.y; btn.w = size.x; btn.h = size.y;
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", GetButtonActionName(btn.action));
            ImGui::TextColored(ImVec4(0.70f, 0.40f, 1.0f, 0.8f), "(drag me)");
            ImGui::End();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(2);
        }
    } else {
        for (size_t i = 0; i < customButtons.size(); i++) {
            CustomButton& btn = customButtons[i];
            if (btn.action < 0 || btn.action >= ButtonActionCount) btn.action = 0;
            if (!btn.enabled) continue;
            if (btn.action == 0) continue;
            ImGui::SetNextWindowPos(ImVec2(btn.x, btn.y), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(btn.w, btn.h), ImGuiCond_Always);
            std::string windowName = "##btnGame_" + std::to_string(i);
            bool isOn = IsButtonActionOn(btn.action);
            ImVec4 bgColor, bgHover, bgActive, borderColor;
            if (isOn) {
                bgColor = ImVec4(0.55f, 0.10f, 0.85f, 1.00f);
                bgHover = ImVec4(0.65f, 0.20f, 0.95f, 1.00f);
                bgActive = ImVec4(0.45f, 0.05f, 0.75f, 1.00f);
                borderColor = ImVec4(1.00f, 0.70f, 1.00f, 1.00f);
            } else {
                bgColor = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                bgHover = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
                bgActive = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
                borderColor = ImVec4(0.70f, 0.00f, 1.00f, 1.00f);
            }
            ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgActive);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);
            std::string label = GetButtonActionName(btn.action);
            if (ImGui::Button(label.c_str(), ImVec2(btn.w, btn.h))) {
                ExecuteButtonAction(btn.action);
                onFunctionSound = true;
            }
            ImGui::End();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(5);
        }
    }
}
