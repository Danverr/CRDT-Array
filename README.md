What algorithms and data structures you have chosen. Why?
What are the alternatives you have considered
Where are design tradeoffs, optimizations required, e.t.c

## Structure description

My solution based on LSEQ CRDT data structure, so basically there are only 2 main parts: 
elements with ID trie and their public representation. 

**Elements**. 
Every element in my array is a tree Node, which consists of its string ID, int value and an array of children.
Because array length is changing, unique IDs for every item is a necessary for synchronizing operations between all clients.
All together they make up an ID trie, DFS traversal of which gives us elements IDs in lexicographic order. 

**Array representation**. 
For public representation I used order statistic tree (I'll use OST to be short) from policy based data structures (PBDS) C++ standard library.
It contains ordered IDs of current array elements and can erase elements, find element by ID and by its index just in O(log(n)) in worst case.
But this tree stores only IDs, to get values I'm using hash table of shared pointers to Nodes.

## Operations

### Insert

The main problem with inserting a new element is finding the right ID for it. To do this, we must go through 
parent node's children: if there is some place to insert (i.e. children count is less than ID alphabet, in our case it's 94),
we just make new node here with the lexicographically smallest possible ID. 
In the other case we need to set a new value for the far left child and perform the insert operation for it with its own old value.

_Complexity: O(log(N)) in best case and O(M) in worst (on a bamboo trie)_  
_N - current Nodes count, M - current and deleted Nodes count_

### Erase

Erasing is quite simple here: we only need to delete given ID from the OST and hash table.
Actual Node cannot be deleted due to possible operations with deleted elements, like insert after it or update.

_Complexity: O(log(N)) in average and O(N) in worst case (when hash table needs reallocation)_

### Update

To update an element we must find a corresponding Node via the OST and hash table and change its value.

_Complexity: O(log(N))_

### Array creation

Array creation consists of building a balanced Trie via BFS for allocating Nodes and DFS for setting values.

_Complexity: O(N*log(N))_

## Tradeoffs

**Trie is not rebalancing**. As you can see, insert time is very dependent on the trie depth, but the problem is that
we can't rebalance it on a go, because in this case we need to change elements IDs. Trie restructure can be done 
only when all clients are online and can download the latest balanced array version without deleted Nodes simultaneously.
But the good news is that on a large amounts of data it will grow slowly, especially if there can be up to 94 children for
each Node. It gives us starting tree depth around log_94(N), which is only 4 for 1e7 elements!

**Deleted Nodes are not actually deleted**. Another tradeoff which is made due to synchronization between clients is that
we keep deleted Nodes in the trie to make possible operations on the Nodes that are not exists anymore, which leds to
higher memory consumption.
