#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define REPORT_COUNT 1000
#define LONGEST_REPORT_COUNT 23
#define LONGEST_REPORT_CHAR_COUNT 23

bool is_report_safe(const int* report, const int len)
{
	assert(len >= 2);

	int prev = report[0];
	int curr = report[1];
	bool is_increasing = curr > prev;
	int i = 1;

	for(;;)
	{
		int diff = curr - prev;

		if (abs(diff) < 1
			|| abs(diff) > 3
			|| (is_increasing && diff < 0)
			|| (!is_increasing && diff > 0))
		{
			return false;
		}

		++i;

		if (i == len)
		{
			return true;
		}
		
		prev = curr;
		curr = report[i];
	}
}

int main()
{
	FILE* file = fopen("input.txt", "r");

	assert(file != NULL);

	// data + \n + \0
	const int current_line_len = LONGEST_REPORT_CHAR_COUNT + 2;
	const int current_line_size = sizeof(char) * current_line_len;
	char* current_line = malloc(current_line_size);
	int* current_report = malloc(sizeof(int) * LONGEST_REPORT_COUNT);

	int safe_reports = 0;
	
	for (int i = 0; i < REPORT_COUNT; ++i)
	{
		assert(fgets(current_line, current_line_len, file));

		int current_report_len = 0;

		char* current_token_ptr = strtok(current_line, " ");
		assert(current_token_ptr != NULL);

		while (current_token_ptr != NULL)
		{
			current_report[current_report_len++] = strtol(current_token_ptr, NULL, 10);

			current_token_ptr = strtok(NULL, " ");
		}

		safe_reports += is_report_safe(current_report, current_report_len);
	}
	
	printf("Part 1 = %d\n", safe_reports);

	free(current_report);
	free(current_line);
	fclose(file);

	return 0;
}

