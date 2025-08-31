// 
/* Распознающий автомат (конечный автомат)
*/
#include <stdio.h>
#include <stdlib.h>



// Узел АВЛ-дерева
struct node 
{ 
    int value;
    int Count; 
    struct node *left, *right; 
}; 

/* Вспомогательная функция, которая выделяет 
новый узел с заданным key и left и right, указывающими в NULL. */
struct node* newNode(int value) 
{ 
    struct node* Node = malloc(sizeof(struct node));// new node();
    Node->value = value; 
    Node->Count = 1; 
    Node->left = Node->right = NULL; 
    return (Node); 
} 

// Служебная функция для разворота поддерева с корнем y вправо.
// Смотрите диаграмму, приведенную выше.
struct node *rightRotate(struct node *x) 
{ 
  struct node *y = x->left; 
    x->left = y->right; 
    y->right = x; 
    return y; 
} 

// Служебная функция для разворота поддерева с корнем x влево 
// Смотрите диаграмму, приведенную выше. 
struct node *leftRotate(struct node *x) 
{ 
    struct node *y = x->right; 
    x->right = y->left; 
    y->left = x; 
    return y; 
} 

// Эта функция поднимет ключ
// в корень, если он присутствует в дереве. 
// Если такой ключ отсутствует в дереве, она
// поднимет в корень самый последний элемент,
// к которому был осуществлен доступ.
// Эта функция изменяет дерево
// и возвращает новый корень (root).
struct node *splay(struct node *root, int value) 
{ 
    // Базовые случаи: root равен NULL или
    // ключ находится в корне
    if (root == NULL || root->value == value) 
        return root; 

    // Ключ лежит в левом поддереве
    if (root->value > value) 
    { 
        // Ключа нет в дереве, мы закончили
        if (root->left == NULL) return root; 

        // Zig-Zig (Левый-левый)
        if (root->left->value > value) 
        { 
            // Сначала рекурсивно поднимем
             // ключ в качестве корня left-left
            root->left->left = splay(root->left->left, value); 

            // Первый разворот для root, 
             // второй разворот выполняется после else 
            root = rightRotate(root); 
        } 
        else if (root->left->value < value) // Zig-Zag (Left Right) 
        { 
            // Сначала рекурсивно поднимаем
             // ключ в качестве кореня left-right
            root->left->right = splay(root->left->right, value); 

            // Выполняем первый разворот для root->left
            if (root->left->right != NULL) 
                root->left = leftRotate(root->left); 
        } 

        // Выполняем второй разворот для корня
        return (root->left == NULL)? root: rightRotate(root); 
    } 
    else // Ключ находится в правом поддереве
    { 
        // Ключа нет в дереве, мы закончили
        if (root->right == NULL) return root; 

        // Zag-Zig (Правый-левый)
        if (root->right->value > value) 
        { 
            // Поднять ключ в качестве кореня right-left
            root->right->left = splay(root->right->left, value); 

            // Выполняем первый поворот для root->right
            if (root->right->left != NULL) 
                root->right = rightRotate(root->right); 
        } 
        else if (root->right->value < value)// Zag-Zag (Правый-правый) 
        { 
            // Поднимаем ключ в качестве корня 
             // right-right и выполняем первый разворот
            root->right->right = splay(root->right->right, value); 
            root = leftRotate(root); 
        } 

        // Выполняем второй разворот для root
        return (root->right == NULL)? root: leftRotate(root); 
    } 
} 

// Функция для вставки нового ключа k в splay-дерево с заданным корнем
struct node *add_node(struct node *root, int k) 
{ 
    // Простой случай: если дерево пусто
    if (root == NULL) return newNode(k); 

    // Делаем ближайший узел-лист корнем 
    root = splay(root, k); 

    // Если ключ уже существует, то возвращаем его
    if (root->value == k) 
    {
      root->Count++;
      return root;
    }

    // В противном случае выделяем память под новый узел
    struct node *newnode = newNode(k); 

    // Если корневой ключ больше, делаем корень правым дочерним элементом нового узла, копируем левый дочерний элемент корня в качестве левого дочернего элемента нового узла
    if (root->value > k) 
    { 
        newnode->right = root; 
        newnode->left = root->left; 
        root->left = NULL; 
    } 

    // Если корневой ключ меньше, делаем корень левым дочерним элементом нового узла, копируем правый дочерний элемент корня в качестве правого дочернего элемента нового узла
    else
    { 
        newnode->left = root; 
        newnode->right = root->right; 
        root->right = NULL; 
    } 

    return newnode; // новый узел становится новым корнем
} 

// Служебная функция для вывода 
// обхода в дерева ширину. 
// Функция также выводит высоту каждого узла
void preOrder(struct node *root) 
{ 
    if (root != NULL) 
    { 
        printf("%d",root->value);
        //cout<<root->key<<" "; 
        preOrder(root->left); 
        preOrder(root->right); 
    } 
} 


struct node* DigitCount(char* FileName) // Распознающий автомат
{ FILE *f;
  int flag=1; // Флаг выхода из цикла распознания чисел
  char c;
  int s=0; // Номер текущего состояния
  char buf[100]; // Буфер для хранения символов числа
  int i=0; // Счетчик символов числа
  f=fopen(FileName,"r");
  struct node *root=NULL; // Корень дерева
//  struct node *head = malloc(sizeof(struct node));
//  struct node *tail = head;

  while(flag)//
    {
      c=fgetc(f);
      if(c == EOF)
      {
        c='\n';
        flag=0; // Устанавливаем флаг выхода из цикла
      }

      switch(s)
      {
        case 0: //Значение состояния, при котором распозна
//          printf("\nstage 0\n");
          if(c=='0') {s=2; buf[i++]='0'; break;}
          if(c=='-') {s=6; buf[i++]='-'; break;}
          if(c=='\"') {s=5; break;}
          if(c>='1' && c<='9') {s=1; buf[i++]=c; break;}
          if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';' || c=='[' || c==']') {s=0; break;} 
          s=4;
          break;
      case 1:
//        printf("\nstage 1\n");
        if(c=='.') {s=10; break;}
        if(c=='u' || c=='U') {s=7; break;}
        if(c=='l' || c=='L') {s=8; break;}
        if(c>='0' && c<='9') {s=1; buf[i++]=c; break;}
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']') 
        {
         // tail->next=malloc(sizeof(struct node));
         // tail=tail->next;
          buf[i]=0; // Добавить нуль-символ в конец строки
          root=add_node(root, atoi(buf)); //atoi - текст в int
        //  tail->value=atoi(buf);
          s=0;
          i=0;
          break;
        } 
        //printf("Digit error!\n");
        flag=0; // Устанавливаем флаг выхода из цикла
        break;
      case 2:
//        printf("\nstage 2\n");
        if(c>='1' && c<='9') {s=1; break;}
        if(c=='b'|| c=='B' ||c=='o'|| c=='O' ||c=='h'|| c=='H') {s=3; break;} // Проверка на двоичное, восьмеричное и шестнадцатеричное число
        //printf("Digit error\n");
       // flag=0; // Устанавливаем флаг выхода из цикла
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']') 
        root=add_node(root, 0);
        i=0; //Обнулить буфер
        s=0;
        break;
      case 10:
        if(c!='.')
          {s=3; i=0; break;}
        else
        {
          buf[i]=0; // Добавить нуль-символ в конец строки
          root=add_node(root, atoi(buf)); //atoi - текст в int
          s=0; i=0;
        }
        break;
      case 3: 
//        printf("\nstage 3\n");
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']') {s=0; i=0; break;} 
        s=3;
        break;
      case 4:
//        printf("\nstage 4\n");
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']') {s=0; break;} 
        s=4;
        break;
      case 5:
  //      printf("\nstage 5\n");
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']') {s=0; break;} 
        s=5;
        break;
      case 6:
          if(c>='1' && c<='9') {s=1; buf[i++]=c; break;}
          i=0;
          s=0;
        break;
      case 7:
        if(c=='l' || c=='L') {s=8; break;}
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']')
          {
          buf[i]=0; // Добавить нуль-символ в конец строки
          root=add_node(root, atoi(buf)); //atoi - текст в int
          s=0; i=0;
          break;
          }
        s=0;
        break;
      case 8:
        if(c=='l' || c=='L') {s=9; break;}
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']')
          {
          buf[i]=0; // Добавить нуль-символ в конец строки
          root=add_node(root, atoi(buf)); //atoi - текст в int
          s=0; i=0;
          break;
          }
        s=0;
        break;
      case 9:
        if(c==' '|| c=='.' || c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='\n' || c=='|'|| c=='&' || c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='!'|| c=='_' || c==';'|| c=='[' || c==']')
          {
          buf[i]=0; // Добавить нуль-символ в конец строки
          root=add_node(root, atoi(buf)); //atoi - текст в int
          s=0; i=0;
          break;
          }
        s=0;
        break;
    }
  }

return root;
//return head;
}

int Recurs(struct node *root) // Рекурсивный обход дерева
{
 if(root==NULL) return 0;
  printf("%d: %d\n", root->value, root->Count);
return (Recurs(root->left) + Recurs(root->right)+ root->Count); 
}

int main()
{
  struct node*headdigit=DigitCount("test.txt");
  int count=0;
  printf("----\n");
  count=Recurs(headdigit);
/*  for(struct node*p=headdigit->next;p!=NULL;p=p->next)
    {
      count++;
      printf("%d\n",p->value);
   }
*/ 
  printf("Count=%d\n",count);
  return 0;
}
