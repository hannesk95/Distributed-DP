# `src`

folder containing all libraries that are used by <a href="../main.cpp" target="_blank">`main.cpp`</a>

<br/><br/>

-------

## <a href="vi_processor_base.cpp" target="_blank">`vi_processor_base.cpp`</a>

Base Class for all following Implementations of the desired task. Responsible for parsing the data from the given dataset from `.npy` files into <a href='http://eigen.tuxfamily.org/index.php?title=Main_Page' target='_blank'>`Eigen`</a> vectors.

Given functionality:
- `VI_Processor_Base` - reads all files `.npy` that are needed for the value iteration, transforms them to `Eigen` vectors/SparseMatrix
- `void Process()` - function that calls `value_iteration_impl` of respective processor
- `std::string GetName()` - get name of implementation
- `bool SetParameter()` - set parameter of implementation
- `std::map<std::string, float> GetParameters()` - get parameter of implementation
- `float iteration_step()` - performs the value iteration for given amount of steps (before remerging whole `probability matrix`) on given subset of the `probability matrix`
- `void debug_message()` - prints given debugging messages for respective processor

<br/><br/>

-------

## <a href="vi_processor_base.h" target="_blank">`vi_processor_base.h`</a>

Header of <a href="vi_processor_base.cpp" target="_blank">`vi_processor_base.cpp`</a>

<br/><br/>

-------

## <a href="vi_processor_impl_distr_01.cpp" target="_blank">`vi_processor_impl_distr_01.cpp`</a>

first implementation of a communication scheme. Using `MPI_Allgatherv`,`MPI_Allreduce` and `MPI_Gatherv`. Waits for given amount of periods before re-gathering the updated `J`, inherits functionality from `VI_Processor_Base`.

Given functionality:
- `std::string GetName()` - get name of implementation
- `bool SetParameter()` - set parameter of implementation
- `std::map<std::string, float> GetParameters()` - get parameter of implementation
- `void value_iteration_impl()` - concrete implementation of communication scheme, doing the value iteration

<br/><br/>

-------

## <a href="vi_processor_impl_distr_01.h" target="_blank">`vi_processor_impl_distr_01.h`</a>

Header of <a href="vi_processor_impl_distr_01.cpp" target="_blank">`vi_processor_impl_distr_01.cpp`</a>

<br/><br/>

-------

## <a href="vi_processor_impl_distr_02.cpp" target="_blank">`vi_processor_impl_distr_02.cpp`</a>

second implementation of a communication scheme. Using `MPI_Send`, `MPI_Recv` and `MPI_Bcast`. Waits for given amount of periods before re-gathering the updated `J`, inherits functionality from `VI_Processor_Base`. Logic of communication scheme is similar to `VI_Processor_Impl_Distr_01`

Given functionality:
- `std::string GetName()` - get name of implementation
- `bool SetParameter()` - set parameter of implementation
- `std::map<std::string, float> GetParameters()` - get parameter of implementation
- `void value_iteration_impl()` - concrete implementation of communication scheme, doing the value iteration

<br/><br/>

-------

## <a href="vi_processor_impl_distr_02.h" target="_blank">`vi_processor_impl_distr_02.h`</a>

Header of <a href="vi_processor_impl_distr_02.cpp" target="_blank">`vi_processor_impl_distr_02.cpp`</a>

<br/><br/>

-------

## <a href="vi_processor_impl_distr_03.cpp" target="_blank">`vi_processor_impl_distr_03.cpp`</a>

third implementation of a communication scheme. Using `MPI_Sendrecv` and `MPI_Gatherv`. Updates `J` every period completely, inherits functionality from `VI_Processor_Base`. Works synchronely

Given functionality:
- `std::string GetName()` - get name of implementation
- `bool SetParameter()` - set parameter of implementation
- `std::map<std::string, float> GetParameters()` - get parameter of implementation
- `void value_iteration_impl()` - concrete implementation of communication scheme, doing the value iteration

<br/><br/>

-------

## <a href="vi_processor_impl_distr_03.h" target="_blank">`vi_processor_impl_distr_03.h`</a>

Header of <a href="vi_processor_impl_distr_03.cpp" target="_blank">`vi_processor_impl_distr_03.cpp`</a>

<br/><br/>

-------

## <a href="vi_processor_impl_distr_04.cpp" target="_blank">`vi_processor_impl_distr_04.cpp`</a>

third implementation of a communication scheme. Using `MPI_Isend` and `MPI_Irecv`, `MPI_Ibcast` and `MPI_Igatherv`. Waits for given amount of periods before re-gathering the updated `J`, inherits functionality from `VI_Processor_Base`. Logic of communication scheme is similar to `VI_Processor_Impl_Distr_02` but non-blocking

Given functionality:
- `std::string GetName()` - get name of implementation
- `bool SetParameter()` - set parameter of implementation
- `std::map<std::string, float> GetParameters()` - get parameter of implementation
- `void value_iteration_impl()` - concrete implementation of communication scheme, doing the value iteration

<br/><br/>

-------

## <a href="vi_processor_impl_distr_04.h" target="_blank">`vi_processor_impl_distr_04.h`</a>

Header of <a href="vi_processor_impl_distr_04.cpp" target="_blank">`vi_processor_impl_distr_04.cpp`</a>

<br/><br/>

-------

## <a href="vi_processor_impl_distr_05.cpp" target="_blank">`vi_processor_impl_distr_05.cpp`</a>

third implementation of a communication scheme. Using `MPI_Igather`, `MPI_Bcast` and `MPI_Gatherv`. Waits for given amount of periods before re-gathering the updated `J`, inherits functionality from `VI_Processor_Base`. Logic of communication scheme is similar to `VI_Processor_Impl_Distr_01` but non-blocking

Given functionality:
- `std::string GetName()` - get name of implementation
- `bool SetParameter()` - set parameter of implementation
- `std::map<std::string, float> GetParameters()` - get parameter of implementation
- `void value_iteration_impl()` - concrete implementation of communication scheme, doing the value iteration

<br/><br/>

-------

## <a href="vi_processor_impl_distr_05.h" target="_blank">`vi_processor_impl_distr_05.h`</a>

Header of <a href="vi_processor_impl_distr_05.cpp" target="_blank">`vi_processor_impl_distr_05.cpp`</a>

<br/><br/>

-------

## <a href="vi_processor_impl_local.cpp" target="_blank">`vi_processor_impl_local.cpp`</a>

local implementation of value iteration without any communication scheme or parallelization. Implemented to use as reference for performance comparisons. Updates `J` every period completely, inherits functionality from `VI_Processor_Base`. Works synchronely

Given functionality:
- `std::string GetName()` - get name of implementation
- `void value_iteration_impl()` - concrete implementation of value iteration

<br/><br/>

-------

## <a href="vi_processor_impl_local.h" target="_blank">`vi_processor_impl_local.h`</a>

Header of <a href="vi_processor_impl_local.cpp" target="_blank">`vi_processor_impl_local.cpp`</a>