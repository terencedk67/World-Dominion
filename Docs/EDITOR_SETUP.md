# Getting This Project Running in UE 5.4

This repository was built without access to the Unreal Editor or
UnrealBuildTool, so the steps below have not been run end-to-end by the
author — follow them as the intended path and expect to fix small issues
(a missing include, a renamed API in your exact 5.4 point release) as they
surface. `ARCHITECTURE.md` explains why each system is built the way it is
if you need context while debugging.

## 1. First open & compile

1. Install **Unreal Engine 5.4** (5.4.x; the `.uproject`'s
   `EngineAssociation` is `"5.4"`).
2. Right-click `WorldDominion.uproject` → **Generate Visual Studio project
   files** (Windows) or run `GenerateProjectFiles.sh`/use the equivalent
   Xcode/Rider flow on Mac/Linux.
3. Build the `WorldDominionEditor` target in your IDE, **or** just
   double-click the `.uproject` and let Unreal prompt to build missing
   modules.
4. Fix whatever the compiler surfaces. The most likely categories, given how
   this was authored, are: a header that needs one more `#include` for a
   symbol that's normally pulled in transitively on some engine versions but
   not others, or a UE 5.4-specific API rename versus adjacent versions.
   Every class's responsibility is documented in its header comment, which
   should make misattributed errors easy to place.

## 2. Author the binary assets Phase 1 needs

None of these exist yet (see `PLACEHOLDER_ASSETS.md` for the full list).
Minimum to see the vertical slice running:

1. **Create three levels** under `Content/WorldDominion/Maps/`:
   `MainMenu`, `NationSelect`, `Washington_DC` (these names match
   `Config/DefaultEngine.ini`'s `GameMapsSettings` and the soft level
   references in `UWDMainMenuWidget`/`UWDNationSelectWidget`).
2. In `Washington_DC`, add a **Nav Mesh Bounds Volume** covering the
   playable area (required for `AWDSecurityAgent`'s `AIController->
   MoveToLocation` pathing) and build navigation.
3. Drag an instance of **`AWDWashingtonLevelScript`** into `Washington_DC`
   and press Play. It procedurally builds the White House placeholder,
   spawns the staff NPCs and motorcade dressing, places the Situation Room
   trigger, and starts the `AttendEmergencySummit` mission — see
   `ARCHITECTURE.md` → "The vertical slice end-to-end" for exactly what
   happens and why.
4. Create `WBP_HUD` (parent class `WDHUDWidget`), assign it to
   `AWDPlayerController::HUDWidgetClass`, and implement
   `OnTopLineStatsUpdated` / `OnObjectivesUpdated` /
   `OnInteractionPromptUpdated` / `OnPresidentialViewToggled` / `OnHeadline`
   with real UMG widgets styled per the key art (gold hairlines, translucent
   black panels, serif branding type + sans gameplay type).
5. Create `WBP_Decision` (parent class `WDDecisionWidget`) and add it to the
   HUD or a separate always-present widget so `OnDecisionPresented` has
   somewhere to render when the Situation Room trigger fires
   `GlobalChipShortage`.
6. Create `WBP_MainMenu` (parent `WDMainMenuWidget`) and `WBP_NationSelect`
   (parent `WDNationSelectWidget`), assign their soft level references
   (`NationSelectLevel`, `DefaultStartLevel`), and set `MainMenu`'s level
   Blueprint (or a small native `AWDMainMenuGameMode`) to create/add
   `WBP_MainMenu` to the viewport on `BeginPlay`.
7. Set each level's **GameMode Override** in World Settings:
   `Washington_DC` → an `AWDGameMode` Blueprint child with
   `OpeningMissionID` left blank (the level script starts the mission
   itself) is fine, or just use `AWDGameMode` directly.

## 3. Play it

`MainMenu` → New Game → `NationSelect` (pick United States — it's the only
nation with `Countries.csv` data *and* a Phase 1 level) → `Washington_DC`.
You should get: an establishing view of the parked motorcade and White House
placeholder massing, the HUD showing the "Attend Emergency Summit"
objectives, security agents already following you, `[` `]` (or right
gamepad stick click) toggling Presidential View, `Y`/gamepad-top-face
talking to the Chief of Staff / Secretary of State / delegation NPCs,
walking into the Situation Room trigger surfacing the chip-shortage
Decision popup, and resolving it updating the top-line stats and starting
the follow-up mission.

## 4. Known gaps to expect on first compile/play

- The `Chaos Vehicles` plugin is enabled in the `.uproject` but
  `AWDPresidentialVehicle` does not yet use it (see `ARCHITECTURE.md` for
  why) — if you don't need the plugin for anything else yet, this won't
  cause errors, it's just unused.
- `UWDTravelSubsystem::TravelTo` will log a warning and fail gracefully for
  every destination until a `UWDCountryDataAsset` with a `CapitalLevel` is
  authored — there is only one Phase 1 destination (Washington) by design.
- Multiplayer replication on `AWDGameState` is wired (`DOREPLIFETIME`) but
  untested against an actual networked session; Phase 1 targets single
  player.
