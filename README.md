# 实验一 可变分区存储管理

## 一、实验题目

编写一个C语言程序，模拟UNIX的可变分区内存管理，使用循环首次适应法实现对一块内存区域的分配和释放管理。

## 二、算法思想及概要设计

主要思想及设计如下：

使用malloc函数申请一块大内存空间，使用一个双向链表储存各个空闲区的地址及长度信息，读取到用户输入指令后，调用内存分配或释放函数对双向链表进行操作，模拟内存分配或释放的过程，之后调用输出函数遍历链表，将现存的空闲区域全部打印出来。

## 三、重要模块的功能、详细设计以及接口说明

* 链表结点构造函数

  ```c
  node *InitList(node *first,char *addr,int size){
      first = (node*)malloc(sizeof(node));
      
      first->size = size;
      first->addr = addr;
      // first->next = next;
      // first->prior = prior;
      
      return first;
  }
  ```

  一个相对简单的列表结点构造函数，利用malloc函数申请空间存储结点。为了适应初始化列表第一个结点的情况，next和prior元素需要在函数外手动赋值。

  同时需要注意，修改形参first的值并不能修改实参的值，故需要将配置好的结点以返回值的形式输出。

* 打印空闲分区函数

  ```c
  void display(){
      node * curr=pointer;
      printf("空闲分区地址及长度分别为：\n");
      do{
          printf("%d,%u\n",curr->addr,curr->size);
          curr=curr->next;
      }while(curr!=pointer);
  }
  ```

  遍历链表，输出每个结点的地址和长度信息。

* 内存分配函数

  ```c
  char *lmalloc(unsigned size){
      // return NULL;
      node *curr = pointer;   //记录一下开始的结点
      node *temp;             //释放空间时储存信息
      if(size>MAXSIZE){
          printf("内存过大无法分配！\n");
          return NULL;
      }
      do{
          // 这块不够大，换下一块
          if( pointer->size < size){
              pointer=pointer->next;
          }
          // 这块刚好，需要把对应的链表结点删除
          else if(pointer->size == size){
              //如果只有一个节点，则本次分配完成后空间已满
              if(pointer->next==pointer&&pointer->prior==pointer)
              {
                  printf("内存已全部分配完毕！");
                  return NULL;
              }
              
              char* address=pointer->addr;
              pointer->prior->next=pointer->next;
              pointer->next->prior=pointer->prior;
  
              // 释放内存
              temp = pointer;
              free(pointer);
              pointer = temp->next;
              free(temp);
  
              display();
              
              return address;
          }
          // 这块够用，修改这一块的size和addr即可
          else{
              char *address=pointer->addr;
              pointer->size=pointer->size-size;
              pointer->addr=pointer->addr+size;
              pointer=pointer->next;
              display();
              return address;
      }}while(pointer!=curr);
      return NULL;
  }
  ```

  

