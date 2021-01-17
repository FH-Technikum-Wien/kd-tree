# Kd-tree
A simple kd-tree implementation for triangles that stores the data in its nodes.

If using an obj file with duplicate vertices, make sure to use `--slow`. Otherwise intersections may not be accurate.

## Build .exe usage

|  |  |
| --- | --- |
| `--load [-l] <file>` | Obj filename to read from |
| `--triangles [-p] <numberOfTriangles>` | Number of random triangles to be generated |
| `--range [-r] <vertexRange>` | Range in which the random vertices will be generated |
| `--interactive [-i] ` | Enables 'interactive-mode' allowing to define custom rays |
| `--verbose [-v]` | Prints out additional information |
| `--slow [-s]` | Uses a slow procedure to check and merge same vertices |
| `--help` | Prints out this table |
