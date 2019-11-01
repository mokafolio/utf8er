project('utf8er', 'cpp', default_options : ['cpp_std=c++14'])

#remove this rather bogus warning
add_project_arguments('-Wno-missing-braces', language: 'cpp')

incDirs = include_directories('.')

if meson.is_subproject() == false or get_option('forceInstallHeaders')
    install_headers('utf8er/utf8er.hpp', subdir: 'utf8er')
endif

utf8erDep = declare_dependency(include_directories: incDirs)

if meson.is_subproject() == false
    subdir('tests')
endif
