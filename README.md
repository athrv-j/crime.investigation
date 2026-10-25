# Crime Investigation Network

A command-line C program that models a criminal case as a graph. People are
nodes, relationships (contact, visits) are directed weighted edges, and the
program answers investigation questions with graph algorithms.

## Data structures and algorithms

| Feature | Structure / algorithm |
|---|---|
| Person lookup by ID | Hash table with chaining |
| Name autocomplete | Trie |
| Relationships | Adjacency list (directed graph) |
| Fewest-step connection | BFS |
| Who is reachable | DFS |
| Lowest-cost route | Dijkstra |
| Loop detection | DFS with three colors |
| Lead ranking | Max-heap |
| Undo | Stack |
| Pending tasks | Queue |

## Build

Run from the project root:

    gcc -Wall -Wextra -Werror src/*.c -o investigation

## Run

    ./investigation

Use the menu. To try the sample case, choose 10 (Load case). The sample
file `tests/demo_case.txt` must be copied to `case.txt` first:

    cp tests/demo_case.txt case.txt

## Tests

Each test is a separate program in `tests/`. Example:

    gcc -Wall -Werror -Iheaders -I. tests/test_trie.c src/trie.c -o test_trie
    ./test_trie

## Save file format

One record per line, fields separated by `|`. Timestamps are integers.

    PERSONS: id|name|role|age|contact|notes
    EDGES: source|destination|type|weight|timestamp
    EVIDENCE: id|type|description|location_id|person_id|timestamp
    LOCATIONS: id|name|type|latitude|longitude|description
    EVENTS: id|timestamp|person_id|location_id|type|description
