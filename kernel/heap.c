#include <rook/core.h>
#include <rook/kernel/util.h>
#include <rook/kernel/lock.h>
#include <rook/kernel/console.h>
#include <rook/kernel/mmu.h>
#include <rook/kernel/heap.h>

struct block {
	size_t size;
	bool available;
	struct block *previous, *next;
};

static void *breakpoint = (void *)0xFFFFFFFE00000000;
static struct block *first = NULL;

static rk_spinlock_t lock;

/* find block of size */
static struct block *find_block(size_t size) {

	struct block *block = first, *previous = NULL;
	while (block) {

		if ((block->size >= size || !block->next) && block->available)
			return block;

		previous = block;
		block = block->next;
	}

	/* set next block */
	previous->next = (struct block *)((void *)(previous + 1) + previous->size);
	if ((void *)(previous->next + 1) > breakpoint) {

		size_t end = RK_ALIGN((size_t)(previous->next + 1), rk_mmu_page_size);
		size_t map_size = end - (size_t)breakpoint;

		if (!rk_mmu_map(breakpoint, NULL, map_size, 0)) {

			previous->next = NULL;
			return NULL;
		}
		breakpoint += map_size;
	}

	previous->next->size = 0;
	previous->next->available = true;
	previous->next->previous = previous;
	previous->next->next = NULL;

	return previous->next;
}

/* split block into two */
static void split_block(struct block *block, size_t size) {

	if (block->size < size + sizeof(struct block))
		return;

	struct block *next = (struct block *)((void *)(block + 1) + size);

	next->size = block->size - size - sizeof(struct block);
	next->available = true;
	next->next = block->next;
	next->previous = block;

	block->size = size;
	block->next = next;

	if (next->next) next->next->previous = next;
}

/* merge block with previous block */
static struct block *merge_block_back(struct block *block) {

	while (block->previous && block->previous->available) {

		if (!block->available)
			block->previous->available = false;

		block->previous->size += sizeof(struct block) + block->size;
		block->previous->next = block->next;

		if (block->next)
			block->next->previous = block->previous;

		block = block->previous;
	}
	return block;
}

/* merge block with next block */
static struct block *merge_block_front(struct block *block) {

	while (block->next && block->next->available &&
	       (block->next->size || !block->next->next)) {

		block->size += sizeof(struct block) + block->next->size;
		block->next = block->next->next;

		if (block->next)
			block->next->previous = block;
	}
	return block;
}

/* merge block with front and back */
static struct block *merge_block(struct block *block) {

	block = merge_block_back(block);
	return merge_block_front(block);
}

/* initialize heap */
extern void rk_heap_init(void) {

	rk_spinlock_init(&lock);

	first = (struct block *)rk_mmu_map(
			breakpoint, NULL,
			rk_mmu_page_size, 0
	);
	if (!first) rk_panic(NULL, "Can't initialize heap");

	breakpoint += rk_mmu_page_size;

	first->size = 0;
	first->available = false;
	first->previous = NULL;
	first->next = NULL;

	rk_log_info("Initialized heap");
}

/* allocate memory */
#define RETURN_AND_UNLOCK(r) ({\
		rk_spinlock_release(&lock);\
		return r;\
	})

extern void *malloc(size_t size) {

	rk_spinlock_release(&lock);

	size = RK_ALIGN(size, 8);

	struct block *block = find_block(size);
	if (!block) RETURN_AND_UNLOCK(NULL);

	/* map memory */
	if ((void *)(block + 1) + size > breakpoint) {

		size_t end = RK_ALIGN((size_t)(block + 1) + size, rk_mmu_page_size);
		size_t map_size = end - (size_t)breakpoint;

		if (!rk_mmu_map(breakpoint, NULL, map_size, 0))
			RETURN_AND_UNLOCK(NULL);
		breakpoint += map_size;
	}

	/* prepare block */
	block->available = false;

	split_block(block, size);
	if (block->size < size)
		block->size = size;

	void *pointer = (void *)(block + 1);
	memset(pointer, 0, size);

	RETURN_AND_UNLOCK(pointer);
}

/* reallocate block */
extern void *realloc(void *pointer, size_t size) {

	size = RK_ALIGN(size, 8);

	if (!pointer) return NULL;
	rk_spinlock_acquire(&lock);

	struct block *block = (struct block *)pointer - 1;

	size_t old_size = block->size;
	block = merge_block(block);

	void *new_pointer = (void *)(block + 1);
	if (new_pointer != pointer) {

		memmove(new_pointer, pointer, old_size);
		pointer = new_pointer;
	}

	/* allocation can fit */
	if (block->size >= size) {

		split_block(block, size);
		RETURN_AND_UNLOCK(pointer);
	}

	/* completely open space */
	else if (!block->next) {

		if ((void *)(block + 1) + size > breakpoint) {

			void *start = (void *)RK_ALIGN(
					(uint64_t)(block + 1), rk_mmu_page_size
			);
			void *end = (void *)RK_ALIGN(
					(uint64_t)(block + 1) + size, rk_mmu_page_size
			);
			size_t map_size = (size_t)end - (size_t)start;
			if (map_size) {

				if (!rk_mmu_map(start, NULL, map_size, 0))
					RETURN_AND_UNLOCK(NULL);
				breakpoint += map_size;
			}
		}
		block->size = size;
		RETURN_AND_UNLOCK(pointer);
	}

	/* get a new allocation */
	else {
		rk_spinlock_release(&lock);
		new_pointer = malloc(size);
		rk_spinlock_acquire(&lock);

		if (!new_pointer)
			RETURN_AND_UNLOCK(NULL);

		memcpy(new_pointer, pointer, old_size);

		rk_spinlock_release(&lock);
		free(pointer);
		rk_spinlock_acquire(&lock);

		RETURN_AND_UNLOCK(new_pointer);
	}
	RETURN_AND_UNLOCK(NULL);
}

/* free memory */
extern void free(void *pointer) {

	if (!pointer) return;
	rk_spinlock_acquire(&lock);

	struct block *block = (struct block *)pointer - 1;

	block->available = true;
	block = merge_block(block);

	rk_spinlock_release(&lock);
}
