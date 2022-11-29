#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_ROWS 10
#define MAX_COLS 10

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define TEST_EQ(expr, expected_value)                           \
  do {                                                          \
    uint8_t n = (expr);                                         \
    printf("Testing: %s == %s: \n", #expr, #expected_value);      \
    if ((expected_value) == (expr)) {                           \
      printf("PASS.\n");                                        \
    } else {                                                    \
      printf("FAIL (%d).\n", n);                                \
    }                                                           \
  } while (0);

#if defined(ASM_VER)
extern uint8_t edit_distance(char *a, char *b);
#endif

void zero_mem(void *p,  size_t n)
{
  explicit_bzero(p, n);
}

void print_matrix(uint8_t n_rows, uint8_t n_cols, uint8_t m[][n_cols])
{
  int i;
  int j;
  for (i = 0; i < n_rows; ++i)
  {
    for (j = 0; j < n_cols; ++j)
    {
      printf("%03d ", m[i][j]);
    }
    printf("\n");
  }
  printf("\n\n\n");
}

#if defined(TABLE_VER)
uint8_t edit[MAX_ROWS + 1][MAX_COLS + 1];

uint8_t edit_distance(char *a, char *b)
{
  uint8_t i;
  uint8_t j;
  uint8_t ins;
  uint8_t del;
  uint8_t rep;
  uint8_t len_a = strlen(a);
  uint8_t len_b = strlen(b);
  for (j = 0; j <= MAX_ROWS; ++j) {
    edit[0][j] = j;
  }
  for (i = 1; i <= len_a; ++i) {
    edit[i][0] = i;
    for (j = 1; j <= len_b; ++j) {
      ins = edit[i][j - 1] + 1;
      del = edit[i - 1][j] + 1;
      if (a[i - 1] == b[j - 1]) {
        rep = edit[i - 1][j - 1];
      } else {
        rep = edit[i - 1][j - 1] + 1;
      }
      edit[i][j] = MIN(ins, MIN(del, rep));
    }
  }
  return edit[len_a][len_b];
}
#endif

#if defined(LINEAR_VER)
#define ROWS (MAX_ROWS + 1)
#define COLS (MAX_COLS + 1)
uint8_t edit_linear[ROWS*COLS];

#define RC_TO_LIN(r,c) (edit_linear + (r)*COLS + (c))

// strlen(a) > 0 && strlen(b) > 0
// strlen(a) < MAX_ROWS
// strlen(b) < MAX_ROWS
uint8_t edit_distance(char *a, char *b)
{
  uint8_t len_a = strlen(a);      // r8
  uint8_t len_b = strlen(b);      // r9
  uint8_t *rc;                    // r10
  uint8_t *backidx;               // r11
  uint8_t idr_min;                // r12
  uint8_t n;                      // r13
  uint8_t row;                    // r14
  uint8_t col;                    // r15
  rc = edit_linear;
  for (col = 0; col < COLS; ++col) {
    *rc = col;
    rc += 1;
  }
  rc = edit_linear;
  for (row = 0; row < ROWS; ++row) {
    *rc = row;
    rc += COLS;
  }
  rc = edit_linear + COLS + 1;
  row = 1;
  do {
    col = 1;
    do {
      // row, col - 1
      backidx = rc - 1;  // okay since col >= 1;
      idr_min = *backidx + 1;
      // row - 1, col
      backidx = rc - COLS;
      n = *backidx + 1;
      if (n < idr_min) {
        idr_min = n;
      }
      // row - 1, col - 1
      backidx -= 1;
      n = *backidx;
      if (a[row - 1] == b[col - 1]) {
        if (n < idr_min) {
          idr_min = n;
        }
      } else {
        n += 1;
        if (n < idr_min) {
          idr_min = n;
        }
      }
      *rc = idr_min;
      col += 1;
      rc += 1;
    } while (col <= len_b);
    row += 1;
    rc = edit_linear + row*COLS + 1;
  } while (row <= len_a);
  return *(edit_linear + len_a*COLS + len_b);
}
#endif

void test_edit_distance(void)
{
  TEST_EQ(edit_distance("ac", "abc"), 1);
  TEST_EQ(edit_distance("acx", "abc"), 2);
  TEST_EQ(edit_distance("algorithm", "altrusitic"), 6);
  TEST_EQ(edit_distance("food", "money"), 4);
  TEST_EQ(edit_distance("mississippi", "mysterious"), 8);
  TEST_EQ(edit_distance("mississippi", "misanthrope"), 7);
  TEST_EQ(edit_distance("Asimov", "asimov"), 1);
}

int main(int argc, char **argv)
{
  if (2 == argc && !strcmp("--test", argv[1])) {
    test_edit_distance();
  } else if (3 != argc) {
    fprintf(stderr, "Usage: la <string> <string>\n");
    return 1;
  } else {
    printf("%d\n", edit_distance(argv[1], argv[2]));
  }
  return 0;
}
