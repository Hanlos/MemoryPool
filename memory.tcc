#ifndef MEMORY_BLOCK_TCC
#define MEMORY_BLOCK_TCC

//计算对齐所需补的空间
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::padPointer(data_pointer_ p, size_type align) const throw()
{
	size_t result = reinterpret_cast<size_t>(p);
	return ((align - result) % align);
}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool() throw()
{
	currentBlock_ =0;
	currentSlot_ =0;
	lastSlot_ =0;
	freeSlots_=0;
}

/*复制构造函数，调用内存池初始化*/
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool(const MemoryPool& memoryPool) throw()
{
   MemoryPool(); 	
} 

/*复制构造函数，调用MemoryPool初始化*/
template <typename T,size_t BlockSize>
template <class U>
MemoryPool<T, BlockSize>::MemoryPool(const MemoryPool<U>& memoryPool) throw()
{
	MemoryPool();
}

/*析构函数，把内存池中所有block delete掉*/
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::~MemoryPool() throw()
{
    slot_pointer_ curr = currentBlock_;
	while (curr != 0) {
		slot_pointer_ prev = curr->next;
		operator delete(reinterpret_cast<void*>(curr));
		curr = prev;
	}	
} 

/*返回地址*/
template <typename T, size_t BlockSize> 
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::address(reference x) const throw()
{
	return &x;
}

/* 返回地址的const重载*/
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::const_pointer
MemoryPool<T, BlockSize>::address(const_reference x) const throw()
{
   return &x;	
} 

/*申请一块空闲的 block 放入内存池中*/
template <typename T,size_t BlockSize>
void MemoryPool<T, BlockSize>::allocateBlock() {
	/*分配空间给新的block 并且存储指向前一个block的指针*/
	
	/*operator new 申请对应大小内存，返回void*指针*/
	data_pointer_ newBlock = reinterpret_cast<data_pointer_> (operator new(BlockSize)); 
	
	/*原来的block链接到newblock*/
	reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;

	/*新的currentblock*/
	currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);

	/*满足元素的偏移量要求*/
	data_pointer_ body = newBlock + sizeof(slot_pointer_);

	/*计算对齐应该空出多少位置*/
	size_type bodyPadding = padPointer(body, sizeof(slot_type_));

	/*currentslot_ 为该block开始的地方加上bodypadding 个char*空间 */
	currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
    
    /*计算最后一个能放置slot_type_的位置 */
	lastSlot_ = reinterpret_cast<slot_pointer_>
	            (newBlock + BlockSize - sizeof(slot_type_) + 1);
}

/*返回指向分配新元素所需要内存的指针*/
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::allocate(size_type, const_pointer)
{
	if (freeSlots_ != 0) {
		pointer result = reinterpret_cast<pointer>(freeSlots_);
		/*更新freeSlots*/
		freeSlots_ = freeSlots_->next;
		return result;
	} else {
		/*如果之前申请的内存用完了，分配新的block*/
		if (currentSlot_ >= lastSlot_)
			allocateBlock();
        return reinterpret_cast<pointer>(currentSlot_++); 
	}
}

/*将元素内存归还给free内存链表*/
template <typename T, size_t BlockSize>
inline void MemoryPool<T, BlockSize>::deallocate(pointer p, size_type)
{
	if (p != 0) {
		reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
		freeSlots_ = reinterpret_cast<slot_pointer_>(p);
	}
}

/*计算可达到的最大元素上限数*/
template <typename T,size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::max_size() const throw()
{
	size_type maxBlocks = -1 / BlockSize;
	return (BlockSize - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
}

/*在已经分配的内存上构造对象*/
template <typename T,size_t BlockSize>
inline void MemoryPool<T, BlockSize>::construct(pointer p, const_reference val)
{
	/*placement new 用法，在已有内存上构造对象，调用T的复制构造函数*/
	new (p) value_type (val);
}

/*销毁对象*/
template <typename T, size_t BlockSize>
inline void 
MemoryPool<T, BlockSize>::destroy(pointer p) 
{
	//placement new 需要手动调用T的析构函数
	p->~value_type();
}

/*创建新元素*/
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::newElement(const_reference val)
{
	//申请内存
	pointer result = allocate();
	//在内存上构造对象
	construct(result, val);
	return result;
}

/*删除对象*/
template <typename T, size_t BlockSize>
inline void MemoryPool<T, BlockSize>::deleteElement(pointer p)
{
	if (p != 0) {
		p->~value_type();
		deallocate(p);
	}
}

#endif //MEMORY_BLOCK_TCC
