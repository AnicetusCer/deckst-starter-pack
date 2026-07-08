# Bulldog

`Bulldog` is a small original Atari ST starter game for DeckST. It is inspired
by playground stop-and-go games such as "red light, green light", but uses
original characters, rules, title, art, and code.

## Core Pitch

A little fox starts at the bottom of the screen and must reach the bulldog at
the far end before the timer runs out. The fox may only move while the bulldog's
back is turned. When the bulldog turns around, the fox must stop immediately.

Each level adds moving obstacles that cross the playfield. The player must plan
movement windows around both the bulldog's attention and the obstacle patterns.

## Target Platform

- Atari ST / STE
- 320x200 low-resolution display
- 16-color palette
- Joystick movement
- One fire button for dash or confirm
- Keyboard fallback for development
- Single floppy-friendly build target

## Controls

- Joystick up: move fox toward the bulldog
- Joystick left/right: sidestep
- Joystick down: slow/backstep, optional
- Fire: short dash while the bulldog is turned away
- Fire on title/results: confirm

The game should be fully playable with only joystick up/left/right and fire.

## Main Rules

- The fox starts at the near edge of the field.
- The bulldog sits at the far edge.
- While the bulldog faces away, movement is allowed.
- While the bulldog faces forward, movement is forbidden.
- If the fox is moving when watched, the fox is sent back to the level start.
- Touching an obstacle sends the fox back to the level start.
- Reaching the finish line clears the level.
- Running out of time loses one attempt or restarts the level.

## Timing

The bulldog should telegraph each turn so the game feels fair.

- Back turned: player may move.
- Warning: bulldog twitches or growls before turning.
- Facing forward: player must be still.
- Reset: bulldog turns away again.

Later levels shorten the warning window and vary the safe movement window.

## Level Plan

### Level 1

- No obstacles.
- Long movement windows.
- Clear tutorial pacing.

### Level 2

- One slow horizontal obstacle.
- Long timer.

### Level 3

- Two horizontal obstacles at different speeds.

### Level 4

- One vertical sweeper obstacle.
- Slightly shorter safe windows.

### Level 5

- Mixed horizontal and vertical movement.
- First shorter countdown timer.

### Level 6

- Obstacles move in lanes with gaps.
- Player must pause in safe pockets.

### Level 7

- Bulldog turn timings become less regular.
- Warning remains visible and fair.

### Level 8

- Faster obstacles.
- Narrower lane gaps.

### Level 9

- Moving obstacle pattern forces side movement.
- Timer pressure is meaningful.

### Level 10

- Final mix of fast lanes, irregular bulldog timing, and tight timer.
- Clear win screen after completion.

## Visual Direction

- Simple bright playground field.
- Fox character should read clearly at small sprite size.
- Bulldog should have two obvious states:
  - back turned;
  - facing forward.
- Obstacles can be floating balls, kites, clouds, or toy carts.
- Avoid realistic violence; failure should be playful.

## Audio Direction

- Short title jingle.
- Footstep or soft move tick.
- Bulldog growl/warning chirp.
- Freeze mistake sound.
- Level clear fanfare.

Audio can be optional for the first playable build.

## Implementation Notes

- Use fixed-point or integer movement.
- Keep sprites small and palette-limited.
- Avoid scrolling for version 1.
- Represent levels as small static structs or tables.
- Keep collision boxes simple rectangles.
- Store all timings in ticks so levels can be tuned without rewriting logic.

## Legal Model

- Bulldog game code: MIT.
- Bulldog original art/audio: CC0 or CC BY 4.0.
- Disk image: redistributable with source, attribution, and build notes.
- Do not include copyrighted characters, names, music, samples, or cloned art.

## First Playable Scope

- Title screen.
- One level.
- Fox movement.
- Bulldog attention cycle.
- Stop/move detection.
- One obstacle lane.
- Countdown timer.
- Win/restart states.

Once the first playable is reliable, add the remaining nine levels as data.
