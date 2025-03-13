cargo_build_exe = executable('cargo_build', 'cargo_build.rs', native : true)

render_crate_build = custom_target(
    'render_crate_build',
    command: [cargo_build_exe, meson.project_source_root(), 'render', '@OUTPUT@'],
    build_always_stale: true, # input files are not known, does it's own tracking anyway
    output: 'render.lib',
)

render_crate = static_library(
    'render_crate',
    render_crate_build,
)

render_crate_dep = declare_dependency(
    link_with : render_crate,
    include_directories: include_directories('Render'),
    # from rust stdlib
    link_args: ['-luserenv', '-lntdll'],
)
