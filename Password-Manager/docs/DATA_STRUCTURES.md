# Data Structures Used

## 1. Hash Table

The vault uses a hash table with separate chaining. A website name is converted into a bucket index using a case-insensitive djb2-style hash.

### Why it is used

The main operation of the application is finding a credential by website. A hash table provides **O(1) average-case lookup**, making it a natural choice for this operation.

### Implementation details

- Initial bucket count: 16
- Separate chaining for collisions
- Case-insensitive site comparison
- Table is resized when the load reaches approximately 75%
- Credential entries are owned by the hash table

## 2. Binary Search Tree

A BST is maintained as a secondary index over the same credential entries.

### Why it is used

A hash table is efficient for direct lookup but does not naturally provide sorted output. The BST allows an in-order traversal to list sites alphabetically.

### Complexity

- Insert: O(h)
- Delete: O(h)
- In-order traversal: O(n)

Here, **h** is the height of the tree and **n** is the number of stored entries.

The current BST is not self-balancing, so its worst-case height can approach O(n).

## 3. Stack

Deleted credentials are copied into a linked stack before removal.

### Why it is used

The undo feature follows **LIFO (Last In, First Out)** behavior:

1. Delete an entry.
2. Push the deleted entry onto the undo stack.
3. Select "Undo delete".
4. Pop the most recent deleted entry.
5. Reinsert it into the vault.

### Complexity

- Push: O(1)
- Pop: O(1)

## 4. Linked List

Each hash-table bucket can contain a linked list of entries when multiple sites map to the same bucket.

This is called **separate chaining** and provides collision handling without requiring open addressing.

## Combined Design

The project intentionally uses multiple structures because each one solves a different problem:

| Requirement | Data structure |
|---|---|
| Fast site lookup | Hash table |
| Alphabetical listing | BST |
| Undo deletion | Stack |
| Hash collision handling | Linked list |

This demonstrates an important data-structures principle: **choose the structure according to the operation that needs to be efficient.**
