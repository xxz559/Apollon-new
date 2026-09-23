#pragma once
#include <ImGui/imgui.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include "menu_utils.h"

// ===== Цвета =====
ImU32 HUD_BG      = IM_COL32(0, 0, 0, 180);
ImU32 HUD_BORDER  = IM_COL32(80, 80, 80, 255);
ImU32 HUD_HEADER  = IM_COL32(180, 0, 255, 255);   // фиолетовый акцент
ImU32 HUD_TEXT    = IM_COL32(255, 255, 255, 255);
ImU32 HUD_TEXT_DIM = IM_COL32(180, 180, 180, 255);

// ===== Helper: рисуем панель с заголовком =====
void DrawHUDPanelEx(ImDrawList* dl, ImVec2 pos, ImVec2 size, const char* title, float scale) {
    float pad = 6.0f * scale;
    float headerH = title ? (20.0f * scale) : 0.0f;
    
    // Фон
    dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), HUD_BG, 4.0f);
    
    // Рамка
    dl->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), HUD_BORDER, 4.0f, 0, 1.5f * scale);
    
    // Заголовок (полоса фиолетовая)
    if (title) {
        dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + headerH), HUD_HEADER, 4.0f, ImDrawFlags_RoundCornersTop);
        dl->AddText(ImVec2(pos.x + pad, pos.y + pad / 2), HUD_TEXT, title);
    }
}

// ===== Watermark =====
void RenderWatermark(ImDrawList* dl, float scale) {
    if (!hudWatermark) return;
    
    ImGuiIO& io = ImGui::GetIO();
    float pad = 6.0f * scale;
    float lineH = 18.0f * scale;
    
    // Формируем строки
    std::vector<std::string> lines;
    lines.push_back("Apollon Client v4.62");
    
    std::stringstream ss;
    ss << "FPS: " << (int)io.Framerate;
    lines.push_back(ss.str());
    
    // Время
    time_t now = time(0);
    struct tm* t = localtime(&now);
    char timeBuf[16];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", t);
    lines.push_back(std::string("Time: ") + timeBuf);
    
    // Размеры панели
    float maxW = 0;
    for (auto& s : lines) {
        float w = ImGui::CalcTextSize(s.c_str()).x;
        if (w > maxW) maxW = w;
    }
    
    float panelW = maxW + pad * 2;
    float panelH = lines.size() * lineH + pad * 2;
    
    ImVec2 pos(hudWatermarkX, hudWatermarkY);
    DrawHUDPanelEx(dl, pos, ImVec2(panelW, panelH), nullptr, scale);
    
    // Рисуем строки
    float y = pos.y + pad;
    for (auto& s : lines) {
        dl->AddText(ImVec2(pos.x + pad, y), HUD_TEXT, s.c_str());
        y += lineH;
    }
}

// ===== Coordinates =====
void RenderCoords(ImDrawList* dl, float scale) {
    if (!hudCoords) return;
    if (localPlayer == NULL) return;
    
    vec3_t* pos = localPlayer->getPosition();
    if (pos == NULL) return;
    
    float pad = 6.0f * scale;
    float lineH = 18.0f * scale;
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0);
    ss << "X: " << pos->x << "  Y: " << pos->y << "  Z: " << pos->z;
    std::string text = ss.str();
    
    float textW = ImGui::CalcTextSize(text.c_str()).x;
    float panelW = textW + pad * 2;
    float panelH = lineH + pad * 2;
    
    // Если hudCoordsX == 0, значит автоматом справа
    float x = (hudCoordsX == 0.0f) ? (screenWidth - panelW - 20.0f * scale) : hudCoordsX;
    
    ImVec2 p(x, hudCoordsY);
    DrawHUDPanelEx(dl, p, ImVec2(panelW, panelH), nullptr, scale);
    dl->AddText(ImVec2(p.x + pad, p.y + pad), HUD_TEXT, text.c_str());
}

// ===== Hotkeys =====
void RenderHotkeys(ImDrawList* dl, float scale) {
    if (!hudHotkeys) return;
    
    float pad = 6.0f * scale;
    float lineH = 18.0f * scale;
    
    // Список хоткеев — можно расширять
    struct HKEntry { const char* name; const char* key; };
    std::vector<HKEntry> hotkeys = {
        { "KillAura", "R" },
        { "NoClip", "N" },
        { "Xray", "X" },
        { "FullBright", "B" },
        { "FastFall", "F" },
    };
    
    // Считаем размеры
    float maxNameW = 0, maxKeyW = 0;
    for (auto& hk : hotkeys) {
        float w1 = ImGui::CalcTextSize(hk.name).x;
        float w2 = ImGui::CalcTextSize(hk.key).x;
        if (w1 > maxNameW) maxNameW = w1;
        if (w2 > maxKeyW) maxKeyW = w2;
    }
    
    float panelW = maxNameW + maxKeyW + pad * 4 + 20.0f * scale;
    float panelH = (hotkeys.size() + 1) * lineH + pad * 2;
    
    ImVec2 pos(20.0f * scale, screenHeight / 2.0f - panelH / 2.0f);
    DrawHUDPanelEx(dl, pos, ImVec2(panelW, panelH), "Hotkeys", scale);
    
    float y = pos.y + lineH + pad + 4.0f * scale;
    for (auto& hk : hotkeys) {
        dl->AddText(ImVec2(pos.x + pad, y), HUD_TEXT_DIM, hk.name);
        
        // Клавиша справа
        float keyX = pos.x + panelW - pad - maxKeyW;
        dl->AddText(ImVec2(keyX, y), HUD_TEXT, hk.key);
        
        y += lineH;
    }
}

// ===== Active Potions (единый блок!) =====
void RenderActivePotions(ImDrawList* dl, float scale) {
    if (!hudActivePotions) return;
    
    float pad = 6.0f * scale;
    float lineH = 20.0f * scale;
    float iconSize = 16.0f * scale;
    
    // ЗАГЛУШКА — пока нет оффсета, показываем пример
    // Когда получим реальные эффекты — заменим на реальные данные
    struct EffectEntry {
        std::string name;
        int level;      // 1 = I, 2 = II
        float seconds;  // оставшееся время
    };
    
    std::vector<EffectEntry> effects;
    // TODO: заменить на реальные эффекты когда будет оффсет
    effects.push_back({ "Скорость", 2, 12.4f });
    effects.push_back({ "Сила", 2, 8.7f });
    effects.push_back({ "Регенерация", 1, 4.2f });
    effects.push_back({ "Сопротивление", 1, 15.0f });
    
    if (effects.empty()) return;
    
    // Размеры панели
    float maxNameW = 0;
    for (auto& e : effects) {
        float w = ImGui::CalcTextSize(e.name.c_str()).x;
        if (w > maxNameW) maxNameW = w;
    }
    
    float panelW = maxNameW + 90.0f * scale + pad * 2;
    float panelH = (effects.size() + 1) * lineH + pad * 2;
    
    float x = (hudPotionsX == 0.0f) ? (screenWidth - panelW - 20.0f * scale) : hudPotionsX;
    ImVec2 pos(x, hudPotionsY);
    
    DrawHUDPanelEx(dl, pos, ImVec2(panelW, panelH), "Active Potions", scale);
    
    // Рисуем эффекты
    float y = pos.y + lineH + pad + 4.0f * scale;
    for (auto& e : effects) {
        // Цветной кружок (иконка эффекта)
        ImU32 iconColor = IM_COL32(255, 100, 100, 255); // красный по умолчанию
        if (e.name == "Скорость")    iconColor = IM_COL32(100, 200, 255, 255); // голубой
        else if (e.name == "Сила")   iconColor = IM_COL32(255, 100, 100, 255); // красный
        else if (e.name == "Регенерация") iconColor = IM_COL32(255, 150, 200, 255); // розовый
        else if (e.name == "Сопротивление") iconColor = IM_COL32(150, 150, 150, 255); // серый
        
        dl->AddCircleFilled(ImVec2(pos.x + pad + iconSize / 2, y + lineH / 2 - 2), iconSize / 2, iconColor);
        
        // Название + уровень (например "Скорость II")
        std::string nameWithLevel = e.name;
        if (e.level > 1) nameWithLevel += " " + std::to_string(e.level);
        
        dl->AddText(ImVec2(pos.x + pad + iconSize + 6.0f * scale, y), HUD_TEXT, nameWithLevel.c_str());
        
        // Время справа (например "0:12")
        int sec = (int)e.seconds;
        char timeBuf[16];
        snprintf(timeBuf, sizeof(timeBuf), "%d:%02d", sec / 60, sec % 60);
        
        float timeW = ImGui::CalcTextSize(timeBuf).x;
        float timeX = pos.x + panelW - pad - timeW;
        dl->AddText(ImVec2(timeX, y), HUD_TEXT_DIM, timeBuf);
        
        y += lineH;
    }
}

// ===== Target HUD =====
void RenderTargetHUD(ImDrawList* dl, float scale) {
    if (!hudTargetHUD) return;
    if (nearestTarget == NULL) return;
    if (inMenu) return;
    
    float pad = 6.0f * scale;
    float lineH = 18.0f * scale;
    
    std::string name = nearestTarget->getName();
    int hp = nearestTarget->getHealth();
    
    // Рамка вокруг цели (как на скрине — квадрат)
    // TODO: нужны координаты экрана для цели (через WorldToScreen)
    // Пока рисуем только текст над целью
    
    float panelW = 200.0f * scale;
    float panelH = lineH * 2 + pad * 2;
    
    // Позиция — по центру сверху экрана (временно)
    ImVec2 pos(screenWidth / 2.0f - panelW / 2.0f, 100.0f * scale);
    
    DrawHUDPanelEx(dl, pos, ImVec2(panelW, panelH), nullptr, scale);
    
    // Имя
    dl->AddText(ImVec2(pos.x + pad, pos.y + pad / 2), HUD_TEXT, name.c_str());
    
    // HP
    char hpBuf[32];
    snprintf(hpBuf, sizeof(hpBuf), "HP: %d", hp);
    dl->AddText(ImVec2(pos.x + pad, pos.y + pad / 2 + lineH), HUD_TEXT, hpBuf);
    
    // Полоска HP
    float barX = pos.x + pad;
    float barY = pos.y + panelH - pad;
    float barW = panelW - pad * 2;
    float barH = 3.0f * scale;
    
    dl->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + barW, barY + barH), IM_COL32(50, 50, 50, 255));
    
    float hpRatio = hp / 20.0f;
    if (hpRatio < 0) hpRatio = 0;
    if (hpRatio > 1) hpRatio = 1;
    
    ImU32 hpColor = IM_COL32(0, 255, 0, 255);
    if (hpRatio < 0.66f) hpColor = IM_COL32(255, 255, 0, 255);
    if (hpRatio < 0.33f) hpColor = IM_COL32(255, 100, 0, 255);
    if (hpRatio < 0.15f) hpColor = IM_COL32(255, 0, 0, 255);
    
    dl->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + barW * hpRatio, barY + barH), hpColor);
}

// ===== Главная функция =====
void RenderHUD() {
    if (!hudEnabled) return;
    if (inMenu) return;
    
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float scale = hudScale * calculateResolutionScale(screenWidth, screenHeight, 2480, 1116) * 2.5f;
    
    RenderWatermark(dl, scale);
    RenderCoords(dl, scale);
    RenderHotkeys(dl, scale);
    RenderActivePotions(dl, scale);
    RenderTargetHUD(dl, scale);
}
