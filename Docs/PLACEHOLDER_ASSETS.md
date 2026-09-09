# Placeholder Asset Manifest

Every placeholder below is built from engine-default content
(`/Engine/BasicShapes/...`) purely in C++, so the project renders *something*
correctly-scaled and correctly-collided the first time it runs — with no
`.uasset` authored yet. This list is the replacement checklist for art/anim.

| System | Class | PLACEHOLDER (shipped) | FINAL ASSET REQUIRED |
|---|---|---|---|
| Player leader | `AWDPlayerCharacter` | Dark scaled cylinder (`PlaceholderBodyMesh`); default `USkeletalMeshComponent` hidden | Rigged leader Skeletal Mesh + full locomotion/interaction Animation Blueprint |
| Security detail | `AWDSecurityAgent` | Dark scaled cylinder | Suited Secret Service agent Skeletal Mesh + anims, earpiece/comms prop, sidearm holster prop |
| Government staff | `AWDGovernmentNPC` | Dark scaled cylinder | Per-role Skeletal Mesh/wardrobe (Chief of Staff, Secretary of State, ambassadors, generals, press secretary, journalists, protesters, civilians) |
| Motorcade | `AWDPresidentialVehicle` | Elongated scaled cube ("Cadillac-One" silhouette) | Presidential limousine, SUV escort, police motorcycle/car, support vehicle meshes — Chaos Vehicle-ready (wheel bones) once real driving lands |
| Travel node | `AWDCountryTravelNode` | Flattened cylinder marker disc | Andrews-style air base signage/tarmac dressing, airport gate dressing per destination |
| White House | `AWDWashingtonLevelScript::BuildWhiteHousePlaceholder` | Cube massing block + portico + 4 columns | Full White House exterior (or a licensed-alike government mansion) environment art, World Partition streamed |
| White House interior | *(not yet built — no level exists)* | — | Oval Office, Situation Room, corridors, staff areas per the key art's "luxurious government interior" direction |
| Presidential View | `UWDPresidentialViewComponent` | Engine custom-depth stencil pass, no post-process material assigned | Bespoke Presidential View post-process material (gold/navy outline + restrained vignette) matching the key art |
| HUD | `UWDHUDWidget` | No visuals — `BlueprintImplementableEvent`s only | `WBP_HUD` UMG layout: top-right leader/influence/treasury/allies/approval, top-left objectives, bottom-left minimap, bottom-right contextual prompt, gold hairlines/translucent black panels per the Washington gameplay reference |
| Main Menu | `UWDMainMenuWidget` | No visuals | `WBP_MainMenu`: WORLD DOMINION logo lockup, New Game/Continue/Settings, poster-quality background per the key art |
| Nation Select | `UWDNationSelectWidget` | No visuals | `WBP_NationSelect`: nation card carousel, portrait/flag/stat-bar layout per the Nation Select reference |
| Decision popup | `UWDDecisionWidget` | No visuals | `WBP_Decision`: cinematic crisis panel + option cards |
| World Map | `UWDWorldMapWidget` | No visuals — tabs/roster/action backing only | `WBP_WorldMap`: the tabbed MAP/TRAVEL/DIPLOMACY/MILITARY/TRADE/INTELLIGENCE/NATIONS command-center UI per the global-map reference, incl. a world map texture with `FWDCountryStats::MapPosition`-driven capital pins |
| Country/leader art | `UWDCountryDataAsset` | `FlagTexture`/`LeaderPortrait` left null (Nation Select falls back to text-only until asset instances are authored) | National flag icons + cinematic leader portrait renders for all 11 (then 100+) nations |
| Input | `AWDPlayerCharacter::BuildNativeInputAssets` | Native C++ `UInputAction`/`UInputMappingContext` (WASD/mouse/gamepad, no icons) | Authored `IMC_Default` + per-action icon set for a remappable-keybinds settings screen |
| Enemy/threat AI | `AWDSecurityAgentController` | Sight perception wired, `OnPerceivedActor` logs only | Threat classification (crowd/NPC alignment tags), automatic protection-state escalation, weapon-ready animation state |

## Also not included (binary assets, out of this environment's reach)

- `Washington_DC.umap`, `MainMenu.umap`, `NationSelect.umap` — every level
  referenced by `Config/DefaultEngine.ini`'s `GameMapsSettings` and by
  `UWDMainMenuWidget`/`UWDNationSelectWidget`'s soft level references must be
  created in-editor. Dropping an `AWDWashingtonLevelScript` instance into a
  new level called `Washington_DC` reproduces the entire opening sequence
  (see `ARCHITECTURE.md`).
- Any `UWDCountryDataAsset`/`UWDMissionDataAsset`/`UWDWorldEventDataAsset`
  instances — fully optional for Phase 1 since the CSV/JSON bootstrap covers
  the vertical slice, but the intended long-term authoring path.
- Materials, Niagara VFX (motorcade dust/exhaust, chopper rotor wash),
  MetaSounds (engine idle, radio chatter, UI stingers).
