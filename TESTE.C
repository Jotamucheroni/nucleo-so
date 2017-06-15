#include <stdio.h>
#include <system.h>

PTR_DESC dtic, dtac, dmain;

void far tic()
{
     while(1)
     {
        printf("tic-");
        transfer(dtic, dtac);
     }
}

void far tac()
{
     int i = 0;
     while(i < 50)
     {
         printf("tac");
         transfer(dtac, dtic);
         i++;
     }
     transfer(dtac, dmain);
}

void main()
{
     dtic = cria_desc();
     dtac = cria_desc();
     newprocess(tic, dtic);
     newprocess(tac, dtac);
     dmain = cria_desc();
     transfer(dmain, dtic);
}
