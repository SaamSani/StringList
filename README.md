# StringList Class with Undo Feature

This project implements a **StringList** class with an **undo** feature, showcasing how to maintain a history of list modifications and roll back to a previous state. The `StringList` class uses a singly linked list for its underlying data structure and a dynamic array-based stack for managing undo actions.

---

## Features

1. **Undoable Operations**:
   - `insertFront`, `insertBack`, and `insertBefore`
   - `set` (modifying list elements)
   - `remove` and `removeAll`
   - Assignment operator (`operator=`)

2. **Custom Stack Implementation**:
   - Dynamic array stack with automatic resizing.
   - Initial capacity of 4, doubles as needed.

3. **Extensibility**:
   - Designed to be adaptable for other data types besides strings, such as integers or objects.

4. **Memory Safety**:
   - Proper memory allocation and deallocation to avoid memory leaks.

---

## Code Structure

### **StringList.h**
Defines the `StringList` class, including public methods for list manipulation, private helper functions for operations, and a nested `stack` class to handle undo actions.

- **Key Methods**:
  - `insertFront`, `insertBack`, `insertBefore`
  - `set`, `remove`, `removeAll`
  - `undo` to revert changes.
  - Overloaded assignment operator for deep copying.

- **Undo Stack**:
  - A private `stack` class within `StringList.h` manages undo operations with commands such as `INSERT`, `REMOVE`, `SET`, etc.

### **StringList.cpp**
Implements the `StringList` methods and the undo functionality. Key highlights:

1. **Undo Stack**:
   - Commands like `INSERT`, `REMOVE`, `SET`, and `REMOVE_ALL` are stored with necessary data.
   - `undo()` applies the inverse of the last operation.

2. **Dynamic Memory Management**:
   - The stack resizes dynamically to handle multiple undo actions efficiently.

3. **Edge Cases**:
   - Handles scenarios like undoing after `removeAll` or undoing `operator=`.

### Example Operations with Undo

```cpp
StringList lst;

// Insert elements and undo them
lst.insertBack("apple");
lst.insertBack("banana");
lst.insertFront("cherry");

// Current list: {"cherry", "apple", "banana"}
lst.undo();
// After undo: {"apple", "banana"}

lst.set(1, "grape");
// Current list: {"apple", "grape"}
lst.undo();
// After undo: {"apple", "banana"}
