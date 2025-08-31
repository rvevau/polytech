#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
//Подходящий тип данных - двусзязный динамический список
struct numb{
  unsigned long long num;
  struct numb *next;
  struct numb *prev;
};

int main(void) {
  struct numb *head = (struct numb*)malloc(sizeof(struct numb));//Создали первый мнимый элемент
  head->prev=NULL;
  head->num=1;
  head->next=(struct numb*)malloc(sizeof(struct numb));
  head->next->num=1;
  head->next->prev=head;
  head->next->next=NULL;
  int N;
  scanf("%d", &N);
  unsigned long long Ans=0;
  while(N>=0)
    {
      struct numb *MinUk=head->next;//Минимальный элемент
      for(struct numb *Uk=head->next; Uk!=NULL;Uk=Uk->next)//Проходимся по всем элементам
        if(Uk->num<MinUk->num) MinUk=Uk; //Если текущий элемент меньше минимального, то минимальный элемент становится равен текущему
    Ans=MinUk->num;//
    unsigned long long d3=Ans*3;
    unsigned long long d5=Ans*5;
    unsigned long long d7=Ans*7;
    for(struct numb *Uk=head->next; Uk!=NULL;Uk=Uk->next) // Проверка элемента на кратность 3 5 7, если да, то удаляем его
      {
        if(Uk->num==d3)//Если элемент кратен 3, то удаляем его
          d3=0; //Присваиваем 0, чтобы не удалять его повторно
        else if(Uk->num==d5)//Если элемент кратен 5, то удаляем его
          d5=0; //Присваиваем 0, чтобы не удалять его повторно
        else if(Uk->num==d7)//Если элемент кратен 7, то удаляем его
          d7=0;//Присваиваем 0, чтобы не удалять его повторно
      }
    if(d3!=0)//Если элемент не кратен 3, то добавляем его в список
    {
      head->next->prev=(struct numb*)malloc(sizeof(struct numb));//Создаем новый элемент
      head->next->prev->next=head->next;
      head->next->prev->prev=head;
      head->next=head->next->prev;
      head->next->num=d3;
    } 
    //printf("test\n");
    if(d5!=0)//Если элемент не кратен 5, то добавляем его в список
    {
      head->next->prev=(struct numb*)malloc(sizeof(struct numb));//Создаем новый элемент
      head->next->prev->next=head->next;
      head->next->prev->prev=head;
      head->next=head->next->prev;
      head->next->num=d5;
    } 
    if(d7!=0)//Если элемент не кратен 7, то добавляем его в список
    {
      head->next->prev=(struct numb*)malloc(sizeof(struct numb));//Создаем новый элемент
      head->next->prev->next=head->next;
      head->next->prev->prev=head;
      head->next=head->next->prev;
      head->next->num=d7;
    }
    MinUk->prev->next=MinUk->next; // Перебрасываем элемент через 1 вперёд
    if(MinUk->next!=NULL) MinUk->next->prev=MinUk->prev; // Перебрасываем элемент через 1 назад
    free(MinUk);  
N--;
}
//printf("test\n"); 
printf("%llu\n",Ans);
return 0;
}
