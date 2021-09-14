# A C++17 Flow-Based Programming Library 

[![Windows Build + Test](https://github.com/beyse/ExecutionNodes/actions/workflows/windows.yml/badge.svg)](https://github.com/beyse/ExecutionNodes/actions/workflows/windows.yml)
[![Ubuntu Build + Test](https://github.com/beyse/ExecutionNodes/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/beyse/ExecutionNodes/actions/workflows/ubuntu.yml)

ExecutionNodes was built with simplicity in mind. The library lets you build applications by connecting reusable building blocks (Nodes). The connected nodes form a graph which ensures that all nodes are executed in the right order. It also supports parallel execution. This makes it very easy to parallelize your application. 

Other than most similar libraries out there, ExecutionNodes provides a standard way to: 
* Inject initial information packages into the graph using JSON
* Connect the multiple inputs and outputs between nodes using names rather than indices
* Define and load graphs using configuration files in JSON format

## Table of Contents


## Introduction
First, I would like to introduce the main concepts.
There is nodes, ports and connections between them. Each node can have any number of input ports and any number of output ports. Ports can be connected with eachother and they transport information (any C++ object) in and out of a node. You can connect any output port of a node to any number of input ports of any other node. You can even use any datatype you want (as long as it is moveable or copyable) and as long as the datatype between input and output match.   

The graph based system automatically resolves the dependencies and ensures that nodes are executed in the right order and the data packages are transferred to the right nodes. Because of that, node execution can be parallelized with the snap of your fingers, greatly reducing runtime for computational demanding processing pipelines.



### Motivation
I first came accross the idea 

### Features

## Getting Started
### Integrating the library
### Writing a first example 
Let's say we want to build that simple logic circuit:
[Image of Logic Circuit]

There is always three things you need:
1. Some nodes you implemented and encapsulate a reusable functionality
2. Something that describes how the nodes shall be connected and interplay with eachother. We call it a `graphDefinition`
3. A main function which constructs the graph and executes it 

We will start with the last one:

```cpp
#include <execution_nodes/execution_nodes.h>

using namespace execution_nodes;
static const NodeRegistry registry = { ... } // This will be explained in a moment

int main() {
  // Graph definition can be loaded from a json file
  std::string filePath = "./example1/graph.json";
  GraphDefinition d = loadGraphDefFromJsonFile(filePath);
  // Graph is constructed using the graph definition and the node registry
  Graph graph(d, registry);
  // Now we can execute the graph which runs all nodes
  graph.executeSerial();
}
```
And this is it! This tiny main function can now perform any behaviour we defined
in the `graph.json`. 

Now, we take a look at what is inside the this `graph.json` file:
```json
{
    "name": "Logic Gates Example",
    "nodes": {
        "random0": {
            "type": "RandomNumberGenerator",
            "settings": {
                "seed": 42
            }
        },
        "random1": {
            "type": "RandomNumberGenerator",
            "settings": {
                "seed": 69
            }
        },
        "adder": {
            "type": "Adder"
        },
        "printer": {
            "type": "NumberPrinter"
        }
    },
    "connections": [
        [ "random0:number" , "adder:a" ],
        [ "random1:number" , "adder:b" ],
        [ "adder:sum" , "printer:number" ]
    ]
}
```
So we can see that we have 4 nodes in there each with a distinct name. The first node is called and has the type. There is also a section definiting the settings for this node. This in literature is sometimes called initial information package. 

The last section is called connections and it contains a complete list of all connections we want to have in this graph.

## Examples


## Build