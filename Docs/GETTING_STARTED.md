# Getting Started — Master Build Checklist

One consolidated, ordered checklist from "I have this repo" to "I'm playing
the Washington D.C. vertical slice with real art." Each phase links to the
document with the full detail; this page is the sequence and the time/cost
reality check.

## Phase 0 — Prerequisites (one-time)

- [ ] A Windows or Mac machine with a dedicated GPU (Unreal Engine 5 with
      Lumen/Nanite needs one — a laptop with only integrated graphics will
      struggle) and **~100 GB free disk space** (engine + project + derived
      data cache).
- [ ] An Epic Games account (free) — needed for the Launcher, MetaHuman, and
      Fab.
- [ ] Install **Unreal Engine 5.4** via the Epic Games Launcher
      (Unreal Engine tab → Library → + → pick version 5.4).
- [ ] Git installed, and access to
      `github.com/terencedk67/World-Dominion` (pull request #1 carries all
      the work so far).

## Phase 1 — Get the code compiling

1. `git clone` the repo, check out branch `claude/world-dominion-architecture-7960et`
   (or `main` once PR #1 is merged).
2. Double-click `WorldDominion.uproject`. Unreal will offer to build the
   missing C++ modules — accept.
3. First compile will take a while (several minutes to ~half an hour
   depending on machine). Fix anything the compiler flags — see
   `Docs/ARCHITECTURE.md`'s "First open & compile" note: this code was
   written carefully against the real UE 5.4 API but has not been
   compiled in this environment (no engine available here), so treat the
   first build as a review pass, not a guaranteed one-shot green build.
4. If you hit an error you can't place, paste it back to me — I can read
   the exact file/line and fix it from here, then push the fix.

**→ Full detail:** `Docs/EDITOR_SETUP.md`, section 1.

## Phase 2 — Make the vertical slice playable (binary assets)

Everything below is a `.uasset`/`.umap` — binary Unreal content I cannot
create from a text-only session, so this is manual Editor work, but it's
short (a few hours, not days):

- [ ] Create 3 levels: `MainMenu`, `NationSelect`, `Washington_DC`
      (`Content/WorldDominion/Maps/`).
- [ ] In `Washington_DC`: add a Nav Mesh Bounds Volume, build navigation,
      drop in one `AWDWashingtonLevelScript` instance. Press Play — it
      procedurally builds the White House placeholder, spawns the staff
      NPCs and motorcade dressing, and starts the opening mission by
      itself.
- [ ] Create `WBP_HUD` (parent class `WDHUDWidget`) and implement its five
      events with real UMG widgets — this is the one piece of real UI
      layout work Phase 1 needs; everything it should display is already
      wired (objectives, influence/treasury/allies/approval, interaction
      prompts, Presidential View toggle, headlines).
- [ ] Create `WBP_Decision` (parent `WDDecisionWidget`) so the Situation
      Room's crisis event has somewhere to render.
- [ ] Create `WBP_MainMenu` and `WBP_NationSelect` (parents
      `WDMainMenuWidget` / `WDNationSelectWidget`) and wire their level
      references.
- [ ] Set each level's GameMode Override in World Settings to
      `AWDGameMode`.

**→ Full detail:** `Docs/EDITOR_SETUP.md`, section 2. **→ What's a
placeholder vs. what needs final art:** `Docs/PLACEHOLDER_ASSETS.md`.

## Phase 3 — Play it end to end

MainMenu → New Game → NationSelect → pick United States → Washington_DC.
Confirm: motorcade + White House massing visible, HUD shows "Attend
Emergency Summit" with its objectives, security agents follow you,
`[`/`]` toggles Presidential View, talking to the staff NPCs completes
objectives, walking into the Situation Room trigger surfaces the chip-
shortage decision, resolving it updates the top-line stats and starts the
follow-up mission. This confirms every system actually works together
before spending any time on art.

## Phase 4 — Make it look like the reference key art

This is the long pole, and it's art production, not code:

1. **Lighting & post-process first** — free, no assets, and changes how
   everything already in the project reads immediately (dusk directional
   light, Sky Atmosphere, a warm/gold color grade, wet-look ground
   materials).
2. **MetaHuman characters** — free, cloud-created; closes most of the gap
   on close-up shots. The code already auto-swaps a placeholder for a real
   Skeletal Mesh the moment you assign one (`WDCharacterVisuals::
   ApplyPlaceholderSwap`) — no extra code needed on your end.
3. **Environment art from Fab** — the White House exterior, then one
   interior room (Situation Room) before the rest. This is the item most
   likely to cost real money (one-time asset-pack purchases) or real time
   (commissioning an environment artist) if the free options don't get
   close enough.
4. **Vehicles**, then **animation polish** (mocap packs / MetaHuman
   Animator for dialogue scenes).

**→ Full detail, exact settings, and search terms:**
`Docs/VISUAL_FIDELITY_ROADMAP.md`.

## Phase 5 — Keep building

Phase 1's systems (missions, countries, diplomacy, economy, world events,
travel, vehicles, save system) already scale past the vertical slice — more
countries are CSV rows, more missions are JSON entries, more world events
are JSON entries, no new code required. Come back here for anything that
*does* need new code: more nations' opening scenes, the World Map's UMG
layout (backing logic — `UWDWorldMapWidget` — already exists), Oval Office/
Situation Room level scripting, richer dialogue, or Phase 3/4 features
(international travel, elections, intelligence operations).
