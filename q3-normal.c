#include<stdio.h>
int arr[10004];
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
void mergesort(int start,int end)
{
	if(start>=end)
		return;
	int mid=start+(end-start)/2;
	mergesort(start,mid);
	mergesort(mid+1,end);
	merge(start,mid,end);
}
int main()
{
	int n;
	scanf("%d",&n);
	for(int i=0;i<n;i++)
		scanf("%d",&arr[i]);
	mergesort(0,n-1);
	for(int i=0;i<n;i++)
		printf("%d  ",arr[i]);
	return 0;
}