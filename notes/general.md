# Tips and Tricks

C++ Shenanigans:

1. Check if pointer is nullptr→ (if ptr* ==null ptr)

1. Array → sizeof(arr), Vectors → arr.size()

1. Vector Comparison: v1==v2

1. Pointers : use “→”

1. Passing vector as a param: <vector>int& v;

1. After creating a vector<vector<int>>, **only outer layer is initialisation**, must call v.push_back({})!!

1. Default initialisation using {} **(if not use {}, it will be undefined!)**
    1.  **array of pointers = nullptr type,**
    2. intergers = 0
    3. chars =”\0”
    
2. Creating a new object and assigning a pointer → **use** `new` keyword

1. Instead of `for (string s: strs)`
    1. use `for (const string& s: strs)`
    2. or even btr! `for (const auto& s: strs)`
    
2. [] operator
    1. Lambda function used inside argument `sort(nums.begin(), nums.end(), [](int a, int b)`
    2. Decomposition for tuple: `auto [a,b,c] = tuple`
    
3. You can use `auto` for anyt on the stack! (but not in structs or classes). Just not idiomatic or good

1. Integer min/max

```cpp
#include <climits>
int min = INT_MIN;
int max = INT_MAX;

// Or do
int INF = 1e9;
```

13. creating a 2d matrix using pushback vs using constructor

```cpp
vector<vector<int>> prefix(m, vector<int>(n, 0));

```

1. Functions passing:
    1. Gold standard → Using “const” + & → Tells the compiler read only no change 
        
        ![Screenshot 2026-01-26 at 9.28.02 PM.png](Tips%20and%20Tricks/Screenshot_2026-01-26_at_9.28.02_PM.png)
        
    2. Reference is prefered over pointers for idiomatic C++ code
    3. Passing a nested array requires only the top level array’s size to be unknown!
    4. Passing (int edges[]) into a function **devolves into a pointer (for optimisation to prevent copying),** can use for (auto x: arr) with arrays passed into functions. **Workaround is to a) use vectors or b) use std::array,** 

1. Using → create alias for types, E.g Using a = int;

1. Decomposition: `(auto&[u,v,w] : edges)` only works with **tuples/pairs/arrays** but not **vectors, deques or lists**
