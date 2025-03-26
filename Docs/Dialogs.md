# Dialog system

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

TODO: overview of dialog lifecyles and usage
