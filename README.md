# Elixia GameEngine

Hey Folks,

this is a personal hobby project, on which I worked to make it simpler to program games. I wanted something between raw C++ programming with SDL and the big game engines like Unity / Unreal etc. This project is far from done, but the aims are:

-   Easily build levels based on tilemaps. The engine should handle loading images containing all the textures, multiple layers of maps and of cause collision
-   Add enemies & props through single C++ Commands
-   Handle all the player movement & inventory etc.

Those are just

Basically you should be able to create a simple action RPG like the first Zelda games with this.
Though the engine should remain modular so that you can just use the parts you need. E.g. if you want to program Pong, you don't need a tile map, so you don't need to include it in the header files.

## ToDo

### Simple features

-   [x] Load Map saved in XML and texture
    -   [ ] Map scrolling
-   [ ] Build example maps to play around with
-   [x] Handle basic player movement with collisions
-   [ ] Enemy management & some kind of simple ai
    -   [ ] Path finding / static paths?
    -   [ ] Aiming
-   [ ] Some kind of player weapons
-   [ ] Effects / Projectiles
-   [ ] Sounds
-   [ ] Menu
    -   [ ] Entry / Main Menu
    -   [ ] Pause Menu
    -   [ ] Settings Menu / Ingame Command Prompt to add enemies?
-   [ ] Minimap for better map overview?
-   [ ] Multiplayer?
-   [ ] Implement an actual game to prove the engine is usable

As you see, there is still a lot to do. The project is still in its starting blocks. As I am busy with studying, this state will continue for now.
