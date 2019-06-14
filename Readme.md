<h1 align="center">OS Concurrency</h1>

#### Author - Himanshu Maheshwari

#### Implementation details 

1) It contains a thread called creator(with function name creatorFunc) which always run in background. It creates new player/refree based on the number of remaining player/refree and also if there if sufficient number of players and refrees and organizer is free then it also ensures that 2 players and 1 refree meets the organizer. Now at first it generates a random number modulo remaining number of players and refrees. If this number is in range [0, remaining refree) then a refree has come else a player has come. Based on this player/refree thread is created. As soon as this refree thread/player thread is created, it invokes the corresponding function, where it has to wait till its time come(i.e. it has to wait for the organizer)(this is done through pthread_cond_wait). As soon as the organizer is free then a signal is broadcast to all the player/refree and the players and refree who came earliest are made to play(this is done by chaning isplaying/isjuding to 1)(to ensure that the earliest coming person is playing we use lowestPlayer/lowestRefree). Thus they play. To get when organizer is free we have variables runningRefree and runningPlayer. When it is 0 that means the organizer is free.

2) In this program there are two threads - one for voters and other for evm which mutually depend upon each other. The structure of the program is that there is a booth structure which contains evm structure and voter structure inside it. After intialization, a thread is created for each booth function. Now every thread creates a new thread for each evm and a new thread for each voter. Now these threads mutually depend upon each other. Now every voter waits till he/she gets a confirmation from the evm. Meanwhile evm decides upon how many slots it has. After that it assigns which voter shall vote and changes the status of those voters to 2. After that it broadcast saying that voters are free to vote. After that all those voters whose status is 2 cast their vote and evm waits till they have casted their vote. If any voter is left to cast his/her vote then the whole process is repeated.	

__For quesions please refer to Assignment4.pdf__

#### How to Run?
To run, type followint command in the terminal - "gcc -pthread <file-name>" and after that type "./a.out". For question 3, where time taken by file to execute is needed type "time ./a.out". The time in this is given by "real".  

#### Cheers!!!
