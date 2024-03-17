# LDtk-SFML-Game

A little project to show how to use LDtkLoader with SFML to render LDtk levels and create a simple topdown game.

- [x] Performant level layers rendering using sf::VertexArray
- [x] Player movement
- [x] Camera following the player
- [x] Simple AABB collisions
- [x] In game reload of the LDtk world (press F5)

You are free to use this code in your own game.

### Dependencies

- [SFML](https://github.com/SFML/SFML), needs to be installed on your system
- [LDtkLoader](https://github.com/Madour/LDtkLoader), automatically fetched by cmake

### Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Video

You can see the demo of this project in this video :

https://user-images.githubusercontent.com/11854124/122610077-57f48a00-d07f-11eb-98ac-278a3d3dbd01.mp4



