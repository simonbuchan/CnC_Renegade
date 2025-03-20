# Rendering Notes

A few notes I've made while debugging the new WGPU render backend.

You may also be interested in the original .txt files in the `Code/ww3d2/` directory,
but they're fairly obscure to start. I'll reference them where appropriate..

## Components

The original code is in `Code/`, where `ww*/` directories are the generic libraries that were
presumably shared between projects. My new Rust code is in `Crates/`, and my dummy external
libraries are in `MockedSdks/`, which get the code building without updating the original
code as much as possible.

The parts we care about are:

- `Code/Commando/` - the root executable, it tracks the top-level game modes, menus etc., so
  we mostly only care about the main game loop dispatching to the other parts here. The rendering
  options will be more relevant later.

- `Code/Combat/` is the main combat game mode, I haven't much looked at this yet. It forwards
  all the fancy rendering to the `PhysicsSceneClass` in `Code/wwphys/` (not actually a physics
  scene in the modern sense as far as I understand)

- `Code/wwphys/` is a generic 3d world game engine. Probably the most interesting part for
  later. Collisions, pathfinding, physics, vehicles, actors, and visual effects logic; that sort
  of thing.

- `Code/ww3d2/` is general 3D rendering wrapper. It mostly handles state management and abstracts
  the underlying Direct3D 8 rendering API: managing materials, meshes & texture loading, cameras,
  scenes etc.

- `MockedSdks/D3D8/` is my stubbed out Direct3D 8 API. It's just enough filled out so the original
  code compiled, and is progressively being filled out to forward to `Crates/Render/`

- `Crates/Render/` is my new Rust crate to perform rendering which is currently just a WGPU thin
  wrapper. I'll be moving a lot more of the rendering logic in here once things are working better.

## High level

Renegade's rendering starts in `_Game_Main_Loop_Loop()` in `mainloop.cpp`, when it
calls `GameModeManager::Render()`.

This does some checks to see if rendering is enabled (e.g. not a dedicated server),
then:
- If in combat mode, updates the `PhysicsSceneClass` before rendering.
- Calls `WW3D::Begin_Render()`:
  - This does some bookkeeping and pumps the texture loading queue.
  - Then clears the screen, then calls `IDirect3DDevice::BeginScene()` which i've
    mapped to `wgpu_commands_begin_render_pass()`
- Then, if the game is in focus, iterates `GameModeList`, and calls `Render()` on each that are not "inactive".

  The modes are all added to the `GameModeManager` in `Game_Init` in `Commando`. They implement
  an activation lifecycle, and a `Think` and `Render` method. They are mostly surprisingly simple,
  the system seems a bit vestigial.

  These modes are, in order (these are the names used to look them up, the class names append `GameModeClass`):
  - `Combat` - the actual gameplay - renders a `PhysicsSceneClass`
  - `LAN`, `WOL` - network, no rendering
  - `Overlay` - renders a `SimpleSceneClass`, mostly used for the menu background?
  - `Menu` - no rendering! just plays music
  - `Movie` - plays BINK movies
  - `Console` - no rendering, forwards input to a dev console
  - `ScoreScreen` - forwards to the dialog system to show `ScoreScreenDialogClass` on init
  - `TextDisplay` - wraps `Render2DTextClass` to act as a text layer
  - `Overlay3D` - another `SimpleSceneClass` renderer like `Overlay`, unused
- It then explicitly calls render for:
  - `CombatManager::MessageWindow`
  - `ObjectiveMananger::Viewer`
  - `DialogMgrClass`
  - `cDiagnostics`
  - `BINKMovie`
- Then `WW3D::End_Render()`: 
  - This calls `SortingRendererClass::Flush()` which draws any queued any queued up triangles.
  - Then calls `DX8Wrapper::End_Scene()` which calls:
    - `IDirect3DDevice::EndScene()` which I've mapped to `wgpu_device_submit()` (which implicitly ends the render pass)
    - `IDirect3DDevice::Present()` which I've mapped to `wgpu_surface_present()`
 
## Menus

The first thing you see (after the currently stubbed out Bink movies) is the main menu, which uses
the `DialogMgrClass` system, which renders a list of `DialogClass` objects.

These, somewhat oddly, parses the information about the dialog from a windows resource compiled
into the executable, just like a standard Windows dialog would. These are all defined in
`Code/Commando/chat.rc`, for example the quit to desktop dialog is defined as:

```
IDD_QUIT_TO_DESKTOP DIALOG DISCARDABLE  0, 0, 211, 69
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION
CAPTION "IDS_MENU_TEXT054"
FONT 8, "MS Sans Serif"
BEGIN
    DEFPUSHBUTTON   "IDS_MENU_TEXT055",IDC_QUIT,33,41,61,20
    LTEXT           "IDS_MENU_TEXT056",IDC_STATIC,7,7,197,20
    PUSHBUTTON      "IDS_MENU_TEXT057",IDC_BACK,117,41,61,20
END
```

The `ID*` values here are defined in `resource.h` as usual, e.g.:

```c
#define IDD_QUIT_TO_DESKTOP             129
// ...
#define IDC_QUIT                        1000
// ...
```

These ids referenced here by `QuitVerificationDialogClass`, which
subclasses `PopupDialagClass`, which subclasses `DialogBaseClass`,
which takes the passed in `IDD_QUIT_TO_DESKTOP` id and reads the
resource to get the dialog layout in `Start_Dialog` then creates
the equivalent control (`DialogControlClass`) objects.

The main menu is a lot more fancy, adding a bunch of spinning models
and transitions, but the basic idea is the same.

## Asset loading

TODO
