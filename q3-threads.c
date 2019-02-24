#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node node;

int arr[10004],temp[10004];
pthread_mutex_t mutex;
struct node
{
    int l,r;
};

void merge(int start,int mid,int end)
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

void * mergesort(void * temp)
{
    node * elem = (node *)temp;
    int l=elem->l;
    int r=elem->r;
    if(l>=r)
    {
        pthread_exit(0);
    }
    if((r-l)<5)
    {
        pthread_mutex_lock(&mutex);
        for(int i=l;i<=r;i++)
        {
            int min=i;
            for(int j=i+1;j<=r;j++)
            {
                if(arr[j]<arr[min])
                    min=j;
            }
            int temp=arr[min];
            arr[min]=arr[i];
            arr[i]=temp;
        }
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
    }
    node leftBound,rightBound;
    int m=(l+r)/2;
    pthread_t leftId,rightId;
    leftBound.l=l;
    leftBound.r=m;
    rightBound.l=m+1;
    rightBound.r=r;
    pthread_create(&leftId,NULL,mergesort,&leftBound);
    pthread_create(&rightId,NULL,mergesort,&rightBound);
    pthread_join(leftId,NULL);
    pthread_join(rightId,NULL);
    pthread_mutex_lock(&mutex);
    merge(l,m,r);
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

int main()
{
    int n;
    scanf("%d",&n);
    for(int i=0;i<n;i++)
        scanf("%d",&arr[i]);
    node temp;
    temp.l=0;
    temp.r=n-1;
    pthread_t root;
    pthread_create(&root,NULL,mergesort,&temp);
    pthread_join(root,NULL);
    for(int i=0;i<n;i++)
    {
        printf("%d  ",arr[i]);
    }
    return 0;
}