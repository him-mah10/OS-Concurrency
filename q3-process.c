#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/wait.h>
#include <unistd.h>

void merge(int arr[],int start,int mid,int end)
{
	int left[10004],right[10004],ll=mid-start+1,rl=end-mid;
	for(int i=0;i<ll;i++)
		left[i]=arr[start+i];
	for(int i=0;i<rl;i++)
		right[i]=arr[mid+1+i];
	int a=0,b=0,c=0;
	while(a<ll && b<rl)
	{
		if(left[a]<right[b])
		{
			arr[c+start]=left[a];
			a++;
			c++;
		}
		else
		{
			arr[c+start]=right[b];
			b++;
			c++;
		}
	}
	while(a<ll)
	{
		arr[c+start]=left[a];
		a++;
		c++;
	}
	while(b<rl)
	{
		arr[c+start]=right[b];
		b++;
		c++;
	}
}

void mergesort(int arr[],int start,int end)
{
	int len=end-start+1;
	if(len<=5)
	{
		//selection sort
		int min,temp;
		for(int i=0;i<len;i++)
		{
			int min=start+i;
			for(int j=i+1;j<len;j++)
			{
				if(arr[start+j]<arr[min])
				{
					min=start+j;
				}
			}
			temp=arr[min];
			arr[min]=arr[start+i];
			arr[start+i]=temp;
		}
	}
	else
	{
		pid_t left,right;
		left=fork();
		if(left==0)
		{
			mergesort(arr,start,start+len/2-1);
			_exit(0);
		}
		else
		{
			right=fork();
			if(right==0)
			{
				mergesort(arr,start+len/2,end);
				_exit(0);
			}
		}
		int status;
		waitpid(left,&status,0);
		waitpid(right,&status,0);
		merge(arr,start,start+len/2-1,end);
	}

}

int main()
{
	int shmid, *arr,n;
	key_t key=IPC_PRIVATE;
	scanf("%d",&n);
	shmid=shmget(key, sizeof(int)*n, IPC_CREAT|0666);
	arr=shmat(shmid,NULL,0);
	for(int i=0;i<n;i++)
	{
		scanf("%d",&arr[i]);
	}
	mergesort(arr,0,n-1);
	for(int i=0;i<n;i++)
	{
		printf("%d   ",arr[i]);
	}
	return 0;
}