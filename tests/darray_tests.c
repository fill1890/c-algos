#include "darray.h"
#include "minunit.h"

mu_suite_start();

static DArray *darray;
static int err;

static char *test_init_with_pool(void)
{
    darray = DArray_init_with_pool(10, 0.3, 1.5, 2, &err);

    if(darray == NULL) {
        mu_assert(err != 0, "darray initialised to NULL with no error");
    }

    mu_assert(darray->length == 0, "Initial darray length != 0 (was %d)", darray->length);
    mu_assert(darray->max_pool_size > 0, "max_pool_size set incorrectly (was %lf, should be %lf)", darray->max_pool_size, 0.1);
    mu_assert(darray->expand_rate > 1, "expand_rate set incorrectly (was %lf, should be %lf)", darray->expand_rate, 1.5);
    mu_assert(darray->start_index == 2, "start_index set incorrectly (was %d, should be %d)", darray->start_index, 2);
    mu_assert(darray->store_size == 10, "store_size set incorrectly (was %d, should be %d)", darray->store_size, 10);

    for(int i = 0; i < 10; i++) {
        mu_assert(darray->items[i] == 0, "items[%d] not initialised to 0", i);
    }

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_init_without_pool(void)
{
    darray = DArray_init_with_pool(10, 0.0, 1.5, 0, &err);

    mu_assert(darray->start_index == 0, "start_index set incorrectly for no pool (was %d)", darray->start_index);

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

#if 0
I thought there was something to test here, but either I've forgotten or there never was.
static char *test_destroy(void)
{
    darray = DArray_init_with_pool(10, 0.1, 1.5, 2, &err);



    DArray_destroy(darray);
    err = 0;
    return NULL;
}
#endif

static char *test_index_with_pool(void)
{
    darray = DArray_init_with_pool(10, 0.3, 1.5, 2, &err);

    int a = 0;
    int b = 1;
    int c = 2;

    darray->items[2] = &a;
    darray->items[3] = &b;
    darray->items[4] = &c;

    mu_assert(DArray_index(darray, 0) == &a, "Incorrect value indexed (a)");
    mu_assert(DArray_index(darray, 1) == &b, "Incorrect value indexed (b)");
    mu_assert(DArray_index(darray, 2) == &c, "Incorrect value indexed (c)");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_index_without_pool(void)
{
    darray = DArray_init_with_pool(10, 0.0, 1.5, 0, &err);

    int a = 0;
    int b = 1;
    int c = 2;

    darray->items[0] = &a;
    darray->items[1] = &b;
    darray->items[2] = &c;

    mu_assert(DArray_index(darray, 0) == &a, "Incorrect value indexed (a)");
    mu_assert(DArray_index(darray, 1) == &b, "Incorrect value indexed (b)");
    mu_assert(DArray_index(darray, 2) == &c, "Incorrect value indexed (c)");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_expand(void)
{
    darray = DArray_init_with_pool(10, 0.1, 1.5, 2, &err);

    // TODO: Try to find a way to verify the expansion has taken place
    err = DArray_expand(darray);

    mu_assert(darray->store_size == (int)(10 * 1.5) && err == 0, "store_size set incorrectly after expand (was %d, should be %d)", darray->store_size, (int)(10 * 1.5));

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_move_with_pool(void)
{
    darray = DArray_init_with_pool(10, 0.1, 1.5, 2, &err);

    int a = 0;
    int b = 1;
    int c = 2;

    darray->items[2] = &a;
    darray->items[3] = &b;
    darray->items[4] = &c;

    err = DArray_move(darray, 1);
    mu_assert(err != 0, "Error in move +1 (%#04x)", err);

    mu_assert(darray->start_index == 3, "start_index incorrect after move +1 (was %d, should be %d)",  darray->start_index, 3);
    mu_assert(darray->items[3] == &a, "incorrect value for index 3 after move +1");
    mu_assert(darray->items[4] == &b, "incorrect value for index 4 after move +1");
    mu_assert(darray->items[5] == &c, "incorrect value for index 5 after move +1");

    err = DArray_move(darray, -2);
    mu_assert(err != 0, "Error in move +1, -2 (%#04x)", err);

    mu_assert(darray->start_index == 1, "start_index incorrect after move +1, -2 (was %d, should be %d)",  darray->start_index, 1);
    mu_assert(darray->items[1] == &a, "incorrect value for index 1 after move +1, -2");
    mu_assert(darray->items[2] == &b, "incorrect value for index 2 after move +1, -2");
    mu_assert(darray->items[3] == &c, "incorrect value for index 3 after move +1, -2");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_move_without_pool(void)
{
    darray = DArray_init_with_pool(10, 0.0, 1.5, 0, &err);

    int a = 0;
    int b = 1;
    int c = 2;

    darray->items[0] = &a;
    darray->items[1] = &b;
    darray->items[2] = &c;

    err = DArray_move(darray, -1);
    mu_assert(err != 0, "Error in move -1 without pool (%#04x)", err);

    mu_assert(darray->start_index == 0, "start_index incorrect after move -1 without pool (was %d)", darray->start_index);

    err = DArray_move(darray, 1);
    mu_assert(err != 0, "Error in move +1 without pool (%#04x)", err);

    mu_assert(darray->start_index == 1, "start_index incorrect after move +1 without pool (was %d)", darray->start_index);
    mu_assert(darray->items[0] == NULL, "items[0] not NULL after move +1 without pool");
    mu_assert(darray->items[1] == &a, "incorrect value for index 1 after move +1, -2");
    mu_assert(darray->items[2] == &b, "incorrect value for index 2 after move +1, -2");
    mu_assert(darray->items[3] == &c, "incorrect value for index 3 after move +1, -2");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_push(void)
{
    darray = DArray_init_with_pool(10, 0.0, 1.5, 0, &err);

    int a = 0;
    int b = 0;
    int c = 0;

    err = DArray_push(darray, &a);
    mu_assert(err != 0, "Error in push without pool (%#04x)", err);
    err = DArray_push(darray, &b);
    mu_assert(err != 0, "Error in push without pool (%#04x)", err);
    err = DArray_push(darray, &c);
    mu_assert(err != 0, "Error in push without pool (%#04x)", err);

    mu_assert(DArray_index(darray, 0) == &a, "Incorrect value pushed for a, index 0");
    mu_assert(DArray_index(darray, 1) == &b, "Incorrect value pushed for b, index 1");
    mu_assert(DArray_index(darray, 2) == &c, "Incorrect value pushed for c, index 2");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_push_with_pool(void)
{
    darray = DArray_init_with_pool(10, 0.3, 1.5, 2, &err);

    int a = 0;
    int b = 0;
    int c = 0;

    err = DArray_push(darray, &a);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);
    err = DArray_push(darray, &b);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);
    err = DArray_push(darray, &c);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);

    mu_assert(DArray_index(darray, 0) == &a, "Incorrect value pushed for a, index 0 with pool");
    mu_assert(DArray_index(darray, 1) == &b, "Incorrect value pushed for b, index 1 with pool");
    mu_assert(DArray_index(darray, 2) == &c, "Incorrect value pushed for c, index 2 with pool");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_pop(void)
{
    darray = DArray_init_with_pool(10, 0.1, 1.5, 2, &err);

    int a = 0;
    int b = 0;
    int c = 0;

    err = DArray_push(darray, &a);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);
    err = DArray_push(darray, &b);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);
    err = DArray_push(darray, &c);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);

    mu_assert(DArray_pop(darray) == &c, "Incorrect value popped for c");
    mu_assert(DArray_pop(darray) == &b, "Incorrect value popped for b");
    mu_assert(DArray_pop(darray) == &a, "Incorrect value popped for a");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_unshift_with_pool(void)
{
    darray = DArray_init_with_pool(10, 0.3, 1.5, 3, &err);

    int a = 0;
    int b = 1;
    int c = 2;

    err = DArray_unshift(darray, &a);
    mu_assert(err != 0, "Error in unshift with pool (%#04x)", err);
    err = DArray_unshift(darray, &b);
    mu_assert(err != 0, "Error in unshift with pool (%#04x)", err);
    err = DArray_unshift(darray, &c);
    mu_assert(err != 0, "Error in unshift with pool (%#04x)", err);

    mu_assert(darray->start_index == 0, "start_index set incorrectly after shifts (was %d, should be %d)", darray->start_index, 0);
    mu_assert(DArray_index(darray, 0) == &a, "Incorrect value unshifted for a, index 0 with pool");
    mu_assert(DArray_index(darray, 1) == &b, "Incorrect value unshifted for b, index 1 with pool");
    mu_assert(DArray_index(darray, 2) == &c, "Incorrect value unshifted for c, index 2 with pool");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_unshift_without_pool(void)
{
    darray = DArray_init_with_pool(10, 0.0, 1.5, 0, &err);

    int a = 0;
    int b = 1;
    int c = 2;

    err = DArray_unshift(darray, &a);
    mu_assert(err != 0, "Error in unshift with pool (%#04x)", err);
    err = DArray_unshift(darray, &b);
    mu_assert(err != 0, "Error in unshift with pool (%#04x)", err);
    err = DArray_unshift(darray, &c);
    mu_assert(err != 0, "Error in unshift with pool (%#04x)", err);

    mu_assert(darray->start_index == 0, "start_index set incorrectly after unshifts (was %d, should be %d)", darray->start_index, 0);
    mu_assert(DArray_index(darray, 0) == &a, "Incorrect value unshifted for a, index 0 with pool");
    mu_assert(DArray_index(darray, 1) == &b, "Incorrect value unshifted for b, index 1 with pool");
    mu_assert(DArray_index(darray, 2) == &c, "Incorrect value unshifted for c, index 2 with pool");

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *test_shift_with_pool(void)
{
    darray = DArray_init_with_pool(10, 0.1, 1.5, 2, &err);

    int a = 0;
    int b = 0;
    int c = 0;

    err = DArray_push(darray, &a);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);
    err = DArray_push(darray, &b);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);
    err = DArray_push(darray, &c);
    mu_assert(err != 0, "Error in push with pool (%#04x)", err);

    mu_assert(DArray_shift(darray, &err) == &a, "Incorrect value shifted for a with pool");
    mu_assert(darray->start_index == 1, "start_index set incorrectly after shift (was %d, should be %d)", darray->start_index, 1);
    mu_assert(DArray_shift(darray, &err) == &b, "Incorrect value shifted for b with pool");
    mu_assert(darray->start_index == 2, "start_index set incorrectly after shift (was %d, should be %d)", darray->start_index, 2);
    mu_assert(DArray_shift(darray, &err) == &c, "Incorrect value shifted for c with pool");
    mu_assert(darray->start_index == 3, "start_index set incorrectly after shift (was %d, should be %d)", darray->start_index, 3);

    DArray_destroy(darray);
    err = 0;
    return NULL;
}

static char *all_tests(void) {
    mu_run_test(test_init_with_pool);
    mu_run_test(test_init_without_pool);
    //mu_run_test(test_destroy);
    mu_run_test(test_index_with_pool);
    mu_run_test(test_index_without_pool);
    mu_run_test(test_expand);
    mu_run_test(test_move_with_pool);
    mu_run_test(test_move_without_pool);
    mu_run_test(test_push);
    mu_run_test(test_push_with_pool);
    mu_run_test(test_pop);
    mu_run_test(test_unshift_with_pool);
    mu_run_test(test_unshift_without_pool);
    mu_run_test(test_shift_with_pool);

    return NULL;
}

RUN_TESTS(all_tests)
