PROGRAM ghex_p2p_bi_cb_avail_mt
  use iso_fortran_env
  use omp_lib
  use ghex_context_mod
  use ghex_comm_mod
  use ghex_message_mod
  use ghex_request_mod
  use ghex_future_mod
  
  implicit none  
  
  include 'mpif.h'  

  ! globally accessible variables
  type(ghex_context) :: context

  ! threadprivate variables
  integer :: comm_cnt = 0, nlsend_cnt = 0, nlrecv_cnt = 0, submit_cnt = 0, submit_recv_cnt = 0
  integer :: thread_id = 0
#ifdef USE_OPENMP
  !$omp threadprivate(comm_cnt, nlsend_cnt, nlrecv_cnt, submit_cnt, submit_recv_cnt, thread_id)
#endif

#ifdef USE_OPENMP
  integer(atomic_int_kind) :: sent[*] = 0, received[*] = 0, tail_send[*] = 0, tail_recv[*] = 0
#else
  integer :: sent = 0, received = 0, tail_send = 0, tail_recv = 0
#endif

  ! local variables
  integer :: mpi_err, mpi_threading
  integer :: num_threads = 1
  character(len=32) :: arg
  integer(8) :: niter, buff_size
  integer :: inflight
  
  if( iargc() /= 3) then
     print *, "Usage: bench [niter] [msg_size] [inflight]";
     call exit(1)
  end if

  call getarg(1, arg);
  read(arg,*) niter
  call getarg(2, arg);
  read(arg,*) buff_size
  call getarg(3, arg);
  read(arg,*) inflight

#ifdef USE_OPENMP
  call mpi_init_thread (MPI_THREAD_MULTIPLE, mpi_threading, mpi_err)
  if (mpi_threading /= MPI_THREAD_MULTIPLE) then
     print *, "MPI_THREAD_MULTIPLE not supported by MPI, aborting";
     call exit(1)
  end if
  !$omp parallel
  num_threads = omp_get_num_threads()
  !$omp end parallel
#else
  call mpi_init_thread (MPI_THREAD_SINGLE, mpi_threading, mpi_err)
#endif

  ! create a context object
  context = context_new(num_threads, mpi_comm_world);

#ifdef USE_OPENMP
  !$omp parallel
#endif

  call run(context)

#ifdef USE_OPENMP
  !$omp end parallel
#endif

  ! delete the ghex context
  call context_delete(context)  
  call mpi_finalize(mpi_err)

contains

  subroutine run(context)
    type(ghex_context) :: context

    ! all below variables are thread-local
    type(ghex_communicator), save :: comm

    integer :: rank = -1, size = -1, num_threads = -1, peer_rank = -1
    logical :: using_mt = .false.

    integer :: last_received = 0
    integer :: last_sent = 0
    integer :: dbg = 0, sdbg = 0, rdbg = 0;
    integer :: j = 0, np = 0
    integer :: incomplete_sends = 0, send_complete = 0

    type(ghex_message), dimension(:), allocatable, save :: smsgs, rmsgs
    type(ghex_request), dimension(:), allocatable, save :: sreqs, rreqs
    type(ghex_future), save :: bsreq, brreq
    type(ghex_message), save :: bsmsg, brmsg
    logical :: result = .false.
    real :: ttic = 0, tic = 0, toc = 0
  
    procedure(f_callback), pointer, save :: rcb, scb

    !$omp threadprivate(comm, rank, size, num_threads, peer_rank)
    !$omp threadprivate(using_mt, last_received, last_sent)
    !$omp threadprivate(dbg, sdbg, rdbg, j, np, incomplete_sends, send_complete)
    !$omp threadprivate(smsgs, rmsgs, sreqs, rreqs)
    !$omp threadprivate(bsreq, brreq, bsmsg, brmsg, result, rcb, scb)
    !$omp threadprivate(ttic, tic, toc)
   
    ! ---------------------------------------
    ! world info
    ! ---------------------------------------

    ! obtain a communicator
    comm = context_get_communicator(context)

    rank        = comm_rank(comm);
    size        = comm_size(comm);
    thread_id   = omp_get_thread_num();
    num_threads = omp_get_num_threads();
    peer_rank   = modulo(rank+1, 2)

#ifdef USE_OPENMP
    using_mt = .true.
#endif

    if (thread_id==0 .and. rank==0) then
       print *, "running ", __FILE__
    end if

    ! ---------------------------------------
    ! data initialization
    ! ---------------------------------------
    rcb => recv_callback
    scb => send_callback

    allocate(smsgs(inflight), rmsgs(inflight), sreqs(inflight), rreqs(inflight))
    do j = 1, inflight
       smsgs(j) = message_new(buff_size, ALLOCATOR_STD);
       rmsgs(j) = message_new(buff_size, ALLOCATOR_STD);
       call message_zero(smsgs(j))
       call message_zero(rmsgs(j))
       call request_init(sreqs(j))
       call request_init(rreqs(j))
    end do

    call comm_barrier(comm)

    if (thread_id == 0) then
       call cpu_time(ttic)
       tic = ttic
       if(rank == 1) then
          print *, "number of threads: ", num_threads, ", multi-threaded: ", using_mt
       end if
    end if

    ! ---------------------------------------
    ! send/recv niter messages - as soon as a slot becomes free
    ! ---------------------------------------
    do while(sent < niter .or. received < niter)
       if (thread_id == 0 .and. dbg >= (niter/10)) then
          dbg = 0
          call cpu_time(toc)
          print *, rank, " total bwdt MB/s:      ", &
               (received-last_received + sent-last_sent)*size*buff_size/2/(toc-tic)*num_threads/1e6
          tic = toc
          last_received = received;
          last_sent = sent;
       end if

       if (rank==0 .and. thread_id==0 .and. rdbg >= (niter/10)) then
          print *,  received, " received"
          rdbg = 0
       end if

       if (rank==0 .and. thread_id==0 .and. sdbg >= (niter/10)) then
          print *, sent, " sent"
          sdbg = 0
       end if

       do j = 1, inflight
          if (request_test(rreqs(j))) then
             submit_recv_cnt = submit_recv_cnt + num_threads
             rdbg = rdbg + num_threads
             dbg = dbg + num_threads
             call comm_post_recv_cb(comm, rmsgs(j), peer_rank, thread_id*inflight+j-1, rcb, rreqs(j))
          else
             np = comm_progress(comm)
          end if

          if (sent < niter .and. request_test(sreqs(j))) then
             submit_cnt = submit_cnt + num_threads
             sdbg = sdbg + num_threads
             dbg = dbg + num_threads
             call comm_post_send_cb(comm, smsgs(j), peer_rank, thread_id*inflight+j-1, scb, sreqs(j))
          else
             np = comm_progress(comm)
          end if
       end do
    end do

    call comm_barrier(comm)

    ! ---------------------------------------
    ! Timing and statistics output
    ! ---------------------------------------
    if (thread_id==0 .and. rank == 0) then
       call cpu_time(toc)
       print *, "time:", (toc-ttic)/num_threads
       print *, "final MB/s: ", (niter*size*buff_size)/(toc-ttic)*num_threads/1e6
    end if

    ! stop here to help produce a nice std output
    call comm_barrier(comm)
#ifdef USE_OPENMP
    !$omp critical
#endif
101 format ("rank ", I0, " thread " , I0 , " sends submitted " , I0,  &
          " serviced " , I0 , ", non-local sends " ,  I0 , " non-local recvs " , I0)
    write (*, 101) rank, thread_id , submit_cnt/num_threads , comm_cnt, nlsend_cnt , nlrecv_cnt
#ifdef USE_OPENMP
    !$omp end critical
#endif

    ! ---------------------------------------
    ! tail loops - submit RECV requests until
    ! all SEND requests have been finalized.
    ! This is because UCX cannot cancel SEND requests.
    ! https://github.com/openucx/ucx/issues/1162
    ! ---------------------------------------

    ! complete all posted sends
    do while(tail_send/=num_threads)

       np = comm_progress(comm)

       ! check if we have completed all our posted sends
       if(send_complete == 0) then
          incomplete_sends = 0;
          do j = 1, inflight
             if(.not. request_test(sreqs(j))) then
                incomplete_sends = incomplete_sends + 1
             end if
          end do

          if (incomplete_sends == 0) then
             ! increase thread counter of threads that are done with the sends
             call atomic_add(tail_send, 1)
             send_complete = 1
          end if
       end if

       ! continue to re-schedule all recvs to allow the peer to complete
       do j = 1, inflight
          if (request_test(rreqs(j))) then
             call comm_post_recv_cb(comm, rmsgs(j), peer_rank, thread_id*inflight+j-1, rcb, rreqs(j))
          end if
       end do
    end do

    ! We have all completed the sends, but the peer might not have yet.
    ! Notify the peer and keep submitting recvs until we get his notification.

#if USE_OPENMP
    !$omp master
#endif
    bsmsg = message_new(1_8, ALLOCATOR_STD);
    brmsg = message_new(1_8, ALLOCATOR_STD);
    call comm_post_send(comm, bsmsg, peer_rank, 800000, bsreq);
    call comm_post_recv(comm, brmsg, peer_rank, 800000, brreq);
#if USE_OPENMP
    !$omp end master
#endif

    do while(tail_recv == 0)

       np = comm_progress(comm)
       
       ! schedule all recvs to allow the peer to complete
       do j = 1, inflight
          if (request_test(rreqs(j))) then
             call comm_post_recv_cb(comm, rmsgs(j), peer_rank, thread_id*inflight+j-1, rcb, rreqs(j))
          end if
       end do

#if USE_OPENMP
       !$omp master
#endif
       if (future_ready(brreq)) then
          tail_recv = 1
       end if
#if USE_OPENMP
    !$omp end master
#endif
    end do

    ! peer has sent everything, so we can cancel all posted recv requests
    do j = 1, inflight
       result = request_cancel(rreqs(j))
    end do

    ! cleanup per-thread. messages are freed by ghex if comm_recv_cb and comm_send_cb
    call comm_delete(comm)
  end subroutine run

  ! ---------------------------------------
  ! callbacks
  ! ---------------------------------------
  subroutine send_callback (mesg, rank, tag)
    type(ghex_message), value :: mesg
    integer(c_int), value :: rank, tag
    integer(1), dimension(:), pointer, save :: msg_data

    if(tag/inflight /= thread_id) nlsend_cnt = nlsend_cnt + 1;
    comm_cnt = comm_cnt + 1;
    sent = sent + 1;
  end subroutine send_callback

  subroutine recv_callback (mesg, rank, tag)
    type(ghex_message), value :: mesg
    integer(c_int), value :: rank, tag
    
    if(tag/inflight /= thread_id) nlrecv_cnt = nlrecv_cnt + 1;
    comm_cnt = comm_cnt + 1;
    received = received + 1;
  end subroutine recv_callback

END PROGRAM ghex_p2p_bi_cb_avail_mt
