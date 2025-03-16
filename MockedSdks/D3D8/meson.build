sdk_d3d8 = static_library(
    'sdk_d3d8',
    'd3d8.cpp',
    'd3dx8core.cpp',
    'd3dx8math.cpp',

    override_options: [
        'cpp_std=c++20',
    ],
    dependencies: render_crate_dep,
)
sdk_d3d8_dep = declare_dependency(
    link_with : sdk_d3d8,
    include_directories : include_directories('.', is_system : true),
    dependencies: render_crate_dep,
)
