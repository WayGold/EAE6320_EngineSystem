# EAE6320 ENGINE SYSTEM



## Where to Find AI Movement System
Go to Engine/AIService

This project provided a few algorithms to move your character using the built in rigidbody struct. These movement steering algorithms and orientation steering algorithms are acceleration based so that the character could move and turn smoothly.  

## To use the algorithms
### link the project into your solution and add reference accordingly, AIService project is the only thing needed. 
### Include the AIService.h file into your project where you want to get steering output. The AIService.h header includes all the available algorithms’ headers and an update function that needs to be called at your MyGame’s UpdateSimulationBasedOnTime to apply the steering output to the rigidbody. 
### To get steering output from any algorithms, aftering initializing the algorithm's object and assigning the necessary references(your character’s rigidbody and in some case the target’s rigidbody), simply call getSteering from the object. 

Sounds complicated but it’s actually very simple if you check the example in my MyGame.cpp line [107](https://github.com/WayGold/EAE6320_EngineSystem/blob/main/MyGame_/MyGame/cMyGame.cpp#L107) in the AI_TEST region. The initialization and cleanup examples could be found at line [299](https://github.com/WayGold/EAE6320_EngineSystem/blob/main/MyGame_/MyGame/cMyGame.cpp#L299) as well.   
One very important thing to remember is the orientation of the model must face the negative z direction to have a correct orientation steering.
To download the project, visit the github repo here: https://github.com/WayGold/EAE6320_EngineSystem  
