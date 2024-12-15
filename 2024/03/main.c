#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct FGetLResult
{
	long value;
	long value_len;
} FGetLResult;

typedef struct ParseResult
{
	bool parsed;
	int parsed_len;
	void* operands;
} ParseResult;

typedef struct ParseMulOperands
{
	int a;
	int b;
} ParseMulOperands;

typedef struct State
{
	bool mul_enabled;
	long mul_sum;
} State;

typedef struct Instruction
{
	void (*parse)(FILE* file, ParseResult* parse_result);
	void (*execute)(State* state, const ParseResult* parse_result);
} Instruction;

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

void parse_mul(FILE* file, ParseResult* parse_result)
{
	bool match = fmatch(file, "mul(", 4);

	if (!match) 
	{
		return;
	}

	FGetLResult a = fgetl(file);

	if (!a.value_len)
	{
		return;
	}

	if (fgetc(file) != ',')
	{
		return;
	}

	FGetLResult b = fgetl(file);

	if (!b.value_len)
	{
		return;
	}

	if (fgetc(file) != ')')
	{
		return;
	}

	parse_result->parsed = true;
	parse_result->parsed_len = 6 + a.value_len + b.value_len;

	ParseMulOperands* operands = malloc(sizeof(ParseMulOperands));

	operands->a = a.value;
	operands->b = b.value;

	parse_result->operands = operands;
}

void mul(State* state, const ParseResult* result) 
{
	assert(result->operands != NULL);

	if (state->mul_enabled)
	{
		ParseMulOperands operands = *(ParseMulOperands*)result->operands;

		state->mul_sum += operands.a * operands.b;
	}
}

Instruction create_mul()
{
	return (Instruction) { .parse = &parse_mul, .execute = &mul };
}

void parse_do(FILE* file, ParseResult* parse_result)
{
	if (fmatch(file, "do()", 4)) 
	{
		parse_result->parsed = true;
		parse_result->parsed_len = 4;
	}
}

void do_(State* state, const ParseResult* result)
{
	state->mul_enabled = true;
}

Instruction create_do()
{
	return (Instruction) { .parse = &parse_do, .execute = &do_ };
}

void parse_dont(FILE* file, ParseResult* parse_result)
{
	if (fmatch(file, "don't()", 7)) 
	{
		parse_result->parsed = true;
		parse_result->parsed_len = 7;
	}
}

void dont(State* state, const ParseResult* result)
{
	state->mul_enabled = false;
}

Instruction create_dont()
{
	return (Instruction) { .parse = &parse_dont, .execute = &dont };
}

long run(const Instruction* instructions, const int instructions_len)
{
	FILE* file = fopen("input.txt", "r");
	assert(file);

	State state = { .mul_enabled = true, .mul_sum = 0 };
	ParseResult* parse_result = malloc(sizeof(ParseResult));

	int c = -EOF;
	
	long file_pos;
	int move_by;

	while (c != EOF)
	{
		file_pos = ftell(file);
		move_by = 0;

		c = fgetc(file);
		ungetc(c, file);

		for (int i = 0; i < instructions_len; ++i)
		{
			fseek(file, file_pos, SEEK_SET);

			parse_result->parsed = false;
			parse_result->parsed_len = 0;
			parse_result->operands = NULL;

			Instruction instruction = instructions[i];

			instruction.parse(file, parse_result);

			if (parse_result->parsed)
			{
				instruction.execute(&state, parse_result);
				move_by = parse_result->parsed_len;
				free(parse_result->operands);
				break;
			}
		}

		fseek(file, file_pos + (move_by || 1), SEEK_SET);
	}
	
	free(parse_result);
	fclose(file);

	return state.mul_sum;
}

void part_1()
{
	Instruction instructions[] = { create_mul() };

	long sum = run(instructions, 1);

	printf("Part 1 = %ld\n", sum);
}

void part_2()
{
	Instruction instructions[] = { create_do(), create_mul(), create_dont(), };

	long sum = run(instructions, 3);

	printf("Part 2 = %ld\n", sum);
}

int main()
{
	fmatch_test();
	fgetl_test();

	part_1();
	part_2();

	return 0;
}

