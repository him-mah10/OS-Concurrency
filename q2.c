#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct Booth Booth;
typedef struct Evm Evm;
typedef struct Voter Voter;

struct Evm
{
	int index,flag,no_of_slots,votersRemaining;
	//flag == 0 => No voting going on
	//flag == 1 => Voting going on
	pthread_t evm;
	Booth * booth;
};

struct Voter
{
	int index,status;
	//status == 0 => newVoter
	//status == 1 => waitingVoter
	//status == 2 => assignedVoter
	pthread_t voter;
	Booth * booth;
	Evm * evm;
};

struct Booth
{
	int index,no_of_voters,doneVoters,no_of_evms;
	pthread_t booth;
	Voter ** voters;
	Evm ** evms;
	short left;
	pthread_mutex_t mutex;
	pthread_cond_t c1,c2;
};

Booth * booth_init(int index,int numofEVMS,int numofVoters)
{
	Booth * booth=(Booth*)malloc(sizeof(Booth));
	booth->left=0;
	booth->index=index;
	booth->left++;
	booth->no_of_voters=numofVoters;
	booth->doneVoters=0;
	booth->no_of_evms=numofEVMS;
	booth->voters=(Voter **)malloc(sizeof(Voter*)*numofVoters);
	booth->evms=(Evm**)malloc(sizeof(Evm*)*numofEVMS);
	pthread_mutex_init(&(booth->mutex),NULL);
	pthread_cond_init(&(booth->c1),NULL);
	pthread_cond_init(&(booth->c2),NULL);
	return booth;
}

Evm* evm_init(int index, Booth * booth)
{
	Evm * evm = (Evm*)malloc(sizeof(Evm));
	evm->no_of_slots=0;
	evm->flag=0;
	evm->votersRemaining=0;
	evm->index=index;
	evm->booth=booth;
	return evm;
}

Voter* voter_init(int index, Booth * booth)
{
	Voter * voter = (Voter*)malloc(sizeof(Voter));
	voter->index=index;
	voter->booth=booth;
	voter->evm=NULL;
	voter->status=0;
	return voter;
}

void voter_wait_for_evm(Voter * voter)
{
	pthread_cond_t *c1=&(voter->booth->c1);
	pthread_mutex_t * mutex_booth = &(voter->booth->mutex);
	pthread_mutex_lock(mutex_booth);
	voter->status=1;
	while(voter->status==1)
		pthread_cond_wait(c1,mutex_booth);
	pthread_mutex_unlock(mutex_booth);
}

void * voterThreadFunc(void * args)
{
	int temp=0;
	Voter * voter = (Voter*)args;
	temp++;
	pthread_cond_t *c1=&(voter->booth->c1), *c2 = &(voter->booth->c2);
	temp++;
	pthread_mutex_t * mutex_booth = &(voter->booth->mutex);
	voter_wait_for_evm(voter);
	temp--;	
	Evm * evm =  voter->evm;
	pthread_mutex_lock(mutex_booth);
	while(evm->flag==0)
		pthread_cond_wait(c1,mutex_booth);
	(evm->no_of_slots)--;
	temp--;
	printf("Voter %d at Booth %d has casted his/her vote%d\x1b[0m\n",voter->index+1,evm->booth->index+1,evm->index+1);
	temp--;
	pthread_cond_broadcast(c2);
	temp++;
	pthread_mutex_unlock(mutex_booth);
}

void * evmThreadFunc(void * args)
{
	int temp=0;
	Evm * evm=(Evm*)args;
	int remaingEvms=0;
	Booth * booth = evm->booth;
	temp+=2;
	pthread_cond_t *c1=&(booth->c1),*c2=&(booth->c2);
	pthread_mutex_t *mutex_booth = &(booth->mutex);
	temp--;
	while(1)
	{
		pthread_mutex_lock(mutex_booth);
		temp++;
		remaingEvms++;
		if(booth->doneVoters == booth->no_of_voters)
		{
			pthread_mutex_unlock(mutex_booth);
			break;
		}
		pthread_mutex_unlock(mutex_booth);
		int slots=(rand()%10)+1;
		pthread_mutex_lock(mutex_booth);
		evm->no_of_slots=slots;
		evm->flag=0;
		pthread_mutex_unlock(mutex_booth);
		printf("\x1b[32mEvm %d at Booth %d is free with slots = %d\x1b[0m\n",evm->index+1,booth->index+1,slots);
		int j=0;
		while(1)
		{
			if(j==slots)
				break;
			int i = rand()%booth->no_of_voters;
			pthread_mutex_lock(mutex_booth);
			if(booth->voters[i]->status==1)
			{
				booth->voters[i]->evm=evm;
				printf("Voter %d at Booth %d got allocated EVM %d\x1b[0m\n",i+1,booth->index+1,evm->index+1);
				(booth->doneVoters)++;
				booth->voters[i]->status=2;
				j++;
			}
			if(booth->doneVoters==booth->no_of_voters)
			{
				pthread_mutex_unlock(mutex_booth);
				break;
			}
			pthread_mutex_unlock(mutex_booth);	
		}
		printf("Evm %d at Booth %d is moving to voting stage.\x1b[0m\n",evm->index+1,booth->index+1);
		pthread_mutex_lock(mutex_booth);
		evm->flag=1;
		evm->no_of_slots=j;
		pthread_cond_broadcast(c1);
		while(evm->no_of_slots)
			pthread_cond_wait(c2,mutex_booth);
		pthread_mutex_unlock(mutex_booth);
		printf("\x1b[32mEvm %d at booth %d has finished voting stage.\x1b[0m\n",evm->index+1,booth->index+1);
	}
}

void * boothThreadFunc(void * args)
{
	Booth * booth = (Booth*)args;
	for(int i=0;i<booth->no_of_voters;i++)
	{
		booth->voters[i]=voter_init(i,booth);
	}
	for(int i=0;i<booth->no_of_evms;i++)
	{
		booth->evms[i]=evm_init(i,booth);
	}
	for(int i=0;i<booth->no_of_voters;i++)
	{
		pthread_create(&(booth->voters[i]->voter),NULL,voterThreadFunc,booth->voters[i]);
	}
	for(int i=0;i<booth->no_of_evms;i++)
	{
		pthread_create(&(booth->evms[i]->evm),NULL,evmThreadFunc,booth->evms[i]);
	}
	int i=0;
	while(1)
	{
		if(i<booth->no_of_evms)
		{
			pthread_join(booth->evms[i]->evm,0);
			i++;
		}
		else
			break;
	}
	i=0;
	while(1)
	{	
		if(i<booth->no_of_voters)
			pthread_join(booth->voters[i]->voter,0);
		else
			break;
		i++;
	}
	printf("Voters at booth number %d are done with voting.\x1b[0m\n",booth->index+1);
}

int main()
{
	int no_of_booths;
	scanf("%d",&no_of_booths);
	int * numofEVMS = (int*)malloc(sizeof(int)*no_of_booths);
	int * numofVoters = (int*)malloc(sizeof(int)*no_of_booths);
	Booth ** booths = (Booth**)malloc(sizeof(Booth*)*no_of_booths);
	for(int i=0;i<no_of_booths;i++)
	{
		scanf("%d %d",&numofVoters[i],&numofEVMS[i]);
	}

	for(int i=0;i<no_of_booths;i++)
	{
		booths[i]=booth_init(i,numofEVMS[i],numofVoters[i]);
	}
	int threadCount=0;
	for(int i=0;i<no_of_booths;i++)
	{
		pthread_create(&(booths[i]->booth),NULL,boothThreadFunc,booths[i]);
		threadCount++;
	}
	
	for(int i=0;i<no_of_booths;i++)
	{
		pthread_join(booths[i]->booth,NULL);
	}
	if(threadCount==0)
		exit(0);
	free(numofVoters);
	free(numofEVMS);
	for(int i=0;i<no_of_booths;i++)
		free(booths[i]);
	free(booths);
	return 0;
}
