#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>

typedef struct priority_queue *queue_pointer;
struct priority_queue {
	int process_id;
	int priority;
	int computing_time;
	int turn_around_time;
	int process_input_time;
	queue_pointer left_link, right_link;
};

typedef struct queue_head *head_pointer;
struct queue_head {
	queue_pointer left_link;
	queue_pointer right_link;
};
head_pointer queue;

void initialize_queue(void);
int insert_queue(int process_id, int priority, int computing_time);
int computing_queue(int turn_around_time);

int input_time, end_time;

int main() 
{
	FILE *fp;
	int type, process_id, priority, computing_time, time_quantum;

	fp = fopen("input_file.txt", "r");
	if (fp == NULL) printf("ERROR: Failed to open file.\n");

	initialize_queue();
	input_time = 0;
	end_time = 0;
	time_quantum = 20;
	printf("*** FCFS(First-Come-First-Served) Scheduling ***\n");
	printf("Process_ID	priority	computing_time	turn_around_time    end_time\n");
	while (!feof(fp))
	{
		fscanf(fp, "%d %d %d %d", &type, &process_id, &priority, &computing_time);
		if (type == 0)
		{
			insert_queue(process_id, priority, computing_time);
		}
		else if (type == 1)
		{
			input_time += time_quantum;
			computing_queue(time_quantum);
		}
		else if (type == -1)
		{
			while (queue->left_link != queue->right_link)
			{
				computing_queue(time_quantum);
			}
		}
	}

	fclose(fp);
	return 0;
}

void initialize_queue()
{
	queue = (head_pointer)malloc(sizeof(head_pointer));
	queue->left_link = queue->right_link = NULL;
}

int insert_queue(int process_id, int priority, int computing_time)
{
	queue_pointer prev_pointer = (queue_pointer)malloc(sizeof(queue_pointer));
	queue_pointer pointer = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->left_link = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->right_link = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->left_link = pointer->right_link = NULL;
	pointer->process_id = process_id;
	pointer->priority = priority;
	pointer->computing_time = computing_time;
	pointer->turn_around_time = 0;
	pointer->process_input_time = input_time;

	if (queue->left_link == NULL)
	{
		queue->left_link = queue->right_link = pointer;
		pointer->left_link = queue->left_link;
		pointer->right_link = queue->right_link;
	}
	else
	{
		prev_pointer = queue->left_link;
		while (prev_pointer != queue->right_link)
		{
			prev_pointer = prev_pointer->right_link;
		}
		pointer->left_link = prev_pointer;
		pointer->right_link = queue->right_link;
		prev_pointer->right_link = pointer;
		queue->right_link = pointer;
	}

	return pointer->process_id;
}

int computing_queue(int turn_around_time)
{
	queue_pointer pointer = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->left_link = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer->right_link = (queue_pointer)malloc(sizeof(queue_pointer));
	pointer = queue->left_link;
	if (pointer == NULL)
	{
		printf("Queue doesn't exist.");
	}
	else 
	{
		if (pointer->computing_time >= turn_around_time)
		{
			pointer->computing_time -= turn_around_time;
			pointer->turn_around_time += turn_around_time;
		}
		else if (pointer->computing_time < turn_around_time)
		{
			turn_around_time -= pointer->computing_time;
			pointer->turn_around_time += pointer->computing_time;
			pointer->computing_time = 0;
		}

		if (pointer->computing_time == 0)
		{
			end_time += pointer->turn_around_time;
			printf("%2d %15d %15d %16d %19d\n", pointer->process_id, pointer->priority, pointer->turn_around_time, end_time - pointer->process_input_time, end_time);
			if (queue->left_link == queue->right_link)
				return pointer->turn_around_time;
			else
			{
				queue->left_link = pointer->right_link;
				pointer->right_link->left_link = queue->left_link;
			}
		}
	}

	if (queue->left_link == queue->right_link)
		computing_queue(turn_around_time);

	return pointer->turn_around_time;
}
