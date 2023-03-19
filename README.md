# Game Third Person Action Game  

Project about simple action game with third person character.  
It has one level and contain enemies, pickupable and platforms.  
Created using free asset from Unreal marketplace and Mixamo. 

## Character :  
- Able to walk/run, jump and sprint.  
- Can pick up weapon and attacking when armed.  
- Has simple pause menu where player can save, load or quit the game.  
- Has stamina bar to be used with sprinting mechanics.  
- Has health and coin variable and death state when health is 0.  

## Enemies :  
- There is 4 variants that inherit from parent class.  
- Has simple AI that can detect move to target.  
- Has hit box to attack player. 
- Has different hit values and health based on type.   

## Weapon :   
- There is 4 variants with different damaging stats.  
- Can be pickup by player and override the previous player weapon.  
- Has trail effect and hit box.  

## Pickupable :  
- One item as pickup (coin).  
- One item as hazard (bomb/explosion).  
- Coin will add player inventory.  
- Bomb will reduce player health.  
- Both inherit from same parent class, but has different behaviour (polymorphism).  

## Level :  
- One main level where player start the game.  
- Main level has spawn volume for enemies and pickups.  
- Has simple flying platforms for player platformer element.  
- Has door and switch mechanic.   
- Has finish line (volume) for player finish.   
- There is one more empty level for player land after finish from previous level.  


Download build for play testing here :  
https://drive.google.com/file/d/1Jbdp7x9sgNJ2Gfu1x9hspua25_QWUT3F/view?usp=share_link   

Assets : 
https://www.mixamo.com/  
https://www.unrealengine.com/marketplace/en-US/product/fxville-s-blood-vfx-pack  
https://www.unrealengine.com/marketplace/en-US/product/infinity-blade-enemies  
https://www.unrealengine.com/marketplace/en-US/product/infinity-blade-effects  
https://www.unrealengine.com/marketplace/en-US/product/infinity-blade-weapons  
https://www.unrealengine.com/marketplace/en-US/product/sun-temple  
https://github.com/DruidMech/LearnCPPForUnrealEngineUnit2  

Thank you!  