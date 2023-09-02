#include "os.h"
// #include "stdlib.h"
struct Node *creatr_list()
{
        struct Node *head = (struct Node *)malloc(sizeof(struct Node));
        head->next = NULL;
        head->data = NULL;
        head->pre = NULL;
        return head;
}
void insert_node(struct Node **head, void *data)
{
        struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->next = NULL;
        newNode->data = data;
        newNode->pre = NULL;

        if ((*head)->next == NULL)
        {
                (*head)->next = newNode;
                newNode->pre = *head;
                return;
        }
        struct Node *current = *head;
        struct timer *temp = (struct timer *)data;
        uint32_t time = temp->timeout_tick;
        while (current->next != NULL)
        {
                // 获取当前节点的下一个节点的数据
                temp = (struct timer *)(current->next->data);
                if (time > temp->timeout_tick)
                {
                        break;
                }
                current = current->next;
        }
        // 在当前节点和下一个节点之间插入新节点
        newNode->pre = current;
        newNode->next = current->next;
        if (current->next != NULL)
        {
                current->next->pre = newNode;
        }
        current->next = newNode;
}

void delete_node(struct Node **head, void *data)
{
        if ((*head)->next == NULL)
                return;
        struct Node *current = (*head)->next;
        while (current != NULL && current->data != data)
        {
                break;
        }
        current->pre->next = current->next;
        if (current->next != NULL)
        {
                current->next->pre = current->pre;
        }
        free(current);
}

void printList(struct Node *head)
{
        struct Node *current = head->next;
        struct timer *temp;
        while (current != NULL)
        {
                temp = (struct timer *)(current->data);
                printf("%d ", temp->timeout_tick);
                current = current->next;
        }
        printf("\n");
}