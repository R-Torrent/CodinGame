#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * Solved using Reversed Polish Notation, to avoid the use of parentheses.
 *
 * Candidate arithmetic expressions are codified into a char array with format
 *   a a (a o a o ...) o
 * where 'a' stands for "push(a)" and 'o' any of the four arithmetic operations,
 * "push(pop() op pop())". After the expression is evaluated, a single integer
 * will be stored in the stack's head.
 *
 * Each expression is recursively explored for each of the four arithmetic
 * operators
 */

#define LIMIT 12 // game's limit on the number of operations
#define DEPTH (LIMIT + 1) // maximum depth of stack

typedef struct {
	int sp; // next free stack position
	int stack[DEPTH];
} stack_t;

bool is_valid(char *, int);
void evaluates_to_N(int *, int, stack_t *,char *, int);
void duplicate_stack(stack_t *, stack_t *);
void push(stack_t *, int);
int pop(stack_t *);

int compare(const void *, const void *);

int N, a;

int main()
{
	scanf("%d%d", &N, &a);

	char expr[2 * LIMIT + 1];
	int sol = LIMIT + 1;

	int size = 2 * LIMIT + 1;
	expr[0] = expr[1] = 1; // 1 == 'a' in the above explanation
	expr[size - 1] = 0;    // 0 == 'o' in the above explanation
	// starting expr = a a (a a a ... a o ... o o o) o
	for (int i = 2; i < size - 1; i++)
		expr[i] = i <= LIMIT;

	while (true) {
		stack_t st = { .sp = 0 };
		if (is_valid(expr, size))
			evaluates_to_N(&sol, 0, &st, expr, size);
		// permutations with duplications for the central ( a o ... ) portion
		// find rightmost one with a zero to its right
		int i;
		for (i = size - 3; i >= 2; i--)
			if (expr[i] && !expr[i + 1])
				break;
		if (i < 2) // none found
			break;
		else {
			expr[i] = 0; // swap elements
			expr[i + 1] = 1;
			// sort in descending order
			qsort(expr + i + 1, size - i - 2, 1, compare);
		}
	}

	if (sol > LIMIT) {
		printf("Solution not found\n");
		return 1;
	}
	printf("%d\n", sol);
	return 0;
}

int compare(const void *a, const void *b)
{
	return *(char *)b - *(char *)a;
}

bool is_valid(char *expr, int size)
{
	int a = 0;

	while (size--)
		if (*expr++)
			a++;
		else if (!--a)
			return false;
	return true;
}

void evaluates_to_N(int *sol, int pushes, stack_t *st, char *expr, int size)
{
	if (!size-- || pushes >= *sol) // cut branch
		return;
	if (st->sp) {
		int op = pop(st);
		if (op == N) {
			*sol = pushes;
			return;
		}
		push(st, op);
	}

	if (*expr++) { // push value 'a'
		push(st, a);
		evaluates_to_N(sol, ++pushes, st, expr, size);
		return;
	}

	int op2 = pop(st), op1 = pop(st);
	stack_t st_copy;

	duplicate_stack(&st_copy, st);
	push(&st_copy, op1 + op2); // add
	evaluates_to_N(sol, pushes, &st_copy, expr, size);

	duplicate_stack(&st_copy, st);
	push(&st_copy, op1 - op2); // subtract
	evaluates_to_N(sol, pushes, &st_copy, expr, size);

	duplicate_stack(&st_copy, st);
	push(&st_copy, op1 * op2); // multiply
	evaluates_to_N(sol, pushes, &st_copy, expr, size);

	if (op2 && !(op1 % op2)) {
		duplicate_stack(&st_copy, st);
		push(&st_copy, op1 / op2); // divide
		evaluates_to_N(sol, pushes, &st_copy, expr, size);
	}
}

void duplicate_stack(stack_t *dst, stack_t *src)
{
	dst->sp = src->sp;
	for (int i = 0; i < DEPTH; i++)
		dst->stack[i] = src->stack[i];
}

void push(stack_t *st, int i)
{
	st->stack[st->sp++] = i;
}

int pop(stack_t *st)
{
	return st->stack[--st->sp];
}
