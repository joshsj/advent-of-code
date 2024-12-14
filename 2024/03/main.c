#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct FGetLResult
{
	long value;
	long value_len;
} FGetLResult;

typedef struct ParseMulResult
{
	bool parsed;
	bool parsed_len;
	int a;
	int b;
} ParseMulResult;

bool fmatch(FILE* file, const char* seq, const int seq_len)
{
	if (!seq_len)
	{
		return true;
	}

	int c = fgetc(file);

	if (c == EOF)
	{
		return false;
	} 

	if (c != seq[0])
	{
		ungetc(c, file);
		return false;
	}

	return fmatch(file, seq + 1, seq_len - 1);
}

void fmatch_test()
{
	FILE* file = fopen("fmatch.txt", "r");

	assert(file);

	// whole line
	assert(fmatch(file, "foobarbaz\n", 10));

	// exits pointing to next char
	assert(!feof(file));
	assert(fgetc(file) == EOF);
	assert(feof(file));

	fseek(file, 0, SEEK_SET);

	// whole line in parts
	assert(fmatch(file, "foo", 3));
	assert(fmatch(file, "bar", 3));
	assert(fmatch(file, "baz\n", 4));
	assert(fgetc(file) == EOF);
	assert(feof(file));

	fseek(file, 0, SEEK_SET);

	// no match
	assert(!fmatch(file, "foonope", 7));
	assert(fmatch(file, "bar", 3));
	assert(fmatch(file, "baz\n", 4));
	assert(fgetc(file) == EOF);
	assert(feof(file));

	fseek(file, 0, SEEK_SET);

	fclose(file);
}

FGetLResult fgetl(FILE* file)
{
	FGetLResult result = { .value = 0, .value_len = 0 };
	int c = 0;

	for (;;)
	{
		c = fgetc(file);

		if (c == EOF || c > '9' || c < '0')
		{
			ungetc(c, file);
			break;
		}

		++result.value_len;
		result.value = result.value * 10 + (c - '0');
	}

	return result;
}

void fgetl_test()
{
	FILE* file = fopen("fgetl.txt", "r");

	assert(file);

	// reads first number
	assert(fgetl(file).value == 123);

	// exits on last digit
	assert(fgetc(file) == ' ');

	// reads second number
	assert(fgetl(file).value == 4);

	// doesn't read past ...
	assert(fgetl(file).value == 0);
	assert(fgetl(file).value == 0);
	assert(fgetl(file).value == 0);
	assert(fgetl(file).value == 0);
	assert(fgetl(file).value == 0);

	assert(fmatch(file, "...", 3));

	// reads last number
	assert(fgetl(file).value == 6);

	// exits on last digit
	assert(fgetc(file) == '\n');

	// sanity: eol
	assert(fgetc(file) == EOF);
	assert(feof(file));

	fclose(file);
}

void part_1(FILE* file)
{
	long sum = 0;

	while (!feof(file))
	{
		bool match = fmatch(file, "mul(", 4);

		if (!match) 
		{
			int curr = fgetc(file);

			if (curr == 'm')
			{
				ungetc(curr, file);
			}

			continue;
		}

		int a = fgetl(file).value;

		if (fgetc(file) != ',')
		{
			continue;
		}

		int b = fgetl(file).value;

		if (fgetc(file) != ')')
		{
			continue;
		}

		sum += a * b;
	}

	printf("Part 1 = %ld\n", sum);
}

ParseMulResult parse_mul(FILE* file)
{
	ParseMulResult result = { .parsed = false, .parsed_len = 0, .a = 0, .b = 0 };

	bool match = fmatch(file, "mul(", 4);

	if (!match) 
	{
		return result;
	}

	FGetLResult a = fgetl(file);

	if (!a.value_len)
	{
		return result;
	}

	if (fgetc(file) != ',')
	{
		return result;
	}

	FGetLResult b = fgetl(file);

	if (!b.value_len)
	{
		return result;
	}

	if (fgetc(file) != ')')
	{
		return result;
	}

	result.parsed = true;
	result.parsed_len = 6 + a.value_len + b.value_len;
	result.a = a.value;
	result.b = b.value;

	return result;
}

void part_2(FILE* file)
{
	int c;
	long file_pos;
	int move_by;

	bool mul_enabled = true;
	long mul_sum = 0;

	while (c != EOF)
	{
		file_pos = ftell(file);
		move_by = 0;

		c = fgetc(file);
		ungetc(c, file);

		if (c == 'm')
		{
			ParseMulResult result = parse_mul(file);

			if (result.parsed)
			{
				move_by = result.parsed_len;

				if (mul_enabled)
				{
					mul_sum += result.a * result.b;
				}
			}
		}
		else if (c == 'd')
		{
			if (fmatch(file, "do()", 4))
			{
				mul_enabled = true;
				move_by = 4;
			}
			else 
			{
				fseek(file, file_pos, SEEK_SET);

				if (fmatch(file, "don't()", 7))
				{
					mul_enabled = false;
					move_by = 7;
				}
			}
		}

		fseek(file, file_pos + (move_by || 1), SEEK_SET);
	}

	printf("Part 2 = %ld\n", mul_sum);
}

int main()
{
	fmatch_test();
	fgetl_test();

	FILE* file = fopen("input.txt", "r");

	assert(file);

	part_1(file);

	fseek(file, 0, SEEK_SET);

	part_2(file);

	fclose(file);

	return 0;
}

