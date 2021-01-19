#include "vi_processor_impl_distr_05.h"
#include <limits>
#include <mpi.h>


#define MPI_Error_Check(x) {const int err=x; if(x!=MPI_SUCCESS) { fprintf(stderr, "MPI ERROR %d at %d.", err, __LINE__);}}


bool VI_Processor_Impl_Distr_05::SetParameter(std::string param, float value)
{
    if(param == "comm_period")
    {
        comm_period = value;
        return true;
    }

    return VI_Processor_Base::SetParameter(param, value);
}

std::map<std::string, float> VI_Processor_Impl_Distr_05::GetParameters()
{
    std::map<std::string, float> parameters = VI_Processor_Base::GetParameters();
    parameters["comm_period"] = comm_period;
    return parameters;
}


void VI_Processor_Impl_Distr_05::value_iteration_impl(
        Eigen::Ref<Eigen::VectorXi> Pi,
        Eigen::Ref<Eigen::VectorXf> J,
        const Eigen::Ref<const SpMat_t> P,
        const unsigned int T)
{
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

//    if (world_size < 2)
//    {
//        std::cerr << "World size must be greater than 1 otherwise OpenMPI makes no sense!" << std::endl;
//        MPI_Abort(MPI_COMM_WORLD, 1);
//    }

    int processor_workload = ceil(float(J.size()) / world_size);
    int processor_workload_last = J.size() - (world_size - 1) * processor_workload;
    int processor_start = processor_workload * world_rank;
    int processor_end = processor_workload * (world_rank +1);

    if (world_rank == world_size -1)
    {
        processor_end = J.size();
    }

    Eigen::VectorXf J_buffer(0);
    J_buffer = J.segment(0, J.size());

    MPI_Request request;
    MPI_Status status;

    std::vector<int> recvcounts;
    std::vector<int> displs;

    for(int i=0; i < world_size; i++)
    {
        if(i == world_size - 1)
        {
            recvcounts.push_back(processor_workload_last);
            displs.push_back(i*processor_workload);
        }

        else
        {
            recvcounts.push_back(processor_workload);
            displs.push_back(i*processor_workload);
        }
    }



    for(unsigned int t=0; t < T; ++t)
    {
        iteration_step(Pi, J, P, processor_start, processor_end);

        if(t % comm_period == 0)
        {
            float* J_raw = J.data();

            MPI_Igatherv(&J_raw[processor_start],
                        processor_end - processor_start,
                        MPI_FLOAT,
                        J_raw,
                        recvcounts.data(),
                        displs.data(),
                        MPI_FLOAT,
                        root_id,
                        MPI_COMM_WORLD,
                        &request);

            ////////////////////////////////////////////////////
            // Do some additional computations here if needed //
            ////////////////////////////////////////////////////

            MPI_Wait(&request, &status);

            //Eigen::Map<Eigen::VectorXf> J_final(J_raw, J.size());
            //Eigen::Map<Eigen::VectorXf> J(J_raw, J_buffer.size());

            //std::cout << "Rank: " << world_rank << std::endl;
            //std::cout << J_buffer << std::endl;


            float deviation = (J_buffer-J).cwiseAbs().maxCoeff();

            MPI_Bcast(&deviation, 1, MPI_FLOAT, root_id, MPI_COMM_WORLD);

            if(deviation <= tolerance)
            {
                debug_message("Converged after " + std::to_string(t) + " iterations with communication period " + std::to_string(comm_period));
                break;
            }

            //J = J_final.segment(0, 125);

            MPI_Bcast(J.data(), J.size(), MPI_FLOAT, root_id, MPI_COMM_WORLD);



            //J_buffer.resize(0);

            //J_buffer = J.segment(0, 125);
            J_buffer = J;

        }
    }

    int* Pi_raw = Pi.data();
    MPI_Status status_gather;
    MPI_Request request_gather;

    MPI_Gatherv(&Pi_raw[processor_start],
                 processor_end - processor_start,
                 MPI_INT,
                 Pi_raw,
                 recvcounts.data(),
                 displs.data(),
                 MPI_INT,
                 root_id,
                 MPI_COMM_WORLD);
                 //&request_gather);

    ////////////////////////////////////////////////////
    // Do some additional computations here if needed //
    ////////////////////////////////////////////////////

    //MPI_Error_Check(MPI_Wait(&request_gather, &status_gather));
}

std::string VI_Processor_Impl_Distr_05::GetName()
{
    return VI_Processor_Base::GetName() + "-" + std::to_string(comm_period);
}