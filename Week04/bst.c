#include<stdio.h>
#include<stdlib.h>

typedef enum { RED, BLACK } COLOR;
typedef struct NODE
{
	int data;
	COLOR color;
	struct NODE* left, * right, * p;

}NODE;
NODE* root;

NODE* insert(NODE** p_root, int data)
{
	if (*p_root != NULL) {
		if (data < (*p_root)->data){
			(*p_root)->left = insert(&((*p_root)->left), data);
			// p_root은 이중포인터이므로 p_root을 역참조해야 해당 노드를 가리킬 수 있다.
		}
		else {
			(*p_root)->right = insert(&((*p_root)->right), data);
		}
	}
	else {
		*p_root = (NODE*)malloc(sizeof(NODE));
		(*p_root)->right = (*p_root)->left = NULL;
		(*p_root)->data = data;
		(*p_root)->color = RED;
		return *p_root;
	}
	return *p_root;
}

NODE* fMin(NODE* root)
{
	NODE* min = root;
	while (min->left != NULL)
		min = min->left;
	return min;
}
NODE* delete(NODE* root, int data)
{
	NODE* tmproot = NULL;

	if (root == NULL)
		return NULL;
	if (data < root->data)
		root->left = delete(root->left, data);
	else if (data > root->data)
		root->right = delete(root->right, data);
	else
	{
		if (root->left != NULL && root->right != NULL)
		{
			tmproot = fMin(root->right);
			root->data = tmproot->data;
			root->right = delete(root->right, tmproot->data);
		}
		else
		{
			tmproot = (root->left == NULL) ? root->right : root->left;
			free(root);
			return tmproot;
		}
	}
	return root;
}

//전위 순회
void preorderPrint(NODE* root)
{
	if (root == NULL)
		return;
	printf("%d ", root->data);
	preorderPrint(root->left);
	preorderPrint(root->right);
}
//중위 순회
void inorderPrint(NODE* root)
{
	if (root == NULL)
		return;
	inorderPrint(root->left);
	printf("%d ", root->data);
	inorderPrint(root->right);
}
//후위 순회
void postorderPrint(NODE* root)
{
	if (root == NULL)
		return;
	postorderPrint(root->left);
	postorderPrint(root->right);
	printf("%d ", root->data);
}

int main()
{
	NODE* root = NULL;

	root = insert(&root, 50);
	root = insert(&root, 30);
	root = insert(&root, 60);
	root = insert(&root, 80);
	root = insert(&root, 20);
	root = insert(&root, 70);

	root = insert(&root, 10);
	root = insert(&root, 40);

	printf("preorder : ");
	preorderPrint(root);
	printf("\n");
	printf("inorder : ");
	inorderPrint(root);
	printf("\n");
	printf("postorder : ");
	postorderPrint(root);
	printf("\n\n");


	printf("preorder : ");
	preorderPrint(root);
	printf("\n");

	printf("delete : 9\n");
	root = delete(root, 9);

	preorderPrint(root);
	printf("\n");

	root = insert(&root, 9);

	printf("delete : 1\n");
	root = delete(root, 1);

	preorderPrint(root);
	printf("\n");

	root = insert(&root, 1);

	printf("delete : 5\n");
	root = delete(root, 5);

	preorderPrint(root);
	printf("\n");
}