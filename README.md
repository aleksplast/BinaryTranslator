# Compiler

## Goal of this project

The goal of this project is to create compiler for my language. We will also compare it's perfomance with CPU emulator perfomance.

## Main idea

In this [project](https://github.com/aleksplast/My-language), we wrote our own Programming language. It translates programm into binary code, which is then executed on our CPU emulator.

Let's take a step further and try to translate our programm into **x86-64 machine code**. Final goal is to create elf file, which can execute our programm.

Let's take a look, how it can be done, step by step.

## Step I: Intermediate representation (IR).

**Intermediate representation** - data structure, that contains info about programm. With the help of IR it is easier to translate code and make optimizations.

First of all, we have a tree, constructed by our frontend. Of course, we can use it as IR, but it lacks information about commands and translation into binary format is harder due to non-linear commands order.

<details>
  <summary> Tree dump </summary>
<img align="center" src = "https://github.com/aleksplast/BinaryTranslator/assets/111467660/8f24ee34-7a5c-4b0c-af8f-89fd5b57d078">
</details>

Let's parse our tree into new form, which looks like this:

~~~
+-------------------------------------+-------------------------------------+
|            Function 1               |            Function 2               |
|-------------------------------------|-------------------------------------|
|    Block0     |        Block1       |    Block0     |        Block1       |
|---------------|---------------------|---------------|---------------------|
|  Cmd1 |  Cmd2 | Cmd1 | Cmd2 | Cmd3  |  Cmd1 |  Cmd2 | Cmd1 | Cmd2 | Cmd3  |
|-------|-------|------|------|-------|-------|-------|------|------|-------|
| info  | info  | info | info | info  | info  | info  | info | info | info  |
+-------------------------------------+-------------------------------------+
~~~

Programm is divided into function structures, then each function is divided into blocks. Each block contain an array of command structures, where commands is executed in linear order. Each command structure contain various information about command itself.

With the IR done, let's proceed to translation itself.

## Step II: Translation into buffer

First of all, we must think about structure of our programm: where to store variables, how to make arithmetic operations, how to pass parameters into functions and many more. Also we must be certain that it works properly. For this we can take an extra step.

### NASM file step

Let's create NASM file from our IR, as it is easier to debug. This step is taken for debug and debug only. In this step we realised the structure of programm: variables is stored in memory, arithmetic operations is done with the help of **rax** and **rbx** registers, parameters is passed throught the stack. When all done and working, we can proceed to the final step.

### Generating binary code

Here is how each command is translated. Each command is shown in assembly analogue for clarity.

**Variable declaration** 
~~~Assembly
mov rax, variable value  
mov [r11 + offset], rax ; r11 + offset is the address, where variable is stored
~~~


**Assignment** 
~~~Assembly
mov rax, variable value  
mov [r11 + offset], rax ; r11 + offset is the address, where variable is stored
~~~

**Arithmetic operations** 
~~~Assembly
mov rax, variable1 value
mov rbx, variable2 value 
Arithmetic operation   
mov [r11 + offset], rax  ; r11 + offset is the address, where variable is stored
~~~

**Parameters** 
~~~Assembly
push param value ; When calling a function
pop param value ; At the start of the function
~~~

**Function call** 
~~~Assembly
call relative address
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

**Printf**

This function is realised like this:
~~~C++
int Print(int num)
{
    printf("%d ", num);

    return NOERR;
}
~~~

Then, by calculating relative address, we can call this function from our bin code.

**Scanf**

This function is similar to Printf:
~~~C++
int Scanf(int* num)
{
    scanf("%d", num);

    return NOERR;
}
~~~

At this point, is we can execute buffer with commands with the help of **mprotect**. When we are sure, that everything is working properly, we can do the final step.

## STEP III: ELF file

Let's create a small ELF file, that contains only elf header, programm header and one section. We can place our buffer with translated commands in here. There is a problem with **printf** and **scanf**, so we must rewrite them by our own hands on assembly. With everything done, let's do some perfomance tests.


## Perfomance tests

To test compiler, we wrote programms, that calculates factorial and fibonacci numbers. You can find examples [here](https://github.com/aleksplast/My-language/tree/main/examples). 

Let's compare elf execution time and our CPU emulator execution time. Last one was compiled with -O3 flag. First of all, let's test calculation of 5'th fibonacci number. Numbers in the table below show average speed of this calculation.

|    | CPU emulator | elf | 
| :----------: | :-------------------: | :-------------------: | 
| Execution time (μs) | 205 |      35        |    
| Speed growth | 1 |      5.8        |    

Now, let's test calculation of 10 factorial. Numbers in the table below show average speed of this calculation.

|    | CPU emulator | elf | 
| :----------: | :-------------------: | :-------------------: | 
| Execution time (μs) | 136.7 |     26.7         |    
| Speed growth | 1 |     5.1        |  

We can notice significant growth in speed.

## Conclusion

In this work, we made a working compiler for our language. It gives significant boost in perfomance. Speed growth is around **5-6** times in tested programms.

## References

https://github.com/aleksplast/My-language

https://www.felixcloutier.com/x86 - x86 instruction reference

http://citforum.ru/programming/theory/serebryakov/8.shtml - some information about IR

https://cirosantilli.com/elf-hello-world - simple elf file











