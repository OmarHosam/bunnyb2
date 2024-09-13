project('autoclicker', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++14'],
  subproject_dir : 'subprojects')

imgui = dependency('imgui')

executable('autoclicker', 'src/autoclicker.cpp',
  dependencies: [imgui])
