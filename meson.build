project('autoclicker', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

# Manually list the source files
imgui_sources = files(
    'ext/imgui/imgui.cpp',
    'ext/imgui/imgui_demo.cpp',
    'ext/imgui/imgui_draw.cpp',
    'ext/imgui/imgui_tables.cpp',
    'ext/imgui/imgui_widgets.cpp',
    'ext/imgui/imgui_impl_dx10.cpp',
    'ext/imgui/imgui_impl_dx9.cpp',
    'ext/imgui/imgui_impl_win32.cpp'
)

# Include the header files directory
imgui_includes = include_directories('ext/imgui')

# DirectX dependencies (DirectX 9 and 10)
dx9_dep = declare_dependency(link_args: ['-ld3d9'])
dx10_dep = declare_dependency(link_args: ['-ld3d10'])
dwm_dep = declare_dependency(link_args: ['-ldwmapi'])

# Compile ImGui as a static library
imgui_library = static_library('imgui', imgui_sources)

executable('autoclicker', 'src/autoclicker.cpp',
  dependencies: [dx9_dep, dx10_dep, dwm_dep], 
    include_directories: [imgui_includes],
    link_with: imgui_library)
