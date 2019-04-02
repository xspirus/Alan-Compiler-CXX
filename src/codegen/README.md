## Codegen Utilities
Defines several useful structures to be used for the
generation of _LLVM IR_.

### FuncMap
`HashMap` of `key: std::string` and `value: llvm::Function*`
#### Usage
During code generation we need to make a call to some function.
Before creating the call, we lookup the HashMap for the function
(it will be there, or else semantic analysis would have given an
error) and now we have information on its parameters. That way,
we can check which parameters need to be passed as values and
which as pointers.

---

### FunctionBlock
`Class` to contain a pseudoscope.
#### Members
* **args**
  * `std::vector<llvm::Type*>`
  * contains the types of the function arguments
* **vars**
  * `std::unordered_map<std::string, llvm::Type*>`
  * contains the types of all scope variables
  * useful to determine when we have a referenced variable
* **vals**
  * `std::unordered_map<std::string, llvm::AllocaInst*>`
  * contains Alloca Instructions for values
* **addrs**
  * `std::unordered_map<std::string, llvm::AllocaInst*>`
  * contains Alloca Instructions for addresses (pointers)
* **currentBB**
  * `llvm::BasicBlock*`
  * current insertion block
  * useful to restore insertion block during other function declaration
