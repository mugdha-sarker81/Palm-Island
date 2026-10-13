# Palm Island — Game Plan

**Status:** Planning proposal  
**Based on:** `idea.txt` (Game Design Document v1.0)  
**Purpose:** Clarify the game's story, progression, combat, weapons, enemies, and level structure before implementation. Recommendations below are proposals, not final decisions.

## 1. Core concept

**Palm Island is a side-scrolling action-adventure about exploring a mysterious island, learning how its five legendary weapons work, and proving Ichigo is worthy of the power behind Excalibur.**

The main reward is discovery and new ways to play, not grinding for larger numbers. Exploration, combat, puzzles, story clues, and weapon abilities should reinforce one another.

## 2. Story direction

Ichigo arrives on Palm Island after discovering an old symbol connected to the island. He wants to uncover its source, rather than simply collect treasure. Although the island appears abandoned, he finds signs of recent visitors: a camp, moved objects, and a journal entry.

The island's five guardians protect weapons created to test five qualities:

- **Thornfang:** Agility
- **Embercleaver:** Strength
- **Stormpiercer:** Awareness
- **Dawnstring:** Precision
- **Voidfang:** Courage

The weapons were not made to help someone conquer the island. They test whether a successor is worthy of protecting it. Excalibur is the final proof of that worthiness, rather than merely a stronger sword.

Deliver the story mainly through short inscriptions, journals, NPC dialogue, guardian encounters, and environmental details. The main story should remain understandable without optional lore.

### Story question to resolve

The GDD describes the island as apparently uninhabited but also includes Driftwood Village and NPCs. A possible reconciliation is that the village is a small, isolated community while the rest of the island is abandoned and guarded.

## 3. Progression and leveling

Use two distinct meanings of "level":

- **World levels:** the island's regions and their platforming areas.
- **Character levels:** XP-based ranks for Ichigo.

### Recommendation: no XP levels in the first version

Let the player's growth come from new weapon abilities, discoveries, and permanent upgrades. This avoids making combat feel like a grind and keeps progression tied to exploration.

Possible optional rewards:

- Health upgrades
- Weapon improvements that change or enhance an ability
- Lore fragments
- Cosmetics
- Shortcuts and hidden challenge rewards

Optional upgrades should help but should not be required to finish the main story.

### Proposed main progression

1. Begin on Palmshore Beach and learn movement and basic combat with a starter weapon.
2. Reach Driftwood Village and learn about the island's legend.
3. Explore the Forgotten Jungle, discover its clue, defeat Thornback, and earn Thornfang.
4. Use Thornfang to cut barriers and open a shortcut into a previously explored area.
5. Follow clues toward the other regions. Weapon abilities open routes and puzzles, rather than simply acting as keys at the end of linear levels.
6. Earn the remaining four weapons and learn the story behind the guardians.
7. Open the Excalibur Sanctuary and complete its final trials.
8. Earn Excalibur and resolve the story's question about responsibility and protecting the island.

The five major regions could be completed in a fixed order or in a few different orders. This is an open design decision.

## 4. Combat direction

### Core fight loop

**Read an enemy's wind-up → dodge or reposition → attack during recovery → use a weapon ability when it creates an advantage.**

Keep early combat small and responsive:

- A short, readable basic attack combo.
- A dodge with a brief invulnerability window and enough recovery to discourage careless spamming.
- Distinct attack wind-ups, impact feedback, and recovery windows.
- Health and checkpoints as the initial survival systems.
- Add stamina only if playtesting shows combat needs another resource.
- Give weapons different play styles, rather than making each newly acquired weapon strictly stronger.

Exact control bindings can be chosen later, once the intended keyboard and controller experience is clearer.

## 5. Weapon roles

Every weapon should matter in combat and exploration. Its use should be visually clear to the player.

| Weapon | Quality | Combat role | Exploration and puzzle role |
|---|---|---|---|
| **Thornfang** | Agility | Fast three-hit combo; recovery slash after a dodge | Cut thorn barriers; follow root markings |
| **Embercleaver** | Strength | Slow, heavy attacks that break guard | Smash cracked stone; hold heavy pressure plates |
| **Stormpiercer** | Awareness | Long thrust; charged strike with deliberate timing | Power crystal nodes; connect electrical mechanisms |
| **Dawnstring** | Precision | Aimed shots and a charged shot | Hit distant switches; cut ropes; solve light-alignment puzzles |
| **Voidfang** | Courage | Short shadow dash and counter | Cross shadow gates; bypass magical barriers |

### Excalibur decision

The game has five legendary weapons plus Excalibur. To avoid an unwieldy sixth weapon slot, the current recommendation is to make Excalibur a final earned weapon or transformation used in the climax, rather than another weapon that must be managed alongside all five.

## 6. Enemy roles

Enemies should teach one lesson at a time and have distinct, readable behaviors.

- **Jungle Slime:** slow, forgiving enemy that teaches basic movement and spacing.
- **Island Raider:** melee enemy that approaches, telegraphs a strike, and punishes careless attacks.
- **Ruin Wraith:** slow projectile attacker that encourages use of cover, movement, or ranged attacks.
- **Armored guardian minion:** blocks careless attacks and encourages heavy or well-timed attacks.

Introduce unfamiliar enemies separately before combining them. Ordinary enemies should complement, not duplicate, the region's guardian.

## 7. Guardian encounter pattern

Each guardian should teach and test the quality represented by its weapon:

1. **Observe:** demonstrate a readable attack pattern.
2. **Adapt:** give the player a chance to dodge, counter, use the environment, or choose a suitable weapon.
3. **Master:** increase pressure as health falls without adding unfair or unreadable attacks.

Use clear visual and audio telegraphs. A missed attack should create a recognizable recovery opportunity. The guardian should teach the player how to use its reward.

### Example: Thornback

- Telegraphs a charge with a visible wind-up.
- Leaves a recovery opening if the charge misses.
- Later adds a second charge direction or a new attack that still has a learnable tell.
- The fight rewards observation and agile repositioning.
- Earning Thornfang lets the player cut a barrier and revisit a previously inaccessible route.

## 8. Region and level structure

The regions in the GDD provide the world map:

1. Palmshore Beach
2. Driftwood Village
3. Forgotten Jungle
4. Ember Caverns
5. Sunken Temple
6. Skywatch Cliffs
7. Lost Kingdom
8. Excalibur Sanctuary

Each region can use this reusable structure:

1. **Arrival:** establish the region's look and a safe route.
2. **Exploration:** reveal a blocked path, landmark, or mystery that invites curiosity.
3. **Challenge:** teach the region's enemy, hazard, or puzzle idea.
4. **Clue:** point toward the weapon or guardian, without requiring a long exposition scene.
5. **Guardian:** test the region's combat and observation lesson.
6. **Reward and return:** award the weapon and open a shortcut or a newly usable route.

This creates satisfying region-sized adventures while allowing the island to remain interconnected. Each region will still need a smaller room-and-path layout; the region list by itself is not a detailed level map.

## 9. Recommended first design exercise

Before planning all regions in detail, outline the Forgotten Jungle as a sequence of areas. For each area, decide:

- What the player sees on arrival.
- What the player is expected to notice or learn.
- What blocks the way and how it is hinted at.
- Which enemy or hazard appears, and what it teaches.
- Where the clue and optional secret are found.
- How the Thornback encounter tests the player's learning.
- How Thornfang changes the return trip.

The existing GDD proposes a 10–15 minute Forgotten Jungle first playable milestone. Use this small adventure to test whether movement, combat, clues, secrets, and the weapon reward work together before expanding the rest of the island.

## 10. Decisions still needed

1. What personal reason brings Ichigo to the island?
2. Who lives in Driftwood Village, and what do they know about the island?
3. Must the five major regions be completed in a fixed order, or can players choose between several?
4. Is Excalibur a sixth weapon, a final transformation, or a story-only reward?
5. Should the game have XP-based character levels? Recommendation: no, at least for the first version.
6. What control scheme should be prioritized: keyboard, controller, or both equally?
7. What is the exact Forgotten Jungle route, including its rooms, encounters, and shortcut?

## 11. Reference cleanup

Some external image links in the current GDD appear unrelated to Palm Island. Treat them as temporary inspiration links, not approved visual direction. Replace them with original concept art or clearly relevant, permission-appropriate references as the art direction is finalized.
