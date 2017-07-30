/**
 * @file darray.h
 * @author Andrew Walls
 * @date 29 July 2017
 * @brief Header file for DArray implementation
 *
 */

#ifndef DArray_h

#include "stdint.h"

/**
 * @brief DArray error types
 *
 * When an error is returned from a function as an int, or a function is given a pointer to an int,
 * the result will be a `darray_err_general` OR'd with another value which can be used to get more detail.
 */
enum darray_err_general {
    DA_ERR_MEMORY   = 0x1,   ///< Error in memory allocation; no other detail available
    DA_ERR_ARGS     = 0x2,   ///< Error in function arguments; will be OR'd with appropriate function error
    DA_ERR_DATA     = 0x3,   ///< Corrupt DArray given to function; will be OR'd with appropriate data error
    DA_ERR_MASK     = 0xF,   ///< AND an error with this mask to retrieve the general error
    DA_DETAIL_MASK  = 0xF0,  ///< AND an error with this mask to retrieve error details
    DA_DETAIL2_MASK = 0xF00, ///< AND an error with this mask to retrieve secondary error details
    DA_DETAIL3_MASK = 0xF000 ///< AND an error with this mask to retrieve tertiary error details
};

/**
 * @brief Dynamic array representation
 *
 * This particular implementation is distinguished by the use of a pool of empty slots
 * at the start of the array, which are used for O(1) insertion of new elements at the
 * start of the array. This pool is usually increased in size when a value is shifted
 * off the start of the array, and is resized when the value of `(size of pool)/(size of array)`
 * exceeds the value of `max_pool_size`. All DArray methods are aware of the pool; when
 * the provided access/manipulation methods are used, users should not need to manipulate
 * the pool.
 */
typedef struct DArray {
    uint32_t length;        ///< Number of values in the dynamic array
    uint32_t store_size;    ///< Maximum number of values in backing store
    uint32_t start_index;   ///< Index of the first item in the array within the backing store
    double expand_rate;     ///< Expansion rate of the dynamic array
    double max_pool_size;   ///< Maximum size of the array's pool
    void **items;           ///< Backing store of the array
} DArray;

/**
 * @brief Initialise a DArray with a pool
 *
 * @see darray_err_init for errors
 *
 * @param length Length of array to create
 * @param max_pool_size Maximum size of initial pool; must be between 0 and 1
 * @param expand_rate Expansion rate of array; suitable value 1.5; must be greater than 1
 * @param pool_size Initial size of pool; must be less than the length
 * @param [out] res Result; 0 on success, otherwise non-0
 *
 * @return New DArray on success, otherwise `NULL`
 */
DArray *DArray_init_with_pool(uint32_t length, double max_pool_size, double expand_rate, uint32_t pool_size, int *res);

/**
 * @brief DArray_init errors
 * @see DArray_init_with_pool
 */
enum darray_err_init {
    DA_INIT_LENGTH      = 0x10,
    DA_INIT_M_POOL_SIZE = 0x20, ///< Invalid max_pool_size: Not between 0 and 1
    DA_INIT_EXPAND_RATE = 0x30, ///< Invalid expand_rate: Less than 1
    DA_INIT_POOL_SIZE   = 0x40, ///< Invalid pool_size: Greater than (length)/(max_pool_size)
};

/**
 * @brief Destroy a DArray and free its memory
 *
 * @param darray DArray to free
 */
void DArray_destroy(DArray *darray);

/**
 * @brief Get item of a darray at an index
 *
 * @param darray DArray to index into
 * @param index Index of value to get
 *
 * @return Value at given index, or `NULL` if it does not exist
 */
void *DArray_index(DArray *darray, uint32_t index);

/**
 * @brief Expand the backing store of a DArray
 *
 * Performance is dependent on the performance of `realloc`; generally good unless element copying
 * is required by `realloc`
 * @see darray_err_expand for errors
 *
 * @param darray DArray to expand
 *
 * @return Result; 0 on success, otherwise non-0
 */
int DArray_expand(DArray *darray);

/**
 * @brief DArray_expand errors
 * @see DArray_expand
 */
enum darray_err_expand {
    DA_EXPAND_REALLOC   = 0x10 ///< Error encountered in `realloc` call
};

/**
 * @brief Move all elements of a darray
 *
 * This function is generally used when shifting or unshifting a value<br>
 * Best-case performance: `O(n)` as each element requires moving<br>
 * Worst-case perforance: `O(n) + expansion` if array is full
 * @see darray_err_move for errors
 *
 * @param darray DArray to shift elements of
 * @param dist Distance to move and direction
 *
 * @return Result; 0 on success, otherwise non-0
 */
int DArray_move(DArray *darray, int dist);

/**
 * @brief DArray_move errors
 * @see DArray_move
 */
enum darray_err_move {
    DA_MOVE_EXPAND  = 0x10 ///< Error encountered in `DArray_expand`, see secondary detail for error
};

/**
 * @brief Push a value onto the end of a darray
 *
 * Best-case performance: `O(1)` as a single insertion is required<br>
 * Worst-case performance: `O(1) + expansion` if array is full
 * @see darray_err_push for errors
 *
 * @param darray Array to push value onto
 * @param value Value to push
 *
 * @return Result; 0 on success, otherwise non-0
 */
int DArray_push(DArray *darray, void *value);

/**
 * @brief DArray_push errors
 * @see DArray_push
 */
enum darray_err_push {
    DA_PUSH_EXPAND  = 0x10 ///< Error encountered in `DArray_expand`, see secondary detail for error
};

/**
 * @brief Pop a value from the end of a darray
 *
 * Remove the value at the end of a darray and return it<br>
 * Performance: O(1)
 *
 * @param darray Array to pop value from
 *
 * @return Value previously at end of array
 */
void *DArray_pop(DArray *darray);

/**
 * @brief Unshift a value onto the start of a darray
 *
 * Best-case performance: `O(1)` if a pool is present<br>
 * Worst-case performance: `O(1) + move` if no pool available
 * @see darray_err_unshift for errors
 *
 * @param darray DArray to push onto
 * @param value Value to push
 *
 * @return Result; 0 on success, otherwise non-0
 */
int DArray_unshift(DArray *darray, void *value);

/**
 * @brief DArray_unshift errors
 * @see DArray_unshift
 */
enum darray_err_unshift {
    DA_UNSHIFT_MOVE = 0x10 ///< Error encountered in `DArray_move`, see secondary detail for error
};

/**
 * @brief Shift a value from the start of a darray
 *
 * Remove the value at the start of a darray and return it<br>
 * Best-case performance: `O(1)` if pool can be used; removal is only step<br>
 * Worst-case performance: `O(1) + move` if pool is unavailable or requires shrinking
 * @see darray_err_shift for errors
 *
 * @param darray DArray to shift from
 * @param [out] res Result; 0 on success, non-0 otherwise
 *
 * @return Value shifted from darray
 */
void *DArray_shift(DArray *darray, int *res);

/**
 * @brief DArray_shift errors
 * @see DArray_shift
 */
enum darray_err_shift {
    DA_SHIFT_MOVE   = 0x10 ///< Error encountered in `DArray_move`, see secondary detail for error
};

/**
 * @brief Quicksort a darray (not yet implemented)
 */
void DArray_qsort(DArray *darray, int (compare)(void *val1, void *val2));

#endif
