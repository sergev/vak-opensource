# An array of tests to make loops easy.
test_scripts = '''
    simple
'''.split()

env = environment()

# This makes sure invocation of test command(s) refer to executables where meson writes them.
# Else you might test system versions of binaries, and miss failures.
env.set('PATH', meson.project_build_root() + ':/usr/bin:/bin:/usr/sbin:/sbin')

# Run the tests.
# Timeout may need tuning if this this used in other projects.
foreach t : test_scripts
    exec = find_program([ t, meson.current_source_dir() ], required : true)
    test(t, exec, env : env, timeout : 1)
endforeach
