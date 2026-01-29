Data Structures & Basic Algos

1.Hashmaps → `unordered_map<char, int>`
2. Vectors → `vector<int> v`
3. Arrays → `int arr[3] ={};` (or array<int, 2> using Containers version, super thin wrapper)
4. Matrices → `int mat[3][4] = {0};`
5. Matrix Vectors → `vector<vector<int>> mat(rows, vector<int>(cols, 0));`

1. Deque → `deque<int> dq;`
2. Priority Queue `priority_queue<int> pq;`

**Vector**

```cpp
vectors.push_back() /.pop_back()

vectors.push_front() /.push_back()

vectors.front()

vectors.back()

vectors.size()
```

(Interestingly, C++ deque is not a linked list, but a map of pointers to large memory chunks. The first element is inserted into middle of chunk, and then the “start” pointer is moved back as we insert front. **By provisioning a large chunk of memory, it protects against need to split it like linked lists**)

**Deque**

```cpp
deque.push_back() /.pop_back()

deque.push_front() /.push_back()

deque.front()

deque.back()

deque.size()
```

**Priority Queue;**

```cpp
#include <queue>
// A Min-Heap (gives the smallest number first)
priority_queue<int, vector<int>, greater<int>> pq;
```

**Array**

```cpp
Thin wrapper around basic C-array, but provides interface compatibilty with Containers library
```

![Screenshot 2025-12-26 at 11.04.07 PM.png](attachment:bb4173b2-2f96-4bc3-860c-36f5acafcce7:Screenshot_2025-12-26_at_11.04.07_PM.png)

**Pair/Tuple**

```cpp
Dynamic not type enforce static size data structure for fast storage on stack

std::tuple<int, int, int> tup = {10, 20, 30};
std::pair<int, int> pair = {10,10};
```

→ Lighter than a vector, stack

→ Fixed size like array

→ Not restricted by having the same types for all elements

<algorithms> 

## **Sorting:**

**Sorting in C++ using the stdlib `sort()` functions works lexicographically on iterators similar to Python**

→ Calling sort works on Containers classes, which implements the .begin() and .end() methods (they take in 2 pointers)

→ std::arrays (fastest, lives on stack, fixed size at compile time)

→ std:: vectors

→ std:: deque
→ std:: string (yes creates a new string!)

Does not work on:

→ set

→ map

→ list(linked list)

## **Binary Search:**

`lower_bound` = bisect_left()

Function signature:

```cpp
template< class ForwardIt, class T >
ForwardIt lower_bound( ForwardIt first, ForwardIt last, const T& value, custom comparator);
```

- **Templates: allow return type to be variable**

Key features → ForwardIt first = pointer start, ForwardIt last = pointer end

const T& value → Return value

custom_comparator → Define using `[](lambda expression) + normal func`  or by passing the **name of a function as a pointer.** 

***Lambda functions dunnid to declare return type!**

**Priority Queue;**

```cpp
#include <queue>
// A Min-Heap (gives the smallest number first)
priority_queue<int, vector<int>, greater<int>> pq;
```
