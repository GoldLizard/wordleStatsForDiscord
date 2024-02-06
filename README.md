Make scores.txt in the same directory and copy over the discord chat logs for the whole month. Thats it.
If 3/6 is the lowest score and 2 people got 3/6 for that day, both players get a point. 

Program has 3 functions

1. input scores.txt
  for each line{
    if username line{
      remember that username
    }
    if score line{
      create day and player objects if they dont exist
      record score
    }
    all other random lines are ignored
  }
    
2. calculate scores
   for each day{
     for each player{
       search for lowest score
     }
     for each player{
       award points
       remember if anyone scored 2/6 or 1/6
     }
   }

3. print stats
   
