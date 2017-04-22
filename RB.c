#include "RB.h"
// Bottom-up red-black tree implementation without deletions,
// so free() does not appear.

// July 22, 2016 - modified for CLRS, 3rd ed. insertion

#include <stdlib.h>
#include <stdio.h>


link z,head; // Pointers to sentinel and root
link live,dead;   //Pointers to live list and Recycling list   
Item NULLitem=(-9999999);  // Data for sentinel

int trace=0;  // Controls trace output for insert

link NEW(Item item, link l, link r, int N)
// Allocates and fills in a node
{
link x = malloc(sizeof *x); 
x->item = item;
x->l = l;
x->r = r;
x->red = 1;
x->N = N;
return x;
}

void STinit()
{
// Root/sentinel (head/z) is always black
head = (z = NEW(NULLitem, 0, 0, 0));
head->red=0;
}



Item searchR(link h, Key v)
// Recursive search for a key
{ 
Key t = key(h->item);
if (h == z) //if examined node at sentinel, not found
  return NULLitem;
if (eq(v, t)) //if new node equals the one in tree return the item
  return h->item;
if (less(v, t)) //if new node is less thant tree, recall the function
  return searchR(h->l, v);
return searchR(h->r, v); //recall funcion if more than the node
}

Item STsearch(Key v) 
{
return searchR(head, v);
}

Item selectR(link h, int i)
// Returns the ith smallest key where i=1 returns the smallest
// key.  Thus, this is like flattening the tree inorder into an array
// and applying i as a subscript.
{ 
int r = h->l->N+1;

if (h == z)
{
  printf("Impossible situation in selectR\n");
  STprintTree();
  exit(0);
}
if (i==r)
  return h->item;
if (i<r)
  return selectR(h->l, i);
return selectR(h->r, i-r);
}

Item STselect(int k)
{
if (k<1 || k>head->N)
{
  printf("Range error in STselect() k %d N %d\n",k,head->N);
  exit(0);
}
return selectR(head, k);
}

int invSelectR(link h, Key v)
// Inverse of selectR
{
Key t = key(h->item);
int work;

if (h==z)
  return -1;  // v doesn't appear as a key
if (eq(v, t))
  return h->l->N+1;
if (less(v, t))
  return invSelectR(h->l,v);
work=invSelectR(h->r,v);
if (work==(-1))
  return -1;  // v doesn't appear as a key
return 1 + h->l->N + work;
}

int STinvSelect(Key v)
{
return invSelectR(head,v);
}

void fixN(link h)
// Fixes subtree size of h, assuming that subtrees have correct sizes
{
h->N=h->l->N + h->r->N + 1;
}

link rotR(link h)
// Rotate right at h, i.e. flip edge between h & h->l
{
link x = h->l;
h->l = x->r;
x->r = h;

x->N = x->r->N;
fixN(x->r);
return x;
}

link rotL(link h)
// Rotate left at h, i.e. flip edge between h & h->r
{
link x = h->r;
h->r = x->l;
x->l = h;

x->N = x->l->N;
fixN(x->l);
return x;
}

void extendedTraceOn()
{
trace=2;
}

void basicTraceOn()
{
trace=1;
}

void traceOff()
{
trace=0;
}

void tracePrint(char *s,link h)
{
if (trace) {
  if (h==z)
    printf("%s at sentinel\n",s);
  else
    printf("%s at %d\n",s,key(h->item));
  }
}

link RBinsert(link h, Item item, int sw, int siblingRed, link hParent)
// CLRS, 3rd ed., RB tree insertion done recursively without parent pointers.
// Also includes tracing.  See 2320 notes.  BPW
// h is present node in search down tree.
// Returns root of modified subtree.
// item is the Item to be inserted.
// sw == 1 <=> h is to the right of its parent.
// siblingRed has color of h's sibling.
// hParent has h's parent to facilitate case 1 color flips.
{
Key v = key(item);
link before;  // Used to trigger printing of an intermediate tree

tracePrint("Down",h);
if (h == z)
  return NEW(item, z, z, 1);  // Attach red leaf

if (less(v, key(h->item))) {
  tracePrint("Insert left",h);
  before=h->l;
  h->l = RBinsert(h->l, item, 0, h->r->red, h); // Insert in left subtree
  if (trace==2 && before!=h->l)  // Has a rotation occurred?
    STprintTree();
  if (h->l->red) {
    if (h->red)
      if (sw)
        if (siblingRed) {
          tracePrint("Case ~1l",hParent);
          hParent->red = 1;
          hParent->l->red = 0;
          hParent->r->red = 0;
          if (trace==2)
            STprintTree();
          }
        else {
          tracePrint("Case ~2",h);
          h = rotR(h);  // Set up case ~3 after return
          }
      else if (siblingRed) {
        tracePrint("Case 1l",hParent);
        hParent->red = 1;
        hParent->l->red = 0;
        hParent->r->red = 0;
        if (trace==2)
          STprintTree();
        }
      else
        ;  // Future case 3
    else if (!h->r->red && h->l->l->red) {
      tracePrint("Case 3",h);
      h = rotR(h);
      h->red = 0;
      h->r->red = 1;
      }
    }
  }
else {
  tracePrint("Insert right",h);
  before=h->r;
  h->r = RBinsert(h->r, item, 1, h->l->red, h); // Insert in right subtree
  if (trace==2 && before!=h->r)  // Has a rotation occurred?
    STprintTree();
  if (h->r->red) {
    if (h->red)
      if (!sw)
        if (siblingRed) {
          tracePrint("Case 1r",hParent);
          hParent->red = 1;
          hParent->l->red = 0;
          hParent->r->red = 0;
          if (trace==2)
            STprintTree();
          }
        else {
          tracePrint("Case 2",h);
          h = rotL(h);  // Set up case 3 after return
          }
      else if (siblingRed) {
        tracePrint("Case ~1r",hParent);
        hParent->red = 1;
        hParent->l->red = 0;
        hParent->r->red = 0;
        if (trace==2)
          STprintTree();
        }
      else
        ;  // Future case ~3
    else if (!h->l->red && h->r->r->red) {
      tracePrint("Case ~3",h);
      h = rotL(h);
      h->red = 0;
      h->l->red = 1;
      }
    }
  }

fixN(h);
tracePrint("Up",h);
return h;
}

void STinsert(Item item)
{
head = RBinsert(head, item, 0, 0, NULL);
if (head->red)
  printf("red to black reset has occurred at root!!!\n");
head->red = 0;
}

void checkRed(link h,int redParent)
// Verifies property 3 in notes
{
if (redParent && h->red)
{
  printf("Red property problem at %d\n",key(h->item));
  STprintTree();
  exit(0);
}
if (h==z)
  return;
checkRed(h->l,h->red);
checkRed(h->r,h->red);
}

int leftPathBlackHeight(link h)
// Counts black nodes on path to the minimum
{
if (h==z)
  return !(h->red);
return leftPathBlackHeight(h->l) + !(h->red);
}

void checkBlack(link h,int blackCount)
// Checks that all paths downward from a node have the same
// number of black nodes
{
if (h==z) {
  if (blackCount==!(h->red))
    return;
  else {
    printf("Black height problem!\n");
    STprintTree();
    exit(0);
    }
  }
if (h->red)
{
  checkBlack(h->l,blackCount);
  checkBlack(h->r,blackCount);
}
else
{
  checkBlack(h->l,blackCount-1);
  checkBlack(h->r,blackCount-1);
}
}

Key lastInorder;    // Saves key from last node processed

void checkInorder(link h)
// Checks that inorder yields keys in ascending order
{
if (h==z)
  return;

checkInorder(h->l);
if (less(h->item,lastInorder))
{
  printf("Inorder error\n");
  STprintTree();
  exit(0);
}
lastInorder=key(h->item);
checkInorder(h->r);
}

int checkN(link h)
// Verifies that subtree sizes are correct
{
int work;

if (h==z)
{
  if (h->N!=0)
  {
    printf("Count for sentinel is %d, should be 0\n",h->N);
    STprintTree();
    exit(0);
  }
}
else
{
  work=checkN(h->l) + checkN(h->r) + 1;
  if (h->N!=work)
  {
    printf("Count for key %d is %d, should be %d\n",key(h->item),h->N,work);
    STprintTree();
    exit(0);
  }
}
return h->N;
}

void verifyRBproperties()
// Checks all required properties.
// If a fatal problem is found, the tree is printed before exit(0)
{
int lpbHeight;

if (head->red)
  printf("Root is not black!\n");
if (z->red)
  printf("Sentinel is not black!\n");
lastInorder=(-99999999);
checkInorder(head);
checkRed(head,0);
lpbHeight=leftPathBlackHeight(head);
checkBlack(head,lpbHeight);
checkN(head);
}

void printTree(link h,int depth,int bhAbove)
{
int i,bhBelow;

if (h==z)
{
  if (bhAbove!=1)
  {
    for (i=0;i<depth;i++)
      printf("     ");
    printf("Black-height issue detected at sentinel\n");
  }

  return;
}

if ((h->red))
  bhBelow=bhAbove;
else
  bhBelow=bhAbove-1;
printTree(h->r,depth+1,bhBelow);
for (i=0;i<depth;i++)
  printf("     ");
if (h->red)
  printf("[%d %d %d]\n",key(h->item),h->N,bhBelow);
else if(h->red == 1 && h->tombstone =='1')
  printf("[(%d) %d %d]\n",key(h->item),h->N,bhBelow);
else if(h->red == 0 && h->tombstone == '1')
  printf("((%d) %d %d)\n",key(h->item),h->N,bhBelow);
else
  printf("(%d %d %d)\n",key(h->item),h->N,bhBelow);
printTree(h->l,depth+1,bhBelow);
}

void STprintTree()
{
printTree(head,0,leftPathBlackHeight(head));
}

void fixAllN(link h)
// Recomputes subtree sizes for an otherwise correct tree
{
if (h->l)
  fixAllN(h->l);
else
  h->l=z;
if (h->r)
  fixAllN(h->r);
else
  h->r=z;
fixN(h);
}

void cleanUpUnbalanced(link h)
// Checks a tree constructed elsewhere (like RB.loadAndGo.c)
{
fixAllN(h);
head=h;
z->red=0;
verifyRBproperties();
}



link returnlocation(link h, Key v)
// Recursive search for pointer to key
{ 
Key t = key(h->item);
if (h == z) 
  return z;
if (eq(v, t))
  return h;
if (less(v, t))
  return returnlocation(h->l, v);
return returnlocation(h->r, v);
}

void insertNode(int n)
{
 if(n > 0)
	{
		int place;
		link location;
		location = NEW(n, location, location, 1);
		place = STsearch(n);
		location = returnlocation(head, n);
		
		
		if(place == NULLitem) //insert item if not found in the tree
			{
			  STinsert(n);
			}
		else if(location->tombstone == '1')//rejuvenate node if found dead at that location Must find locatioN!
			{
			location->tombstone == '0';
			fixN(head);//update size of tree
			}
		else //ignore because it's a duplicate with a live node
		{ }

	}

}

void STdelete(Key v)
{
	link location;
	location = returnlocation(head, v);
	if(location == z || location->tombstone == '1'){
	printf("Error. Node not in tree or already dead.");}
	else
	{ location->tombstone = '1';
	//printf("node %d made tombstone", location->item);
	}

}


void divvyLists()
{
// Root/sentinel (head/z) is always black
live = (z = NEW(NULLitem, 0, 0, 0));
live->red=0;
dead = (z = NEW(NULLitem, 0, 0, 0));
dead->red=0;

//loop through each member of head list and divvy up based on tombstone



}


int findRankOfX(link h, Item v)
{
   int rank = 1; //include the number itself since not larger than
	while(h != z)
	{

	if(v < h->item)
	{
	h = h->l;
	}
	else if(v > h->item && h->tombstone == 0)
	{
	rank += 1 + h->l->N;
	h = h->r;
	}
	else
	{
	printf("Rank is %d\n", (rank + h->l->N));
	return rank + h->l->N;
	}
	}
	return printf("not found.\n"); //not in the tree
}

void findKeyWithRankK(link h, int rank)
{
	int found;
  while(h != z)
	{ 
	found = findRankOfX(head, h->item);
	if(found == rank)
	{ printf("%d\n", h->item);
	h = h->l;
	 }
	}


}

int main( int argc, const char* argv[] )
{	

	//Note: if entering an option that only needs one number, please enter that number twice
	int choice;
	int choice2;
	int rank;

	STinit(); //initialize the red black tree with a black sentinel node
	traceOff();
	//basicTraceOn(); testing function
	
	while(choice != 0)
	{
		scanf("%d %d",&choice, &choice2);
	 	
		if (choice == 1)
		{
		insertNode(choice2);
		}
		else if(choice ==2)
		{
		STdelete(choice2); 
		}
		else if(choice ==3)
		{
		findRankOfX(head, choice2);
		}
		else if(choice ==4)
		{
		findKeyWithRankK(head, choice2);
		}
		else if(choice ==5)
		{


		}
		else if(choice ==6)
		{
		divvyLists();

		}
		else if(choice ==7)
		{
		STprintTree();
		}
		else if(choice==8)
		{

		}
	}

}
