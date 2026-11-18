#include <stdio.h>
#include <string.h>

void findXOR(char dividend[], char divisor[], int pos, int m)
{
    dividend[pos] = '0';
    for (int i = 1; i < m; i++)
    {
        dividend[pos + i] = (dividend[pos + i] == divisor[i]) ? '0' : '1';
    }
}

void mod2Div(char dividend[], char divisor[], char remainder[])
{
    int n = strlen(dividend);
    int m = strlen(divisor);

    for (int i = 0; i <= n - m; i++)
    {
        if (dividend[i] == '1')
        {
            findXOR(dividend, divisor, i, m);
        }
    }

    strcpy(remainder, dividend + n - m + 1);
}

void encodeData(char data[], char divisor[], char encoded[])
{
    int d = strlen(data);
    int m = strlen(divisor);

    char dividend[200];
    char remainder[100];

    strcpy(dividend, data);

    for (int i = 0; i < m - 1; i++)
    {
        dividend[d + i] = '0';
    }
    dividend[d + m - 1] = '\0';

    mod2Div(dividend, divisor, remainder);
    printf("CRC Remainder    : %s\n", remainder);

    strcpy(encoded, data);
    strcat(encoded, remainder);
}


void reciever(char recieved[], char divisor[])
{
    char remainder[100];
    mod2Div(recieved, divisor, remainder);
    printf("Recieved remainder : %s\n", remainder);

    if (strspn(remainder, "0") == strlen(remainder))
    {
        printf("No error detected\n");
    }
    else
    {
        printf("Error detected \n");
    }
}

int main()
{
    char data[100], divisor[50], encoded[200], received[200];

    printf("Enter dataword (binary) : ");
    scanf("%99s", data);

    printf("Enter divisor (binary)  : ");
    scanf("%49s", divisor);


    if(strlen(divisor)<2 || divisor[0]!='1' || strlen(data) + strlen(divisor)>199){
        printf("Invalid Input\n");
        return 1;
    }

    encodeData(data,divisor,encoded);
    printf("\nSender Side\n");
    printf("Transmitted Codeword: %s\n",encoded);

    printf("\nReciever Side \n");
    strcpy(received,encoded);

    // reciever(received, divisor);

    printf("\nError Simulator\n");
    strcpy(received,encoded);

    received[0] = received[0]=='0'?'1':'0';
    printf("Corrupted Data : %s \n",received);
    reciever(received,divisor);

    return 0;
}