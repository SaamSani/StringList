#include "StringList.h"
#include <stdexcept>
#include <iostream>
using std::out_of_range;
using std::cout;
using std::endl;

// Default constructor - makes an empty list of capacity 10
StringList::StringList()
{
	capacity = 10;
	n = 0;
	arr = new string[capacity];
}

// Copy constructor
StringList::StringList(const StringList& other)
{
	copyList(other);
}

// ***UNDOABLE
// Overloaded assignment operator
StringList& StringList::operator=(const StringList& other)
{
	if(&other != this)
	{
		// Push the current full list onto the undo stack before modifying the array
		undoStack.push("ASSIGN", 0, "", *this);		
		delete[] arr;
		copyList(other);
	}
	return *this;
}

// Destructor
StringList::~StringList()
{
	delete[] arr;
}

// ACCESSORS
// Returns the number of elements in the list
int StringList::size() const
{
	return n;
}

// Returns true if the list is empty, false otherwise
bool StringList::empty() const
{
	return n == 0;
}

// Returns the string at the given index
string StringList::get(int i) const
{
	checkBounds(i, "get");
	return arr[i];
}

// Returns the index of the first occurrence of the given string
int StringList::index(string s) const
{
	int i = 0;
	while (i < n && s != arr[i]) {
		i++;
	}
	if (i == n) {
		return -1;
	}
	else {
		return i;
	}
}

// Returns true iff the given string is in the list
bool StringList::contains(string str) const
{
	return !(index(str) == -1);
}

// Returns true if the two lists are equal, false otherwise.
// Does *not* consider any undo information when comparing two Stringlists. All
// that matters is that they have the same strings in the same order.
bool StringList::operator==(const StringList& sl) const
{
	if (size() != sl.size())
	{
		return false;
	}
	for (int i = 0; i < size(); i++)
	{
		if (get(i) != sl.get(i))
		{
			return false;
		}
	}
	return true;
}

// Returns true if the two lists are not equal, false otherwise
bool StringList::operator!=(const StringList& sl) const
{
	return !(*this == sl);
}

// Returns a string representation of the list.
string StringList::toString() const
{
	string result = "{";
	for (int i = 0; i < size(); i++)
	{
		if (i > 0) {
			result += ", ";
		}
		result += "\"" + get(i) + "\"";
	}
	return result + "}";
}


// MUTATORS

// ***UNDOABLE
// Sets the value at the given index
void StringList::set(int i, string str)
{
	checkBounds(i, "set");
	// Push the current full list onto the undo stack before modifying the array
	undoStack.push("SET", i, arr[i], *this);
	arr[i] = str;
}

// ***UNDOABLE
// Inserts the given string *before* the given index
void StringList::insertBefore(int index, string str)
{
	// Doesn't use checkBounds because it's okay to insert at the end
	if (index < 0 || index > size()) {
		throw out_of_range("StringList::insertBefore index out of bounds");
	}
	checkCapacity();

	for (int i = n; i > index; i--) {
		arr[i] = arr[i-1];
	}
	// Push the current full list onto the undo stack before modifying the array
	undoStack.push("INSERT", index, "", *this);

	arr[index] = str;
	n++;
}

// ***UNDOABLE
// Inserts the given string at the front of the list
void StringList::insertFront(string str)
{
	checkCapacity();
	insertBefore(0, str);
}

// ***UNDOABLE
// Inserts the given string at the back of the list
void StringList::insertBack(string str)
{
	checkCapacity();
	insertBefore(n, str);
}

// ***UNDOABLE
// Removes the element at the given index and moves elements after it down
void StringList::remove(int index)
{
	checkBounds(index, "remove");
	// Push the current full list onto the undo stack before modifying the array
	undoStack.push("REMOVE", index, arr[index], *this);
	for (int i = index; i < n; i++) {
		arr[i] = arr[i + 1];
	}
	n--;
}

// ***UNDOABLE
// Empties the list
void StringList::removeAll() {
    // Push the current full list onto the undo stack before modifying the array
    undoStack.push("REMOVE_ALL", 0, "", *this);
    for (int i = 0; i < n; i++) {
        arr[i] = "";
    }
    n = 0;
}


// Prints the list
void StringList::print() const
{
	for (int i = 0; i < n; i++) {
		cout << arr[i] << endl;
	}
}

// Helper function for checking index bounds.
void StringList::checkBounds(int i, string s) const
{
	if (i < 0 || i >= size()) {
		throw out_of_range("StringList::" + s + " index out of bounds");
	}
}

// indexT: Doubles the capacity of the list if n == capacity
void StringList::checkCapacity()
{
	if (n == capacity) {
		capacity *= 2;
		string* temp = new string[capacity];
		
		for (int i = 0; i < n; i++) {
			temp[i] = arr[i];
		}
		delete[] arr;
		arr = temp;
	}
}

// Helper function to copy the contents of another list
void StringList::copyList(const StringList& lst)
{
	capacity = lst.capacity;
	n = lst.n;
	arr = new string[capacity];

	for (int i = 0; i < n; i++) {
		arr[i] = lst.arr[i];
	}
}


// Undo function to undo actions
void StringList::undo() {
	if (undoStack.size == 0) return; // No actions to undo

    stack::action& lastAction = undoStack.actions[undoStack.size - 1];

    if (lastAction.command == "INSERT") {

        for (int i = lastAction.index; i < n - 1; i++) {
            arr[i] = arr[i + 1];	// Shift elements to the left, starting from the removed index
        }
        n--;  // Decrease the number of elements
    } 
    else if (lastAction.command == "REMOVE") {

		for (int i = n; i > lastAction.index; i--) {
            arr[i] = arr[i - 1];   // Shift elements to the right, starting from the last index up to the insertion point
        }
        arr[lastAction.index] = lastAction.value;  // Reinsert the removed value
        n++;  // Increase the number of elements
        
    } 
    else if (lastAction.command == "SET") {
        // Ensure the index is valid before setting the value back
        if (lastAction.index >= 0 && lastAction.index < n) {
            arr[lastAction.index] = lastAction.value; // Undo a set by restoring the original value
        }
    } 
    else if (lastAction.command == "REMOVE_ALL" || lastAction.command == "ASSIGN") {
        // Free the current array as we are about to restore the full list
        delete[] arr;

        // Allocate a new array with the size of the restored full list
        arr = new string[lastAction.fullListSize];

        // Copy the restored list from the undo action's fullList into the current array
        for (int i = 0; i < lastAction.fullListSize; ++i) {
            arr[i] = lastAction.fullList[i];
        }

        // Update the number of elements in the list to match the restored full list size
        n = lastAction.fullListSize;
        capacity = lastAction.fullListSize;
    }

    undoStack.pop(); // Remove the undo action from the undo stack
}



// Resize the undo stack
void StringList::stack::resize() {
    // Doubling the stack size
	capacity *= 2;

    action* newActions = new action[capacity];

	// Copying all the old actions from the old stack
    for (int i = 0; i < size; ++i) {
        newActions[i] = actions[i];
    }

	// Deleting old action and setting it to the new one
    delete[] actions;
    actions = newActions;
}

void StringList::stack::copyArrayHelper(string* dest, const string* src, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

// Constructor
StringList::stack::stack() : capacity(4), size(0) {
	// Initialize the stack with a starting capacity of 4 actions
    // The array 'actions' will store the undoable operations
    actions = new action[capacity];
}

// Deconstructor
StringList::stack::~stack() {
	// Loop through each action and delete its associated fullList if it exists
    for (int i = 0; i < size; ++i) {
        delete[] actions[i].fullList;
    }
    delete[] actions;
}

// Pop the latest undo action 
void StringList::stack::pop() {
    if (size == 0) {  // Check if there are no elements in the stack
        return;
    }

    // Deallocate memory if fullList was used (in the case of REMOVE_ALL or ASSIGN)
    if (actions[size - 1].fullList != nullptr) {
        delete[] actions[size - 1].fullList;  // Free dynamically allocated memory
        actions[size - 1].fullList = nullptr;  // Set pointer to null after deletion
    }

    size--;  // Reduce the size to pop the last element
}

// Push actions into the stack
void StringList::stack::push(const std::string& command, int index, const std::string& value, const StringList& lst) {
    if (size == capacity) resize(); // Resize stack if full
   
    actions[size].command = command;

    actions[size].index = index;

    actions[size].value = value;

    if (command == "REMOVE_ALL" || command == "ASSIGN") {
        actions[size].fullListSize = lst.size();	// Store the current size of the list
        actions[size].fullList = new string[lst.size()];	// Allocate memory for the full list
        copyArrayHelper(actions[size].fullList, lst.arr, lst.size());	// Copy the list's elements
    }

	// Increment the size of the stack to point to the next available slot
    size++;
}









void StringList::insertBefore(int pos, string str){
	if(pos > n || pos < 0){
		return;
	}
	node* newNode = new node;
	newNode->data = str;

	if(pos == 0){
		if(head == nullptr){
			head = newNode;
			tail = newNode;
			newNode->next = nullptr;
            newNode->prev = nullptr;
		}
		else{
			newNode->next = head;
			newNode->prev = nullptr;
			head->prev = newNode;
			head = newNode;
		}
	}

	else if(pos == n){
		newNode->prev = tail;
		newNode->next = nullptr;
		if(tail != nullptr){
			tail->next = newNode;
		}
		tail = newNode;
	}

	else{
		node* ptr = head;
		int num = 0;
		while(num != pos-1){
			ptr = ptr->next;
			num += 1;
		}
		newNode->next = ptr->next;  // New node's next points to the current next
        newNode->prev = ptr;  // New node's prev points to current node (ptr)

        ptr->next->prev = newNode;  // Update the next node's previous pointer
        ptr->next = newNode;  // Current node's next points to the new node

	}
	n++;

}


void StringList::checkCapacity(){
	
}
