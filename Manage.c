// ����ϵͳʵ�顪���ڴ��������
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <malloc.h>

// ʹ��˫������
typedef struct map{
    unsigned size;
    char *addr;
    struct map *next, *prior;
}node;
node *pointer;   //ָ�����
node *head;      //ָ���������addr��С�Ľ��
// node *end;       //ָ���������addr���Ľ��
# define MAXSIZE 1024

// ����������
node *InitList(node *first,char *addr,int size){
    first = (node*)malloc(sizeof(node));
    
    first->size = size;
    first->addr = addr;
    // first->next = next;
    // first->prior = prior;
    
    return first;
}

// ��ӡ���п��з�������������
void display(){
    node * curr=pointer;
    printf("���з������ȼ���ַ�ֱ�Ϊ��\n");
    do{
        printf("%u,%u\n",curr->size,curr->addr);
        curr=curr->next;
    }while(curr!=pointer);
    printf("------------------------------\n");
}

// �ڴ����
char *lmalloc(unsigned size){
    // return NULL;
    int flag = 0;
    node *curr = pointer;   //��¼һ�¿�ʼ�Ľ��
    node *temp;             //�ͷſռ�ʱ������Ϣ
    do{
        // ��鲻���󣬻���һ��
        if( pointer->size < size){
            pointer=pointer->next;
        }
        // ���պã���Ҫ�Ѷ�Ӧ��������ɾ��
        else if(pointer->size == size){
            //���ֻ��һ���ڵ㣬�򱾴η�����ɺ�ռ�����
            if(pointer->next==pointer&&pointer->prior==pointer)
            {
                printf("�ڴ���ȫ��������ϣ�\n");
                pointer->size=0;
                flag=1;
                return NULL;
            }
            
            char* address=pointer->addr;
            pointer->prior->next=pointer->next;
            pointer->next->prior=pointer->prior;

            // �ͷ��ڴ�
            temp = pointer;
            free(pointer);
            pointer = temp->next;
            free(temp);
            flag=1;
            display();
            
            return address;
        }
        // ��鹻�ã��޸���һ���size��addr����
        else{
            char *address=pointer->addr;
            pointer->size=pointer->size-size;
            pointer->addr=pointer->addr+size;
            pointer=pointer->next;
            flag=1;
            display();
            return address;
    }}while(pointer!=curr);
    // û���㹻��ռ�����
    if(flag==0)printf("û���㹻�ռ䴦����������\n");
    return NULL;
}

// �ͷź���
// Ĭ���ϲ�Ӧ�ô��ݵ�size��addr����ȷ�ģ����ṩ������
void *lfree(unsigned size, char *addr){
    node *current = head;
    node *new;
    // �ر������ȫ��������
    if(pointer->size==0){
        pointer->addr=addr;
        pointer->size=size;
        pointer->next=pointer;
        pointer->prior=pointer;
        display();
        return NULL;
    }
    // һ���������������
    do{
        // ��������������ͷŵ��ڴ��ַ����ǰ�����
        if (addr<head->addr)
        {
            // û�и���ߵİ���,�����½��
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
            // ����ߵİ��ţ�ֱ�Ӽ���
            if (addr+size==head->addr){
                head->addr=addr;
                head->size=head->size+size;
                display();
                break;
            }
        }
        // ����һ��������ͷŵ��ڴ��ַ���������ĵ�ַ֮��
        // �ͷŵĿռ䲻�������¿�������ֱ�Ӽ�һ�����
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
        // �ͷŵĿռ䰤���Ͽ���������һ��size����
        else if((current->addr+current->size==addr)&&
              (current->next->addr>addr+size)){
            current->size=current->size+size;
            display();
            break;
        }
        // �ͷŵĿռ䰤���¿��������޸�size��addr
        else if ((current->addr+current->size<addr)&&
              (current->next->addr==addr+size)){
            current->next->addr=addr;
            current->next->size+=size;
            display();
            break;
        }
        // �ͷŵĿռ䰤���������������
        else if((current->addr+current->size==addr)&&
              (current->next->addr==addr+size)){
            // �ر�ע�⣬ɾ�����Ĺ��̿���ɾ���������pointer������Ҫ��ǰ�ж�һ��
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

// �������
int main(){
    char *firstloc;
    char input;
    char *address;
    char *addr;
    unsigned size;
    // �����ڴ�ռ�
    firstloc = malloc(MAXSIZE);
    // ��ʼ��һ�±�־����
    pointer = InitList(pointer,firstloc,MAXSIZE);
    pointer->next=pointer;
    pointer->prior=pointer;
    head=pointer;
    // head = pointer;
    // end = pointer;          
    printf("FirstLoc:%u\n",firstloc);
    // printf("Pointer:%s\n",pointer);
    // ��ʼ��ȡ�û�����
    printf("������ָ���ʽ����m 100������f 100,781998������\n");
    do
    {
        // display(chain);   
        scanf("%s",&input);
        // �����ڴ�
        if(input=='m'){
            scanf("%u",&size);
            address = lmalloc(size);
        }
        // �ͷ��ڴ�
        else if (input=='f'){
            scanf("%u,%u",&size,(unsigned)&addr);
            lfree(size,addr);
        }
        // �������
        else break;
    } while (input=='m' || input=='f');
    
    free(firstloc);
    free(pointer);
    free(head);
    return 0;
}