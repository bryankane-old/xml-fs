/* Lindsey DiAntonio. LGD@gwmail.guw.edu */

#include <stdio.h>

//ll is the head of the linked list

/* Allocates and returns a pointer to a linked list.
If unable to allocate the list, ll_create returns Null */


//allowed? or do I have to use create?
struct linked_list{
    void* data; //do I need this?
    int count;
    struct linked_list *next;
};

struct linked_list* ll_create(void){
       struct linked_list *head;
        //necessary to malloc here for a null node?
        head = malloc(sizeof(struct linked_list));
        head->data = NULL;
        head->next = NULL;
        head->count = 0;
        return head;
}

void ll_destroy(struct linked_list *ll){
    //if the list is already empty
    if(ll->next == NULL){
        printf("Destroy: This list is empty\n");
        free(ll);
    } else{
        //create a pointer to iterate through the list effectively
        //doesn't matter if it starts at true head while destroying
        //in fact you should destroy the null node anyways
        struct linked_list *head = ll->next;
        while(head != NULL){
            ll->next = head->next;
            free(head);
            head = ll->next;
        }
        //Then delete the last one
        free(head);
        free(ll);
        printf("Destroy: This linked list has been destroyed\n");
    }
}

//this adds a new node to the list of value *value
void ll_add(struct linked_list *ll, void *value){
    //save the head before adding in in "place 2" node to be your head of list
   // notSuccess = pthread_mutex_lock(&mutex);
     //               if(notSuccess) { printf("lock failed");}
    printf("in ll_add\n");
    struct linked_list *tofill = ll_create();
    tofill->count = 0;
    if(ll->next == NULL){
        tofill->data = value;
        tofill->next = NULL;
        ll->next = tofill;
    }else{
        struct linked_list *head = ll->next;
        tofill->data = value;
        tofill->next = head;
        ll->next = tofill;
    }
//    notSuccess = pthread_mutex_unlock(&mutex);
    //printf("Something got added.\n");
}

int ll_length(struct linked_list *ll){
    if(ll->next == NULL){
        return 0;
    }

    struct linked_list *head = ll->next;
    if(head->next == NULL){
        return 1;
    }
    else{
        int count = 1;
        while(head->next != NULL){
            head = head->next;
            count++;
        }
        return count;
    }
}

void* ll_remove_first(struct linked_list *ll){
    //notSuccess = pthread_mutex_lock(&mutex);
      //              if(notSuccess) { printf("lock failed");}


    if(ll->next == NULL){
        printf("This list is empty\n");
       //  notSuccess = pthread_mutex_unlock(&mutex);
        return NULL;
    }
    struct linked_list *head = ll->next;
    void* ret = head->data;
    if(head->next == NULL){
        ll->next = NULL;
        free(head);
        printf("RemoveFirst: This list only had one element, so now is now empty\n");
        //notSuccess = pthread_mutex_unlock(&mutex);
        return ret;
    } else{
        ll->next = head->next;
        free(head);
        //notSuccess = pthread_mutex_unlock(&mutex);
        return ret;
    }
}

int ll_plusplus(struct linked_list *ll, void* value){
    // //increments the count at the given index
    // //returns how many of this value are in the list
    // struct linked_list * node = ll->next;
    // int i;
    // for(i=1; i<index; i++){
    //     node = node->next;   
    // }
    // node->count = node->count + 1;
    // return node->count;




    if(ll->next == NULL)
        return 0;
        //Is == the correct thing to use in this situation??
    struct linked_list * head = ll->next;
    if(strcmp(value, head->data) == 0) {
        head->count = head->count + 1;
        return head->count;
    }
    else{
        int count=1;
        head = head->next;
        while(head != NULL){
            count++;
            if(strcmp(value, head->data) == 0)
                head->count = head->count + 1;
                return head->count;
            head = head->next;
        }
        return 0;
    }
}

struct linked_list* ll_contains(struct linked_list *ll, void *value){
    if(ll->next == NULL)
        return NULL;
        //Is == the correct thing to use in this situation??
    struct linked_list * head = ll->next;
    if(strcmp(value, head->data) == 0)
        return head;
    else{
        int count=1;
        head = head->next;
        while(head != NULL){
            count++;
            if(strcmp(value, head->data) == 0)
                return head;
            head = head->next;
        }
        return NULL;
    }
}

void ll_print_list(struct linked_list *ll)
{
    // Print a list from head to tail.
	if(ll->next != NULL){
        struct linked_list* head = ll->next;
        while(head != NULL)
        {
            printf("%p ", head);
            head = (struct linked_list*) head->next;
        }
        printf("\n");
	}
}

