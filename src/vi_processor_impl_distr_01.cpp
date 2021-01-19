// first implementation, using Allgatherv, Allreduce and Gatherv

// includes
#include "vi_processor_impl_distr_01.h" // header
#include <mpi.h> // openmpi support

/**
 * get name of implementation
 * @return name: name of implementation [String]
 */
std::string VI_Processor_Impl_Distr_01::GetName()
{
    return VI_Processor_Base::GetName() + "-" + std::to_string(comm_period);
}

/**
 * set parameter of implementation
 * @param param: param name to set [String]
 * @param value: value of param to set [String]
 * @return hasParam: whether implementation has parameters [=true] or not [=false] [Boolean]
 */
bool VI_Processor_Impl_Distr_01::SetParameter(std::string param, float value)
{
    if(param == "comm_period")
    {
        comm_period = value;
        return true;
    }
    return VI_Processor_Base::SetParameter(param, value);
}

/**
 * get parameter of implementation
 * @return parameters: mapped pair of "alpha" and "tolerance" [Pair]
 */
std::map<std::string, float> VI_Processor_Impl_Distr_01::GetParameters()
{
    std::map<std::string, float> parameters = VI_Processor_Base::GetParameters();
    parameters["comm_period"] = comm_period;
    return parameters;
}

/**
 * actual implementation of inherited abstract function for the communication scheme
 * @param Pi: initialized Pi [Integer vector reference]
 * @param J: initialized J [Float vector reference]
 * @param P: probability matrix [const Eigen SparseMatrix<RowMajor> reference]
 * @param max_iter: maximum number of iterations before stopping anyway [const unsigned Integer]
 */
void VI_Processor_Impl_Distr_01::value_iteration_impl(
        Eigen::Ref<Eigen::VectorXi> Pi, 
        Eigen::Ref<Eigen::VectorXf> J, 
        const Eigen::Ref<const SpMat_t> P, 
        const unsigned int max_iter
    )
{
    // init openmpi parameters
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of this process

    // Split up states for each process (last process gets additional remainder)
    const int process_first_state = (J.size() / world_size) * world_rank;
    const int process_last_state =  (world_size == world_rank + 1) ? J.size() : (J.size() / world_size) * (world_rank + 1);

    // set number of states per process and displacements of subvectors
    std::vector<int> recvcounts;    // Number of states for each process
    std::vector<int> displs;        // Displacement of sub vectors for each process
    for(int i=0; i < world_size; ++i)
    {
        if(i+1 < world_size) recvcounts.push_back((J.size() / world_size));
        else recvcounts.push_back((J.size() / world_size) + J.size() % world_size);

        if(i == 0) displs.push_back(0);
        else displs.push_back(displs[i-1] + recvcounts[i-1]);
    }

    // Keeps track of the change in J vector
    float error = 0;

    for(unsigned int t=0; t < max_iter; ++t)
    {
        // Compute one value iteration step for a range of states
        float max_diff = iteration_step(Pi, J, P, process_first_state, process_last_state);

        // Store value of biggest change that appeared while updating J
        if(max_diff > error) error = max_diff;

        // Exchange results every comm_period cycles
        if(t % comm_period == 0)
        {
            // Merge local updates together
            float* J_raw = J.data();
            MPI_Allgatherv(&J_raw[process_first_state], 
                        process_last_state - process_first_state,
                        MPI_FLOAT, 
                        J_raw, 
                        recvcounts.data(), 
                        displs.data(), 
                        MPI_FLOAT, 
                        MPI_COMM_WORLD
            );

            // Check for convergence by seaching the biggest change in J from all processors
            MPI_Allreduce(&error, &error, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);

            // If convergence rate is below threshold stop
            if(error <= tolerance)
            {
                debug_message("Converged after " + std::to_string(t) + " iterations with communication period " + std::to_string(comm_period));
                break;
            } 

            // Reset change rate to 0 such that it can be overwritten with a new (smaller) value
            error = 0;
        }

    }
    // Merge Policy
    
    int* Pi_raw = Pi.data();
    MPI_Gatherv(&Pi_raw[process_first_state], 
                process_last_state - process_first_state,
                MPI_INT, 
                Pi_raw, 
                recvcounts.data(), 
                displs.data(), 
                MPI_INT, 
                root_id, 
                MPI_COMM_WORLD
    );
}