# Reaching the Reference Key-Art Quality Bar

The four reference images (poster, Washington gameplay, Nation Select,
global map) are the visual contract for WORLD DOMINION. This document is the
concrete, technical path from where the project stands today — code and
systems, with primitive-shape placeholders — to that quality bar, inside
Unreal Engine 5.4. **This is fundamentally an art-production task, not a
programming one**: no amount of C++/Blueprint work substitutes for the
character, environment, and lighting art the reference images show. What
follows is the specific, actionable checklist for whoever *does* have an
Unreal Editor open to work through.

Every item below assumes UE 5.4, the free Fab marketplace (fab.com, Epic's
account), and this repository already open as `WorldDominion.uproject`.

## 1. Characters — MetaHuman (free, built into UE5)

The single highest-leverage step. MetaHuman gives photorealistic
faces/skin/hair/eyes for free, cloud-created, with a UE5-native retarget
path — this is what closes most of the gap between the current placeholder
cylinders and the reference art's close-up character shots.

**Create:**
1. In the Editor: Window → Quixel Bridge (or the standalone MetaHuman
   Creator at metahuman.unrealengine.com) → sign in with the same Epic
   account as the project.
2. Build one MetaHuman per named role that needs a close-up (see the roster
   `AWDWashingtonLevelScript::SpawnStaffNPCs` already spawns): the
   President/leader, Chief of Staff, Secretary of State, a generic
   "Foreign Delegation" ambassador, and one "Secret Service Agent" template
   (suits, sunglasses, earpiece — reuse this MetaHuman for every
   `AWDSecurityAgent` instance rather than making eight unique ones).
3. Aim for the reference art's specific look: dark tailored suits, groomed
   hair, neutral-to-serious expression presets, mid-30s-to-50s age range,
   business-formal — MetaHuman Creator's wardrobe presets include suits;
   for a closer match to the key art's red tie / gold lapel pin, a Marketplace
   clothing pack (search "business suit MetaHuman") or a simple material
   recolor on the stock suit gets there quickly.
4. Bridge exports the MetaHuman as a Skeletal Mesh + Animation Blueprint +
   physics asset directly into `/Game/MetaHumans/<Name>/`.

**Wire it in — no C++ changes needed:**
1. Create a Blueprint child of `AWDPlayerCharacter` (e.g. `BP_President`),
   `AWDGovernmentNPC` (`BP_ChiefOfStaff`, etc.), or `AWDSecurityAgent`
   (`BP_SecurityAgent`).
2. In the Blueprint's Class Defaults / Components panel, set the inherited
   `Mesh` component's Skeletal Mesh to the MetaHuman's body mesh, and add
   the MetaHuman's face mesh + Anim Blueprint per Epic's standard MetaHuman
   setup (Face component parented to the body, driven by the MetaHuman
   Animation Blueprint).
3. That's it — every character class in this codebase calls
   `WDCharacterVisuals::ApplyPlaceholderSwap(GetMesh(), PlaceholderBodyMesh)`
   in `BeginPlay()` (see `Source/WorldDominion/Core/WDCharacterVisuals.h`),
   which detects a real Skeletal Mesh via `GetSkeletalMeshAsset()` and
   automatically hides the placeholder cylinder and shows the real mesh.
   Nothing to toggle by hand.
4. Retarget existing/Marketplace animations to the MetaHuman skeleton with
   UE5's built-in IK Retargeter (Epic ships a MetaHuman ↔ UE5 Mannequin
   retarget profile out of the box) so `AWDPlayerCharacter`'s locomotion and
   `AWDSecurityAgent`'s follow-walk work immediately.

## 2. Environments — Fab

Search Fab (fab.com) for, in priority order:

- **"government building exterior"**, **"neoclassical building pack"**,
  **"White House"** — several Washington/neoclassical-architecture packs
  exist; look for Nanite-enabled, PBR-material listings.
- **"luxury office interior"**, **"executive office pack"**, **"marble
  corridor"** — for the Oval Office / Situation Room / West Wing corridors.
- **"Washington DC street pack"** or a general **"modern city street pack"**
  for the motorcade route dressing (street furniture, barriers, lamp posts —
  the reference gameplay shot's crowd barriers and press pen).
- Quixel Megascans surfaces/props (now folded into Fab, largely free for
  UE) for ground materials — asphalt, wet-stone plaza, marble floors — which
  do most of the work for the "wet-road reflections" look from the brief.

**Where it plugs in:** replace
`AWDWashingtonLevelScript::BuildWhiteHousePlaceholder`'s procedurally-spawned
cube/cylinder massing with a real Static/Nanite Mesh actor placed in the
level (or keep the level script for gameplay-actor spawning and just build
real geometry around it in the level itself — the level script only touches
its own spawned placeholder components, it won't fight with hand-placed art).

## 3. Vehicles — Fab

Search **"presidential limousine"**, **"SUV pack"**, **"police motorcycle
pack"**. `AWDPresidentialVehicle` (Vehicles/) already has driver/protectee/
2× security seat sockets and a kinematic waypoint-follow system ready to
receive any Static Mesh — reassign `PlaceholderBodyMesh`'s mesh to the real
vehicle model and adjust the seat `USceneComponent` offsets to match its
actual interior. Upgrading to full Chaos Vehicle physics (so the player can
drive) additionally needs the model rigged with wheel bones — see the
"Chaos Vehicles" note in `Docs/ARCHITECTURE.md`.

## 4. Lighting & post-process — no art required, just settings

This is the one pillar achievable with configuration alone, and it carries a
lot of the reference art's mood (dusk light, warm gold rim-light, filmic
contrast). Baseline Lumen/Nanite/shadow settings are already in
`Config/DefaultEngine.ini`. In each level:

1. **Directional Light**: rotate to a low golden-hour elevation (~15-20°),
   warm color temperature (~3000-3300K), intensity ~3-4 lux, "Atmosphere Sun
   Light" enabled so it drives the Sky Atmosphere.
2. **Sky Atmosphere + Volumetric Clouds**: add both actors for the dramatic
   layered sky the poster and gameplay shots share; a light haze density
   sells the "hazy dusk skyline" look.
3. **Post Process Volume** (unbound, covering the whole level):
   - Color Grading → Shadows: slight cool-navy tint; Midtones: neutral to
     very slightly warm; Highlights: warm gold push.
   - Film → increase Slope (contrast) modestly, keep Toe/Shoulder gentle to
     avoid crushing blacks (the reference art keeps shadow detail).
   - Bloom: intensity ~0.4-0.6, threshold ~1.0 — enough for practical
     lights/headlights to glow without blooming the whole frame.
   - Vignette intensity ~0.3-0.4, subtle Chromatic Aberration (~0.2) for the
     cinematic-lens read.
   - Depth of Field: enable for dialogue/cutscene cameras (Cine Camera
     Actor), keep off or very shallow for gameplay so HUD/objectives stay
     legible.
4. **Reflections**: Lumen Reflections (already on) plus a wet-look material
   parameter (lower roughness, subtle normal ripple) on street/plaza
   materials for the "wet-road reflections" the brief calls out explicitly.
5. If the target GPU supports it, flip `r.Lumen.HardwareRayTracing` to
   `True` in `DefaultEngine.ini` for the ray-traced reflections/GI the key
   art's glassy skyscraper shot implies; it's left `False` as the safe
   default for hardware that doesn't support it.

## 5. Animation

- **MetaHuman Animator** (free, uses an iPhone with a TrueDepth camera, or a
  stereo head-rig) for the Chief of Staff / dialogue-scene facial
  performances referenced in `AWDGovernmentNPC::DialogueLines`.
- Marketplace mocap packs: search **"business walk cycle"**, **"formal
  idle pack"**, **"tactical security walk"** for `AWDSecurityAgent` and
  `AWDPlayerCharacter` locomotion that reads as "deliberate and powerful"
  per the brief, not a generic soldier crouch-walk.

## Priority order

If doing this incrementally, this order gives the fastest visible return:

1. **Lighting & post-process** (section 4) — free, no assets, changes how
   *everything* already in the project reads immediately.
2. **MetaHuman characters** (section 1) — free, and close-ups are what the
   reference images sell hardest.
3. **Environment art** (section 2) — the biggest cost/time item; start with
   just the White House exterior + one interior room (Situation Room) rather
   than the whole level.
4. **Vehicles** (section 3), **animation polish** (section 5).

## What's already done on the code side

Every character class (`AWDPlayerCharacter`, `AWDSecurityAgent`,
`AWDGovernmentNPC`) auto-detects a real Skeletal Mesh and swaps off its
placeholder with zero extra code (`WDCharacterVisuals::ApplyPlaceholderSwap`).
`AWDPresidentialVehicle`'s seats and movement are mesh-agnostic. Nothing in
the mission/dialogue/HUD/diplomacy systems reads mesh or material data, so
none of it needs to change as real art comes in — this checklist is purely
additive.
