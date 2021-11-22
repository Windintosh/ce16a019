#include <stdio.h>
#include <stdlib.h>

typedef struct priority_queue *queue_pointer;
struct priority_queue {
	int priority;
	int computing_time;
	queue_pointer left_link, right_link;
};

typedef struct queue_head *head_pointer;
struct queue_head {
	queue_pointer left_link;
	queue_pointer right_link;
};

void initialize_queue(void);
int insert_queue(int priority, int computing_time);
int delete_queue(int priority);
void print_queue(void);

head_pointer queue[3];

int main()
{
	printf("*** Priority Queue Management *** \n");
	printf("Queue ID	priority	computing_time \n");
	int type, priority, computing_time;
	FILE *fp = fopen("input_file.txt", "r");
	if (fp == NULL) {	
		printf("ERROR: File Open Failed \n");
		return 0;
	}
	initialize_queue();
	while(1)
	{
		fscanf(fp, "%d %d %d", &type, &priority, &computing_time);
		if (type == 0)
		{
			insert_queue(priority, computing_time);
		}
		else if (type == 1)
		{
			delete_queue(priority);
		}
		else if (type == -1)
		{
			break;
		}
		else
		{
			printf("ERROR: Type Error");
			continue;
		}
	}
	print_queue();
	fclose(fp);
	return 0;
}

void initialize_queue()
{
	int i;
	for (i = 0; i < 3; i++)
	{
		queue[i] = (head_pointer)malloc(sizeof(head_pointer));
		queue[i]->left_link = queue[i]->right_link = NULL;
	}
}

int insert_queue(int priority, int computing_time)
{
	int index;
	queue_pointer prev_pointer = (queue_pointer)malloc(sizeof(queue_pointer));
	queue_pointer pointer = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->left_link = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->right_link = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->left_link = pointer->right_link = NULL;
	pointer->priority = priority;
	pointer->computing_time = computing_time;

	if (priority <= 10)
		index = 0;
	else if (priority <= 20)
		index = 1;
	else if (priority <= 30)
		index = 2;
	else
		printf("ERROR: Wrong Priority Input \n");

	prev_pointer = queue[index]->left_link;

	if (queue[index]->left_link == NULL)
	{
		queue[index]->left_link = queue[index]->right_link = pointer;
		pointer->left_link = queue[index]->left_link;
		pointer->right_link = queue[index]->right_link;
	}
	else if (prev_pointer->priority > pointer->priority)
	{
		queue[index]->left_link = pointer;
		pointer->left_link = queue[index]->left_link;
		pointer->right_link = prev_pointer;
		prev_pointer->left_link = pointer;
	}
	else
	{
		prev_pointer = queue[index]->left_link;
		while (prev_pointer->right_link != queue[index]->right_link)
		{
			if (prev_pointer->priority > pointer->priority)
				break;
			prev_pointer = prev_pointer->right_link;
		}
		if (prev_pointer->right_link == queue[index]->right_link)
		{
			if (prev_pointer->right_link->priority < pointer->priority)
			{
				prev_pointer = prev_pointer->right_link;
				pointer->left_link = prev_pointer;
				queue[index]->right_link = pointer;
				pointer->right_link = queue[index]->right_link;
				prev_pointer->right_link = pointer;
			}
			else if (prev_pointer->priority > pointer->priority)
			{
				prev_pointer = prev_pointer->left_link;
				pointer->left_link = prev_pointer;
				pointer->right_link = prev_pointer->right_link;
				prev_pointer->right_link->left_link = pointer;
				prev_pointer->right_link = pointer;
			}
			else
			{
				pointer->left_link = prev_pointer;
				pointer->right_link = prev_pointer->right_link;
				prev_pointer->right_link->left_link = pointer;
				prev_pointer->right_link = pointer;
			}
		}
		else
		{
			prev_pointer = prev_pointer->left_link;
			pointer->left_link = prev_pointer;
			pointer->right_link = prev_pointer->right_link;
			prev_pointer->right_link->left_link = pointer;
			prev_pointer->right_link = pointer;
		}
	}

	return pointer->priority;
}

int delete_queue(int priority)
{
	int index;
	int p;
	int ct;
	queue_pointer pointer = (queue_pointer)malloc(sizeof(queue_pointer));
	if (priority <= 10) {
		index = 0;
	}
	else if (priority <= 20) {
		index = 1;
	}
	else if (priority <= 30) {
		index = 2;
	}
	else
		printf("ERROR: Wrong Priority Input \n");
	
	pointer = queue[index]->left_link;
	queue[index]->left_link = pointer->right_link;
	pointer->left_link = pointer->right_link = NULL;
	p = pointer->priority;
	ct = pointer->computing_time;
	printf("%d	%16d	%15d\n", index+1, p, ct);
	return pointer->priority;
}

void print_queue()
{
	int i;
	for (i = 0; i < 3; i++)
	{
		queue_pointer print_pointer = (queue_pointer)malloc(sizeof(queue_pointer));
		print_pointer = queue[i]->left_link;
		while (print_pointer != queue[i]->right_link)
		{
			printf("%d 	%16d 	%15d\n", i + 1, print_pointer->priority, print_pointer->computing_time);
			print_pointer = print_pointer->right_link;
		}
		printf("%d 	%16d 	%15d\n", i + 1, print_pointer->priority, print_pointer->computing_time);
	}
}
