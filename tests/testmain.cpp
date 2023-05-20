#include <cstdio>
#include <cstdlib>

int TestBinTrans(char* file, int num, int* counter);

int main()
{
    char fib[100] = "fibonacci.stass";
    char fac[100] = "factorialrec.stass";
    int counter = 0;

    for (int i = 1; i < 20; i++)
    {
        TestBinTrans(fib, i, &counter);
    }

    for (int i = 1; i < 20; i++)
    {
        TestBinTrans(fac, i, &counter);
    }

    return 0;
}

int TestBinTrans(char* file, int num, int* counter)
{
    FILE* test = fopen("test.txt", "w");
    FILE* res = fopen("testres.txt", "w");

    fprintf(test, "%d", num);
    fclose(test);

    char str1[200] = "";
    char str2[200] = "";
    char str3[200] = "";
    int num1 = 0;
    int num2 = 0;
    sprintf(str3, "./main %s", file);
    system(str3);
    sprintf(str1, "./elffile %s < test.txt >> testres.txt", file);
    system(str1);
    sprintf(str2, "sudo ./asm asmprog.txt");
    system(str2);
    system("sudo ./cpu < test.txt >> testres.txt");

    fscanf(res, "%d\n%d", &num1, &num2);

    if (num1 == num2)
        printf("Test %d success\n", *counter);
    else
        printf("Test %d failed\n", *counter);

    fclose(res);

    *counter += 1;

    return 0;
}
