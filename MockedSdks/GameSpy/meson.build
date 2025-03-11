sdk_gamespy = static_library(
    'sdk_gamespy',
    'GameSpy.cpp',
)
sdk_gamespy_dep = declare_dependency(
    link_with : sdk_gamespy,
    include_directories : include_directories('.', is_system : true),
)
