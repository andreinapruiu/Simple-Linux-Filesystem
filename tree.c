#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

// All the error messages specified in the homework
#define ALLOCATION_ERROR(name) \
(fprintf(stderr, "Error with the allocation [malloc] %s\n", name))
#define LS_PROBLEM(name) \
(printf("ls: cannot access '%s': No such file or directory\n", name))
#define MKDIR_PROBLEM(name) \
(printf("mkdir: cannot create directory '%s': File exists", name))
#define CD_PROBLEM(path) \
(printf("cd: no such file or directory: %s\n", path))
#define RMDIR_INEXISTENT(name) \
(printf("rmdir: failed to remove '%s': No such file or directory\n", name))
#define RMDIR_FILE(name) \
(printf("rmdir: failed to remove '%s': Not a directory\n", name))
#define RMDIR_CONTENT(name) \
(printf("rmdir: failed to remove '%s': Directory not empty\n", name))
#define RM_INEXISTENT(name) \
(printf("rm: failed to remove '%s': No such file or directory\n", name))
#define RM_FOLDER(name) \
(printf("rm: cannot remove '%s': Is a directory\n", name))
#define RMREC_INEXISTENT(name) \
(printf("rmrec: failed to remove '%s': No such file or directory\n", name))
#define CP_FOLDER(name) \
(printf("cp: -r not specified; omitting directory '%s'\n", name))
#define CP_DEST(name) \
(printf("cp: failed to access '%s': Not a directory\n", name))
#define MV_DEST(name) \
(printf("mv: failed to access '%s': Not a directory\n", name))

// We copied the function from a site in order to fix some
// checker problems
char *strdup(char *org)
{
    int org_size;
    static char *dup;
    char *dup_offset;

    /* Allocate memory for duplicate */
    org_size = strlen(org);
    dup = (char *)malloc(sizeof(char) * org_size+1);
    if (dup == NULL)
        return( (char *)NULL);

    /* Copy string */
    dup_offset = dup;
    while (*org)
    {
        *dup_offset = *org;
        dup_offset++;
        org++;
    }
    *dup_offset = '\0';

    return(dup);
}

// Function which creates the File Tree
// we allocated memory space for the root and
// whatever it may contain later
FileTree createFileTree(char* rootFolderName) {
	FileTree filetree;

	filetree.root = malloc(sizeof(TreeNode));
	if (!filetree.root) {
		ALLOCATION_ERROR("root");
		return filetree;
	}

	filetree.root->parent = NULL;
	filetree.root->type = FOLDER_NODE;

	filetree.root->name = malloc((strlen(rootFolderName) + 1) * sizeof(char));
	if (!filetree.root->name) {
		ALLOCATION_ERROR("root name");
		return filetree;
	}

	memcpy(filetree.root->name, rootFolderName, strlen(rootFolderName) + 1);

	FolderContent *content = malloc(sizeof(FolderContent));
	if (!content) {
		ALLOCATION_ERROR("content");
		return filetree;
	}

	content->children = malloc(sizeof(List));
	if (!content->children) {
		ALLOCATION_ERROR("children");
		return filetree;
	}

	content->children->head = NULL;
	filetree.root->content = content;

	free(rootFolderName);

	return filetree;
}

// Added function which frees all the nodes from
// the file tree, except for the root
void freeNodes(ListNode *head) {
	if (!head)
		return;

	if (head->info->type == FOLDER_NODE) {
		freeNodes(((FolderContent *)head->info->content)->children->head);
		free(((FolderContent *)head->info->content)->children);
	} else {
		if (((FileContent *)head->info->content)->text)
			free(((FileContent *)head->info->content)->text);
	}

	free(head->info->content);
	free(head->info->name);
	free(head->info);

	freeNodes(head->next);
	free(head);
}

// Function which calls the freeNodes function,
// freeing all the nodes, and after that, the root
// basically, we free all the structure
void freeTree(FileTree fileTree) {
	freeNodes(((FolderContent *)fileTree.root->content)->children->head);

	free(((FolderContent *)fileTree.root->content)->children);
	free(fileTree.root->content);
	free(fileTree.root->name);
	free(fileTree.root);
}

// Function which prints the files and directories from
// a specified path. The path can be a folder, in which
// case we are printing all the content from the folder,
// or it can be a file, in which case we are printing
// the text from that certain file. Moreover, if the argument
// is empty, we are printing the elements from the current directory
// pointed by currentNode
void ls(TreeNode* currentNode, char* arg) {
	List *list = ((FolderContent *)currentNode->content)->children;

	if (!list)
		return;

	ListNode *curr = list->head;

	// first case
	if (!strcmp(arg, NO_ARG)) {
		while (curr) {
			printf("%s\n", curr->info->name);
			curr = curr->next;
		}

		return;
	}

	curr = list->head;
	// checking if arg exists
	int ok = 0;
	while (curr) {
		if (!strcmp(arg, curr->info->name)) {
			ok = 1;
			break;
		}
		curr = curr->next;
	}

	if (!ok) {
		LS_PROBLEM(arg);
		return;
	}

	if (curr->info->type == FILE_NODE) {
		printf("%s: %s\n", curr->info->name,
			   ((FileContent *)curr->info->content)->text);
	} else {
		List* list2 = ((FolderContent *)curr->info->content)->children;

		if (!list2)
			return;

		curr = list2->head;

		while (curr) {
			printf("%s\n", curr->info->name);
			curr = curr->next;
		}
	}
}

// Function in which we are printing the absolute path of the current
// directory
void pwd(TreeNode* treeNode) {
	if (!treeNode)
		return;

	pwd(treeNode->parent);

	if (strcmp(treeNode->name, "root"))
		printf("/");

	printf("%s", treeNode->name);
}

// Added function which will help us move from one folder to another,
// by spliting the path with "/". It will return a directory
TreeNode* cd_helper(TreeNode* currentNode, char* path) {
	TreeNode *currentDir = currentNode;

	char *p = strtok(path, "/");
	while (p) {
		if (!strcmp(p, PARENT_DIR)) {
			if (currentDir->parent) {
				currentDir = currentDir->parent;
			} else {
				return currentNode;
			}
		} else {
			List *list = ((FolderContent *)currentDir->content)->children;
			if (!list)
				return currentNode;

			ListNode *curr = list->head;
			int ok = 0;
			while (curr) {
				if (!strcmp(curr->info->name, p) && curr->info->type == FOLDER_NODE) {
					ok = 1;
					break;
				}
				curr = curr->next;
			}

			if (!ok)
				return currentNode;

			currentDir = curr->info;
		}

		p = strtok(NULL, "/");
	}

	return currentDir;
}

// We are using cd_helper in order to print the errors and to return
// the changed directory (the one in which we moved to)
TreeNode* cd(TreeNode* currentNode, char* path) {
	TreeNode *currentDir = cd_helper(currentNode, path);

	if (currentDir == currentNode) {
		CD_PROBLEM(path);
		return currentNode;
	} else {
		return currentDir;
	}
}

// Added function which will help us iterate through the file system,
// starting from the current folder and moving on to the final element
// from the hierarchy
void tree_helper(ListNode *head, int tabs, int *no_files, int *no_folders) {
	if (!head)
		return;

	for (int i = 0; i < tabs; i++)
		printf("\t");

	if (head->info->type == FILE_NODE) {
		printf("%s\n", head->info->name);
		(*no_files)++;
	} else {
		printf("%s\n", head->info->name);
		tree_helper(((FolderContent *)head->info->content)->children->head,
					tabs + 1, no_files, no_folders);
		(*no_folders)++;
	}

	tree_helper(head->next, tabs, no_files, no_folders);
}

// Function which is printing the hierarchy starting from either
// the current directory, or from the path that is given as an argument
void tree(TreeNode* currentNode, char* arg) {
	int no_files = 0, no_folders = 0;

	if (!strcmp(arg, NO_ARG)) {
		tree_helper(((FolderContent*)currentNode->content)->children->head, 0,
					&no_files, &no_folders);
	} else {
		TreeNode *currentDir = cd_helper(currentNode, arg);

		if (currentDir != currentNode) {
			tree_helper(((FolderContent*)currentDir->content)->children->head,
						0, &no_files, &no_folders);
		} else {
			printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
			return;
		}
	}

	printf("%d directories, %d files\n", no_folders, no_files);
}

// Function which creates a new directory in the current directory
// pointed by the currentNode
void mkdir(TreeNode* currentNode, char* folderName) {
	List *list = ((FolderContent *)currentNode->content)->children;
	ListNode *curr = list->head;

	while (curr) {
		if (!strcmp(folderName, curr->info->name)) {
			MKDIR_PROBLEM(folderName);

			free(folderName);

			return;
		}
		curr = curr->next;
	}

	if (!list) {
		free(folderName);

		return;
	}

	ListNode *new = malloc(sizeof(ListNode));
	if (!new) {
		ALLOCATION_ERROR("new");
		free(folderName);

		return;
	}

	new->info = malloc(sizeof(TreeNode));
	if (!new->info) {
		ALLOCATION_ERROR("new info");
		free(folderName);

		return;
	}

	new->info->parent = currentNode;

	new->info->name = malloc((strlen(folderName) + 1) * sizeof(char));
	if (!new->info->name) {
		ALLOCATION_ERROR("new name");
		free(folderName);

		return;
	}
	memcpy(new->info->name, folderName, strlen(folderName) + 1);

	new->info->type = FOLDER_NODE;

	FolderContent *content = malloc(sizeof(FolderContent));
	if (!content) {
		ALLOCATION_ERROR("new content");
		free(folderName);

		return;
	}

	content->children = malloc(sizeof(List));
	if (!content->children) {
		ALLOCATION_ERROR("children");
		free(folderName);

		return;
	}

	content->children->head = NULL;

	new->info->content = content;

	new->next = NULL;

	if (!list->head) {
		list->head = new;
		free(folderName);

		return;
	}

	new->next = list->head;
	list->head = new;

	free(folderName);
}

// Function which deletes a file/directory, whether it has content or not
void rmrec(TreeNode* currentNode, char* resourceName) {
	List *list = ((FolderContent *)currentNode->content)->children;

	if (!list)
		return;

	ListNode *curr = list->head;
	ListNode *prev = NULL;

	int ok = 0;
	while (curr) {
		if (!strcmp(resourceName, curr->info->name)) {
			ok = 1;
			break;
		}
		prev = curr;
		curr = curr->next;
	}

	if (!ok) {
		RMREC_INEXISTENT(resourceName);
		return;
	}

	freeNodes(((FolderContent *)curr->info->content)->children->head);
	free(((FolderContent *)curr->info->content)->children);
	free(curr->info->content);
	free(curr->info->name);
	free(curr->info);

	prev->next = curr->next;
	free(curr);
}

// Function which removes a file from the FileTree
void rm(TreeNode* currentNode, char* fileName) {
	List *list = ((FolderContent *)currentNode->content)->children;

	if (!list)
		return;

	ListNode *curr = list->head;
	ListNode *prev = NULL;

	int ok = 0;
	while (curr) {
		if (!strcmp(fileName, curr->info->name)) {
			ok = 1;
			break;
		}
		prev = curr;
		curr = curr->next;
	}

	if (!ok) {
		RM_INEXISTENT(fileName);
		return;
	}

	if (curr->info->type == FOLDER_NODE) {
		RM_FOLDER(fileName);
		return;
	}

	if (((FileContent *)curr->info->content)->text)
		free(((FileContent *)curr->info->content)->text);

	free(curr->info->content);
	free(curr->info->name);
	free(curr->info);

	prev->next = curr->next;
	free(curr);
}

// Function which removes a folder from the FileTree, only if it does
// not have content
void rmdir(TreeNode* currentNode, char* folderName) {
	List *list = ((FolderContent *)currentNode->content)->children;

	if (!list)
		return;

	ListNode *curr = list->head;
	ListNode *prev = NULL;

	int ok = 0;
	while (curr) {
		if (!strcmp(folderName, curr->info->name)) {
			ok = 1;
			break;
		}
		prev = curr;
		curr = curr->next;
	}

	if (!ok) {
		RMDIR_INEXISTENT(folderName);
		return;
	}

	if (curr->info->type == FILE_NODE) {
		RMDIR_FILE(folderName);
		return;
	}

	if (((FolderContent *)curr->info->content)->children->head) {
		RMDIR_CONTENT(folderName);
		return;
	}

	free(((FolderContent *)curr->info->content)->children);
	free(curr->info->content);
	free(curr->info->name);
	free(curr->info);

	prev->next = curr->next;
	free(curr);
}

// Function which creates an empty file with a given name
// (if there is not a second argument) in the current directory.
// If there is a second argument [filecontent], then the new file will also
// have a content - text
void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
	List *list = ((FolderContent *)currentNode->content)->children;
	ListNode *curr = list->head;

	while (curr) {
		if (!strcmp(fileName, curr->info->name)) {
			free(fileName);
			free(fileContent);

			return;
		}
		curr = curr->next;
	}

	if (!list) {
		free(fileName);
		free(fileContent);

		return;
	}

	ListNode *new = malloc(sizeof(ListNode));
	if (!new) {
		ALLOCATION_ERROR("new");
		free(fileName);
		free(fileContent);

		return;
	}

	new->info = malloc(sizeof(TreeNode));
	if (!new->info) {
		ALLOCATION_ERROR("new info");
		free(fileName);
		free(fileContent);

		return;
	}

	new->info->parent = currentNode;

	new->info->name = malloc((strlen(fileName) + 1) * sizeof(char));
	if (!new->info->name) {
		ALLOCATION_ERROR("new name");
		free(fileName);
		free(fileContent);

		return;
	}
	memcpy(new->info->name, fileName, strlen(fileName) + 1);

	new->info->type = FILE_NODE;

	FileContent *content = malloc(sizeof(FileContent));
	if (!content) {
		ALLOCATION_ERROR("new content");
		free(fileName);
		free(fileContent);

		return;
	}

	if (fileContent) {
		content->text = malloc((strlen(fileContent) + 1) * sizeof(char));
		if (!content->text) {
			ALLOCATION_ERROR("new content text");
			free(fileName);
			free(fileContent);

			return;
		}

		memcpy(content->text, fileContent, strlen(fileContent) + 1);
	} else {
		content->text = NULL;
	}

	new->info->content = content;

	new->next = NULL;

	if (!list->head) {
		list->head = new;
		free(fileName);
		free(fileContent);

		return;
	}

	new->next = list->head;
	list->head = new;

	free(fileName);
	free(fileContent);
}

// Added function (similar to cd_helper), which will return the address of the
// currentNode from the given path, whether it is a folder or a file
TreeNode* cp_helper(TreeNode* currentNode, char* path) {
	TreeNode *currentDir = currentNode;
	char *copy = strdup(path);

	char *p = strtok(copy, "/");
	while (p) {
		if (!strcmp(p, PARENT_DIR)) {
			if (currentDir->parent) {
				currentDir = currentDir->parent;
			} else {
				free(copy);
				return currentNode;
			}
		} else {
			List *list = ((FolderContent *)currentDir->content)->children;
			if (!list) {
				free(copy);
				return currentNode;
			}

			ListNode *curr = list->head;
			int ok = 0;
			while (curr) {
				if (!strcmp(curr->info->name, p)) {
					ok = 1;
					break;
				}
				curr = curr->next;
			}

			if (!ok) {
				free(copy);
				return currentNode;
			}

			currentDir = curr->info;
		}

		p = strtok(NULL, "/");
	}

	free(copy);
	return currentDir;
}

// Function which will copy the file from a given source path to a given
// destination path (with its content - text).
// If the destination is a folder, it will copy the file inside the destination
// folder, and if the destination is a file, then the content from the
// destination file will be overwritten with the source's text
void cp(TreeNode* currentNode, char* source, char* destination) {
	TreeNode *currSource = cp_helper(currentNode, source);

	if (currSource->type == FOLDER_NODE) {
		CP_FOLDER(source);
		return;
	}

	TreeNode *currDestination = cp_helper(currentNode, destination);

	if (currentNode == currDestination) {
		CP_DEST(destination);
		return;
	}

	if (currDestination->type == FOLDER_NODE) {
		char *name = malloc(strlen(currSource->name) + 1);
		memcpy(name, currSource->name, strlen(currSource->name) + 1);

		char *text =
			malloc(strlen(((FileContent *)currSource->content)->text) + 1);
		memcpy(text, ((FileContent *)currSource->content)->text,
			   strlen(((FileContent *)currSource->content)->text) + 1);

		touch(currDestination, name, text);
	} else {
		if (((FileContent *)currDestination->content)->text)
			free(((FileContent *)currDestination->content)->text);

		((FileContent *)currDestination->content)->text =
			malloc((strlen(((FileContent *)currSource->content)->text) + 1)
					* sizeof(char));
		if (!((FileContent *)currDestination->content)->text) {
			ALLOCATION_ERROR("destination text");
			return;
		}

		memcpy(((FileContent *)currDestination->content)->text,
				((FileContent *)currSource->content)->text,
				strlen(((FileContent *)currSource->content)->text) + 1);
	}
}

void mv(TreeNode* currentNode, char* source, char* destination) {
	TreeNode *currSource = cp_helper(currentNode, source);

	TreeNode *currDestination = cp_helper(currentNode, destination);

	if (currentNode == currDestination) {
		MV_DEST(destination);
		return;
	}

	if (currDestination->type == FOLDER_NODE) {
		List *listDest = ((FolderContent *)currDestination->content)->children;

		List *listSrc = ((FolderContent *)currSource->parent->content)->children;

		ListNode *currSrc = listSrc->head;
		ListNode *prevSrc = NULL;
		ListNode *currDest = listDest->head;

		while (currSrc) {
			if (!strcmp(currSrc->info->name, currSource->name))
				break;

			prevSrc = currSrc;
			currSrc = currSrc->next;
		}

		if (prevSrc)
			prevSrc->next = currSrc->next;
		else
			listSrc->head = currSrc->next;

		currSrc->next = listDest->head;
		listDest->head = currSrc;
	} else {
		List *listDest = ((FolderContent *)currDestination->content)->children;

		List *listSrc = ((FolderContent *)currSource->parent->content)->children;

		ListNode *currSrc = listSrc->head;
		ListNode *prevSrc = NULL;
		ListNode *currDest = listDest->head;

		while (currSrc) {
			if (!strcmp(currSrc->info->name, currSource->name))
				break;

			prevSrc = currSrc;
			currSrc = currSrc->next;
		}

		if (prevSrc)
			prevSrc->next = currSrc->next;
		else
			listSrc->head = currSrc->next;

		if (((FileContent *)currDestination->content)->text)
			free(((FileContent *)currDestination->content)->text);

		((FileContent *)currDestination->content)->text =
			malloc((strlen(((FileContent *)currSource->content)->text) + 1) *
					sizeof(char));
		if (!((FileContent *)currDestination->content)->text) {
			ALLOCATION_ERROR("destination text");
			return;
		}

		memcpy(((FileContent *)currDestination->content)->text,
				((FileContent *)currSource->content)->text,
				strlen(((FileContent *)currSource->content)->text) + 1);

		free(currSource->name);
		free(((FileContent *)currSource->content)->text);
		free(currSource->content);
		free(currSource);
		free(currSrc);
	}
}

