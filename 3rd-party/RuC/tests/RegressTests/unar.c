   int main()
{
   int a=1,b=0, c;
   c = ~a;
   print ("c  0");
   printid(0);

   �= ~b;
   print ("c  1");
   printid(1);

   �= a&b;
   print ("c  0");
   printid(0);

   �= a&a;
   print ("c  1");
   printid(1);

   �= b&b;
   print ("c  0");
   printid(0);

   �=(b!=a);
   print ("c  1");
   printid(1);

   �= a!=a;
   print ("c  0");
   printid(0);

   �=(a==a);
   print ("c  1");
   printid(1);

   c = b==a;
   print ("c  0");
   printid(0);

   return 0;
}