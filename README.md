# JIT

## Goal of this project

The goal of this project is to create JIT compiler for my language. We will also compare it's working speed with my CPU's working speed.

## Main idea

In this [project](https://github.com/aleksplast/My-language), we wrote our own Programming language. It translates programm into binary code, which is then executed on our Processor emulator.

Let's take a step further and try to translate our programm into **x86-64 machine code**. We will put it into a buffer and then execute it with the help of **mprotect**.

Let's take a look, how it can be done, step by step.

## Step I: Intermediate representation (IR).

**Intermediate representation** - data structure, that contains info about programm. With the help of IR it is easier to translate code and make optimizations.

First of all, we have a tree, constructed by our frontend. Of course, we can use it as IR, but it lacks information about commands and translation into binary format is harder due to non-linear commands order.

ЗДЕСЬ ЖЕСКАЯ КАРТИНКА ДЕРЕВА 

Let's parse our tree into new form, which looks like this:

ЗДЕСЬ ЖЕСКАЯ КАРТИНКА ИР'а

Programm is divided into function structures, then each function is divided into blocks. Where each block contain an array of command structures, where commands is executed in linear order. Each command structer contain various information about command itself.

For example, IF command creates two new blocks: first is executed if the condition is true, second if not. 

With the IR done, let's proceed to translation itself.

## Step II: Translation

First of all, we must think about structure of our programm: where to store variables, how to make arithmetic operations, how to pass parameters into functions and many more. Also we must be certain that it works properly. For this we can take an extra step.

### NASM file step

Let's create NASM file from our IR, as it is easier to debug. In this step we realised the structure of programm: variables is stored in memory, arithmetic operations is done with the help of **rax** and **rbx** registers, parameters is passed throught the stack. When all done and working, we can proceed to the final step.

### Generating binary code

Here is how each command is translated:

**Variable declaration** 
~~~Assembly
mov rax, variable value  
mov [r11 + offset], rax ; r11 + offset is the adress, where variable is stored
~~~


**Assignment** 
~~~Assembly
mov rax, variable value  
mov [r11 + offset], rax ; r11 + offset is the adress, where variable is stored
~~~

**Arithmetic operations** 
~~~Assembly
mov rax, variable1 value
mov rbx, variable2 value 
Arithmetic operation   
mov [r11 + offset], rax  ; r11 + offset is the adress, where variable is stored
~~~

**Parameters** 
~~~Assembly
push param value ; When calling a function
pop param value ; At the start of the function
~~~

**Function call** 
~~~Assembly
call relative adress
~~~

**Return** 
~~~Assembly
ret  
~~~

**IF** 
~~~Assembly
cmp cond, 0  
jne blockIF  
jmp blockELSE   
~~~






