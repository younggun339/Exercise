/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

static char *heap_listp = 0;


static void *find_fit(size_t asize){

    void *bp;

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
        if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
            return bp;
        }
    }

    return NULL;
}

static void place(void *bp, size_t asize){

    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize)>= (2*DSIZE)){
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
    }
    else{
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }

}


// size_t align_size(size_t size) {
//     return (size + 7) & ~7;
// }

// static void *find_fit(size_t asize){

//     char *first_bp = heap_listp + (2*WSIZE);
//     char *next_bp;

//     size_t first_alloc = GET_ALLOC(first_bp);
//     size_t first_size = GET_SIZE(first_bp);

//     size_t next_alloc;
//     size_t next_size;

//     char *last_bp;


//     if(first_alloc && first_size < asize){
//         next_bp = HDRP(NEXT_BLKP(first_bp));
//         next_alloc = GET_ALLOC(next_bp);
//         next_size = GET_SIZE(next_bp);
//     }
//     else{
//         return (void *)first_bp;
//     }

//     while (next_alloc && next_size < asize)
//     {
//         if(next_bp == (last_bp = mem_heap_hi())){
//             return NULL;
//         }
//         next_bp = HDRP(NEXT_BLKP(next_bp));
//         next_alloc = GET_ALLOC(next_bp);
//         next_size = GET_SIZE(next_bp);
//         if(!next_alloc && next_size >= asize){
//             return (void *)next_bp;
//         }
//     }
    
// }

// static void place(void *bp, size_t asize){

//     size_t block_size = GET_SIZE(bp);
//     if(block_size/2 >= asize){
//         PUT(HDRP(bp), PACK(align_size(block_size/2), 1));
//         PUT(FTRP(bp), PACK(align_size(block_size/2), 1));
//     }
//     else{
//         PUT(HDRP(bp), PACK(align_size(block_size), 1));
//         PUT(FTRP(bp), PACK(align_size(block_size), 1));
//     }

// }


static void *coalesce(void *bp)
{
	
    // 앞, 뒤 블록의 가용 여부 가져오기.
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    // 현재 블록 크기 가져오기.
    size_t size = GET_SIZE(HDRP(bp));
    
    // case 1 : 둘 다 할당.
    if(prev_alloc && next_alloc) {
    	return bp;
        }
    // case 2 : 뒤가 가용.
    else if (prev_alloc && !next_alloc){
    	size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));}
    // case 3 : 앞이 가용.
    else if (!prev_alloc && next_alloc){
    	size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        bp = PREV_BLKP(bp);}
    // case 4 : 앞 뒤 가용.
    else{
    	size += GET_SIZE(HDRP(PREV_BLKP(bp))) 
        	+ GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));
        bp= PREV_BLKP(bp);
        }
    return bp;
    
    }
     

static void *extend_heap(size_t words)
{
	char *bp;
    size_t size;
   
   // 짝수배의 워드만큼 할당.
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
    	return NULL;
        
    //free block 헤더, 풋터, 에필로그 헤더 초기화.
    PUT(HDRP(bp), PACK(size,0)); // free 헤더
    PUT(FTRP(bp), PACK(size,0)); // free 풋터
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1)); // 새로운 에필로그 헤더
    
    //앞선 블록이 free라면 연결해주겠다..
    return coalesce(bp);
    
    }

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    	// 만약 현재 4배의 더블 워드를 더한다면 죽는다고 선언할 경우 실패.
	if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
    	return -1;
    
    
    // padding 정렬용.
    PUT(heap_listp, 0);
    // 프롤로그 헤더.
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));
    // 프롤로그 풋터.
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));
    // 에필로그 헤더.
    PUT(heap_listp + (3*WSIZE), PACK(0,1));
    heap_listp += (2*WSIZE);
    
    // 다 배정했으니 heap의 크기를 늘려놓겠다.
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
    	return -1;
    return 0;
}


// /* 
//  * mm_malloc - Allocate a block by incrementing the brk pointer.
//  *     Always allocate a block whose size is a multiple of the alignment.
//  */

// // 아래 함수는 정렬만 하고 있을 뿐, find_fit, extend_heap의 경우까지 고려하지 않고 있음.
// void *mm_malloc(size_t size)
// {
//     // 받은 size에 패딩 등 거친 크기 설정.
//     int newsize = ALIGN(size + SIZE_T_SIZE);
//     //newsize만큼 p를 늘림.
//     void *p = mem_sbrk(newsize);
//     if (p == (void *)-1)
// 	return NULL;
//     else {
//         // 블록 시작주소 p에 크기 정보 저장.
//         //(size_t *) <== (void *)를 형변환.
//         // * ((size_t *)p) <== 형변환한 포인터를 참조해서 값을 설정하겠다.
//         // ...size로! 
//         *(size_t *)p = size;
//         // 크기 정보 다음의 실제 데이터 부분 시작 주소 반환.
//         return (void *)((char *)p + SIZE_T_SIZE);
//     }
// }

void *mm_malloc(size_t size)
{
	size_t asize; // 적정 블록 사이즈.
    size_t extendsize; // 만약 초과한다면 참고할 사이즈. 
    char *bp;
    
    // 만약 용량을 요구하지 않을 경우 무시.
    if (size == 0)
    	return NULL;
    
    // overhead와 정렬 reqs를 포함한 사이즈.
    if (size <= DSIZE)
    	asize = 2*DSIZE;
    else
    	asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
        
    // free list에서 딱 맞는 거 찾기.
    if ((bp = find_fit(asize)) != NULL){
    	place(bp, asize);}
        return bp;
        
    // 딱 맞는거 못 찾았다면 메모리 더 갖고 오기.    
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
    	return NULL;
    place(bp, asize);
    return bp;
    
    }

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{

    // 헤더 위치에 있는 사이즈를 가져온다.
	size_t size = GET_SIZE(HDRP(ptr));
    
    // 가용 가능인 걸로 헤더와 풋터를 바꿔버림.
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    // 기준으로 연결 가능한 지에 따라 연결.
    coalesce(ptr);
    }

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}












