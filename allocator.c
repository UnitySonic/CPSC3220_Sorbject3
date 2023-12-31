#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/user.h>
#define _GNU_SOURCE


const int PAGESIZE = 4096;
const int CUSTOMPAGEINDEX = 10;
const int PAGELISTSIZE =  11;


/**
 *
 * Need to make some sort of cache to find freeBlocks faster
 * Faster Find needed
*/


typedef struct memBlock
{
    struct memBlock * nextFreeBlock;
}memBlock_t;

//Basic Linked List data structure
typedef struct pageType{
    struct pageType* nextPage;
    struct pageType* prevPage;
    int pageSize;
    struct memBlock * firstFreeBlock;
} pageType_t;

pageType_t* pageList[11];
memBlock_t* blockCache[10];
int fd;


pageType_t * makePage(size_t, int);
void * malloc(size_t);
void * calloc (size_t, size_t);
void * realloc(void*, size_t);
pageType_t *  find(int, void *);
void normalPageClean(struct pageType*, void *);
void customPageClean(struct pageType*, void *);
void __attribute__((constructor)) globalSetup();

void free(void*);

void globalSetup()
{

    fd = open ("/dev/zero" , O_RDWR ) ;

    for(int i = 0; i < 10; i++)
    {
        blockCache[i] = NULL;
    }

}


int calculatePageType(size_t size)
{
    double alloc = log2((int) size);
    alloc = ceil(alloc);
    int typeOfAlloc = alloc;
    typeOfAlloc--;
    typeOfAlloc = typeOfAlloc <=0 ? 0 : typeOfAlloc;
    return (typeOfAlloc > CUSTOMPAGEINDEX ? 10 : typeOfAlloc);
}

//Shim for malloc. Calls malloc and adds node to our linked list containing the pointer malloc just returned
void * malloc(size_t size)
{
    if(size == 0)
        return NULL;
    int typeOfAlloc = calculatePageType(size);


    //This must be a biggg object, so we'll create a page
    if(typeOfAlloc > 9)
    {
        if(pageList[CUSTOMPAGEINDEX] == NULL)
        {
            pageList[CUSTOMPAGEINDEX] = makePage(size, CUSTOMPAGEINDEX);
            pageList[CUSTOMPAGEINDEX]->prevPage = NULL;
            pageList[CUSTOMPAGEINDEX]->nextPage = NULL;
            memset(pageList[CUSTOMPAGEINDEX]->firstFreeBlock, 0, pageList[CUSTOMPAGEINDEX]->pageSize);
            return((void*) pageList[CUSTOMPAGEINDEX]->firstFreeBlock);
        }
        else
        {
            pageType_t * traveler = pageList[CUSTOMPAGEINDEX];
            while(traveler->nextPage != NULL)
            {
                traveler = traveler->nextPage;
            }
            traveler->nextPage = makePage(size, CUSTOMPAGEINDEX);
            traveler->nextPage->prevPage = traveler;
            traveler->nextPage->nextPage = NULL;
            memset(traveler->nextPage->firstFreeBlock, 0, traveler->nextPage->pageSize);
            return((void*) traveler->nextPage->firstFreeBlock);

        }
    }
    else
    {
        if(pageList[typeOfAlloc] == NULL)
        {
            pageList[typeOfAlloc] = makePage(size, typeOfAlloc);
            memBlock_t* temp = pageList[typeOfAlloc]->firstFreeBlock;
            pageList[typeOfAlloc]->firstFreeBlock = temp->nextFreeBlock;
            pageList[typeOfAlloc]->prevPage = NULL;
            memset(temp, 0, pageList[typeOfAlloc]->pageSize);
            return (void*)temp;
        }
        else
        {

            pageType_t * traveler = pageList[typeOfAlloc];
            bool morePagesToCheck = true;



            while(morePagesToCheck)
            {
                if(traveler->firstFreeBlock != NULL)
                {
                    memBlock_t* temp = traveler->firstFreeBlock;
                    traveler->firstFreeBlock = temp->nextFreeBlock;
                    memset(temp, 0, traveler->pageSize);
                    return (void*)temp;
                }
                if(traveler->nextPage == NULL)
                {
                    morePagesToCheck = false;
                }
                else
                {
                    traveler = traveler->nextPage;
                }
            }
            traveler->nextPage = makePage(size, typeOfAlloc);
            memBlock_t* temp = traveler->nextPage->firstFreeBlock;
            traveler->nextPage->firstFreeBlock = temp->nextFreeBlock;
            traveler->nextPage->nextPage = NULL;
            traveler->nextPage->prevPage = traveler;
            memset(temp, 0, traveler->pageSize);
            return (void*)temp;
        }
    }


}

pageType_t * makePage(size_t size, int typeOfAlloc)
{
    int pageSize = size > 1024 ? size + sizeof(pageType_t) : PAGESIZE + sizeof(pageType_t   );
    void * page = mmap ( NULL , pageSize ,
    PROT_READ | PROT_WRITE ,
    MAP_PRIVATE|MAP_ANONYMOUS , -1 , 0) ;

    pageType_t * header = ((pageType_t*) page);
    header->nextPage = NULL;

    if(typeOfAlloc < CUSTOMPAGEINDEX)
    {
        header->firstFreeBlock = (memBlock_t*)((uintptr_t) page + sizeof(pageType_t));
        int increment = size > 8 ? exp2(typeOfAlloc+1) : 8;
        header->pageSize = increment;

        for(int i = sizeof(pageType_t); i < (PAGESIZE + sizeof(pageType_t)); i= i + increment)
        {
            uintptr_t uintBlock = ((uintptr_t) page) + i;
            memBlock_t* currentBlock = (memBlock_t*)(uintBlock);
            //If i isn't at the final iteration
            if(i != ((PAGESIZE + sizeof(pageType_t)) - increment))
            {
                currentBlock->nextFreeBlock = (memBlock_t*)(uintBlock + increment);
            }
            else
            {
                currentBlock->nextFreeBlock = NULL;
            }
        }
        return header;
    }
    else
    {
        header->firstFreeBlock = (memBlock_t*)((uintptr_t) page + sizeof(pageType_t));
        header->pageSize = size;
        //header->firstFreeBlock = NULL;
        return header;
    }
}

void * calloc (size_t nmemb, size_t size)
{
    if((int) nmemb == 0 || (int) size == 0)
    {
        return NULL;
    }
    else
    {
        void * array = malloc(nmemb * size);
        memset(array, 0, (size * nmemb));
        //***I think this memset is correct but I may need to come back to it later
        return array;
    }
}

void * realloc(void *ptr, size_t size)
{
    if(ptr == NULL)
    {
        return (malloc(size));
    }
    else if (size == 0)
    {
        free(ptr);
        return NULL;
    }
    else
    {
        pageType_t * foundPage;
        for(int i = 0; i < PAGELISTSIZE; i++)
        {
            foundPage = find(i,ptr);
            if(foundPage != NULL)
                break;

        }

        if(foundPage == NULL)
        {
            return(NULL);
        }
        else
        {
            int oldSize = foundPage->pageSize;
            int oldAllocType = calculatePageType(oldSize);
            int newAllocType = calculatePageType(size);

            if(oldAllocType == newAllocType && oldAllocType < CUSTOMPAGEINDEX)
            {
                return ptr;
            }
            void * newLoc = malloc(size);

            int newSize = oldSize > size ? size : oldSize;
            memcpy(newLoc, ptr, newSize);

            if(foundPage->pageSize > 1024)
            {
                //customPageClean(foundPage, ptr);
            }
            //cast as a memBlock_t and change some stuff
            else
            {
                normalPageClean(foundPage,ptr);
            }
            return newLoc;
        }

    }
}


//Tries to see if a pointer is on the page category
pageType_t *  find(int pageIndex, void * ptr)
{

   void* page = (void*)(((uintptr_t)ptr) & PAGE_MASK);
   return(page);
}

// Shim for free. Calls free function and removes associated pointer from our Linked List
void free(void* ptr)
{
    pageType_t * pageWithPointer;
    for(int i = 0; i < PAGELISTSIZE; i++)
    {
        pageWithPointer = find(i,ptr);
        if(pageWithPointer != NULL)
            break;

    }
    if(pageWithPointer == NULL)
    {
        //exit(0);
        return;
    }
    else
    {
        //Custom page, time to use munmap
        if(pageWithPointer->pageSize > 1024)
        {
              customPageClean(pageWithPointer,ptr);
        }
        //cast as a memBlock_t and change some stuff
        else
        {
            normalPageClean(pageWithPointer,ptr);
        }
    }
}

void customPageClean(struct pageType* pageWithPointer, void * ptr)
{
    if(pageWithPointer->prevPage != NULL)
    {
        pageWithPointer->prevPage->nextPage = pageWithPointer->nextPage;
        if(pageWithPointer->nextPage != NULL)
            pageWithPointer->nextPage->prevPage = pageWithPointer->prevPage;
    }

    else
    {
        if(pageWithPointer->nextPage == NULL)
            pageList[CUSTOMPAGEINDEX] = NULL;
        else
            pageList[CUSTOMPAGEINDEX] = pageWithPointer->nextPage;
    }
    munmap(pageWithPointer, (pageWithPointer->pageSize + sizeof(pageType_t)));
}

void normalPageClean(struct pageType* pageWithPointer, void * ptr )
{
    if(pageWithPointer->firstFreeBlock == NULL)
    {

        pageWithPointer->firstFreeBlock = (memBlock_t*)ptr;
        memset(pageWithPointer->firstFreeBlock, 0, pageWithPointer->pageSize);
        pageWithPointer->firstFreeBlock->nextFreeBlock = NULL;
    }
    else
    {
        memBlock_t* temp = pageWithPointer->firstFreeBlock;

        pageWithPointer->firstFreeBlock = (memBlock_t*)ptr;
        memset(ptr, 0, pageWithPointer->pageSize);
        pageWithPointer->firstFreeBlock->nextFreeBlock = (memBlock_t*)temp;
    }
}









