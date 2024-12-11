#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define REPORT_COUNT 1000
#define LONGEST_REPORT_COUNT 23
#define LONGEST_REPORT_CHAR_COUNT 23

void copy_array_excluding_index(int* dest, const int* src, const int src_len, const int index)
{
	assert(index < src_len);

	memcpy(dest, src, sizeof(int) * index);
	memcpy(dest + index, src + index + 1, sizeof(int) * (src_len - (index + 1)));
}

bool is_report_safe(const int* report, const int report_len)
{
	assert(report_len >= 2);

	bool report_increasing; 
	bool report_increasing_init;

	int i_left = 0;
	int i_right = 1;
	bool is_safe = true;

	while (i_right < report_len && is_safe)
	{
		int l = report[i_left];
		int r = report[i_right];

		int diff = r - l;
		int abs_diff = abs(diff);

		if (abs_diff < 1 || abs_diff > 3)
		{
			is_safe = false;
			break;
		}

		bool pair_increasing = diff > 0;

		if (!report_increasing_init) 
		{
			report_increasing = pair_increasing;
			report_increasing_init = true;

			i_left = i_right;
			++i_right;
			continue;
		}
		
		if (pair_increasing != report_increasing)
		{
			is_safe = false;
			break;
		}

		i_left = i_right;
		++i_right;
	}

	return is_safe;
}

bool is_report_safe_with_dampener(const int* report, const int report_len)
{
	const int damp_report_len = report_len - 1;
	int* damp_report = malloc(sizeof(int) * damp_report_len);
	
	assert(damp_report);

	int i = 0;
	bool is_safe = false;

	while (!is_safe && i < report_len)
	{
		copy_array_excluding_index(damp_report, report, report_len, i);
		usleep(1); // WTF WHY DO I NEED AN ARBITRARY CALL TO ANY METHOD FOR THE OUTPUT TO BE CORRECT
		is_safe = is_report_safe(damp_report, damp_report_len);
		++i;
	}

	free(damp_report);
	return is_safe;
}

int main()
{
	FILE* file = fopen("input.txt", "r");

	assert(file);

	// data + \n + \0
	const int current_line_len = LONGEST_REPORT_CHAR_COUNT + 2;
	const int current_line_size = sizeof(char) * current_line_len;
	char* current_line = malloc(current_line_size);
	assert(current_line);

	int* current_report = malloc(sizeof(int) * LONGEST_REPORT_COUNT);
	assert(current_report);

	int safe_reports_without_dampener = 0;
	int safe_reports_with_dampener = 0;
	
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

		bool is_safe = is_report_safe(current_report, current_report_len);

		safe_reports_without_dampener += is_safe;
		safe_reports_with_dampener += is_safe || is_report_safe_with_dampener(current_report, current_report_len);
	}
	
	free(current_report);
	free(current_line);
	fclose(file);

	printf("Part 1 = %d\n", safe_reports_without_dampener);
	printf("Part 2 = %d\n", safe_reports_with_dampener);

	return 0;
}

