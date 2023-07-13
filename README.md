# JNP1 Tasks

This repository contains a collection of collaborative solutions to tasks from
the course "Languages and Programming Tools 1".

## Overview

JNP1 stands for "Języki i Narzędzia Programowania 1" in Polish, which translates
to "Languages and Programming Tools 1". The number "1" indicates that it is the
first course in the series. There are also courses JNP2 and JNP3, but they are
not directly connected to this one. This repository includes solutions to the
tasks from JNP1. All the tasks are related to C++. Each task was completed by
different teams, with up to three members per team. There are 7 tasks in total,
but I only solved the first 6, which was enough to complete the course.

## Task Descriptions

Here is a list of short task descriptions:

1. **Top 7**: Practice using the C++ standard library to create a program that
manages a music chart, allowing voting on songs within a specified range and
generating summaries of the top seven highest-ranked tracks.

2. **Hash**: Develop a C++ module, "hash," that implements a hash table for
storing sequences of `uint64_t` numbers. The module provides functions for
creating and deleting hash tables, inserting and removing sequences, retrieving
table size, and checking for sequence existence. It utilizes C++ standard
library containers and methods while encapsulating non-interface functions and
global variables.

3. **Money Bag**: Create a class called `Moneybag` that represents money pouches
in the Frankish state. The class should support operations such as creating a
pouch with a specified number of coins, performing arithmetic operations on
pouches, retrieving coin counts, and handling exceptions for arithmetic
overflow. Additionally, implement a Value class to represent the value of a
pouch in denarii and provide comparison and conversion methods.

4. **Organism**: Simulate interactions between organisms in a natural
environment using the `Organism` template. Organisms have different species,
vitality, and dietary preferences. They can mate, fight, and consume each other
based on predefined rules. Implement the required functionality using template
specialization and constexpr calculations.

5. **KV FIFO**: Implement a container pattern that behaves like a FIFO queue,
where each element is associated with a key. The container should provide strong
exception safety guarantees and implement copy-on-write semantics. The `kvfifo`
class template should support operations such as pushing elements, popping
elements, moving elements to the back, accessing front and back elements,
retrieving elements by key, checking the size and emptiness, counting elements
by key, clearing the container, and iterating over the keys.

6. **World Cup**: Implement the board game "Wyjście z Grupy" (Exit from the
Group) in C++. The game involves 2 to 11 players who compete to accumulate the
most points and exit the group. Players roll dice, move on a cyclic board with
12 fields, and perform actions associated with each field. The implementation
should handle game mechanics, player interactions, and game termination
conditions.

Please refer to the respective task directories for detailed solutions.
