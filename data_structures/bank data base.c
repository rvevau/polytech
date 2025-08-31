#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

enum Month {
  January=1, 
  Febrary=2,
  March=3,
  April=4,
  May=5,
  June=6,
  July=7,
  August=8,  
  September=9,
  October=10,
  November=11,
  December=12
};

struct date{
  int Year;
  int Day;
  enum Month month;
};

union category{
  char symb;
  short int digit;

};

struct client{
  char name[256];
  struct date born;
  double discont;
  char gender;
  char city[256];
  unsigned int id;
  union category Category;
  struct client * prev;
  struct client * next;
};

void rec_enter(struct client * new_client)//Ввод данных с терминала
{
  printf("Введите имя клиента: \n");
  scanf("%s", new_client->name);
  printf("Введите идентификатор клиента: \n");
  scanf("%d", &new_client->id);
  printf("Введите дату рождения ДД ММ ГГ\n: ");
  scanf("%d %d %d", &new_client->born.Day, &new_client->born.month, &new_client->born.Year);
  /*printf("Введите день рождения\n: ");
  scanf("%d", &new_client->born.Day);
  printf("Введите месяц рождения: \n");
  scanf("%d", &new_client->born.month);
  printf("Введите год рождения: \n");
  scanf("%d", &new_client->born.Year);*/
  printf("Введите размер дисконта: \n");
  scanf("%lf", &new_client->discont);
  printf("Введите пол: \n");
  while ( getchar() != '\n' );
  scanf("%c", &new_client->gender);
  while ( getchar() != '\n' );
  printf("Введите город: \n");
  scanf("%s", new_client->city);
  printf("Введите категорию клиента: \n");
  while ( getchar() != '\n' );
  scanf("%c", &new_client->Category.symb);
  while ( getchar() != '\n' );
}
struct client * new_rec() //Ввод данных
{
  struct client * new_client = (struct client *) malloc(sizeof(struct client));
  new_client->next = NULL;
  rec_enter(new_client);
  return new_client;
}

void add(struct client * head){
  struct client * new_client = new_rec();
  new_client->next=NULL;
  struct client* uk=head;
  for(;uk->next!=NULL;uk=uk->next);
  printf("Конец добавления нового элемента\n\n");
  new_client->prev=uk;
  uk->next=new_client;
}

void print(struct client* new_client) // Вывод данных о клиенте в терминал
{
   printf("Имя клиента: %s\n",new_client->name);
   printf("Идентификатор клиента: %d\n", new_client->id);
   printf("Дата рождения: %d %d %d\n", new_client->born.Day, new_client->born.month, new_client->born.Year);
   /*printf("День рождения: %d\n", new_client->born.Day);
   printf("Месяц рождения: %d\n", new_client->born.month);
   printf("Год рождения: %d\n", new_client->born.Year);*/
   printf("Размер дисконта: %lf\n", new_client->discont);
   printf("Пол: %c\n", new_client->gender);
   printf("Город: %s\n",new_client->city);
   printf("Категория клиента: %c\n", new_client->Category.symb);
   printf("\n");
}

void Remove(struct client* uk) //Удаление клинта из базы данных
{
  if(uk==NULL) return; //Если указатель на клиента равен NULL, то выходим из функции
  uk->prev->next=uk->next; // С предыдущего элемента перебросить связь на следующий
  if(uk->next!=NULL) uk->next->prev=uk->prev; //Со следующего элемента перебросить связь на предыдущий
  free(uk);
}

void insert(struct client* uk) //Вставка клиента в базу данных
{
  struct client* new_client = new_rec();
  new_client->prev=uk; // ссылка на предыдущий элемент для нового элемента
  new_client->next=uk->next; // ссылка на следующей элемент для нового элемента
  if(uk->next!=NULL) uk->next->prev=new_client; // ссылка на предыдущий элемент для следующего элемента
  uk->next=new_client; // Указатель на новый элемент для элемента, после которого вставляем новый элемент
}

struct client* find(struct client* head, int id) //Поиск клиента по идентификатору
{
  struct client* uk=head;
  for(;uk!=NULL;uk=uk->next)
    if(uk->id==id)
      break;
  return uk;
}

int main(void) {
  setlocale(LC_ALL, "Russian");
  struct client * head = (struct client*) malloc(sizeof(struct client));//Выделение памяти для первого мнивого элемента
  head->next=NULL;
  struct client * CurrentRec=NULL; // Указатель на текущий элемент
  int m, Out=0;
  
  while(!Out)
    {
      printf("\nВыберите действие:\n");
      printf("1. Чтение базы данных из файла\n");
      printf("2. Добавить клиента\n");
      printf("3. Редактировать данные клиента\n");
      printf("4. Удалить клиента\n");
      printf("5. Поиск по базе данных\n");
      printf("6. Вывод базы данных\n");
      printf("7. Запись базы данных в файл\n");
      printf("8. Выход\n");
      printf("-> ");
      scanf("%d",&m);
      switch(m)
        {
        case 1: // Чтение базы данных из файла
          {
            FILE *FIn;
              FIn = fopen("base.txt", "r");
            // Очистить старую базу            
            for(struct client* uk=head->next;uk!=NULL;)
              {
                struct client* tmp=uk;  
                uk=uk->next;
                free(tmp);
              }
            head->next=NULL;
            while(!feof(FIn))
              {
                char str[10];
                struct client * new_client = (struct client *) malloc(sizeof(struct client));//Выделение памяти для нового элемента
                fscanf(FIn, "%s", new_client->name);// Чтение имени
                fscanf(FIn, "%d", &new_client->id);// Чтение идентификатора
                fscanf(FIn, "%d", &new_client->born.Day);
                int x;
               // fscanf(FIn, "%d", &x);
               // new_client->born.month=x;
                fscanf(FIn, "%d", &new_client->born.month);
                fscanf(FIn, "%d", &new_client->born.Year);
                fscanf(FIn, "%lf", &new_client->discont);
                fscanf(FIn, "%s", str);
                new_client->gender=str[0];
                fscanf(FIn, "%s", new_client->city);
                fscanf(FIn, "%s", str);
                new_client->Category.symb=str[0];
                
                new_client->next=NULL;
                struct client* uk=head;
                for(;uk->next!=NULL;uk=uk->next);
                new_client->prev=uk;
                uk->next=new_client;
              }
            fclose(FIn);
          }
          break;
        case 2: // Добавить клиента
          add(head);
          printf("Конец ввода даных \n");
          print(head->next);
          break;
          case 3: // Редактирование данных клиента
            if(CurrentRec==NULL)
            {
              printf("Не выбрана текущая запись \n\n");
              break;
            }
            rec_enter(CurrentRec);
            break;
          case 4: // Удалить клиента
          {
          printf("Введите идентификатор клиента, которого хотите удалить: ");
          int id;
          scanf("%d", &id);
          struct client* uk=find(head, id);
          if(uk==NULL){
            printf("Идентификатор не найден в базе данных\n");
            break;
          }
          Remove(uk);
          printf("Запись удалена из базы данных\n\n");
          break;
          }
        case 5: // Поиск по базе данных
          {
            printf("Введите идентификатор клиента: ");
            int id;
            scanf("%d", &id);
            struct client* uk=find(head, id);
            if(uk==NULL){
              printf("Идентификатор не найден в базе данных\n");
              break;
            }
            print(uk);
            CurrentRec=uk;// Запомнить адрес найденной записи
            break;
          }
        case 6: // Вывод базы данных
          {
            struct client* uk=head->next;
            for(;uk!=NULL;uk=uk->next)
              {
                print(uk);
                printf("-------------------\n");
              }
            break;
          }
          case 7: // Запись базы данных в файл
            {
              FILE *FOut;
              FOut = fopen("base.txt", "w");
              for(struct client* uk=head->next;uk!=NULL;uk=uk->next)
                {
                  fprintf(FOut, "%s\n", uk->name);
                  fprintf(FOut, "%d\n", uk->id);
                  fprintf(FOut, "%d\n",uk->born.Day);
                  fprintf(FOut, "%d\n", uk->born.month);
                  fprintf(FOut, "%d\n", uk->born.Year);
                  fprintf(FOut, "%lf\n",uk->discont);
                  fprintf(FOut, "%c\n", uk->gender);
                  fprintf(FOut, "%s\n", uk->city);
                  fprintf(FOut, "%c\n", uk->Category.symb);
                }
              fclose(FOut);
            }
            break;
        case 8: // Выход
          printf("Выход из программы...\n");
          Out=1;
          break;

        default:
          printf("Неверный ввод комманды!\n");
          break;
        }
    }
  return 0;
}
