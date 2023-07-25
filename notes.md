- Uniforms are global variables that are available at any shader stage. They are often used to pass data that is identical for all vertices or fragments.
- Uniform blocks seem to just be structs for uniforms. The benefit is they can be bound to binding points and transfer all relevant uniform data at once.

- std140 aligns and packs memory predictably for memory allocation optimizations
    The std140 layout specifies the following rules for the memory layout of a uniform block:
    - Each member of the uniform block is aligned to a 4-byte boundary.
    - Vectors and matrices are aligned to a multiple of their size. For example, a vec3 is aligned to a 12-byte boundary (3 * 4 bytes), while a mat4 is aligned to a 64-byte boundary (4 * 4 * 4 bytes).
    - Arrays of scalars, vectors, or matrices are laid out in memory as a sequence of elements, with each element aligned according to the rules above.
    - The overall size of the uniform block is rounded up to a multiple of 16 bytes.
- Alternative layouts:
    - 430 seems to enable faster access at memory penalty
    - packed is the opposite, tightly packs to conserve memory but requires extra operations to unpack the data
    - shared allows multiple uniform blocks to share the same memory region, which can be useful for reducing memory usage and improving performance in some cases. However, this can also make it harder to predict the layout of data in memory, which can make debugging more difficult.

- "Array of structures" vertex attributes are generally more performant (locality caching) unless only one dimension of the structure is commonly changing, which will force a re-upload of the entire array. Highly-dynamic data should be stored in a separate array and then only that array can be re-uploaded as a vertex attribute.

- **glShaderSource** accepts an *array* or strings. If you allocate a char array for one string, you will need to provide a pointer to *the starting pointer*. I accomplished this with anonymous array containing the address to the string like: `(const char*[]){&shader_body}`.

Textures have 8 or 16 slots. Not sure how big each slot is. You can activate the slot 

- https://en.wikipedia.org/wiki/Stdarg.h#Declaring_variadic_functions for printf-style indefinite args


- **glBufferSubData**!!!!!