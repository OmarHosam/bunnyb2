#include "gui.h"

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <sstream>  // For stringstream
#include <iomanip>  // For std::setprecision and std::fixed

#include <cmath> // Include for fabs

#include "../utils/utils.h"
#include "../globals.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter
);

long __stdcall WindowProcess(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    gui::hwnd = window;

    switch (message)
    {
    case WM_SIZE: {
        if (gui::device && wideParameter != SIZE_MINIMIZED)
        {
            gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
            gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
            gui::ResetDevice();
        }
    }return 0;

    case WM_SYSCOMMAND: {
        if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
    }break;

    case WM_DESTROY: {
        PostQuitMessage(0);
    }return 0;

    case WM_LBUTTONDOWN: {
        gui::position = MAKEPOINTS(longParameter); // set click points
    }return 0;

    case WM_MOUSEMOVE: {
        if (wideParameter == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(longParameter);
            auto rect = ::RECT{ };

            GetWindowRect(gui::window, &rect);

            rect.left += points.x - gui::position.x;
            rect.top += points.y - gui::position.y;

            if (gui::position.x >= 0 &&
                gui::position.x <= gui::WIDTH &&
                gui::position.y >= 0 && gui::position.y <= 19)
                SetWindowPos(
                    gui::window,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }

    }return 0;

    }

    return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = (WNDPROC)WindowProcess;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleA(0);
    windowClass.hIcon = 0;
    windowClass.hCursor = 0;
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = "class001";
    windowClass.hIconSm = 0;

    RegisterClassEx(&windowClass);

    window = CreateWindowEx(
        0,
        "class001",
        windowName,
        WS_POPUP,
        100,
        100,
        WIDTH,
        HEIGHT,
        0,
        0,
        windowClass.hInstance,
        0
    );

    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParameters,
        &device) < 0)
        return false;

    return true;
}

void gui::ResetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (d3d)
    {
        d3d->Release();
        d3d = nullptr;
    }
}

void gui::CreateImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO();

    io.IniFilename = NULL;

    ImGui::StyleColorsDark();

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.f);

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            isRunning = !isRunning;
            return;
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0, 0, 0, 0);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

void gui::Style() noexcept {
    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowTitleAlign = ImVec2(0.5f,0.5f);
    style->FramePadding = ImVec2(5, 6);

    style->TabRounding = 0.f;
    style->WindowRounding = 3.f;
    style->FrameRounding = 3.f;
    style->GrabRounding = 4.f;

    // Gruvbox my beloved.
    ImVec4* colors = style->Colors;

    // Text and background
    colors[ImGuiCol_Text] = Color::fg;
    colors[ImGuiCol_TextDisabled] = Color::gray;
    colors[ImGuiCol_WindowBg] = Color::bg0;
    colors[ImGuiCol_ChildBg] = Color::bg1;
    colors[ImGuiCol_PopupBg] = Color::bg1;
    colors[ImGuiCol_Border] = Color::gray;
    colors[ImGuiCol_BorderShadow] = Color::bg0;
    colors[ImGuiCol_FrameBg] = Color::bg2;
    colors[ImGuiCol_FrameBgHovered] = Color::bg1;
    colors[ImGuiCol_FrameBgActive] = Color::bg1;
    colors[ImGuiCol_TitleBg] = Color::bg1;
    colors[ImGuiCol_TitleBgActive] = Color::bg2;
    colors[ImGuiCol_TitleBgCollapsed] = Color::bg0;
    
    // Buttons
    colors[ImGuiCol_Button] = Color::bg2;
    colors[ImGuiCol_ButtonHovered] = Color::blue;
    colors[ImGuiCol_ButtonActive] = Color::red;

    // Headers
    colors[ImGuiCol_Header] = Color::bg1;
    colors[ImGuiCol_HeaderHovered] = Color::aqua;
    colors[ImGuiCol_HeaderActive] = Color::blue;

    // Checkmarks, sliders, progress bars
    colors[ImGuiCol_CheckMark] = Color::green;
    colors[ImGuiCol_SliderGrab] = Color::blue;
    colors[ImGuiCol_SliderGrabActive] = Color::orange;

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = Color::bg1;
    colors[ImGuiCol_ScrollbarGrab] = Color::gray;
    colors[ImGuiCol_ScrollbarGrabHovered] = Color::blue;
    colors[ImGuiCol_ScrollbarGrabActive] = Color::aqua;

    // Tabs
    colors[ImGuiCol_Tab] = Color::bg1;
    colors[ImGuiCol_TabHovered] = Color::bg2;
    colors[ImGuiCol_TabActive] = Color::bg2;
    colors[ImGuiCol_TabUnfocused] = Color::bg1;
    colors[ImGuiCol_TabUnfocusedActive] = Color::bg2;

    // Resizing grip
    colors[ImGuiCol_ResizeGrip] = Color::gray;
    colors[ImGuiCol_ResizeGripHovered] = Color::aqua;
    colors[ImGuiCol_ResizeGripActive] = Color::orange;

    // Plot
    colors[ImGuiCol_PlotLines] = Color::blue;
    colors[ImGuiCol_PlotLinesHovered] = Color::yellow;
    colors[ImGuiCol_PlotHistogram] = Color::green;
    colors[ImGuiCol_PlotHistogramHovered] = Color::orange;

    // Text fields
    colors[ImGuiCol_TextSelectedBg] = Color::orange;
    colors[ImGuiCol_DragDropTarget] = Color::orange;
    colors[ImGuiCol_NavHighlight] = Color::green;
    colors[ImGuiCol_NavWindowingHighlight] = Color::fg;
    colors[ImGuiCol_NavWindowingDimBg] = Color::bg0;
    colors[ImGuiCol_ModalWindowDimBg] = Color::bg0;
}
// Clamp function
inline float ImClamp(int value, int min, int max) {
    return value < min ? min : (value > max ? max : value);
}

void TwoHandleSlider(int* minValue, int* maxValue, int minLimit, int maxLimit) {
    // Clamp the values
    *minValue = ImClamp(*minValue, minLimit, *maxValue);
    *maxValue = ImClamp(*maxValue, *minValue, maxLimit);
    
    // Create a single slider and manage the min/max values
    ImGui::SliderInt("Min CPS", minValue, minLimit, maxLimit);
    
    // Clamp the values
    *maxValue = ImClamp(*maxValue, *minValue, maxLimit);
    *minValue = ImClamp(*minValue, minLimit, *maxValue);
    
    // Render a separate visual representation for max
    ImGui::SliderInt("Max CPS", maxValue, minLimit, maxLimit);
}

// Generalized function to render the click tab for either left or right
void RenderClickTab(const char* label, bool& canClick, Settings::ClickSettings& settings) {
    if (ImGui::BeginTabItem(label)) {
        ImGui::Columns(2);
        ImGui::Checkbox(std::string("Toggle ").append(label).append("click").c_str(), &canClick);

        if (settings.mode == AUTOCLICK) {
            ImGui::Checkbox("Jitter", &settings.autoclick.jitter);
            ImGui::NextColumn();

            TwoHandleSlider(&settings.autoclick.cps_min, &settings.autoclick.cps_max, 1, 20);
        } else if (settings.mode == MULTIPLIE) {
            ImGui::NextColumn();

            ImGui::SliderInt("Multiplier", &settings.multiplie.multiplier, 1, 10);
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << (float)settings.multiplie.delay / 1000.f;  // Set precision to 2 decimal places.
            
            ImGui::SliderInt("Delay", &settings.multiplie.delay, 0, 2000, ss.str().c_str());
            ImGui::SliderInt("Chance (%)", &settings.multiplie.chance, 1, 100);
        }

        ImGui::Columns(1); // Closing the column.

        if (settings.mode == AUTOCLICK && settings.autoclick.jitter) {
            ImGui::SliderInt("Jitter Intensity", &settings.autoclick.jitterIntensity, 1, 20);
        }

        if (ImGui::Button((settings.mode == AUTOCLICK ? "Toggle Multiplier" : "Toggle Autoclicker"))) {
            toggleModes(&settings.mode);
        }

        ImGui::EndTabItem();
    }
}

// Main interface rendering function.
void RenderInterface() {
    RenderClickTab("Left", Settings::canLeftClick, Settings::leftClickSettings);
    RenderClickTab("Right", Settings::canRightClick, Settings::rightClickSettings);
}

bool HotkeyWidget(const char* label, int& key) {
    ImGui::TextUnformatted(label); // Label for the hotkey widget
    ImGui::SameLine();

    // Determine the text to display based on the key value
    char keyDisplay[64];
    if (key == 0) {
        strcpy_s(keyDisplay, "None");
    } else {
        GetKeyNameTextA(MapVirtualKey(key, MAPVK_VK_TO_VSC) << 16, keyDisplay, sizeof(keyDisplay));
    }

    // Button to capture the key press
    if (ImGui::Button(keyDisplay)) {
        ImGui::OpenPopup("Set Hotkey");
    }

    // Popup to listen for the hotkey input
    if (ImGui::BeginPopupModal("Set Hotkey", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Press any key...");

        // If escape key is pressed, close the popup
        if (GetAsyncKeyState(VK_ESCAPE)) {
            ImGui::CloseCurrentPopup();
        } else {
            // Loop through all possible virtual key codes
            for (int vk = 0x01; vk <= 0xFE; ++vk) {
                if (GetAsyncKeyState(vk) & 0x8000) { // Detect if key is pressed
                    key = vk; // Set the virtual key as the hotkey
                    ImGui::CloseCurrentPopup();
                    Settings::toggled = !Settings::toggled; // Since the check runs nonstop, it will toggle when we're changing the hotkey, this is just a hacky workaround.
                    break;
                }
            }
        }

        ImGui::EndPopup();
    }


    return false;
}


void gui::Render() noexcept
{
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
    ImGui::Begin(
        "BunnyB2",
        &isRunning,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    );

    gui::Style();

    if (ImGui::BeginTabBar("tabs")) {
        if (ImGui::BeginTabItem("General")) {
            ImGui::Checkbox("Enable", &Settings::toggled);
            ImGui::Checkbox("Disable on focus", &Settings::disableOnFocus);

            HotkeyWidget("Toggle Hotkey", Settings::toggleKey);

            ImGui::EndTabItem();
        }

        RenderInterface();

        const char* credit = "Made by @cixq.";
        const char* version = "BunnyB2 BUILD 0.5.0";

        ImVec2 creditSize = ImGui::CalcTextSize(credit);
        ImVec2 versionSize = ImGui::CalcTextSize(version);

        ImVec2 creditPos(WIDTH - creditSize.x - 8.0f, HEIGHT - creditSize.y - 8.0f);
        ImVec2 versionPos(8.0f, HEIGHT - versionSize.y - 8.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, Color::bg2);
        
        ImGui::SetCursorPos(creditPos);
        ImGui::Text("%s", credit);

        ImGui::SetCursorPos(versionPos);
        ImGui::Text("%s", version);
        
        ImGui::PopStyleColor();
        ImGui::EndTabBar();
    }

    // Check if the window is focused, and only disable toggling
    // if the checkbox was not interacted with this frame
    if (Settings::disableOnFocus) {
        if (GetForegroundWindow() == gui::hwnd) {
            if (Settings::toggled && gui::isWindowNotFocused) {
                gui::isWindowNotFocused = false;
                Settings::toggled = false;   
            }
        } else {
            gui::isWindowNotFocused = true;
        }
    }

    ImGui::End();
}