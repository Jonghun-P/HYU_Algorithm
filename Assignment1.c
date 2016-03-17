/*

2012003716 Cho kwang il

Assignment1) 6.5-9
Give an O(nlgk)-time algorithm to merge k sorted lists into one sorted list,
where n is the total number of elements in all the input lists. (Hint: Use a minheap
for k-way merging.)

-----------------PROCEDURE------------------------------
1. Initialize condition
1) Get numbers of input and sorted array.
2) Generate random numbers * inputsize
3) Put numbers into k array with sorted by Quicksort
2. Make k-node Heap
-Time Complexity : BuildingHeap = O(klogk)
3. Use k-node Heap to Heapsort maintaining k-node until all inputs replaced from sorted array.
*At this point, to guarantee the smallest property inserting value should follow index of last extracted min value came from.
-Time Complexity : Extract_Min = O(1), Insert&Heapify = O((n-k)logk)
4. Print sorted list which merged from k sorted list.
--------------------------------------------------------
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Arr* p_Array;
typedef struct HeapStructure* HeapStruct;

struct Arr
{
	int n;
	int k;
	struct Array* pointer;
};

struct Array
{
	int size;
	int index_arr;
	int* arr;
};

struct HeapStructure
{
	int Capacity;
	int Size;
	struct key* Elements;
};

struct key
{
	int index; //index which marks array this key came from
	int value;
};

//Minheap functions-------------------------------------------
HeapStruct CreateHeap(p_Array P);
int Extract_Min(HeapStruct H, p_Array P);
void PrintHeap(HeapStruct H);
int IsEmpty(HeapStruct H);
int IsFull(HeapStruct H);
void MakeEmpty(HeapStruct H);
void key_swap(struct key* a, struct key* b);
int IsEnd(p_Array P);
HeapStruct BuildHeap(HeapStruct H, int index_parent);
//------------------------------------------------------------

//Quicksort functions-----------------------------------------
void Quicksort(int A[], int start, int last);
int Partition(int A[], int start, int last);
void swap(int* a, int* b);
//------------------------------------------------------------

//Other functions---------------------------------------------
p_Array Initialize();
//------------------------------------------------------------

int main()
{
	int i; // for iteration
	char flag_print;
	int* sortedlist;

	srand(time(NULL)); //randomize input number
	p_Array P = Initialize();
	HeapStruct H = CreateHeap(P);

	sortedlist = (int*)malloc(sizeof(int)*P->n);

	printf("Do you want to check step by step by printing Heap? (Y/N)");
	scanf("%c", &flag_print); //kind of fflush
	scanf("%c", &flag_print);

	if((flag_print == 'Y') || flag_print == 'y')
		for (i = 0; i < P->n; i++)
		{
			PrintHeap(H);
			sortedlist[i] = Extract_Min(H, P);
		}

	else
		for (i = 0; i < P->n; i++)
			sortedlist[i] = Extract_Min(H, P);

	printf("--------------------MERGED & SORTED LIST-------------------------\n");
	for (i = 0; i < P->n; i++)
	{
		if ((i != 0) && (i % 10 == 0))
			printf("\n");

		if (sortedlist[i] < 10)
			printf("  ");

		else if (sortedlist[i] < 100)
			printf(" ");

		printf("%d ", sortedlist[i]);
	}
	printf("\n");
	printf("-----------------------------------------------------------------\n");

	return 0;
}

p_Array Initialize()
{
	int i, j; //for iteration
	int n; // number of input, less than 1000
	int k; // number of sorted array less than 1000

	int remainder = 0;
	int temp = 0;

	p_Array P;
	printf("Get n?\n[n is number of input, n < 1000] \n");
	scanf("%d", &n); //get number to generate numbers

	printf("Get k?\n[k is number of sorted lists, k < n < 1000] \n");
	scanf("%d", &k); // get number to make sorted lists

	temp = n / k; //divide equally 
	remainder = n - temp*k;

	P = (p_Array)malloc(sizeof(struct Arr)); //set k lists
	P->pointer = (struct Array*)malloc(sizeof(struct Array)*k);
	P->k = k;
	P->n = n;

	for (i = 0; i < k; i++)
	{
		if (i < remainder)
			P->pointer[i].size = temp + 1;
		else
			P->pointer[i].size = temp;


		P->pointer[i].index_arr = 0;
		P->pointer[i].arr = (int*)malloc(sizeof(int)*P->pointer[i].size);

		for (j = 0; j < P->pointer[i].size; j++) // put randomize numbers into k lists
			P->pointer[i].arr[j] = rand() % 1000;

		Quicksort(P->pointer[i].arr, 0, P->pointer[i].size - 1); //Quicksort k lists
	}

	printf("-------------------INITIALIZING COMPLETE-------------------------\n");
	for (i = 0; i < k; i++) //print initialized condition
	{
		printf("k[%d] : ", i);
		for (j = 0; j < P->pointer[i].size; j++)
		{
			if ((j != 0) && (j % 10 == 0))
			{
				if (i < 10)
					printf("\n       ");
				else if (i < 100)
					printf("\n        ");
				else if (i < 1000)
					printf("\n         ");
			}

			if (P->pointer[i].arr[j] < 10)
				printf("  ");

			else if (P->pointer[i].arr[j] < 100)
				printf(" ");

			printf("%d ", P->pointer[i].arr[j]);
		}
			
		printf("\n");
	}
	printf("-----------------------------------------------------------------\n");

	
	return P;
}

//QUICKSORT FUNCTIONS------------------------------------------------------------
void Quicksort(int A[], int start, int last)
{
	if (start >= last)
		return;
	int p = Partition(A, start, last);
	Quicksort(A, start, p - 1);
	Quicksort(A, p + 1, last);
}

int Partition(int A[], int start, int last)
{
	int i = start - 1;
	int j = last;
	int pivot = last;
	for (;;)
	{
		while (A[--j] >= A[pivot]);
		while (A[++i] < A[pivot]);
		if (i < j)
			swap(&A[i], &A[j]);

		else
		{
			swap(&A[i], &A[pivot]);
			return i;
		}
	}
}

void swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}
//Quicksort----------------------------------------------------------------------

//HEAP FUNCTIONS-----------------------------------------------------------------

HeapStruct CreateHeap(p_Array P)
{
	HeapStruct H;
	int heapsize = P->k;
	int i, index_parent;

	H = (struct HeapStructure*)malloc(sizeof(struct HeapStructure));
	H->Capacity = heapsize;
	H->Elements = (struct key*)malloc(sizeof(struct key)*heapsize + 1);
	MakeEmpty(H);
	
	for (i = 0; i < heapsize; i++) //building initial minheap, k size
	{
		H->Elements[i + 1].value = P->pointer[i].arr[0];
		H->Elements[i + 1].index = i;
		H->Size++;
	}

	index_parent = H->Size / 2; //initial heapify
	while (index_parent > 0)
	{
		H = BuildHeap(H, index_parent);
		index_parent--;
	}

	return H;
}
HeapStruct BuildHeap(HeapStruct H, int index_parent) //Use recursive call to heapify
{
	if ((index_parent * 2 == H->Size) && (H->Elements[index_parent * 2].value <= H->Elements[index_parent].value))
	{
		key_swap(&H->Elements[index_parent * 2], &H->Elements[index_parent]);
		H = BuildHeap(H, index_parent * 2);
	}
	else if (index_parent * 2 + 1 <= H->Size)
	{
		if ((H->Elements[index_parent * 2].value <= H->Elements[index_parent * 2 + 1].value) && (H->Elements[index_parent * 2].value <= H->Elements[index_parent].value))
		{
			key_swap(&H->Elements[index_parent * 2], &H->Elements[index_parent]);
			H = BuildHeap(H, index_parent * 2);
		}

		else if ((H->Elements[index_parent * 2 + 1].value <= H->Elements[index_parent * 2].value) && (H->Elements[index_parent * 2 + 1].value <= H->Elements[index_parent].value))
		{
			key_swap(&H->Elements[index_parent * 2 + 1], &H->Elements[index_parent]);
			H = BuildHeap(H, index_parent * 2 + 1);
		}
	}
	return H;
}
int Extract_Min(HeapStruct H, p_Array P)
{
	int i, Child, last_index;
	struct key MinElement, NewElement;

	MinElement.value = H->Elements[1].value;
	last_index = H->Elements[1].index; //get index of last min

	//Edit NewElement property-------------------------------------------------------------------

	if ((P->pointer[last_index].index_arr) != (P->pointer[last_index].size - 1))
	{
		NewElement.index = last_index;
		NewElement.value = P->pointer[last_index].arr[++(P->pointer[last_index].index_arr)];
	}

	else if (!IsEnd(P))
	{
		while ((P->pointer[last_index].index_arr) == (P->pointer[last_index].size - 1))
			last_index = (last_index + 1) % (P->k);

		NewElement.index = last_index;
		NewElement.value = P->pointer[last_index].arr[++(P->pointer[last_index].index_arr)];
	}
	else if (IsEnd(P)) // after all elements replaced from sorted list. (= last k nodes)
	{
		NewElement = H->Elements[H->Size--];
	}

	//-------------------------------------------------------------------------------------------

	for (i = 1; i * 2 <= H->Size; i = Child)
	{
		Child = i * 2;
		if (Child != H->Size && H->Elements[Child + 1].value < H->Elements[Child].value)
			Child++;

		if (NewElement.value >= H->Elements[Child].value)
		{
			H->Elements[i].index = H->Elements[Child].index;
			H->Elements[i].value = H->Elements[Child].value;
		}

		else
			break;
	}

	H->Elements[i] = NewElement;
	return MinElement.value;
}
int IsEnd(p_Array P)
{
	int i;
	for (i = 0; i < P->k; i++)
		if (P->pointer[i].index_arr != P->pointer[i].size - 1)
			return 0;

	return 1;
}
void PrintHeap(HeapStruct H)
{
	int i;
	if (IsEmpty(H))
	{
		printf("Print Failed! : Heap is Empty\n");
		return;
	}
	else
	{

		printf("Heap> ");
		for (i = 1; i < (H->Size + 1); i++)
		{
			if ((i != 1) && (i % 15 == 1))
				printf("\n      ");
			
			if (H->Elements[i].value < 10)
				printf("  ");
			else if (H->Elements[i].value < 100)
				printf(" ");
			
			printf("%d ", H->Elements[i].value);
		}


//		for (i = 1; i < (H->Size + 1); i++)
//			printf("index : %d ", H->Elements[i].index);

		printf("\n");
	}
}

void key_swap(struct key* a, struct key* b)
{
	struct key temp = *a;
	*a = *b;
	*b = temp;
}

void MakeEmpty(HeapStruct H)
{
	H->Size = 0;
	H->Elements[0].value = 0;
	H->Elements[0].index = 0;
}

int IsEmpty(HeapStruct H)
{
	return H->Size == 0;
}

int IsFull(HeapStruct H)
{
	return H->Size == H->Capacity;
}
