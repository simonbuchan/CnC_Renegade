render_crate_build = custom_target(
    'render_crate_build',
    command: [
        'cargo',
        'run',
        '--quiet',
        '--package',
        'xbuild',
        '@SOURCE_ROOT@',
        'render',
        '@OUTPUT@',
        '@DEPFILE@',
    ],
    # build_always_stale: true, # input files are not known, does it's own tracking anyway
    output: ['render_crate.lib', 'render_crate.h'],
    # hope this works?
    depfile: 'render_crate.d',
)

# This creates a second library from the first :|
# meson.get_compiler('cpp').find_library() should work too, but
# it doesn't have a way to specify dependencies so it just fails
# for a clean build :(
#render_crate = static_library('render_crate_wrap', render_crate_build)

render_crate_dep = declare_dependency(
    sources : render_crate_build,
    # apparently this is both the current source *and* build dirs?
    include_directories : include_directories('.'),
    # seems rustc doesn't link in dependencies
    link_args: [
        # stdlib
        '-luserenv',
        '-lntdll',
        # wgpu
        '-lopengl32',
        '-ld3dcompiler',
        '-lruntimeobject',
        '-lpropsys',
    ],
)
