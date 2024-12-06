#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define LIST_LINE_COUNT 1000
#define NUM_CHAR_COUNT 5
#define SEP_CHAR_COUNT 3

int compare_list_item(const void *a_ptr, const void *b_ptr)
{
	int a = *(int*)a_ptr;
	int b = *(int*)b_ptr;

	return a - b;
}

int main()
{
	FILE* file = fopen("input.txt", "r");

	assert(file != NULL);

	// line content + \n + \0
	const int current_line_len = NUM_CHAR_COUNT * 2 + SEP_CHAR_COUNT + 2;
	const int current_line_size = sizeof(char) * current_line_len;
	char* current_line = malloc(current_line_size);

	int* list_a = malloc(sizeof(int) * LIST_LINE_COUNT);
	int* list_b = malloc(sizeof(int) * LIST_LINE_COUNT);
		
	for (int i = 0; i < LIST_LINE_COUNT; ++i)
	{
		assert(fgets(current_line, current_line_len, file));

		char* current_line_parsing_ptr = current_line;

		list_a[i] = strtol(current_line_parsing_ptr, &current_line_parsing_ptr, 10);
		list_b[i] = strtol(current_line_parsing_ptr, &current_line_parsing_ptr, 10);
	}
	
	qsort(list_a, LIST_LINE_COUNT, sizeof(int), compare_list_item);
	qsort(list_b, LIST_LINE_COUNT, sizeof(int), compare_list_item);

	int total_distance = 0;

	for (int i = 0; i < LIST_LINE_COUNT; ++i)
	{
		int a = list_a[i];
		int b = list_b[i];

		total_distance += abs(a - b);
	}

	printf("Part 1 = %d\n", total_distance);

	free(list_b);
	free(list_a);
	free(current_line);
	fclose(file);

	return 0;
}

