project(
    'renegade',
    'c',
    'cpp',
    'rust',
    meson_version : '1.7.0',
    default_options : [
        'cpp_std=c++20',
        'rust_std=2024',
        # don't conflict with the library pulled in by rust
        'b_vscrt=none',
    ],
)

add_project_arguments('-C', 'target-feature=+crt-static', language : 'rust')
add_project_arguments('-C', 'target-feature=+crt-static', language : 'rust', native: true)

# pointer-truncation warnings!!!
add_project_arguments('-wd4302', '-wd4311', '-wd4312', language : 'cpp')

# mul/div between enum and float
add_project_arguments('-wd5055', language : 'cpp')

# defines
add_project_arguments('-DWINDOWS', language : 'cpp')
add_project_arguments('-D_WINDOWS', language : 'cpp')
add_project_arguments('-DWIN32_LEAN_AND_MEAN', language : 'cpp')
add_project_arguments('-DWWDEBUG', language : 'cpp')

run_dir = meson.current_source_dir() / 'Run'

subdir('Crates')

subdir('MockedSdks/Bink')
subdir('MockedSdks/D3D8')
subdir('MockedSdks/Miles')
subdir('MockedSdks/GameSpy')

# universal
subdir('Code/wwdebug')
subdir('Code/wwlib')

# utilities
subdir('Code/BandTest')
subdir('Code/wwbitpack')
subdir('Code/wwmath')
subdir('Code/wwsaveload')
subdir('Code/wwutil')
subdir('Code/wwnet')
subdir('Code/ww3d2')
subdir('Code/wwaudio')
subdir('Code/wwphys')
subdir('Code/wwtranslatedb')
subdir('Code/wwui')
subdir('Code/BinkMovie')

# logic
subdir('Code/SControl')
subdir('Code/Scripts')
subdir('Code/Combat')

# online
subdir('Code/WOLBrowser')
subdir('Code/wolapi')
subdir('Code/WWOnline')

# game
subdir('Code/Commando')