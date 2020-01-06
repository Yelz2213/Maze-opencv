# Maze-opencv
## Installation Guide
1. Opencv2 Required. Link : <https://opencv.org/releases/>
2. Copy opencv/build/include to your solutiondir.
3. C/C++ -> General -> Additonal Dependencis : input the path where you placed the include downloaded from opencv
4. Linker -> input -> Additional Dependencis : opencv_world420.lib & opencv_world420d.lib.
5. Copy opencv_world420.dll & opencv_world420d.dll from opencvdir to solutiondir.

## The concept of the Maze Game
1. Using an array to create a map
2. Render the maze by for-loop, the maze is combined by walls
3. Spawn the player in the place of '2'
4. The player can play in first-person and third-person
5. allow the player to move around the map using the WASD keys
6.  when the player manages to get to '3' the finish the map (win)
7. the player can not pass through or move walls, '1' but can walk on empty tiles '0' as well as over the exit 'E' 

## Controller
F1: first person view  
F2: third person view  
UP: move forward   
DOWN: move back  
LEFT: left turn  
RIGHT: right turn  
