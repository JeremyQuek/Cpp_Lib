**Pointer Type VS Array Type**

Arrays decay into pointers when passed into functions. `p = &p[0]`
However, the underlying typing of array and pointers is different

A variable of type array ≠ a variable of type int pointers!

A variable of type vector ≠ a variable of type int pointer!

This comes from how the compiler regards the fundamental type differences under the hood.

When trying to declare a pointer:

```python
int* arr= &p; //Creates a pointer to address p
P can be an array int type
BUT p cannot be a int[] type!!
Theres is a fundamental difference.
```

In other words, type integer pointers and type arrays are NOT compatible and mutually exchangeable. **Performing sizeof(arr) for arr of type int*, even if it points to an array is NOT defined and should not be used!!**

However, declaring an array of pointers is possible:

```python
int* []arr = {}
In which case, when arr is passed into a function, arr decays to type **int
```

**Malloc & Pointers**

Malloc always return a pointer regardless of the type of memory structure being allocated. This is because malloc doesn’t care or know what its allocating, it simply reserves the required amount of space required and returns a pointer to that starting address.

**Hence, when it comes to deferencing the type of pointer u must be careful!! You may do so safely after initialising the memory at that location, however this uniquely fails for arrays.**

→ For ints, strings, chars, floats, primitives → Its safe

→ For objects, it is also safe

→ For vectors, yes works, because vectors are objects with constructs

→ Arrays (uniquely non viable!!)

The reason is because malloc returns a pointer of a specific type, however, 1) arrays by definition are a primitive with no constructor (u cant create an type of “array” on the heap) and 2) arrays do not have a “array pointer” type which malloc can return!
