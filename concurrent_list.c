#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

struct node {
  int value;
	node* next;
	pthread_mutex_t lock;
  // add more fields
};

struct list {
  pthread_mutex_t lock;
	node* head;
};

void print_node(node* node)
{
  // DO NOT DELETE
  if(node)
  {
		pthread_mutex_lock(&(node->lock));
    printf("%d ", node->value);
		pthread_mutex_unlock(&(node->lock));
  }
}

list* create_list()
{
	list* list;
	list=(struct list*)malloc(sizeof(list));
	list->head=NULL;
	pthread_mutex_init(&(list->lock),NULL);
  return list; // REPLACE WITH YOUR OWN CODE
}

void delete_list(list* list)
{
	if(!list)
		return;
  if(!list->head){
		pthread_mutex_destroy(&(list->lock));
		free(list);
		return;
	}
	node* temp1;
	node* temp2;
	temp1=list->head;
	while(temp1->next){
		temp2=temp1->next;
		pthread_mutex_destroy(&(temp1->lock));
		free(temp1);
		temp1=temp2;
	}
	pthread_mutex_destroy(&(temp1->lock));
	free(temp1);
	pthread_mutex_destroy(&(list->lock));
	free(list);
	return;
}

void insert_first(list* list, int val){
	list->head=(struct node*)malloc(sizeof(node));
	list->head->value=val;
	pthread_mutex_init(&(list->head->lock),NULL);
	list->head->next=NULL;
	return;
}

void insert_value(list* list, int value)
{
	node* temp;
	if(!list)
		return;
  pthread_mutex_lock(&(list->lock));
  if(!list->head){
	  insert_first(list,value);
	  pthread_mutex_unlock(&(list->lock));
	  return;
  }
  pthread_mutex_lock(&(list->head->lock)); //head is lock so curr=head is lock rn
  node* curr=list->head;
  node* node_to_insert=(struct node*)malloc(sizeof(node));
  node_to_insert->value=value; // initialize val
  node_to_insert->next=NULL;
  pthread_mutex_init(&(node_to_insert->lock),NULL);//initialize new node lock
	if(curr->value>=value){//case we dont even need to go throught list cuzz val is smallest
		node_to_insert->next=curr;
		list->head=node_to_insert;
		pthread_mutex_unlock(&(curr->lock));
		pthread_mutex_unlock(&(list->lock));
		return;
	}
	//if we got here than new node is not new head
	pthread_mutex_unlock(&(list->lock));
  while(curr->next){
	  pthread_mutex_lock(&(curr->next->lock));
	  if(curr->next->value<value){
			temp=curr->next;
			pthread_mutex_unlock(&(curr->lock));
			curr=temp;
	  }
	  else{
		  break;
	  }
  }
  //right now, curr is lock.
  //case add to end of list, here only curr is lock
  if(!curr->next){
	  curr->next=node_to_insert;
	  pthread_mutex_unlock(&(curr->lock));
	  return;
  }
  //case somewhere in the middle (curr and curr->next are lock)
  node_to_insert->next=curr->next;
  curr->next=node_to_insert;
  pthread_mutex_unlock(&(node_to_insert->next->lock));
  pthread_mutex_unlock(&(curr->lock));
  return;
}

void remove_value(list* list, int value)
{
  node* curr;
	node* temp;
	//pre-first check if list exists in first place
	if(!list)
		return;
	pthread_mutex_lock(&(list->lock));
	//first check that list isnt empty
	if(!list->head){
		pthread_mutex_unlock(&(list->lock));
		return;
	}
	pthread_mutex_lock(&(list->head->lock));
	curr=list->head;
	//second check if first node is to be removed
	if(curr->value==value){
		list->head=list->head->next;
		pthread_mutex_unlock(&(curr->lock));
		pthread_mutex_destroy(&(curr->lock));
		free(curr);
		pthread_mutex_unlock(&(list->lock));
		return;
	}
	pthread_mutex_unlock(&(list->lock));
	//at this point curr=list->head is locked, list is unlocked
	while(curr->next){
		pthread_mutex_lock(&(curr->next->lock));
		if(curr->next->value==value){
			temp=curr->next;
			curr->next=temp->next;
			pthread_mutex_unlock(&(curr->lock));
			pthread_mutex_unlock(&(temp->lock));
			pthread_mutex_destroy(&(temp->lock));
			free(temp);
			return;
		}
		temp = curr->next;
		pthread_mutex_unlock(&(curr->lock));
		curr = temp;
	}

	return;
}

void print_list(list* list)
{
  if(!list)
	{
		printf("\n ");
		return;
	}
	pthread_mutex_lock(&(list->lock));
	if(!list->head){
		pthread_mutex_unlock(&(list->lock));
		printf("\n ");
		return;
	}
	node* curr;
	pthread_mutex_lock(&(list->head->lock));
	node* temp;
	curr=list->head;
	pthread_mutex_unlock(&(list->lock));
	while(curr->next){
		printf("%d ",curr->value);
		temp=curr->next;
		pthread_mutex_unlock(&(curr->lock));
		curr=temp;
		pthread_mutex_lock(&(curr->lock));
	}
	printf("%d ",curr->value);
	pthread_mutex_unlock(&(curr->lock));
  printf("\n"); // DO NOT DELETE comment: okay
	return;
}

void count_list(list* list, int (*predicate)(int))
{
  int count = 0; // DO NOT DELETE
	if(!list)
	{
		printf("\n ");
		return;
	}
	pthread_mutex_lock(&(list->lock));
	if(!list->head){
		printf("\n ");
		pthread_mutex_unlock(&(list->lock));
		return;
	}
	pthread_mutex_lock(&(list->head->lock));
	node* curr;
	node* temp;
	curr=list->head;
	pthread_mutex_unlock(&(list->lock));
	while(curr->next){
		if(predicate(curr->value))
			count++;
		temp=curr->next;
		pthread_mutex_unlock(&(curr->lock));
		curr=temp;
		pthread_mutex_lock(&(curr->lock));
	}
	if(predicate(curr->value))
			count++;
	pthread_mutex_unlock(&(curr->lock));
  printf("%d items were counted\n", count); // DO NOT DELETE okay please dont scream
	return;
}
