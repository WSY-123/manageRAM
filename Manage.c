// 操作系统实验——内存分区管理
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <malloc.h>

// 使用双向链表
typedef struct map{
    unsigned size;
    char *addr;
    struct map *next, *prior;
}node;
node *pointer;   //指向变量
node *head;      //指向这个链表addr最小的结点
// node *end;       //指向这个链表addr最大的结点
# define MAXSIZE 1024

// 构建链表结点
node *InitList(node *first,char *addr,int size){
    first = (node*)malloc(sizeof(node));
    
    first->size = size;
    first->addr = addr;
    // first->next = next;
    // first->prior = prior;
    
    return first;
}

// 打印现有空闲分区（遍历链表）
void display(){
    node * curr=pointer;
    printf("空闲分区长度及地址分别为：\n");
    do{
        printf("%u,%u\n",curr->size,curr->addr);
        curr=curr->next;
    }while(curr!=pointer);
    printf("------------------------------\n");
}

// 内存分配
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

// 释放函数
// 默认上层应用传递的size和addr是正确的，不提供纠错功能
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

// 程序入口
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