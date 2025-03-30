# no functions in meson, so it's either copy paste or looping...
audio_crate_build = custom_target(
    'audio_crate_build',
    command: [
        'cargo',
        'run',
        '--quiet',
        '--package',
        'xbuild',
        '@SOURCE_ROOT@',
        'audio',
        '@OUTPUT@',
        '@DEPFILE@',
    ],
    # build_always_stale: true, # input files are not known, does it's own tracking anyway
    output: ['audio_crate.lib', 'audio_crate.h'],
    depfile: 'audio_crate.d',
)

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
    depfile: 'render_crate.d',
)

audio_crate_dep = declare_dependency(
    sources : audio_crate_build,
    # apparently this is both the current source *and* build dirs?
    include_directories : include_directories('.'),
)

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
