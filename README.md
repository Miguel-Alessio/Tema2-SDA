# In-Memory Search Engine 🔍⚡

A fast, lightweight, and efficient text-indexing system built entirely in C. This project simulates the core mechanics of a search engine, supporting operations like dynamic document indexing, fast keyword lookups, prefix-based autocomplete, and TOP-K relevance retrieval.

## 🧠 Data Structures & Architecture

To achieve optimal performance across all operations, the engine relies on a synergy of three fundamental data structures:

### 1. Radix Tree / Trie (The Core Engine)
* **Purpose:** Handles all string-matching operations with $O(w)$ time complexity (where $w$ is the word length), making search speed entirely independent of the total number of documents in the database.
* **Mechanism:** Each Trie node that marks the end of a word holds a Linked List of references pointing back to the specific files containing that keyword. When keywords are removed (`DELKW`), the Trie dynamically prunes dead branches to prevent memory leaks.

### 2. Max/Min-Heap (For TOP-K Retrieval)
* **Purpose:** Efficiently fetching the most relevant search results.
* **Mechanism:** Instead of sorting the entire database (which would be computationally expensive), matched documents are pushed into a Heap prioritized by their relevance score (and lexicographically in case of ties). The system then extracts only the requested top $K$ results using `heapifyDown` operations.

### 3. Doubly Linked List (Document Database)
* **Purpose:** Acts as the primary database holding document metadata.
* **Mechanism:** Stores file IDs, relevance scores, and dynamically allocated arrays of keywords. It allows for fast document insertion (`ADD`) and $O(1)$ pointer-based deletion (`DEL`).

---

## ⚙️ Features & Operations

* **`ADD` / `DEL`:** Dynamically inserts or removes documents from the database while synchronizing the Trie index. 
* **`ADDKW` / `DELKW`:** Mutates the keywords of an existing document and updates the localized Trie branches without rebuilding the index.
* **`FIND`:** Executes a direct $O(w)$ traversal on the Trie. If a match is found, it collects all referenced files, stores their pointers in a dynamically sized array, and sorts them using `qsort` for immediate display.
* **`PREFIX` (Autocomplete):** Traverses the Trie to the end of the given prefix, then launches a recursive Depth-First Search (DFS) across the subtree to aggregate all unique files containing any word starting with that prefix.

---

## 🛠️ Engineering Challenges & Optimizations

* **Memory Limits & Segmentation Faults:** During extreme stress testing, allocating massive 2D arrays on the stack for sorting (e.g., `char file_names[25000][50]`) caused stack overflows. 
  **Solution:** Refactored the collection logic to store only pointers to the original File IDs (`const char**`). I added a pre-counting loop to allocate exact memory on the heap via `malloc`, drastically reducing the memory footprint and preventing crashes.
* **Robust Input Parsing:** Some malformed test cases had metadata headers (e.g., number of operations) that didn't match the actual file contents. 
  **Solution:** Implemented a resilient I/O fallback mechanism relying on `scanf` return values rather than hardcoded operation counts, ensuring perfect synchronization with the testing checker.
* **Granular Error Handling:**
  Implemented strict error separation to distinguish between structural failures (`NOT FOUND` when trying to delete non-existent data) and empty search results (`EMPTY` for valid queries with zero hits).

---

## 💻 Tech Stack
* **Language:** C
* **Concepts:** Advanced Memory Management, Tree Traversals (DFS), Heapify Algorithms, Pointer Arithmetic.
