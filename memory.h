#ifndef MEMORY_H
#define MEMORY_H

#include <limits.h>
#include <stddef.h>

template <typename T, size_t BlockSize = 4096>
class MemoryPool {
	public:
		/*Memory types*/
		typedef T  value_type;  // T的value类型 
		typedef T* pointer;     // T的指针类型
		typedef T& reference;   // T的引用类型
		typedef const T* const_pointer; 
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type; //指针减法结果类型 
		
		template <typename U> struct rebind {
		    typedef MemoryPool<U> other;
		};
		
		/*成员函数*/
		/*构造函数*/
		MemoryPool() throw();
		MemoryPool(const MemoryPool& memoryPool) throw();
		template <class U> MemoryPool(const MemoryPool<U>& memoryPool) throw();
		
		/*析构函数*/
		~MemoryPool() throw();
		
		/*元素取址*/
		pointer address(reference x) const throw();
		const_pointer address(const_reference x) const throw();
		
		//分配和回收一个元素的内存空间
		pointer allocate(size_type n = 1, const_pointer hint = 0);
		void deallocate(pointer p, size_type n = 1);
		
		//可达到的最多元素
		size_type max_size() const throw();
		
		//基于内存池的元素构造和析构
		void construct(pointer p, const_reference val);
		void destroy(pointer p);
		
		//自带申请内存和释放内存的构造和析构
		pointer newElement(const_reference val);
		void deleteElement(pointer p); 
	
	private:
		//存放元素或者next指针 
		union Slot_ {
			value_type element;
			Slot_* next;
		};
		
		typedef char* data_pointer_; //char* 指针，指向内存首地址 
		typedef Slot_ slot_type_;    //slot_ 值的类型
		typedef Slot_* slot_pointer_; //Slot_*指针类型
		
		slot_pointer_ currentBlock_; //内存块链表的头指针
		slot_pointer_ currentSlot_;  // 元素链表的头指针
		slot_pointer_ lastSlot_;     // 可存放元素的最后指针
		slot_pointer_ freeSlots_;    //元素构造后释放掉的内存链表头指针
		
		size_type padPointer(data_pointer_ p, size_type align) const throw(); //计算对齐所需空间 
		void allocateBlock(); //申请内存放到内存池中 
};

#include "memory.tcc"
#endif 
