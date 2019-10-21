/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student : 32131727 윤지원, 32121826 서영준
*
*   lab2_bst.c :
*       - thread-safe bst code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe bst for coarse-grained version and fine-grained version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "lab2_sync_types.h"

int tmp_cnt, cnt; // 노드의 수를 세는데 필요한 변수

/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */

// 노드의 수를 세는데 필요한 함수
void countaction() { 
	cnt = cnt + 1;
}

// 중위순회 하여 노드의 수를 세는 함수
int lab2_node_print_inorder(lab2_node *root) {
    // You need to implement lab2_node_print_inorder function.
	if(root == NULL)
		return 0;
	
	inorder_count(root);

	tmp_cnt=cnt;
	cnt=0;

	return tmp_cnt;
}

// 노드의 수를 세는데 필요한 함수2
void inorder_count(lab2_node *node)
{
	if(node==NULL) return;
	if(node->left) inorder_count(node->left);
	countaction();	
	if(node->right) inorder_count(node->right);
}

/*
 * TODO
 *  Implement function which creates struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_tree )
 * 
 *  @return                 : bst which you created in this function.
 */
// 트리 생성
lab2_tree *lab2_tree_create() {
    // You need to implement lab2_tree_create function.
	lab2_tree *tree = (lab2_tree *)malloc(sizeof(lab2_tree));
	tree->root = NULL;
	return tree;
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : bst node's key to creates
 *  @return                 : bst node which you created in this function.
 */
// 노드 생성
lab2_node * lab2_node_create(int key) {
    // You need to implement lab2_node_create function.
	lab2_node * nd = (lab2_node *)malloc(sizeof(lab2_node));
	nd->left = NULL;
	nd->right = NULL;
	nd->key = key;
	return nd;
}

// 노드의 키를 가져오는 함수
int GetKey(lab2_node * bt)
{
	return bt->key;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST. 
 *  
 *  @param lab2_tree *tree      : bst which you need to insert new node.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                 : satus (success or fail)
 */
// Single-Thread 노드 삽입
void lab2_node_insert(lab2_tree *tree, lab2_node * new_node){
    // You need to implement lab2_node_insert function.
	lab2_node * pNode = NULL;    // parent node
	lab2_node * cNode = tree->root;    // current node
	lab2_node * nNode = NULL;    // new node

	// 새로운 노드가 추가될 위치를 찾는다.
	while(cNode != NULL)
	{
		pNode = cNode;

		if(GetKey(cNode) > new_node->key)
			cNode = cNode->left;
		else
			cNode = cNode->right;
	}
	
	// pNode의 서브 노드에 추가할 새 노드의 생성
	nNode = lab2_node_create(new_node->key);    // 새 노드의 생성
	nNode->key = new_node->key;    // 새 노드에 데이터 저장

	// pNode의 서브 노드에 새 노드를 추가
	if(pNode != NULL)    // 새 노드가 루트 노드가 아니라면,
	{
		if(new_node->key < GetKey(pNode))
			{
				if(pNode->left != NULL)
					free(pNode->left);

					pNode->left = nNode;
			}
		else
			{
			if(pNode->right != NULL)
			free(pNode->right);

			pNode->right = nNode;
			}	
	}
	else    // 새 노드가 루트 노드라면,
	{
		tree->root = nNode;
	}
}


/* 
 * TODO
 *  Implement a function which insert nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in fine-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
// Multi-Thread Fine-Grained 노드 삽입
void lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert_fg function.
	
	pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
	
	lab2_node * pNode = NULL;    // parent node
	lab2_node * cNode = tree->root;    // current node
	lab2_node * nNode = NULL;    // new node
	
	if(cNode!=NULL)
	{
		pthread_mutex_lock(&cNode->mutex_node);
	}

	else
	{	
		pthread_mutex_lock(&tree->mutex_tree);
		tree->root = new_node;
		pthread_mutex_unlock(&tree->mutex_tree);
		return;
	}
	
	while(cNode != NULL)
	{
		pNode = cNode;
		pthread_mutex_unlock(&cNode->mutex_node);
		pthread_mutex_lock(&pNode->mutex_node);
		
		if(GetKey(cNode) > new_node->key)
		{
			pthread_mutex_unlock(&pNode->mutex_node);
			if(cNode->left!=NULL){
			pthread_mutex_lock(&cNode->left->mutex_node);
			cNode = cNode->left;
			pthread_mutex_unlock(&cNode->mutex_node);
			}
			else
			{cNode = cNode->left;}
			if(cNode!=NULL)
			{
				pthread_mutex_lock(&cNode->mutex_node);
			}
		}
		else
		{
			pthread_mutex_unlock(&pNode->mutex_node);
			if(cNode->right!=NULL){
			pthread_mutex_lock(&cNode->right->mutex_node);
			cNode = cNode->right;
			pthread_mutex_unlock(&cNode->mutex_node);
			}
			else
			{cNode = cNode->right;}
			if(cNode!=NULL)
			{
				pthread_mutex_lock(&cNode->mutex_node);
			}
		}
	}
	// pNode의 서브 노드에 추가할 새 노드의 생성
	if(pNode!=NULL)
	{
		pthread_mutex_lock(&pNode->mutex_node);
	}
	nNode = lab2_node_create(new_node->key);    // 새 노드의 생성
	pthread_mutex_lock(&nNode->mutex_node);
	nNode->key = new_node->key;    // 새 노드에 데이터 저장
	
	// pNode의 서브 노드에 새 노드를 추가
	
	if(pNode != NULL)    // 새 노드가 루트 노드가 아니라면,
	{
		if(new_node->key < GetKey(pNode))
		{
			if(pNode->left != NULL)
			{
				free(pNode->left);
			}
				
			pNode->left = nNode;
			pthread_mutex_unlock(&pNode->mutex_node);
			pthread_mutex_unlock(&nNode->mutex_node);
		}

		else
		{
			if(pNode->right != NULL)
			{
				free(pNode->right);
			}

			pNode->right = nNode;
			pthread_mutex_unlock(&pNode->mutex_node);
			pthread_mutex_unlock(&nNode->mutex_node);
		}	
	}
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in coarse-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
// Multi-Thread Coarse-Grained 노드 삽입
void lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert_cg function.

	pthread_mutex_lock(&tree->mutex_tree);

	lab2_node * pNode = NULL;    // parent node
	lab2_node * cNode = tree->root;    // current node
	lab2_node * nNode = NULL;    // new node

	// 새로운 노드가 추가될 위치를 찾는다.
	while(cNode != NULL)
	{
		pNode = cNode;

		if(GetKey(cNode) > new_node->key)
			cNode = cNode->left;
		else
			cNode = cNode->right;
	}
	
	// pNode의 서브 노드에 추가할 새 노드의 생성
	nNode = lab2_node_create(new_node->key);    // 새 노드의 생성
	nNode->key = new_node->key;    // 새 노드에 데이터 저장

	
	// pNode의 서브 노드에 새 노드를 추가
	if(pNode != NULL)    // 새 노드가 루트 노드가 아니라면,
	{
		if(new_node->key < GetKey(pNode))
			{
			if(pNode->left != NULL)
			free(pNode->left);
	
			pNode->left = nNode;
			pthread_mutex_unlock(&tree->mutex_tree);
	}
		else
		{
			if(pNode->right != NULL)
			free(pNode->right);

			pNode->right = nNode;
			pthread_mutex_unlock(&tree->mutex_tree);
		}
	}
	else    // 새 노드가 루트 노드라면,
	{
		tree->root = nNode;
		pthread_mutex_unlock(&tree->mutex_tree);
	}
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
// Single-Thread 노드 삭제
void lab2_node_remove(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove function.
	lab2_node * pVRoot = lab2_node_create(key);

	lab2_node * pNode = pVRoot;


	lab2_node * cNode = tree->root;
	lab2_node * dNode;

	pVRoot->right = tree->root;

	while(cNode != NULL && GetKey(cNode) != key)
	{
		pNode = cNode;

		if(key < GetKey(cNode))
			cNode = cNode->left;
		else
			cNode = cNode->right;
	}

	if(cNode == NULL)
		return;

	dNode = cNode;

	if(cNode->left == NULL && cNode->right == NULL)
	{
		tree->root = NULL;
		return;
	}

	if(dNode->left == NULL && dNode->right == NULL)
	{
		if(pNode->left == dNode)
		{
			lab2_node * delNode;

			if(pNode != NULL)
			{
				delNode = pNode->left;
				pNode->left = NULL;
			}
			return;
		}
	
		else
		{
			lab2_node * delNode;

			if(pNode != NULL)
			{
				delNode = pNode->right;
				pNode->right = NULL;
			}
			return ;
		}
	}
	else if (dNode->left == NULL || dNode->right == NULL)
	{
		lab2_node * dcNode;

		if(dNode->left != NULL)
			dcNode = dNode->left;
		else
			dcNode = dNode->right;

		if(pNode->left == dNode)
			pNode->left = dcNode;
		else
			pNode->right = dcNode;
	}

	else
	{
		lab2_node * mNode = dNode->right;
		lab2_node * mpNode = dNode;
		int delKey;
	
		while(mNode->left != NULL)
		{
			mpNode = mNode;
			mNode = mNode->left;
		}

		delKey = GetKey(dNode);
		dNode->key=mNode->key;
	

		if(mpNode->left == mNode) mpNode->left=mNode->right;
		else
			mpNode->right=mNode->right;
	
		dNode = mNode;
		dNode->key = delKey;
	}

	if(pVRoot->right != tree->root)
		tree->root = pVRoot->right;

	free(pVRoot);
	return ;

}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in fine-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
// Multi-Thread Fine-Grained 노드 삭제
void lab2_node_remove_fg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_fg function.

	lab2_node * pVRoot = lab2_node_create(key);

	lab2_node * pNode = pVRoot;


	lab2_node * cNode = tree->root;
	lab2_node * dNode;
pthread_mutex_lock(&pVRoot->mutex_node);
	pVRoot->right = tree->root;
pthread_mutex_unlock(&pVRoot->mutex_node);


if(cNode!=NULL)
	{
		pthread_mutex_lock(&cNode->mutex_node);
	}	
	else
	{
		pthread_mutex_lock(&tree->mutex_tree);
		goto SKIP2;
	}

	while(cNode != NULL && GetKey(cNode) != key)
	{
		pNode = cNode;


	pthread_mutex_unlock(&cNode->mutex_node);
		pthread_mutex_lock(&pNode->mutex_node);

		if(key < GetKey(cNode))
		{
			pthread_mutex_unlock(&pNode->mutex_node);
			cNode = cNode->left;
			if(cNode!=NULL)
			{
		 		pthread_mutex_lock(&cNode->mutex_node);
			}
		}
		else
		{
			pthread_mutex_unlock(&pNode->mutex_node);
			cNode = cNode->right;
			if(cNode!=NULL)
			{
				pthread_mutex_lock(&cNode->mutex_node);
			}
		}
	}
	pthread_mutex_unlock(&cNode->mutex_node);
	if(cNode == NULL)
	{
SKIP2:	
		pthread_mutex_unlock(&tree->mutex_tree);
		return;
	}

	pthread_mutex_lock(&cNode->mutex_node);
	dNode = cNode;
	pthread_mutex_unlock(&cNode->mutex_node);
	pthread_mutex_lock(&dNode->mutex_node);

	
	if(cNode->left == NULL && cNode->right == NULL)
	{
		tree->root = NULL;
		pthread_mutex_unlock(&cNode->mutex_node);
		pthread_mutex_unlock(&dNode->mutex_node);
		pthread_mutex_unlock(&pNode->mutex_node);
		return;
	}

	if(dNode->left == NULL && dNode->right == NULL)
	{
		
		if(pNode->left == dNode)
		{
			pthread_mutex_unlock(&dNode->mutex_node);
			lab2_node * delNode;
			
			if(pNode != NULL)
			{
				pthread_mutex_lock(&pNode->mutex_node);
				delNode = pNode->left;
				pthread_mutex_unlock(&pNode->mutex_node);
				pthread_mutex_lock(&delNode->mutex_node);
				pthread_mutex_lock(&pNode->mutex_node);
				pNode->left = NULL;		
				pthread_mutex_unlock(&pNode->mutex_node);
				pthread_mutex_unlock(&delNode->mutex_node);
			}
			return;
		}
	
		else
		{		
			pthread_mutex_unlock(&dNode->mutex_node);
			lab2_node * delNode;

			if(pNode != NULL)
			{
				pthread_mutex_lock(&pNode->mutex_node);
				delNode = pNode->right;
				pthread_mutex_unlock(&pNode->mutex_node);
				pthread_mutex_lock(&delNode->mutex_node);
				pthread_mutex_lock(&pNode->mutex_node);
				pNode->right = NULL;
				pthread_mutex_unlock(&pNode->mutex_node);
				pthread_mutex_unlock(&delNode->mutex_node);
			}
			return ;
		}
	}

	else if (dNode->left == NULL || dNode->right == NULL)
	{
		lab2_node * dcNode;

		if(dNode->left != NULL)
		{
			dcNode = dNode->left;
			pthread_mutex_unlock(&dNode->mutex_node);
			pthread_mutex_lock(&dcNode->mutex_node);
		}
		else
		{
			dcNode = dNode->right;	
			pthread_mutex_unlock(&dNode->mutex_node);
			pthread_mutex_lock(&dcNode->mutex_node);
		}

		if(pNode->left == dNode)
		{
			pthread_mutex_lock(&pNode->mutex_node);
			pNode->left = dcNode;
			pthread_mutex_unlock(&dcNode->mutex_node);
			pthread_mutex_unlock(&pNode->mutex_node);
		}
		else
		{
			pthread_mutex_lock(&pNode->mutex_node);
			pNode->right = dcNode;
			pthread_mutex_unlock(&dcNode->mutex_node);
			pthread_mutex_unlock(&pNode->mutex_node);
		}
	}

	else
	{
		lab2_node * mNode = dNode->right;
		lab2_node * mpNode = dNode;
		int delKey;
		pthread_mutex_unlock(&dNode->mutex_node);
		
		
		while(mNode->left != NULL)
		{
			pthread_mutex_lock(&mNode->mutex_node);
			mpNode = mNode;
			pthread_mutex_unlock(&mNode->mutex_node);
			mNode = mNode->left;
		}

		delKey = GetKey(dNode);
		
		pthread_mutex_lock(&mNode->mutex_node);
		dNode->key=mNode->key;
		
		pthread_mutex_unlock(&mNode->mutex_node);

		pthread_mutex_lock(&mpNode->mutex_node);
		if(mpNode->left == mNode) 
		{
			mpNode->left=mNode->right;
		}
		else
		{
			mpNode->right=mNode->right;
		}
		
		dNode = mNode;
		pthread_mutex_unlock(&mpNode->mutex_node);
		pthread_mutex_lock(&dNode->mutex_node);
		dNode->key = delKey;
		pthread_mutex_unlock(&dNode->mutex_node);
	}

	if(pVRoot->right != tree->root)
	{
		pthread_mutex_lock(&pVRoot->mutex_node);
		tree->root = pVRoot->right;
		pthread_mutex_unlock(&pVRoot->mutex_node);
	}

	free(pVRoot);
	return ;	
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in coarse-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
// Multi-Thread Coarse-Grained 노드 삭제
void lab2_node_remove_cg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_cg function.
	pthread_mutex_lock(&tree->mutex_tree);
	lab2_node * pVRoot = lab2_node_create(key);

	lab2_node * pNode = pVRoot;

	lab2_node * cNode = tree->root;
	lab2_node * dNode;

	pVRoot->right = tree->root;

	while(cNode != NULL && GetKey(cNode) != key)
	{
		pNode = cNode;

		if(key < GetKey(cNode))
			cNode = cNode->left;
		else
			cNode = cNode->right;
	}

	if(cNode == NULL)
	{
		pthread_mutex_unlock(&tree->mutex_tree);
		return;
	}
	dNode = cNode;

		if(cNode->left == NULL && cNode->right == NULL)
	{
		tree->root = NULL;
		pthread_mutex_unlock(&tree->mutex_tree);
		return;
	}

	if(dNode->left == NULL && dNode->right == NULL)
	{
		if(pNode->left == dNode)
		{
			lab2_node * delNode;

			if(pNode != NULL)
			{
				delNode = pNode->left;
				pNode->left = NULL;
			}
			pthread_mutex_unlock(&tree->mutex_tree);
			return;
		}
	
		else
		{
			lab2_node * delNode;

			if(pNode != NULL)
			{
				delNode = pNode->right;
				pNode->right = NULL;
			}
			pthread_mutex_unlock(&tree->mutex_tree);
			return ;
		}
	}
	else if (dNode->left == NULL || dNode->right == NULL)
	{
		lab2_node * dcNode;

		if(dNode->left != NULL)
			dcNode = dNode->left;
		else
			dcNode = dNode->right;

		if(pNode->left == dNode)
			pNode->left = dcNode;
		else
			pNode->right = dcNode;
	}

	else
	{
		lab2_node * mNode = dNode->right;
		lab2_node * mpNode = dNode;
		int delKey;
	
		while(mNode->left != NULL)
		{
			mpNode = mNode;
			mNode = mNode->left;
		}

		delKey = GetKey(dNode);
		dNode->key=mNode->key;
	

		if(mpNode->left == mNode) mpNode->left=mNode->right;
		else
			mpNode->right=mNode->right;
	
		dNode = mNode;
		dNode->key = delKey;
	}

	if(pVRoot->right != tree->root)
		tree->root = pVRoot->right;

	free(pVRoot);
	pthread_mutex_unlock(&tree->mutex_tree);
	return ;

}


/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
// 트리 삭제
void lab2_tree_delete(lab2_tree *tree) {
    // You need to implement lab2_tree_delete function.
	pthread_mutex_unlock(&tree->mutex_tree);
	free(tree);
}

/*
 * TODO
 *  Implement function which delete struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst node which you want to remove. 
 *  @return                 : status(success or fail)
 */
void lab2_node_delete(lab2_node *node) {
    // You need to implement lab2_node_delete function.
}