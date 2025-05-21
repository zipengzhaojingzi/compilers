#ifndef _SYNANALYSIS_H  
#define _SYNANALYSIS_H  
  
#define GRAMMAR_ARROW 2000 //->  
#define GRAMMAR_OR 2001 // |  
#define GRAMMAR_NULL 2002 //��ֵ  
#define GRAMMAR_SPECIAL 2003 //�������#  
#define GRAMMAR_BASE 2010 //��̬���ɵĻ�ֵ  
  
#define Stack_Size 5000  
  
typedef struct  
{  
    int elem[Stack_Size];  
    int top;  
} SeqStack;  
  
void initGrammer();  
int seekCodeNum(char * word);  
void ceshi();  
void First();  
void Follow();  
void Select();  
void MTable();  
void Analysis();  
#endif  
