#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

int remPlayers,remRefrees,availPlayers,availRefrees,runningPlayer,runningRefree,whichPlayer,whichRefree,n,lowestRefree,lowestPlayer;
struct player * players;
struct refree * refrees;
pthread_cond_t c;
pthread_mutex_t m;

struct player
{
	int index,isplaying;
	pthread_t playerThreadId;
};

struct refree
{
	int index,isjuding;
	pthread_t refreeThreadId;
};

void * refreeFunc(void * args)
{
	struct refree *arg=(struct refree *)args;
	printf("\x1b[32mRefree %d has entered the academy\x1b[0m\n",arg->index);
	pthread_mutex_lock(&m);
	while(arg->isjuding==0)
		pthread_cond_wait(&c,&m);
	pthread_mutex_unlock(&m);
	printf("\x1b[33mRefree %d has met the organizer\x1b[0m\n",arg->index);
	printf("\x1b[36mRefree %d has entered the court\x1b[0m\n",arg->index);
	printf("Refree %d is adjusting the equipment\n",arg->index);
	sleep(0.5);
	printf("Refree %d has adjusted the equipment\n",arg->index);
	sleep(0.6);
	printf("\x1b[31mRefree %d has started the game\x1b[0m\n",arg->index);
	runningRefree--;//Should be at the end
}

void * playerFunc(void * args)
{
	struct player * arg=(struct player *)args;
	printf("\x1b[32mPlayer %d has entered the academy\x1b[0m\n",arg->index);
	pthread_mutex_lock(&m);
	while(arg->isplaying==0)
		pthread_cond_wait(&c,&m);
	pthread_mutex_unlock(&m);
	printf("\x1b[33mPlayer %d has met the organizer\x1b[0m\n",arg->index);
	printf("\x1b[36mPlayer %d has entered the court\x1b[0m\n",arg->index);
	printf("Player %d is warming up\n",arg->index);
	sleep(1);
	pthread_mutex_lock(&m);
	runningPlayer--;//Should be at the end 
	pthread_mutex_unlock(&m);
}

void * creatorFunc(void * args)
{
	while((remRefrees+remPlayers)>0)
	{
		sleep(rand()%3);
		int i=rand()%(remPlayers+remRefrees);
		if(i>=0&&i<remRefrees)
		{
			pthread_create(&(refrees[whichRefree].refreeThreadId),NULL,refreeFunc,&refrees[whichRefree]);
			whichRefree++;
			availRefrees++;
			remRefrees--;
			//printf("availRefrees = %d   availPlayers = %d   runningPlayer = %d    runningRefree = %d",availRefrees,availPlayers,runningPlayer,runningRefree);
		}
		else
		{
			pthread_create(&(players[whichPlayer].playerThreadId),NULL,playerFunc,&players[whichPlayer]);
			availPlayers++;
			remPlayers--;
			whichPlayer++;
			//printf("availRefrees = %d   availPlayers = %d   runningPlayer = %d    runningRefree = %d",availRefrees,availPlayers,runningPlayer,runningRefree);
		}
		if(availPlayers>=2 && availRefrees>=1 && runningPlayer==0 && runningRefree==0)
		{
			availRefrees-=1;
			availPlayers-=2;
			//change is of whichplayer and whichplayer+1
			players[lowestPlayer].isplaying=1;
			players[lowestPlayer+1].isplaying=1;
			//change is of whichrefree
			refrees[lowestRefree].isjuding=1;
			//broadcast
			pthread_cond_broadcast(&c);	
			runningRefree+=1;
			lowestRefree++;
			lowestPlayer+=2;
			runningPlayer+=2; 
			//printf("OUT");
		}
	}
	for(int i=0;i<2*n;i++)
		pthread_join(players[i].playerThreadId,NULL);

	//for(int i=0;i<n;i++)
	//	pthread_join(refrees[i].refreeThreadId,NULL);
}

int main()
{
	srand(time(0));
	availRefrees=0;
	availPlayers=0;
	runningRefree=0;
	runningPlayer=0;
	whichPlayer=0;
	whichRefree=0;
	lowestPlayer=0;
	lowestRefree=0;
	scanf("%d",&n);
	remRefrees=n;
	remPlayers=2*n;
	players = (struct player*)malloc(sizeof(struct player)*n*2);
	refrees = (struct refree*)malloc(sizeof(struct refree)*n);
	for(int i=0;i<2*n;i++)
	{
		players[i].index=i+1;
		players[i].isplaying=0;
	}
	for(int i=0;i<n;i++)
	{
		refrees[i].index=i+1;
		refrees[i].isjuding=0;
	}
	pthread_cond_init(&c,NULL);
	pthread_mutex_init(&m,NULL);
	pthread_t creator;
	pthread_create(&creator,NULL,creatorFunc,NULL);
	/*printf("ERE");
	while((remPlayers+remRefrees)>0)
	{
		

		sleep(0.1);
	}
	printf("ERE");*/
	pthread_join(creator,NULL);
	return 0;
}