#include "gui.h"

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include "../utils/utils.h"

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

ImVec4 convertRGB(int r, int g, int b) {
    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
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
    // Gruvbox color palette
    ImVec4 bg0 = convertRGB(29, 32, 33);      // background
    ImVec4 bg1 = convertRGB(40, 40, 40);      // light background
    ImVec4 bg2 = convertRGB(50, 48, 47);      // lighter background
    ImVec4 fg = convertRGB(235, 219, 178);    // foreground
    ImVec4 gray = convertRGB(146, 131, 116);  // gray
    ImVec4 red = convertRGB(204, 36, 29);     // red
    ImVec4 green = convertRGB(152, 151, 26);  // green
    ImVec4 yellow = convertRGB(215, 153, 33); // yellow
    ImVec4 blue = convertRGB(69, 133, 136);   // blue
    ImVec4 purple = convertRGB(177, 98, 134); // purple
    ImVec4 aqua = convertRGB(104, 157, 106);  // aqua
    ImVec4 orange = convertRGB(214, 93, 14);  // orange

    // Text and background
    colors[ImGuiCol_Text] = fg;
    colors[ImGuiCol_TextDisabled] = gray;
    colors[ImGuiCol_WindowBg] = bg0;
    colors[ImGuiCol_ChildBg] = bg1;
    colors[ImGuiCol_PopupBg] = bg1;
    colors[ImGuiCol_Border] = gray;
    colors[ImGuiCol_BorderShadow] = bg0;
    colors[ImGuiCol_FrameBg] = bg2;
    colors[ImGuiCol_FrameBgHovered] = bg1;
    colors[ImGuiCol_FrameBgActive] = bg1;
    colors[ImGuiCol_TitleBg] = bg1;
    colors[ImGuiCol_TitleBgActive] = bg2;
    colors[ImGuiCol_TitleBgCollapsed] = bg0;
    
    // Buttons
    colors[ImGuiCol_Button] = bg2;
    colors[ImGuiCol_ButtonHovered] = blue;
    colors[ImGuiCol_ButtonActive] = orange;

    // Headers
    colors[ImGuiCol_Header] = bg1;
    colors[ImGuiCol_HeaderHovered] = aqua;
    colors[ImGuiCol_HeaderActive] = blue;

    // Checkmarks, sliders, progress bars
    colors[ImGuiCol_CheckMark] = green;
    colors[ImGuiCol_SliderGrab] = blue;
    colors[ImGuiCol_SliderGrabActive] = orange;

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = bg1;
    colors[ImGuiCol_ScrollbarGrab] = gray;
    colors[ImGuiCol_ScrollbarGrabHovered] = blue;
    colors[ImGuiCol_ScrollbarGrabActive] = aqua;

    // Tabs
    colors[ImGuiCol_Tab] = bg1;
    colors[ImGuiCol_TabHovered] = bg2;
    colors[ImGuiCol_TabActive] = bg2;
    colors[ImGuiCol_TabUnfocused] = bg1;
    colors[ImGuiCol_TabUnfocusedActive] = bg2;

    // Resizing grip
    colors[ImGuiCol_ResizeGrip] = gray;
    colors[ImGuiCol_ResizeGripHovered] = aqua;
    colors[ImGuiCol_ResizeGripActive] = orange;

    // Plot
    colors[ImGuiCol_PlotLines] = blue;
    colors[ImGuiCol_PlotLinesHovered] = yellow;
    colors[ImGuiCol_PlotHistogram] = green;
    colors[ImGuiCol_PlotHistogramHovered] = orange;

    // Text fields
    colors[ImGuiCol_TextSelectedBg] = orange;
    colors[ImGuiCol_DragDropTarget] = orange;
    colors[ImGuiCol_NavHighlight] = green;
    colors[ImGuiCol_NavWindowingHighlight] = fg;
    colors[ImGuiCol_NavWindowingDimBg] = bg0;
    colors[ImGuiCol_ModalWindowDimBg] = bg0;
}

void RenderClickTab(const char* tabName, bool* canClick, Mode* mode, int* cps, int* multiplier, int* delay) {
    if (ImGui::BeginTabItem(tabName)) {
        ImGui::Columns(2);
        ImGui::Checkbox(std::string("Toggle ").append(tabName).append("click").c_str(), canClick);

        ImGui::NextColumn();

        if (*mode == AUTOCLICK) {
            ImGui::SliderInt("CPS", cps, 1, 30);
        }

        if (*mode == MULTIPLIE) {
            ImGui::SliderInt("Multiplier", multiplier, 1, 10);
            ImGui::SliderInt("Delay", delay, 0, 2000);
        }

        ImGui::Columns(1); // Closing the column.

        if (ImGui::Button((*mode == AUTOCLICK ? "Toggle Multiplier" : "Toggle Autoclicker"))) {
            toggleModes(mode);
        }

        ImGui::EndTabItem();
    }
}

void RenderInterface() {
    RenderClickTab("Left", &(Settings::canLeftClick), &(Settings::Modes::mode_left), 
                   &(Settings::Left::Autoclick::cps), &(Settings::Left::Multiplie::multiplier), 
                   &(Settings::Left::Multiplie::delay));

    RenderClickTab("Right", &(Settings::canRightClick), &(Settings::Modes::mode_right), 
                   &(Settings::Right::Autoclick::cps), &(Settings::Right::Multiplie::multiplier), 
                   &(Settings::Right::Multiplie::delay));
}

bool HotkeyWidget(const char* label, int& key) {
    ImGui::TextUnformatted(label); // Label for the hotkey widget
    ImGui::SameLine();
    int temp;

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
            ImGui::Checkbox("Enable", &(Settings::toggled));
            HotkeyWidget("Toggle Hotkey", Settings::toggleKey);
            ImGui::EndTabItem();
        }

        RenderInterface();
        
        ImGui::EndTabBar();
    }

    ImGui::End();
}