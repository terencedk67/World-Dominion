# World Dominion

**A bigger world. A bolder you.**

WORLD DOMINION is a premium, open-world geopolitical action/strategy game
built in Unreal Engine 5.4: lead a nation as its head of state, physically
walk the halls of power, ride in the presidential motorcade, fly between
capitals, negotiate with foreign leaders, and steer your country's economy,
military, and intelligence apparatus — all from data-driven systems built to
scale from one country to a hundred.

This repository currently contains the **Phase 1 vertical slice**
architecture: the full C++ subsystem/component/interface layer, the
United States/Washington D.C. opening scenario assembled procedurally by
`AWDWashingtonLevelScript`, and every piece of gameplay data (11 nations,
the opening mission, the first world event) needed to play it.

- **`Docs/GETTING_STARTED.md`** — the master build checklist: one ordered
  path from "clone this repo" to "playing the vertical slice with real art."
- **`Docs/ARCHITECTURE.md`** — module layout, how the data-driven
  country/mission/event pipeline works, and why specific engineering
  tradeoffs were made (Enhanced Input, vehicle movement, AI, UMG).
- **`Docs/EDITOR_SETUP.md`** — step-by-step guide to opening this project in
  UE 5.4, compiling it, and authoring the handful of binary assets (levels,
  UMG widgets) Phase 1 needs on top of the code.
- **`Docs/PLACEHOLDER_ASSETS.md`** — every placeholder currently shipping
  (all built from engine-default primitives in C++) paired with the final
  art/animation it needs to be replaced with.
- **`Docs/VISUAL_FIDELITY_ROADMAP.md`** — the concrete, step-by-step path to
  the reference key art's quality bar: MetaHuman characters, Fab environment/
  vehicle packs, and exact lighting/post-process settings.

Built with C++ (core systems) + Blueprint/UMG (level scripting, final UI
layout) on Enhanced Input, Gameplay Tags, Data Assets, World Partition,
Chaos, and Niagara.
