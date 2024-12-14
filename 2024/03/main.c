#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

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

long fgetl(FILE* file)
{
	long l = 0;
	int c = 0;

	for (;;)
	{
		c = fgetc(file);

		if (c == EOF || c > '9' || c < '0')
		{
			ungetc(c, file);
			break;
		}

		l = l * 10 + (c - '0');
	}

	return l;
}

void fgetl_test()
{
	FILE* file = fopen("fgetl.txt", "r");

	assert(file);

	// reads first number
	assert(fgetl(file) == 123);

	// exits on last digit
	assert(fgetc(file) == ' ');

	// reads second number
	assert(fgetl(file) == 4);

	// doesn't read past ...
	assert(fgetl(file) == 0);
	assert(fgetl(file) == 0);
	assert(fgetl(file) == 0);
	assert(fgetl(file) == 0);
	assert(fgetl(file) == 0);

	assert(fmatch(file, "...", 3));

	// reads last number
	assert(fgetl(file) == 6);

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

		int a = fgetl(file);

		if (fgetc(file) != ',')
		{
			continue;
		}

		int b = fgetl(file);

		if (fgetc(file) != ')')
		{
			continue;
		}

		printf("mul(%d,%d)\n", a, b);
		sum += a * b;
	}

	printf("Part 1 = %ld\n", sum);
}

int main()
{
	fmatch_test();
	fgetl_test();

	FILE* file = fopen("input.txt", "r");

	assert(file);

	part_1(file);

	fseek(file, 0, SEEK_SET);

	fclose(file);

	return 0;
}

