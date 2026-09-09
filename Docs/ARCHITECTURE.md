# WORLD DOMINION — Phase 1 Architecture

This document explains what was built, why it's structured the way it is, and
what an engineer opening this project in Unreal Engine 5.4 needs to know
before pressing Compile for the first time.

## Environment constraint this project was authored under

This codebase was written in a Linux CLI environment with **no Unreal Engine
installation and no access to the Epic Games Launcher/UnrealBuildTool**. That
means:

- Every `.cpp`/`.h` file here was written to the real UE 5.4 C++ API from
  careful, deliberate knowledge of that API, but **has not been compiled or
  run**. Treat this as a thorough, review-ready first pass, not a
  guaranteed-green build. Compile it first thing (see `EDITOR_SETUP.md`) and
  fix whatever the compiler finds — the architecture and gameplay wiring are
  sound, but a header path or macro on this scale can still be wrong in a
  place a human reviewer needs to catch.
- No `.uasset`/`.umap` (binary) files exist in this repository. Everything
  that must be a binary asset — the actual Washington_DC level, UMG widget
  Blueprints, Input Action/Mapping Context assets, materials, meshes,
  animations — is **not included** and is called out explicitly below and in
  `PLACEHOLDER_ASSETS.md`. Where a system could be made to work without an
  authored asset (Enhanced Input, White House massing, vehicle placeholders,
  country/mission/event data), it was, specifically so the vertical slice is
  testable the moment someone opens the project in-editor.

## Module layout

```
Source/WorldDominion/
  Core/            GameMode, GameState, PlayerController, GameInstance,
                    shared enums/structs (WDTypes.h), native Gameplay Tags
  Core/Input/       (reserved for a future authored Input Config data asset)
  Characters/      AWDPlayerCharacter, UWDPresidentialViewComponent
  Interaction/     IWDInteractable / IWDMissionTarget / IWDDiplomaticActor /
                    IWDVehicleInteractable, UWDInteractionComponent
  Government/      UWDPresidentialSecurityComponent, AWDGovernmentNPC,
                    UWDInfluenceComponent
  AI/              AWDSecurityAgent, AWDSecurityAgentController
  Countries/       FWDCountryStats, UWDCountryDataAsset, UWDCountrySubsystem
  Diplomacy/       FWDRelationshipState, UWDDiplomacySubsystem,
                    UWDRelationshipComponent
  Economy/         UWDEconomySubsystem (generic FWDStatEffect application)
  Military/        UWDMilitarySubsystem (Phase 4 scaffold)
  Intelligence/    UWDIntelligenceSubsystem (Phase 4 scaffold)
  Missions/        FWDMissionObjective/Definition, UWDMissionDataAsset,
                    UWDMissionSubsystem, AWDMissionTriggerVolume
  WorldEvents/     FWDWorldEventDefinition, UWDWorldEventDataAsset,
                    UWDWorldEventSubsystem, AWDWorldEventManager
  Travel/          UWDTravelSubsystem, AWDCountryTravelNode
  Vehicles/        AWDPresidentialVehicle, AWDMotorcadeManager
  UI/              AWDHUD, UWDHUDWidget, UWDMainMenuWidget,
                    UWDNationSelectWidget, UWDDecisionWidget
  SaveSystem/      UWDSaveGame, UWDSaveSubsystem
  Levels/          AWDWashingtonLevelScript (assembles the vertical slice)
  Data/            (reserved for future native data helpers)
```

Every system above is a `UGameInstanceSubsystem` or component, never a
God-object on `AWDGameMode`/`AWDPlayerController`. New gameplay is added by
adding data (a CSV row, a JSON entry, a new `UWDCountryDataAsset`/
`UWDMissionDataAsset`/`UWDWorldEventDataAsset` instance) or a small new
`IWDInteractable`/`IWDMissionTarget` actor — not by editing a switch
statement keyed on a specific country.

## Data-driven, not hardcoded: how country/mission/event data actually loads

Three subsystems (`UWDCountrySubsystem`, `UWDMissionSubsystem`,
`UWDWorldEventSubsystem`) share one loading pattern:

1. **Asset Manager first.** Each registers a `PrimaryAssetType` (`Country`,
   `Mission`, `WorldEvent` — see `DefaultGame.ini`) and, at `Initialize()`,
   asks the Asset Manager for every instance of that type under
   `/Game/WorldDominion/Data/<Type>s/`. This is the intended long-term path:
   a content designer creates a `UWDCountryDataAsset`
   (`DA_Country_China`, etc.), fills in art references, and it just appears.
2. **Plain-text fallback.** If step 1 finds nothing (true today — no
   `.uasset` instances exist yet), each subsystem parses a plain-text file
   from `Content/WorldDominion/Data/`:
   - `Countries.csv` — 11 nations (USA, China, UK, France, Germany, Russia,
     India, Japan, Brazil, Saudi Arabia, UAE), one row each, all
     `FWDCountryStats` fields from the design brief.
   - `Missions.json` — the vertical slice's `AttendEmergencySummit` mission
     and its `RespondToGlobalCrisis` follow-up.
   - `WorldEvents.json` — the `GlobalChipShortage` three-option crisis from
     the brief, wired to complete the `RespondToCrisis` objective.

Nothing about country/mission/event *count* or *identity* is compiled into
C++. Add a 12th country by adding a CSV row (see `FWDCountryStats` field
order in the header comment) or a `UWDCountryDataAsset` instance — no code
changes, no UI changes (Nation Select reads `UWDCountrySubsystem::
GetAllCountryIDs()`).

The one place that intentionally *does* pattern-match on a fixed vocabulary
is `FWDCountryStats::ModifyNumericStat`/`FWDRelationshipState::
ModifyDimension` — a small, closed set of *stat channel names*
(`"Treasury"`, `"PublicApproval"`, `"Trust"`, ...), not countries. That's
the generic effect-routing table every decision/event/diplomatic action
targets via `FWDStatEffect`, and it's the correct place for that kind of
fixed vocabulary to live.

## The vertical slice end-to-end

`AWDWashingtonLevelScript` (Levels/) is a single actor that, dropped into a
level and given `BeginPlay`, reproduces the whole opening sequence from the
brief without any other manual placement:

1. Builds a White House exterior placeholder from engine basic-shape meshes
   (`BuildWhiteHousePlaceholder`).
2. Spawns the Chief of Staff, Secretary of State, and a foreign delegation
   NPC (`AWDGovernmentNPC`) with the brief's scripted dialogue lines, each
   wired to complete its matching mission objective on `Interact`.
3. Spawns a parked limousine + two escort SUVs (`AWDPresidentialVehicle`) as
   motorcade dressing, owned by an `AWDMotorcadeManager` ready for a later
   scripted departure.
4. Spawns an `AWDMissionTriggerVolume` standing in for the Situation Room
   door: walking into it completes `EnterSituationRoom` +
   `AttendNSCMeeting` and fires the `GlobalChipShortage` world event.
5. Starts the `AttendEmergencySummit` mission.

From there: `AWDPlayerController::BeginPlay` creates the HUD widget, which
listens to `UWDMissionSubsystem::OnObjectiveStateChanged` for the objectives
panel and `AWDGameState::OnStatsChangedDelegate` for the top-line stats.
`AWDPlayerController::OnPossess` spawns/positions the Secret Service detail
via `UWDPresidentialSecurityComponent`, which follows the player
automatically from that point on — nothing in the level script manages
agents directly. Resolving the crisis in the Decision UI
(`UWDDecisionWidget` → `UWDWorldEventSubsystem::ResolveEvent`) applies its
`FWDStatEffect`s through `UWDEconomySubsystem`, completes `RespondToCrisis`,
which completes the mission, which auto-starts `RespondToGlobalCrisis`
(`FWDMissionDefinition::NextMissionID`) — a live example of the
"decisions feed real systems, systems create new missions" loop the brief
asks for.

## Deliberate scope decisions (and why)

- **Chaos Vehicles.** `AWDPresidentialVehicle` uses simple kinematic
  waypoint-following (`MoveAlongWaypoints`), not
  `AWheeledVehiclePawn`/`UChaosWheeledVehicleMovementComponent`. A real Chaos
  wheeled vehicle needs per-wheel bone data on a Skeletal Mesh, which does
  not exist without an authored vehicle mesh. Building that setup in C++
  with placeholder geometry would produce code that either doesn't compile
  cleanly against a missing skeleton or silently fails to drive correctly,
  with no way to verify it in this environment. The seat/interaction
  contract (`IWDVehicleInteractable`) is deliberately physics-agnostic, so
  swapping to a real Chaos Vehicle later (Phase 3's "personally drive"
  feature) touches only `AWDPresidentialVehicle`'s movement internals —
  nothing else in the game needs to change.
- **Behavior Trees / State Trees.** `AWDSecurityAgent` uses a small native
  C++ state machine instead of a `.uasset` Behavior Tree/Blackboard, again
  because those are binary assets this environment can't author. The
  `StateTree`/`GameplayStateTreeModule` plugins are still enabled in the
  `.uproject` so a designer can migrate agent logic to a real State Tree
  asset later without a plugin-enablement step.
- **Enhanced Input without `.uasset` Input Actions.** `AWDPlayerCharacter`
  constructs its `UInputAction`/`UInputMappingContext` objects natively in
  C++ (`BuildNativeInputAssets`) with the same modifier stack (swizzle/negate)
  Epic's own Third Person template ships, so the vertical slice has working
  WASD + mouse-look + gamepad input with zero input-asset authoring. A
  content team can replace this with an authored `IMC_Default` asset later;
  only `SetupPlayerInputComponent`'s binding calls would need to move.
- **UMG layouts.** Every UI class (`UWDHUDWidget`, `UWDMainMenuWidget`,
  `UWDNationSelectWidget`, `UWDDecisionWidget`) is a C++ base class exposing
  `BlueprintImplementableEvent`s and `BlueprintCallable` actions. The actual
  gold/navy/crimson premium layout from the key art must be built as a UMG
  Widget Blueprint child in-editor (`WBP_HUD` etc.) — that's real visual
  design work no amount of C++ can substitute for. See
  `PLACEHOLDER_ASSETS.md`.

## What's next (Phase 2+, per the brief's phase plan)

Phase 1 is complete per the brief's own checklist: project architecture,
character controller, interaction system, mission system, CountryData
system, basic HUD backing, a Washington test scene, a White House exterior
placeholder, and the security-agent follower system. Phase 2's items
(presidential motorcade *cinematics*, White House interior, Oval
Office/Situation Room *art*, richer NPC dialogue, the World Map *UI*, more
world events) are architecturally unblocked — the subsystems they need
(`UWDDiplomacySubsystem`, `UWDWorldEventSubsystem`, `UWDTravelSubsystem`,
`AWDMotorcadeManager`) already exist and only need level art, UMG layout,
and additional data entries layered on top.
