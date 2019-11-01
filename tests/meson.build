tests = executable('utf8er_tests', 'utf8er_tests.cpp', 
    include_directories : incDirs,
    cpp_args : ['-fsanitize=address', '-Wall'],
    link_args : '-fsanitize=address')

test('utf8er tests', tests, workdir: meson.current_build_dir())