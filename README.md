# TS2 Visible Hood FX
## About
Allows neighbourhood-exclusive effects such as particle effects and decals to appear in lot view.

| Vanilla | Mod |
| :-----: | :-: |
| <img src="media/vanillafx.png" width="450"> | <img src="media/modfx.png" width="450"> |

## Requirements
- The Sims 2: Ultimate Collection <ins>**OR**</ins> The Sims 2 disc version with all EPs and SPs.
- [Sims2RPC](https://modthesims.info/d/648220/sims2rpc-modded-sims-2-launcher-for-mansion-and-garden.html) <ins>**OR**</ins>
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader).

## Known Issues
The lot skirt is much simpler than the neighbourhood terrain in terms of functionality, which causes a few minor issues with the creation
of neighbourhood effects in lot view:

- Decals that overlap the current lot bounds will be cut off.
- The lot skirt only supports a single overlay layer for shadows, whereas the neighbourhood terrain supports two (normals and shadows).
Because of this, decals will be drawn on the shadow layer and appear on top of roads.

Several effects require something known as an 'effect map' to be positioned correctly in the world. The lot skirt lacks this, meaning these
effects will spawn either in the sky, or directly on the current lot. To prevent this, the following effects have been blacklisted from appearing
in lot view:

- Gondolas
- Sailboats
- Birds
- Ducks
- Swans
- Ocean surfs

See the [Configuration](#configuration) section for more information on blacklisting effects.

## Installation
**Sims2RPC Users**

1. Download `TS2VisibleHoodFX.zip`, found under the [Releases](https://github.com/spockthewok/TS2VisibleHoodFX/releases/latest) section of this repository.
2. Extract both the `.asi` and `.ini` files to your `\TSBin\mods` directory, found wherever you have the Sims 2 installed to. For example, on my machine,
they would be extracted to:

   `E:\Games\The Sims 2\Fun with Pets\SP9\TSBin\mods`

**Ultimate ASI Loader Users**

1. Download Ultimate ASI Loader from [here](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/Win32-latest/dsound-Win32.zip).
2. Extract `dsound.dll` from the zip file and place it in the game's `\TSBin` directory. On my machine, it would go here:

   `E:\Games\The Sims 2\Fun with Pets\SP9\TSBin`
3. Download `TS2VisibleHoodFX.zip`, found under the [Releases](https://github.com/spockthewok/TS2VisibleHoodFX/releases/latest) section of this repository.
4. Extract both the `.asi` and `.ini` files to the same `\TSBin` directory Ultimate ASI Loader was extracted to.

## Configuration
The included `TS2VisibleHoodFX.ini` file contains all of the effects blacklisted from appearing in lot view. Effects can be blacklisted by adding them to the
config file and vice versa.

Each effect you wish to blacklist should be on a separate line &mdash; an example of the correct structure can be seen below:

```
[Blacklist]
effect1 = nhood_object1_name
effect2 = nhood_object2_name
effect3 = nhood_object3_name
```

> [!NOTE]
> The name of the neighbourhood object that creates the effect should be added to the blacklist, rather than the name of the effect itself.

The plugin falls back to a default built-in blacklist if either the config file is missing, or the config file doesn't contain the `[Blacklist]` section.
To blacklist no effects, simply remove all of the entries listed under `[Blacklist]`, like so:

```
[Blacklist]
```

## Thanks
[metayeti](https://github.com/metayeti), for [mINI](https://github.com/metayeti/mINI/).

[LazyDuchess](https://github.com/LazyDuchess), for the hooking code used in this mod.