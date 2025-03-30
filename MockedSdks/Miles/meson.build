sdk_miles = static_library(
    'sdk_miles',
    'Mss.cpp',
    dependencies : audio_crate_dep,
)
sdk_miles_dep = declare_dependency(
    link_with : sdk_miles,
    include_directories : include_directories('.'),
)
