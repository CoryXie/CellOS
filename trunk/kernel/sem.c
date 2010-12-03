/* sem.c - XSI semaphore operations */

/*
  NAME
  
  semctl - XSI semaphore control operations
  
  SYNOPSIS
  
  #include <sys/sem.h>
  
  int semctl(int semid, int semnum, int cmd, ...); 
  
  DESCRIPTION
  
  The semctl() function operates on XSI semaphores (see XBD Semaphore ).
  It is unspecified whether this function interoperates with the realtime 
  interprocess communication facilities defined in Realtime .
  
  The semctl() function provides a variety of semaphore control operations as
  specified by cmd. The fourth argument is optional and depends upon the 
  operation requested. If required, it is of type union semun, which the 
  application shall explicitly declare:
  
  union semun {
      int val;
      struct semid_ds *buf;
      unsigned short  *array;
  } arg;
  
  The following semaphore control operations as specified by cmd are executed 
  with respect to the semaphore specified by semid and semnum. The level of 
  permission required for each operation is shown with each command; see XSI 
  Interprocess Communication . The symbolic names for the values of cmd are
  defined in the <sys/sem.h> header:
  
  GETVAL
  
  Return the value of semval; see <sys/sem.h>. Requires read permission.
  
  SETVAL
  
  Set the value of semval to arg.val, where arg is the value of the fourth
  argument to semctl(). When this command is successfully executed, the semadj 
  value corresponding to the specified semaphore in all processes is cleared.
  Requires alter permission; see XSI Interprocess Communication .
  
  GETPID
  
  Return the value of sempid. Requires read permission.
  
  GETNCNT
  
  Return the value of semncnt. Requires read permission.
  
  GETZCNT
  
  Return the value of semzcnt. Requires read permission.
  
  The following values of cmd operate on each semval in the set of semaphores:
  
  GETALL
  
  Return the value of semval for each semaphore in the semaphore set and place
  into the array pointed to by arg.array, where arg is the fourth argument to 
  semctl(). Requires read permission.
  
  SETALL
  
  Set the value of semval for each semaphore in the semaphore set according 
  to the array pointed to by arg.array, where arg is the fourth argument to 
  semctl(). When this command is successfully executed, the semadj values 
  corresponding to each specified semaphore in all processes are cleared. 
  Requires alter permission.
  
  The following values of cmd are also available:
  
  IPC_STAT
  
  Place the current value of each member of the semid_ds data structure 
  associated with semid into the structure pointed to by arg.buf, where arg 
  is the fourth argument to semctl(). The contents of this structure are 
  defined in <sys/sem.h>. Requires read permission.
  
  IPC_SET
  
  Set the value of the following members of the semid_ds data structure 
  associated with semid to the corresponding value found in the structure 
  pointed to by arg.buf, where arg is the fourth argument to semctl():
  
  sem_perm.uid
  sem_perm.gid
  sem_perm.mode
  
  The mode bits specified in IPC General Description are copied into the 
  corresponding bits of the sem_perm.mode associated with semid. The stored 
  values of any other bits are unspecified.
  
  This command can only be executed by a process that has an effective user 
  ID equal to either that of a process with appropriate privileges or to 
  the value of sem_perm.cuid or sem_perm.uid in the semid_ds data structure 
  associated with semid.
  
  IPC_RMID
  
  Remove the semaphore identifier specified by semid from the system and 
  destroy the set of semaphores and semid_ds data structure associated with 
  it. This command can only be executed by a process that has an effective
  user ID equal to either that of a process with appropriate privileges or 
  to the value of sem_perm.cuid or sem_perm.uid in the semid_ds data 
  structure associated with semid.
  
  RETURN VALUE
  
  If successful, the value returned by semctl() depends on cmd as follows:
  
  GETVAL
  
  The value of semval.
  
  GETPID
  
  The value of sempid.
  
  GETNCNT
  
  The value of semncnt.
  
  GETZCNT
  
  The value of semzcnt.
  
  All others
  
  0.
  
  Otherwise, semctl() shall return -1 and set errno to indicate the error.
  
  ERRORS
  
  The semctl() function shall fail if:
  
  [EACCES]
  
  Operation permission is denied to the calling process; see XSI Interprocess
  Communication .
  
  [EINVAL]
  
  The value of semid is not a valid semaphore identifier, or the value of 
  semnum is less than 0 or greater than or equal to sem_nsems, or the value 
  of cmd is not a valid command.
  
  [EPERM]
  
  The argument cmd is equal to IPC_RMID or IPC_SET and the effective user ID
  of the calling process is not equal to that of a process with appropriate 
  privileges and it is not equal to the value of sem_perm.cuid or sem_perm.uid 
  in the data structure associated with semid.
  
  [ERANGE]
  
  The argument cmd is equal to SETVAL or SETALL and the value to which semval 
  is to be set is greater than the system-imposed maximum.
*/

int semctl(int semid, int semnum, int cmd, ...)
    {

    }

/*
  NAME
  
  semop - XSI semaphore operations
  SYNOPSIS
  
  #include <sys/sem.h>
  
  int semop(int semid, struct sembuf *sops, size_t nsops); 
  
  DESCRIPTION
  
  The semop() function operates on XSI semaphores (see XBD Semaphore ). It 
  is unspecified whether this function interoperates with the realtime 
  interprocess communication facilities defined in Realtime .
  
  The semop() function shall perform atomically a user-defined array of 
  semaphore operations in array order on the set of semaphores associated 
  with the semaphore identifier specified by the argument semid.
  
  The argument sops is a pointer to a user-defined array of semaphore 
  operation structures. The implementation shall not modify elements of 
  this array unless the application uses implementation-defined extensions.
  
  The argument nsops is the number of such structures in the array.
  
  Each structure, sembuf, includes the following members:
  
  Member Type       Member Name         Description
  
  short             sem_num             Semaphore number.
  
  short             sem_op              Semaphore operation.
  
  short             sem_flg             Operation flags.
  
  Each semaphore operation specified by sem_op is performed on the 
  corresponding semaphore specified by semid and sem_num.
  
  The variable sem_op specifies one of three semaphore operations:
  
  If sem_op is a negative integer and the calling process has alter permission, 
  one of the following shall occur:
  
  If semval(see <sys/sem.h>) is greater than or equal to the absolute value 
  of sem_op, the absolute value of sem_op is subtracted from semval. Also,
  if (sem_flg &SEM_UNDO) is non-zero, the absolute value of sem_op shall be 
  added to the semadj value of the calling process for the specified semaphore.
  
  If semval is less than the absolute value of sem_op and (sem_flg &IPC_NOWAIT) 
  is non-zero, semop() shall return immediately.
  
  If semval is less than the absolute value of sem_op and (sem_flg &IPC_NOWAIT)
  is 0, semop() shall increment the semncnt associated with the specified 
  semaphore and suspend execution of the calling thread until one of the
  following conditions occurs:
  
  The value of semval becomes greater than or equal to the absolute value of 
  sem_op. When this occurs, the value of semncnt associated with the specified 
  semaphore shall be decremented, the absolute value of sem_op shall be 
  subtracted from semval and, if (sem_flg &SEM_UNDO) is non-zero, the absolute 
  value of sem_op shall be added to the semadj value of the calling process 
  for the specified semaphore.
  
  The semid for which the calling thread is awaiting action is removed from 
  the system. When this occurs, errno shall be set equal to [EIDRM] and -1 
  shall be returned.
  
  The calling thread receives a signal that is to be caught. When this occurs,
  the value of semncnt associated with the specified semaphore shall be 
  decremented, and the calling thread shall resume execution in the manner 
  prescribed in sigaction .
  
  If sem_op is a positive integer and the calling process has alter permission,
  the value of sem_op shall be added to semval and, if (sem_flg &SEM_UNDO) is
  non-zero, the value of sem_op shall be subtracted from the semadj value of 
  the calling process for the specified semaphore.
  
  If sem_op is 0 and the calling process has read permission, one of the 
  following shall occur:
  
  If semval is 0, semop() shall return immediately.
  
  If semval is non-zero and (sem_flg &IPC_NOWAIT) is non-zero, semop() shall 
  return immediately.
  
  If semval is non-zero and (sem_flg &IPC_NOWAIT) is 0, semop() shall increment 
  the semzcnt associated with the specified semaphore and suspend execution of 
  the calling thread until one of the following occurs:
  
  The value of semval becomes 0, at which time the value of semzcnt associated 
  with the specified semaphore shall be decremented.
  
  The semid for which the calling thread is awaiting action is removed from 
  the system. When this occurs, errno shall be set equal to [EIDRM] and -1 
  shall be returned.
  
  The calling thread receives a signal that is to be caught. When this occurs, 
  the value of semzcnt associated with the specified semaphore shall be 
  decremented, and the calling thread shall resume execution in the manner 
  prescribed in sigaction .
  
  Upon successful completion, the value of sempid for each semaphore specified 
  in the array pointed to by sops shall be set equal to the process ID of the 
  calling process.
  
  RETURN VALUE
  
  Upon successful completion, semop() shall return 0; otherwise, it shall 
  return -1 and set errno to indicate the error.
  
  ERRORS
  
  The semop() function shall fail if:
  
  [E2BIG]
  
  The value of nsops is greater than the system-imposed maximum.
  
  [EACCES]
  
  Operation permission is denied to the calling process; see XSI Interprocess
  Communication.
  
  [EAGAIN]
  
  The operation would result in suspension of the calling process but 
  (sem_flg &IPC_NOWAIT) is non-zero.
  
  [EFBIG]
  
  The value of sem_num is less than 0 or greater than or equal to the number
  of semaphores in the set associated with semid.
  
  [EIDRM]
  
  The semaphore identifier semid is removed from the system.
  
  [EINTR]
  
  The semop() function was interrupted by a signal.
  
  [EINVAL]
  
  The value of semid is not a valid semaphore identifier, or the number of 
  individual semaphores for which the calling process requests a SEM_UNDO 
  would exceed the system-imposed limit.
  
  [ENOSPC]
  
  The limit on the number of individual processes requesting a SEM_UNDO would 
  be exceeded.
  
  [ERANGE]
  
  An operation would cause a semval to overflow the system-imposed limit, or
  an operation would cause a semadj value to overflow the system-imposed limit.
*/
int semop(int semid, struct sembuf *sops, size_t nsops)
    {

    }

