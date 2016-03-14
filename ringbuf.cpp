#include <iostream>
#include <stdio.h>
#include <assert.h>

int *buf = NULL;
size_t buf_sz = 0;
size_t write_ndx = 0;
size_t read_ndx = 0;
size_t cnt = 0;         // number of items in buf

bool
is_empty()
{
    return cnt == 0;
}

bool
is_full()
{
    return cnt == buf_sz;
}

// Read-pre, Write-post
// _ _ _ _
// w     r
// 1 _ _ _
//   w   r
// 1 2 _ _
//     w r
// 1 2 3 _
//       r
//       w
// 9 2 3 _
// r 
// w
// 9 _ 3 _
// w r
// 9 _ _ _
// w   r
//

int
read(int* data)
{
    if (is_empty())
        return 0;

    --cnt;
    read_ndx = (read_ndx+1) % buf_sz;
    *data = buf[read_ndx];

    return 1;       // success
}

void
write(int data)
{
    if (is_full()) {
        int data;
        read(&data);        // discard oldest item
    }
    ++cnt;
    buf[write_ndx] = data;
    write_ndx = (write_ndx+1) % buf_sz;
}

void
deinitialize()
{
    assert(buf);
    free(buf);
    buf = NULL;
}

int
initialize(size_t _buf_sz)
{
    assert(!buf && _buf_sz > 2);

    buf_sz = _buf_sz;
    buf = (int*) malloc(sizeof(int) * buf_sz);

    write_ndx = 0;
    read_ndx = buf_sz-1;    // preincrement read

    if (!buf) {
        return 0;
    } else {
        return 1;
    }
}

int
main()
{
    if (!initialize(4)) {
        fprintf(stderr, "Failed to initialize\n");
        return -1;
    }

    write(1); write(2); write(3); write(4);

    int data;
    assert(read(&data) == 1 && data == 1);
    assert(read(&data) == 1 && data == 2);
    assert(read(&data) == 1 && data == 3);
    assert(read(&data) == 1 && data == 4);
    assert(is_empty());

    write(1); write(2); write(3); write(4); write(5);
    assert(is_full());
    assert(read(&data) == 1 && data == 2);
    assert(read(&data) == 1 && data == 3);
    assert(read(&data) == 1 && data == 4);
    assert(read(&data) == 1 && data == 5);
    assert(is_empty());

    write(1); write(2); write(3); write(4); write(5); write(6);
    assert(is_full());
    assert(read(&data) == 1 && data == 3);
    assert(read(&data) == 1 && data == 4);
    assert(read(&data) == 1 && data == 5);
    assert(read(&data) == 1 && data == 6);
    assert(is_empty());

    deinitialize();

    return 0;
}

