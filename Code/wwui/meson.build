wwui = static_library(
    'wwui',
    'buttonctrl.cpp',
    'checkboxctrl.cpp',
    'childdialog.cpp',
    'comboboxctrl.cpp',
    'dialogbase.cpp',
    'dialogbutton.cpp',
    'dialogcontrol.cpp',
    'dialogmgr.cpp',
    'dialogparser.cpp',
    'dialogtext.cpp',
    'dialogtransition.cpp',
    'dropdownctrl.cpp',
    'editctrl.cpp',
    'healthbarctrl.cpp',
    'imagectrl.cpp',
    'IMECandidate.cpp',
    'IMECandidateCtrl.cpp',
    'IMEManager.cpp',
    'inputctrl.cpp',
    'listctrl.cpp',
    'listiconmgr.cpp',
    'mapctrl.cpp',
    'menubackdrop.cpp',
    'menudialog.cpp',
    'menuentryctrl.cpp',
    'merchandisectrl.cpp',
    'mousemgr.cpp',
    'multilinetextctrl.cpp',
    'popupdialog.cpp',
    'ProgressCtrl.cpp',
    'screencursor.cpp',
    'scrollbarctrl.cpp',
    'shortcutbarctrl.cpp',
    'sliderctrl.cpp',
    'stylemgr.cpp',
    'tabctrl.cpp',
    'textmarqueectrl.cpp',
    'tooltip.cpp',
    'tooltipmgr.cpp',
    'treectrl.cpp',
    'viewerctrl.cpp',
    'wwuiinput.cpp',
    dependencies : [
        wwlib_dep,
        wwtranslatedb_dep,
        wwaudio_dep,
        wwmath_dep,
        ww3d2_dep,
    ],
)
wwui_dep = declare_dependency(
    link_with : wwui,
    include_directories : include_directories('.'),
)
