# Application palette: `cpAppColor` and `cpAppBlackWhite`

This document describes the **1-based** indices in the system palettes `cpAppColor` and `cpAppBlackWhite` in [`include/tvision/app.h`](include/tvision/app.h). Both macros define **135** palette slots (indices **1–135**); they use the **same layout**. Only the **stored attribute bytes** differ so color vs. black-and-white displays look acceptable ([`TProgram::initScreen`](source/tvision/tprogram.cpp) selects among `cpAppColor`, `cpAppBlackWhite`, and `cpAppMonochrome`).

A third palette, `cpAppMonochrome`, shares the **same index meanings** and is selected on monochrome displays.

## How indices become on-screen colors

Turbo Vision resolves colors through **`TView::mapColor`** ([`source/tvision/mapcolor.cpp`](source/tvision/mapcolor.cpp)):

1. The current view’s **`getPalette()`** supplies a `TPalette`: `data[0]` is the length; `data[index]` is the value for that index.
2. If that value is non-zero and the view has an **owner**, the value is treated as the **next index** and `owner->mapColor` is called (palette **walk** toward the application root).
3. At the top, **`TProgram::getPalette()`** ([`source/tvision/tprogram.cpp`](source/tvision/tprogram.cpp)) returns one of `cpAppColor` / `cpAppBlackWhite` / `cpAppMonochrome`. When there is no owner, the last value is the **`TColorAttr`** used for drawing.

So many widgets do not “hard-code” an app index; they use **small local palettes** whose bytes point into parent indices (e.g. `cpGrayDialog` entries `\x20`–`\x3F` point at app indices **32–63**). The **semantic role** of app index *N* is still “whatever the framework wires to that slot,” as summarized below.

**Note:** `TDeskTop` does not contribute a palette in the usual way (see [examples/palette/README.md](examples/palette/README.md)); children chain through to `TProgram`.

---

## Official application bands (from `app.h`)

| App indices | Role |
|-------------|------|
| **1** | `TBackground` |
| **2–7** | `TMenuView`, **`TMenuBar` / `TMenuBox`**, **`TStatusLine`**, and **`TIndicator`** (shared indices; see below) |
| **8–15** | **`TWindow` blue** (`wpBlueWindow`, `cpBlueWindow`) |
| **16–23** | **`TWindow` cyan** (`cpCyanWindow`) |
| **24–31** | **`TWindow` gray** (`cpGrayWindow`) |
| **32–63** | **`TDialog` gray** (`dpGrayDialog`, `cpGrayDialog`) |
| **64–95** | **`TDialog` blue** (`cpBlueDialog`) |
| **96–127** | **`TDialog` cyan** (`cpCyanDialog`) |
| **128–135** | **`THelpWindow`** |

---

## Indices 2–7: menu bar, pop-up menus, status line, indicator

These app slots are **shared**. Child palettes map into them as follows:

### `TMenuView` (pop-up / pull-down menu content)

Per [`include/tvision/menus.h`](include/tvision/menus.h):

| Local index | Meaning |
|-------------|---------|
| 1 | Normal text |
| 2 | Disabled text |
| 3 | Shortcut text |
| 4 | Normal selection |
| 5 | Disabled selection |
| 6 | Shortcut selection |

`cpMenuView` is `"\x02\x03\x04\x05\x06\x07"` ([`source/tvision/tmnuview.cpp`](source/tvision/tmnuview.cpp)), so local indices 1–6 use **app palette indices 2–7**.

### `TMenuBar` and `TMenuBox`

They combine pairs of palette indices with `getColor` (e.g. `0x0301`, `0x0604`) so **foreground/background** pairs come from app indices **1–7** ([`source/tvision/tmenubar.cpp`](source/tvision/tmenubar.cpp), [`source/tvision/tmenubox.cpp`](source/tvision/tmenubox.cpp)).

### `TStatusLine`

Uses the same **app indices 2–7** via `cpStatusLine` `"\x02\x03\x04\x05\x06\x07"` ([`source/tvision/tstatusl.cpp`](source/tvision/tstatusl.cpp)): pairs such as `getColor(0x0301)` and `getColor(0x0604)` drive normal vs. selected hints and disabled variants.

### `TIndicator` (e.g. line/column in editors)

`cpIndicator` is `"\x02\x03"` ([`source/tvision/tindictr.cpp`](source/tvision/tindictr.cpp)): local 1–2 → **app 2–3**.

---

## Indices 8–31: `TWindow` (three chrome themes)

Each **8-index band** matches [`include/tvision/views.h`](include/tvision/views.h) (`TWindow` palette layout) and `cpBlueWindow` / `cpCyanWindow` / `cpGrayWindow`:

| Offset in band (1–8) | Meaning |
|----------------------|---------|
| 1 | Frame passive |
| 2 | Frame active |
| 3 | Frame icon |
| 4 | Scroll bar page area |
| 5 | Scroll bar controls (arrows + thumb) |
| 6 | Scroller normal text |
| 7 | Scroller selected text |
| 8 | Reserved |

**`TFrame`** uses its own local palette `cpFrame` `"\x01\x01\x02\x02\x03"` ([`source/tvision/tframe.cpp`](source/tvision/tframe.cpp)); [`include/tvision/views.h`](include/tvision/views.h) documents local indices **1–5** as passive frame, passive title, active frame, active title, icons.

**`TScrollBar`:** local 1–3 = page, arrows, indicator ([`include/tvision/views.h`](include/tvision/views.h)); `cpScrollBar` `"\x04\x05\x05"` ([`source/tvision/tscrlbar.cpp`](source/tvision/tscrlbar.cpp)).

**`TScroller`:** local 1–2 = normal / selected text ([`include/tvision/views.h`](include/tvision/views.h)); `cpScroller` `"\x06\x07"` ([`source/tvision/tscrolle.cpp`](source/tvision/tscrolle.cpp)).

**`TStaticText`** inside a window: `cpStaticText` `"\x06"` → window slot 6 (scroller normal) ([`source/tvision/tstatict.cpp`](source/tvision/tstatict.cpp)).

---

## Indices 32–127: `TDialog` (three dialog chrome themes × 32 slots)

[`include/tvision/dialogs.h`](include/tvision/dialogs.h) defines **32** logical slots for each dialog palette (`cpGrayDialog`, `cpBlueDialog`, `cpCyanDialog`). The macros store bytes `\x20`–`\x3F`, `\x40`–`\x5F`, `\x60`–`\x7F`, which are exactly **app indices 32–63**, **64–95**, **96–127**.

| Dialog local index | Meaning |
|--------------------|---------|
| 1 | Frame passive |
| 2 | Frame active |
| 3 | Frame icon |
| 4 | Scroll bar page area |
| 5 | Scroll bar controls |
| 6 | `TStaticText` |
| 7 | `TLabel` normal |
| 8 | `TLabel` selected |
| 9 | `TLabel` shortcut |
| 10 | `TButton` normal |
| 11 | `TButton` default |
| 12 | `TButton` selected |
| 13 | `TButton` disabled |
| 14 | `TButton` shortcut |
| 15 | `TButton` shadow |
| 16 | `TCluster` normal |
| 17 | `TCluster` selected |
| 18 | `TCluster` shortcut |
| 19 | `TInputLine` normal text |
| 20 | `TInputLine` selected text |
| 21 | `TInputLine` arrows |
| 22 | History arrow |
| 23 | History sides |
| 24 | `THistoryWindow` scroll bar page |
| 25 | `THistoryWindow` scroll bar controls |
| 26 | `TListViewer` normal |
| 27 | `TListViewer` focused |
| 28 | `TListViewer` selected |
| 29 | `TListViewer` divider |
| 30 | `TFileInfoPane` (info pane) |
| 31 | `TCluster` disabled |
| 32 | Reserved |

Widget bridge examples (see `source/tvision/*.cpp`): `cpButton`, `cpCluster`, `cpInputLine`, `cpLabel`, `cpListViewer`, `cpMemo`, `cpHistory`, `cpHistoryWindow`, `cpInfoPane`, etc.

---

## Indices 128–135: `THelpWindow`

[`include/tvision/helpbase.h`](include/tvision/helpbase.h): `cHelpWindow` is `"\x80\x81\x82\x83\x84\x85\x86\x87"` (hex) → app indices **128–135**.

[`include/tvision/help.h`](include/tvision/help.h) **`THelpWindow`** palette layout:

| Local index | Meaning |
|-------------|---------|
| 1 | Frame passive |
| 2 | Frame active |
| 3 | Frame icon |
| 4 | Scroll bar page area |
| 5 | Scroll bar controls |
| 6 | `THelpViewer` normal |
| 7 | `THelpViewer` keyword |
| 8 | `THelpViewer` selected keyword |

`THelpViewer` locally documents indices **1–3** as normal / keyword / selected keyword; `cHelpViewer` is `"\x06\x07\x08"` ([`include/tvision/helpbase.h`](include/tvision/helpbase.h)), mapping those locals to the parent window’s slots 6–8.

---

## Complete index list (1–135)

For each **app index**, the **meaning** is the role of that slot in the tree above. **Gray / blue / cyan dialog** rows repeat the same **dialog local** column three times (indices 32–63, 64–95, 96–127).

| App idx | Meaning |
|--------:|---------|
| **1** | `TBackground` fill pattern (`TBackground` uses local index 1 → app 1). |
| **2** | Shared: `TMenuView` normal text; `TStatusLine` / menu bar pair colors; `TIndicator` first color. |
| **3** | Shared: `TMenuView` disabled text; status/menu pairs; `TIndicator` second color. |
| **4** | Shared: `TMenuView` shortcut text; status/menu pairs. |
| **5** | Shared: `TMenuView` normal selection; status/menu pairs. |
| **6** | Shared: `TMenuView` disabled selection; status/menu pairs. |
| **7** | Shared: `TMenuView` shortcut selection; status/menu pairs. |
| **8** | Blue `TWindow`: frame passive. |
| **9** | Blue `TWindow`: frame active. |
| **10** | Blue `TWindow`: frame icons. |
| **11** | Blue `TWindow`: scroll bar page. |
| **12** | Blue `TWindow`: scroll bar controls. |
| **13** | Blue `TWindow`: scroller normal / static text default. |
| **14** | Blue `TWindow`: scroller selected. |
| **15** | Blue `TWindow`: reserved (slot 8). |
| **16** | Cyan `TWindow`: frame passive. |
| **17** | Cyan `TWindow`: frame active. |
| **18** | Cyan `TWindow`: frame icons. |
| **19** | Cyan `TWindow`: scroll bar page. |
| **20** | Cyan `TWindow`: scroll bar controls. |
| **21** | Cyan `TWindow`: scroller normal. |
| **22** | Cyan `TWindow`: scroller selected. |
| **23** | Cyan `TWindow`: reserved. |
| **24** | Gray `TWindow`: frame passive. |
| **25** | Gray `TWindow`: frame active. |
| **26** | Gray `TWindow`: frame icons. |
| **27** | Gray `TWindow`: scroll bar page. |
| **28** | Gray `TWindow`: scroll bar controls. |
| **29** | Gray `TWindow`: scroller normal. |
| **30** | Gray `TWindow`: scroller selected. |
| **31** | Gray `TWindow`: reserved. |
| **32** | Gray `TDialog` local **1**: frame passive. |
| **33** | Gray `TDialog` local **2**: frame active. |
| **34** | Gray `TDialog` local **3**: frame icon. |
| **35** | Gray `TDialog` local **4**: scroll bar page. |
| **36** | Gray `TDialog` local **5**: scroll bar controls. |
| **37** | Gray `TDialog` local **6**: `TStaticText`. |
| **38** | Gray `TDialog` local **7**: `TLabel` normal. |
| **39** | Gray `TDialog` local **8**: `TLabel` selected. |
| **40** | Gray `TDialog` local **9**: `TLabel` shortcut. |
| **41** | Gray `TDialog` local **10**: `TButton` normal. |
| **42** | Gray `TDialog` local **11**: `TButton` default. |
| **43** | Gray `TDialog` local **12**: `TButton` selected. |
| **44** | Gray `TDialog` local **13**: `TButton` disabled. |
| **45** | Gray `TDialog` local **14**: `TButton` shortcut. |
| **46** | Gray `TDialog` local **15**: `TButton` shadow. |
| **47** | Gray `TDialog` local **16**: `TCluster` normal. |
| **48** | Gray `TDialog` local **17**: `TCluster` selected. |
| **49** | Gray `TDialog` local **18**: `TCluster` shortcut. |
| **50** | Gray `TDialog` local **19**: `TInputLine` normal. |
| **51** | Gray `TDialog` local **20**: `TInputLine` selected. |
| **52** | Gray `TDialog` local **21**: `TInputLine` arrows. |
| **53** | Gray `TDialog` local **22**: history arrow. |
| **54** | Gray `TDialog` local **23**: history sides. |
| **55** | Gray `TDialog` local **24**: `THistoryWindow` scroll page. |
| **56** | Gray `TDialog` local **25**: `THistoryWindow` scroll controls. |
| **57** | Gray `TDialog` local **26**: `TListViewer` normal. |
| **58** | Gray `TDialog` local **27**: `TListViewer` focused. |
| **59** | Gray `TDialog` local **28**: `TListViewer` selected. |
| **60** | Gray `TDialog` local **29**: `TListViewer` divider. |
| **61** | Gray `TDialog` local **30**: `TFileInfoPane`. |
| **62** | Gray `TDialog` local **31**: `TCluster` disabled. |
| **63** | Gray `TDialog` local **32**: reserved. |
| **64** | Blue `TDialog` local **1**: frame passive. |
| **65** | Blue `TDialog` local **2**: frame active. |
| **66** | Blue `TDialog` local **3**: frame icon. |
| **67** | Blue `TDialog` local **4**: scroll bar page. |
| **68** | Blue `TDialog` local **5**: scroll bar controls. |
| **69** | Blue `TDialog` local **6**: `TStaticText`. |
| **70** | Blue `TDialog` local **7**: `TLabel` normal. |
| **71** | Blue `TDialog` local **8**: `TLabel` selected. |
| **72** | Blue `TDialog` local **9**: `TLabel` shortcut. |
| **73** | Blue `TDialog` local **10**: `TButton` normal. |
| **74** | Blue `TDialog` local **11**: `TButton` default. |
| **75** | Blue `TDialog` local **12**: `TButton` selected. |
| **76** | Blue `TDialog` local **13**: `TButton` disabled. |
| **77** | Blue `TDialog` local **14**: `TButton` shortcut. |
| **78** | Blue `TDialog` local **15**: `TButton` shadow. |
| **79** | Blue `TDialog` local **16**: `TCluster` normal. |
| **80** | Blue `TDialog` local **17**: `TCluster` selected. |
| **81** | Blue `TDialog` local **18**: `TCluster` shortcut. |
| **82** | Blue `TDialog` local **19**: `TInputLine` normal. |
| **83** | Blue `TDialog` local **20**: `TInputLine` selected. |
| **84** | Blue `TDialog` local **21**: `TInputLine` arrows. |
| **85** | Blue `TDialog` local **22**: history arrow. |
| **86** | Blue `TDialog` local **23**: history sides. |
| **87** | Blue `TDialog` local **24**: `THistoryWindow` scroll page. |
| **88** | Blue `TDialog` local **25**: `THistoryWindow` scroll controls. |
| **89** | Blue `TDialog` local **26**: `TListViewer` normal. |
| **90** | Blue `TDialog` local **27**: `TListViewer` focused. |
| **91** | Blue `TDialog` local **28**: `TListViewer` selected. |
| **92** | Blue `TDialog` local **29**: `TListViewer` divider. |
| **93** | Blue `TDialog` local **30**: `TFileInfoPane`. |
| **94** | Blue `TDialog` local **31**: `TCluster` disabled. |
| **95** | Blue `TDialog` local **32**: reserved. |
| **96** | Cyan `TDialog` local **1**: frame passive. |
| **97** | Cyan `TDialog` local **2**: frame active. |
| **98** | Cyan `TDialog` local **3**: frame icon. |
| **99** | Cyan `TDialog` local **4**: scroll bar page. |
| **100** | Cyan `TDialog` local **5**: scroll bar controls. |
| **101** | Cyan `TDialog` local **6**: `TStaticText`. |
| **102** | Cyan `TDialog` local **7**: `TLabel` normal. |
| **103** | Cyan `TDialog` local **8**: `TLabel` selected. |
| **104** | Cyan `TDialog` local **9**: `TLabel` shortcut. |
| **105** | Cyan `TDialog` local **10**: `TButton` normal. |
| **106** | Cyan `TDialog` local **11**: `TButton` default. |
| **107** | Cyan `TDialog` local **12**: `TButton` selected. |
| **108** | Cyan `TDialog` local **13**: `TButton` disabled. |
| **109** | Cyan `TDialog` local **14**: `TButton` shortcut. |
| **110** | Cyan `TDialog` local **15**: `TButton` shadow. |
| **111** | Cyan `TDialog` local **16**: `TCluster` normal. |
| **112** | Cyan `TDialog` local **17**: `TCluster` selected. |
| **113** | Cyan `TDialog` local **18**: `TCluster` shortcut. |
| **114** | Cyan `TDialog` local **19**: `TInputLine` normal. |
| **115** | Cyan `TDialog` local **20**: `TInputLine` selected. |
| **116** | Cyan `TDialog` local **21**: `TInputLine` arrows. |
| **117** | Cyan `TDialog` local **22**: history arrow. |
| **118** | Cyan `TDialog` local **23**: history sides. |
| **119** | Cyan `TDialog` local **24**: `THistoryWindow` scroll page. |
| **120** | Cyan `TDialog` local **25**: `THistoryWindow` scroll controls. |
| **121** | Cyan `TDialog` local **26**: `TListViewer` normal. |
| **122** | Cyan `TDialog` local **27**: `TListViewer` focused. |
| **123** | Cyan `TDialog` local **28**: `TListViewer` selected. |
| **124** | Cyan `TDialog` local **29**: `TListViewer` divider. |
| **125** | Cyan `TDialog` local **30**: `TFileInfoPane`. |
| **126** | Cyan `TDialog` local **31**: `TCluster` disabled. |
| **127** | Cyan `TDialog` local **32**: reserved. |
| **128** | `THelpWindow` local **1**: frame passive. |
| **129** | `THelpWindow` local **2**: frame active. |
| **130** | `THelpWindow` local **3**: frame icon. |
| **131** | `THelpWindow` local **4**: scroll bar page. |
| **132** | `THelpWindow` local **5**: scroll bar controls. |
| **133** | `THelpWindow` local **6**: `THelpViewer` normal text. |
| **134** | `THelpWindow` local **7**: `THelpViewer` keyword. |
| **135** | `THelpWindow` local **8**: `THelpViewer` selected keyword. |

---

## See also

- [`examples/palette/README.md`](examples/palette/README.md) — palette walk tutorial.
- [`README.md`](README.md) — `TPalette`, `mapColor`, extended attributes.
