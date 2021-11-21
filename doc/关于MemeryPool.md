### 技术指标
1. 内存申请时的命中率
> 用户调用申请内存的次数为N，直接从池中分配用户申请的内存而不是通过系统分配的次数为M，则命中率R=M/N。从内存池的设计初衷来看，命中率指标应该作为内存池性能判断的核心指标。


2. 冗余度
> 当用户申请了MemCount_Using大小的数据，而此时内存池中总共向系统申请了MemCount_Total大小的数据，此时冗余度 DupRatio=(MemCount_Total-MemCount_Using)/MemCount_Total。在满足命中率指标的前提下，冗余度指标将是一个很关键的指标。

### 关于ObjectPool
+ 这个ObjectPool的实现只是一个实验性质的东西，没有用于任何项目中进行过验证
+ ObjectPool中使用new的方式创建新的对象，这样是想要避免无法调用Object的构造函数而造成一些隐秘的未初始化而导致的错误。当这种设想可能并符合实际的应用场景中，因为使用ObjectPool的话调用了get()方法一般是会独立再使用memset()之类的api对对象内存进行重置。
+ ObjectPool中在不够内存提供给用户时会直接从系统中通过new object的方式来申请新的内存，但是在用户将Object返回Pool时没有主动收缩Pool的规模回收内存。而是通过用户显式调用GC的方式进行内存的回收。