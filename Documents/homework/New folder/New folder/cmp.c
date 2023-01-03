//Import necessairy library
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#define BUFF_SIZE 999999

enum CPUMethods
{
	NONE,
	FCFS,
	SJF,
	PS,
	RR
} method = NONE; // CPU Scheduling Methods Enumeration
enum PMode
{
	OFF,
	ON
} mode = OFF; // Preemtive Mode Enumeration

struct node
{
	int process_id;
	int burst_time;
	int arrival_time;
	int priority;
	int waiting_time;
	int turnaround_time;
	int first_response;
	int how_much_left;
	int time_slices;
	int last_slice_burst;
	bool is_terminated;
	bool in_cpu;
	struct node *next;
};

char buff[BUFF_SIZE];
char buffer_output[BUFF_SIZE * 6];
char *input_filename = NULL;
char *output_filename = NULL;
bool fcfs_first = true;
char *exe;


struct node *header_original = NULL;
struct node *create_node(int pid, int burst_time, int arrival_time, int priority)
{
	struct node *temp;
	temp = (struct node *)malloc(sizeof(struct node));
	memset(temp, '\0', sizeof(struct node));

	temp->process_id = pid;
	temp->burst_time = burst_time;
	temp->arrival_time = arrival_time;
	temp->priority = priority;
	temp->waiting_time = 0;
	temp->turnaround_time = 0;
	temp->how_much_left = burst_time;
	temp->first_response = 0;
	temp->time_slices = 0;
	temp->last_slice_burst = 0;
	if (temp->burst_time == 0)
		temp->is_terminated = false;
	temp->in_cpu = false;
	temp->next = NULL;

	return temp;
}

// Insert back to the LL (Function)
struct node *insert_back(struct node *header, int id, int burst_time, int arrival_time, int priority)
{
	struct node *temp = create_node(id, burst_time, arrival_time, priority);
	struct node *header_temp;

	// Check if the linked list is empty
	if (header == NULL)
	{
		header = temp;
		return header;
	}

	header_temp = header;
	while (header_temp->next != NULL) // Iterate until we reach the last node
		header_temp = header_temp->next;

	header_temp->next = temp;
	return header;
}

// Delete front of the LL (Function)
struct node *delete_front(struct node *header)
{
	struct node *temp;

	if (header == NULL)
	{
		return header;
	}

	temp = header;
	header = header->next;
	memset(temp, '\0', sizeof(struct node));
	free(temp);
	return header;
}

// Displaying the Linked List Items(For Debugging Purposes Only) (Function)
void display_LL(struct node *header)
{
	struct node *temp = header;
	while (temp != NULL)
	{
		int a, b, c, d, e, f, g, h, i, j;
		bool t;
		a = temp->process_id;
		b = temp->burst_time;
		c = temp->arrival_time;
		d = temp->priority;
		e = temp->waiting_time;
		f = temp->turnaround_time;
		g = temp->how_much_left;
		h = temp->first_response;
		i = temp->time_slices;
		j = temp->last_slice_burst;

		printf("ID:%d\tBurst:%d\tArrival:%d\tPriority:%d\tWait:%d\tTurn:%d\tLeft:%d\tResponse:%d\tSlices:%d\tLastSlice:%d\n", a, b, c, d, e, f, g, h, i, j);
		temp = temp->next;
	}

	getchar();
	getchar();
}

// Cloning Main LL (Function)
struct node *clone_LL(struct node *header)
{
	struct node *header_temp = header;
	struct node *clone_header = NULL;

	while (header_temp != NULL)
	{
		int pid = 0, burst = 0, arrival = 0, priority = 0;
		pid = header_temp->process_id;
		burst = header_temp->burst_time;
		arrival = header_temp->arrival_time;
		priority = header_temp->priority;
		clone_header = insert_back(clone_header, pid, burst, arrival, priority);

		header_temp = header_temp->next;
	}

	return clone_header;
}

// This funtions is used to print programs usage and what arguments are needed to pass (Function)

void print_usage()
{
	printf("Usage: %s -f <input filename> -o <output filename>\n", exe);
	exit(1);
}
// Reading from Input File to Write it to LL (Function)
void write_input_to_LL(char *input_filename)
{
	FILE *finput = fopen(input_filename, "r");
	int id_counter = 1;
	if (feof(finput))
	{
		printf("The input file is empty\n");
		exit(1);
	}
	else
	{
		while (!feof(finput)) // Reading the input file and recording the values to Linked List
		{
			int a, b, c;
			fscanf(finput, "%d:%d:%d\n", &a, &b, &c);
			header_original = insert_back(header_original, id_counter, a, b, c);
			id_counter++;
		}
	}
	fclose(finput);
}
void fcfs();

// Counts How many process' are in the LL (Function)
int process_counter(struct node *header)
{
	struct node *temp = header;
	int counter = 0;
	while (temp != NULL)
	{
		counter++;
		temp = temp->next;
	}

	return counter;
}

// Swapping nodes (Function)
struct node *swap_nodes(struct node *temp1, struct node *temp2)
{
	struct node *tmp = temp2->next;
	temp2->next = temp1;
	temp1->next = tmp;
	return temp2;
}

// Sorts LL in ascending order (Function)
void bubble_sort(struct node **header, int counter, char *sort_mode)
{
	struct node **header_temp;
	int swapped, max_at = 0;
	int i, j;

	for (i = 0; i < counter; i++)
	{
		header_temp = header;
		swapped = 0;
		max_at = 0;

		for (j = 0; j < counter - 1 - i; j++)
		{
			struct node *temp1 = *header_temp;
			struct node *temp2 = temp1->next;

			if (!strcmp(sort_mode, "PID"))
			{
				if (temp1->process_id >= temp2->process_id)
				{
					*header_temp = swap_nodes(temp1, temp2);
					swapped = 1;
				}
				header_temp = &(*header_temp)->next; // Setting the header_temp's addres to the address of next node which is in the header_temp's address
			}

			else if (!strcmp(sort_mode, "AT"))
			{
				if (temp1->arrival_time > temp2->arrival_time)
				{
					*header_temp = swap_nodes(temp1, temp2);
					swapped = 1;
				}

				else if (temp1->arrival_time == temp2->arrival_time)
				{
					if (temp1->process_id > temp2->process_id)
					{
						*header_temp = swap_nodes(temp1, temp2);
						swapped = 1;
					}
				}
				header_temp = &(*header_temp)->next;
			}

			else if (!strcmp(sort_mode, "SJF"))
			{
				if (temp1->arrival_time <= max_at && temp2->arrival_time <= max_at)
				{
					if (temp1->burst_time > temp2->burst_time)
					{
						*header_temp = swap_nodes(temp1, temp2);
						swapped = 1;
					}

					else if (temp1->burst_time == temp2->burst_time)
					{
						if (temp1->process_id > temp2->process_id)
						{
							*header_temp = swap_nodes(temp1, temp2);
							swapped = 1;
						}
					}
					max_at += (*header_temp)->burst_time;
				}
				else
				{
					if (temp2->arrival_time > max_at)
						max_at = temp2->arrival_time;
				}

				header_temp = &(*header_temp)->next;
			}

			else if (!strcmp(sort_mode, "PS"))
			{
				if (temp1->arrival_time <= max_at && temp2->arrival_time <= max_at)
				{
					if (temp1->priority > temp2->priority)
					{
						*header_temp = swap_nodes(temp1, temp2);
						swapped = 1;
					}

					else if (temp1->priority == temp2->priority)
					{
						if (temp1->process_id > temp2->process_id)
						{
							*header_temp = swap_nodes(temp1, temp2);
							swapped = 1;
						}
					}
					max_at += (*header_temp)->burst_time;
				}
				else
				{
					if (temp2->arrival_time > max_at)
						max_at = temp2->arrival_time;
				}

				header_temp = &(*header_temp)->next;
			}
		}

		if (swapped == 0)
		{
			break;
		}
	}
}

// Checking if all the processes are done returning true if all done (Function)
bool is_all_done(struct node *header)
{
	bool done = true;
	while (header != NULL)
	{
		if (!header->is_terminated)
			done = false;
		header = header->next;
	}

	return done;
}

// Checking if all the processes before arrival time is done (Function)
bool is_previous_ones_done(struct node *header, int at_limit)
{
	bool done = true;
	while (header != NULL)
	{
		if (header->arrival_time <= at_limit)
		{
			if (!header->is_terminated)
			{
				done = false;
			}
		}
		header = header->next;
	}

	return done;
}

// Finding the node which has the least time left
struct node *find_least_left(struct node *header, int at_limit)
{
	struct node *temp = NULL;
	int x = INT_MAX;
	while (header != NULL)
	{
		if (!header->is_terminated)
		{
			if (header->arrival_time <= at_limit)
			{
				if (header->how_much_left < x)
				{
					temp = header;
					x = header->how_much_left;
				}
			}
		}
		header = header->next;
	}

	return temp;
}

// Finding the node which has the least priority
struct node *find_least_priority(struct node *header, int at_limit)
{
	struct node *temp = NULL;
	int x = INT_MAX;
	while (header != NULL)
	{
		if (!header->is_terminated)
		{
			if (header->arrival_time <= at_limit)
			{
				if (header->priority < x)
				{
					temp = header;
					x = header->priority;
				}
			}
		}
		header = header->next;
	}

	return temp;
}

int main(int argc, char *argv[])
{
    exe = argv[0];
    int options = 0;
    // Here we check if the correct options are used
	while ((options = getopt(argc, argv, "f:o:")) != -1)
	{
		switch (options)
		{
		case 'f':
			input_filename = optarg;
			break;
		case 'o':
			output_filename = optarg;
			break;
		default:
			print_usage();
			break;
		}
	}
    // Here we check if the arguments are passed for options
	if (input_filename == NULL || output_filename == NULL)
	{
		print_usage();
	}

	FILE *finput = fopen(input_filename, "r");
	if (finput == NULL) // Checking if the input file argument exists.
	{
		printf("The argument that you passed as input file does not exists.\n");
		printf("Please check the input file argument and run the program again\n");
		exit(1);
	}
	fclose(finput);

	write_input_to_LL(input_filename);

	fcfs();

	return 0;
}
// Creating node (Function)



void fcfs()
{
	struct node *clone_header = clone_LL(header_original);
	struct node *temp1, *temp2, *t;
	int program_counter = 0;
	float average_wait = 0.0f;
	int number_of_process = process_counter(clone_header);
	bool is_first = true;
	bubble_sort(&clone_header, number_of_process, "AT");
	temp1 = clone_LL(clone_header);
	while (clone_header != NULL)
	{
		clone_header = delete_front(clone_header);
	}
	t = temp2 = temp1;

	while (temp1 != NULL)
	{
		if (temp1->arrival_time <= program_counter)
		{
			program_counter += temp1->burst_time;
			temp1->turnaround_time = program_counter;
			if (is_first)
			{
				if ((temp1->waiting_time = temp1->turnaround_time - temp1->burst_time) < 0)
					temp1->waiting_time = 0;
				is_first = false;
			}
			else
			{
				if ((temp1->waiting_time = temp1->turnaround_time - temp1->burst_time - temp1->arrival_time) < 0)
					temp1->waiting_time = 0;
			}
		}

		else
		{
			program_counter = temp1->arrival_time;
			program_counter += temp1->burst_time;
			temp1->turnaround_time = program_counter;
			if ((temp1->waiting_time = temp1->turnaround_time - temp1->burst_time - temp1->arrival_time) < 0)
				temp1->waiting_time = 0;
		}

		temp1 = temp1->next;
	}

	strcpy(buff, "");
	bubble_sort(&temp2, number_of_process, "PID");
	system("clear");
	strcat(buff, "Scheduling Method: First Come First Served\n");
	strcat(buff, "Process Waiting Times:\n");
	while (temp2 != NULL)
	{
		int pid = temp2->process_id;
		int wait = temp2->waiting_time;
		average_wait += wait;
		char buff_1[20] = "";
		snprintf(buff_1, 19, "PS%d: %d ms\n", pid, wait);
		strcat(buff, buff_1);
		temp2 = temp2->next;
	}
	average_wait /= number_of_process;
	char buff_2[40];
	snprintf(buff_2, 39, "Average Waiting Time: %.3f ms\n\n", average_wait);
	strcat(buff, buff_2);
	if (fcfs_first)
	{
		strcat(buffer_output, buff);
		fcfs_first = false;
	}
	printf("%s", buff);

	while (t != NULL)
	{
		t = delete_front(t);
	}
}

