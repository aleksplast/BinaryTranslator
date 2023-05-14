# JIT

## Goal of this project

The goal of this project is to create JIT compiler for my language. We will also compare it's working speed with my CPU's working speed.

## Main idea

In this project (¬—“¿¬»“‹ —ﬁƒ¿ ——€À ”), we wrote our own Programming language. It translates programm into binary code, which is then executed on our Processor emulator.

Let's take a step further and try to translate our programm into x86-64 machine code. We will put it into a buffer and then execute it with the help of **mprotect**.

Let's take a look, how it can be done, step by step.

## Step I: Itermediate representation.


