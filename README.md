# 实验一 可变分区存储管理

## 一、实验题目

编写一个C语言程序，模拟UNIX的可变分区内存管理，使用循环首次适应法实现对一块内存区域的分配和释放管理。

## 二、算法思想及概要设计

主要思想及设计如下：

使用malloc函数申请一块大内存空间，使用一个双向链表储存各个空闲区的地址及长度信息，读取到用户输入指令后，调用内存分配或释放函数对双向链表进行操作，模拟内存分配或释放的过程，之后调用输出函数遍历链表，将现存的空闲区域全部打印出来。

为了实现循环首次适应法，我使用了一个全局变量pointer，它是一个链表结点类型的指针，指向链表中上次经过分配的结点的子节点，即下次分配要使用的空闲区。

## 三、重要模块的功能、详细设计以及接口说明

* **链表结点构造函数**

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

* **打印空闲分区函数**

  ```c
  void display(){
      node * curr=pointer;
      printf("空闲分区长度及地址分别为：\n");
      do{
          printf("%u,%u\n",curr->size,curr->addr);
          curr=curr->next;
      }while(curr!=pointer);
      printf("------------------------------\n");
  }
  ```

  遍历链表，输出每个结点的地址和长度信息。

* **内存分配函数**

  ```c
  char *lmalloc(unsigned size){
      // return NULL;
      int flag = 0;
      node *curr = pointer;   //记录一下开始的结点
      node *temp;             //释放空间时储存信息
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
                  printf("内存已全部分配完毕！\n");
                  pointer->size=0;
                  flag=1;
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
              flag=1;
              display();
              
              return address;
          }
          // 这块够用，修改这一块的size和addr即可
          else{
              char *address=pointer->addr;
              pointer->size=pointer->size-size;
              pointer->addr=pointer->addr+size;
              pointer=pointer->next;
              flag=1;
              display();
              return address;
      }}while(pointer!=curr);
      // 没有足够大空间的情况
      if(flag==0)printf("没有足够空间处理您的请求！\n");
      return NULL;
}
  ```
  
  内存分配过程对应键盘输入的“m”指令，主要分为以下几种情况：
  
  * pointer指向的结点对应的空闲区不够大，则检查其子结点对应的空闲区。
  * pointer指向的结点对应的空闲区大小刚好等于所需内存大小，则直接将这一部分占用，删去对应的子结点。在这一步中进行内存空间是否全部占满的判断，具体方法是若这一块内存是最后一块空闲区，即对应的结点是链表中唯一的结点，则向用户输出内存已全部分配的信息。
  * pointer指向的结点对应的空闲区大小大于所需内存大小，则在这块空闲区内将相应大小的内存设置为占用，即修改对应结点的大小和起始地址即可。
  * 如果遍历链表之后没有找到合适的空闲区，代表整个内存区已经存不下这样大小的内容，则向用户输出空间不足的信息。
  
  特别需要注意的是，输入指令的正确性默认由上层应用负责，故本程序不提供任何的差错检测功能，遇到错误指令输入则立即终止进程。
  
* **内存释放函数**

  ```c
  void *lfree(unsigned size, char *addr){
      node *current = head;
      node *new;
      // 特别情况，全部填满后
      if(pointer->size==0){
          pointer->addr=addr;
          pointer->size=size;
          pointer->next=pointer;
          pointer->prior=pointer;
          display();
          return NULL;
      }
      // 一般情况，共分六种
      do{
          // 两种特殊情况：释放的内存地址在最前或最后
          if (addr<head->addr)
          {
              // 没有跟后边的挨着,插入新结点
              if (addr+size<head->addr){
                  new = InitList(new,addr,size);
                  new->next=head;
                  new->prior=head->prior;
                  head->prior->next=new;
                  head->prior=new;
                  head = new;
                  display();
                  break;
              }
              // 跟后边的挨着，直接加入
              if (addr+size==head->addr){
                  head->addr=addr;
                  head->size=head->size+size;
                  display();
                  break;
              }
          }
          // 四种一般情况：释放的内存地址在两个结点的地址之间
          // 释放的空间不挨着上下空闲区，直接加一个结点
          if((current->addr+current->size<addr)
              &&(current->next->addr>addr+size)){
              new=InitList(new,addr,size);
              new->next=current->next;
              new->prior=current;
              current->next->prior = new;
              current->next = new;
              display();
              break;
          }
          // 释放的空间挨着上空闲区，改一下size即可
          else if((current->addr+current->size==addr)&&
                (current->next->addr>addr+size)){
              current->size=current->size+size;
              display();
              break;
          }
          // 释放的空间挨着下空闲区，修改size和addr
          else if ((current->addr+current->size<addr)&&
                (current->next->addr==addr+size)){
              current->next->addr=addr;
              current->next->size+=size;
              display();
              break;
          }
          // 释放的空间挨着上下两块空闲区
          else if((current->addr+current->size==addr)&&
                (current->next->addr==addr+size)){
              // 特别注意，删除结点的过程可能删掉链表入口pointer，故需要提前判断一下
              if(pointer==current->next)pointer=current->next->next;
              current->size=current->size+size+current->next->size;
              current->next->next->prior=current;
              current->next=current->next->next;
              display();
              break;
          }
          
          else current=current->next;
      }while(current!=head);
  }
  ```

  内存释放过程对应键盘输入的“f”指令，主要分为以下七种情况：

  * 一种特别情况：即内存已经全部被占用，则根据输入的地址和大小重建链表
  * 六种常规情况：
    * 释放的内存地址在所有空闲区地址之前，共两种情况
    * 释放的内存地址在两个空闲地址之间或在所有空闲地址之后，共四种情况

  本函数同样不提供纠错功能，遇错则停止运行。

* **程序入口**

  ```c
  int main(){
      char *firstloc;
      char input;
      char *address;
      char *addr;
      unsigned size;
      // 创建内存空间
      firstloc = malloc(MAXSIZE);
      // 初始化一下标志变量
      pointer = InitList(pointer,firstloc,MAXSIZE);
      pointer->next=pointer;
      pointer->prior=pointer;
      head=pointer;
      // head = pointer;
      // end = pointer;          
      printf("FirstLoc:%u\n",firstloc);
      // printf("Pointer:%s\n",pointer);
      // 开始读取用户输入
      printf("请输入指令（格式：“m 100”、“f 100,781998”）：\n");
      do
      {
          // display(chain);   
          scanf("%s",&input);
          // 分配内存
          if(input=='m'){
              scanf("%u",&size);
              address = lmalloc(size);
          }
          // 释放内存
          else if (input=='f'){
              scanf("%u,%u",&size,(unsigned)&addr);
              lfree(size,addr);
          }
          // 其他情况
          else break;
      } while (input=='m' || input=='f');
      
      free(firstloc);
      free(pointer);
      free(head);
      return 0;
  }	
  ```

  主函数主要负责初始化用到的内存空间和指向变量，读取用户输入并调用响应的处理函数。

## 四、重要数据结构及变量的说明

* **数据结构**

  最主要的数据结构是常见的双向链表，具体定义如下：

  ```c
  typedef struct map{
      unsigned size;
      char *addr;
      struct map *next, *prior;
  }node;
  ```

  双向链表用于存储内存区中空闲区域的地址和大小，并通过两个指针使各个空闲区之间可以互相调用。

* **重要变量说明**
  * 全局变量`node *pointer`：用于指向上次经过分配的空闲区的下一个空闲区，实现循环首次适应法。
  * 全局变量`node *head`：指向存储空闲区的链表中对应内存地址最小的结点，以进行内存释放过程中特殊情况的处理
  * `char *firstloc`：用于记录申请到的内存的首地址
  * `unsigned size char *addr`：接收用户指令输入

## 五、测试方法、结果及错误分析

通过控制台输入指令进行测试：

初始情况：

* **循环首次适应的测试**

  * 初始情况：                                    

    <img src='C:\Users\Wsy\Desktop\算法测试1.png' style='float:left'>

  * 第一次分配：

    <img src='C:\Users\Wsy\Desktop\算法测试2.png' style='float:left'>

  * 第二次分配：

    <img src='C:\Users\Wsy\Desktop\算法测试3.png' style='float:left'>

  * 第三次分配

    <img src='C:\Users\Wsy\Desktop\算法测试4.png' style='float:left'>

  根据上面的测试结果可以看出，内存分配满足循环首次适应法。

* **分配过程测试**

  * 初始情况

    <img src='C:\Users\Wsy\Desktop\算法测试4.png' style='float:left'>

  * 当前一块大小刚好满足

    <img src='C:\Users\Wsy\Desktop\分配测试1.png' style='float:left'>

    ​	特殊情况（内存全部分配完）

    ​	<img src='C:\Users\Wsy\Desktop\分配测试4.png' style='float:left'>

    

    

    

    

  * 当前一块可以容纳

    <img src='C:\Users\Wsy\Desktop\分配测试2.png' style='float:left'>

  * 找不到合适大小的空闲区

    <img src='C:\Users\Wsy\Desktop\分配测试3.png' style='float:left'>

* **释放过程测试**

  * 在所有空闲区之前插入新空闲区

    <img src='C:\Users\Wsy\Desktop\释放测试1.png' style='float:left'>

  * 在两个空闲区之间插入（四种情况）

    * 上下均不连接

      <img src='C:\Users\Wsy\Desktop\释放测试2.png' style='float:left'>

    * 上连下不连

      <img src='C:\Users\Wsy\Desktop\释放测试3.png' style='float:left'>

    * 下连上不连

      <img src='C:\Users\Wsy\Desktop\释放测试4.png' style='float:left'>

    * 上下均连接

      <img src='C:\Users\Wsy\Desktop\释放测试5.png' style='float:left'>

* **错误指令**

  由于本程序在实际应用场景中面向的并不是普通用户而是上层系统，故输入的正确性应由上层应用保证，因此本程序针对错误只进行简单的跳出，而没有进行错误类型的分类和打印。

## 六、改进点和个人感想

本次作业的过程可以说是一波三折，完成的也比较仓促，留下了一些有待完善的地方，比如malloc申请的内存空间的释放等，并没有进行一个很好的匹配。

这次实验中我的收获还是很大的，一方面复习了c语言中指针和链表的使用，另一方面也练习了代码的编写，一定程度上提高了代码能力。最后感谢刘老师的悉心讲解和助教的帮助。

