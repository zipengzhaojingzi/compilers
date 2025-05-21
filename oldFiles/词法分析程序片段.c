#include <stdio.h>
#include <string.h>
 
typedef struct Books
{
   char  title[50];
   char  author[50];
   int   book_id;
   int   price;
} Book;
 
int main( )
{
   Book book;
 
   strcpy(book.title, "Compilers: Principles, Techniques, and Tools");
   strcpy(book.author, "Alfred V. Aho et al."); 
   book.book_id = 0x12;   //十六进制
   book.price = 034;      //八进制
 
   printf("Price of book %s is: %d\n", book.title, book.price);
}
