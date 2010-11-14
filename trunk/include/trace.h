/* trace.h - tracing */

#ifndef _POSIX_TRACE_H
#define _POSIX_TRACE_H


#ifdef __cplusplus
extern "C" {
#endif

/* The <trace.h> header shall define the posix_trace_event_info structure */

typedef struct posix_trace_event_info
    {
    trace_event_id_t  posix_event_id;
    pid_t             posix_pid;
    void             *posix_prog_address;
    pthread_t         posix_thread_id;
    struct timespec   posix_timestamp;
    int               posix_truncation_status;
    }posix_trace_event_info_t;

/* The <trace.h> header shall define the posix_trace_status_info structure */

typedef struct posix_trace_status_info
    {
    int     posix_stream_full_status;
    int     posix_stream_overrun_status;
    int     posix_stream_status;
    int     posix_log_full_status;
    int     posix_log_overrun_status;
    int     posix_stream_flush_error;
    int     posix_stream_flush_status;
    }posix_trace_status_info_t;

/*
 * The <trace.h> header shall define the following symbols:
 */

/* Allowed values for posix_stream_flush_status */

#define POSIX_TRACE_NOT_FLUSHING        0
#define POSIX_TRACE_FLUSHING            1

/* Allowed values for posix_stream_full_status and posix_log_full_status */

#define POSIX_TRACE_NOT_FULL            0
#define POSIX_TRACE_FULL                1


/* Allowed values for posix_stream_status */

#define POSIX_TRACE_SUSPENDED           0
#define POSIX_TRACE_RUNNING             1


/*
 * Allowed values for posix_stream_overrun_status and
 * posix_log_overrun_status
 */

#define POSIX_TRACE_NO_OVERRUN          0
#define POSIX_TRACE_OVERRUN             1

/* Allowed values for stream_full_policy */

#define POSIX_TRACE_LOOP                1
#define POSIX_TRACE_UNTIL_FULL          2
#define POSIX_TRACE_FLUSH               3


/* Allowed values for log_full_policy */

#define POSIX_TRACE_APPEND              3


/* Allowed values for inheritance */

#if _POSIX_TRACE_INHERIT > 0
#define POSIX_TRACE_CLOSE_FOR_CHILD     0
#endif /* _POSIX_TRACE_INHERIT */


/* Allowed values for posix_truncation_status */

#define POSIX_TRACE_NOT_TRUNCATED       0
#define POSIX_TRACE_TRUNCATED_RECORD    1
#define POSIX_TRACE_TRUNCATED_READ      2


#define POSIX_TRACE_WOPID_EVENTS        0
#define POSIX_TRACE_SYSTEM_EVENTS       1
#define POSIX_TRACE_ALL_EVENTS          2

/* Options for manipulating event sets */
#define POSIX_TRACE_SET_EVENTSET        1
#define POSIX_TRACE_ADD_EVENTSET        2
#define POSIX_TRACE_SUB_EVENTSET        3


/* These definitions must track the definitions in target/usr/h/limits.h */

#ifndef TRACE_USER_EVENT_MAX
#define TRACE_USER_EVENT_MAX    256
#endif

#ifndef TRACE_EVENT_NAME_MAX
#define TRACE_EVENT_NAME_MAX    64
#endif

#ifndef TRACE_NAME_MAX
#define TRACE_NAME_MAX          64
#endif

/*
 * These are the range of ids for each event range. Note the values in
 * base/b_trace_event_set_t.h too.
 */

#define _TRACE_IMPLDEF_SYS_EVENTS_START 256
#define _TRACE_SYS_EVENTS_START         512
#define _TRACE_USR_EVENTS_START         1024
#define _TRACE_FIRST_USER_EVENT         (_TRACE_USR_EVENTS_START + 1)


#define POSIX_TRACE_ERROR       _TRACE_SYS_EVENTS_START
#define POSIX_TRACE_START       (_TRACE_SYS_EVENTS_START+1)
#define POSIX_TRACE_STOP        (_TRACE_SYS_EVENTS_START+2)
#define POSIX_TRACE_OVERFLOW    (_TRACE_SYS_EVENTS_START+3)
#define POSIX_TRACE_RESUME      (_TRACE_SYS_EVENTS_START+4)
#define POSIX_TRACE_FILTER      (_TRACE_SYS_EVENTS_START+5)
#define POSIX_TRACE_FLUSH_START (_TRACE_SYS_EVENTS_START+6)
#define POSIX_TRACE_FLUSH_STOP  (_TRACE_SYS_EVENTS_START+7)

#define POSIX_TRACE_UNNAMED_USEREVENT   _TRACE_USR_EVENTS_START

#define _TRACE_NAME_MAX      64

typedef struct trace_attr
    {
    int                 _Trace_attr_magic;
    char                _Generation_version [_TRACE_NAME_MAX];
    char                _Trace_name [_TRACE_NAME_MAX];
    struct timespec     _Creation_time;
    struct timespec     _Clock_resolution;
    size_t              _Stream_min_size;
    int                 _Stream_full_policy;
    size_t              _Max_data_size;
    int                 _Log_full_policy;
    size_t              _Log_max_size;
    } trace_attr_t;

typedef id_t    trace_event_id_t;
typedef id_t    trace_id_t;

/*
 * For filters, a set bit represents an event which is filtered out. So a
 * default filter set, which allows everything through, is represented by a
 * structure where the arrays are all zeros.
 */

/* These are the sizes of the event id ranges */

#define _TRACE_NUM_IMPLDEF_SYS_EVENTS    64
#define _TRACE_NUM_SYS_EVENTS            64
#define _TRACE_NUM_USER_EVENTS           256

/* Calculate the number of bytes required to store the event set */

#define _TRACE_FILTER_ARRAY_SIZE(_NumEvents) (((_NumEvents) + 7) / 8)

typedef struct _TRACE_EVENT_SET_DATA
    {
    /* Implementation-defined system events */
    unsigned char       _ImplDefSysEventSet [
            _TRACE_FILTER_ARRAY_SIZE(_TRACE_NUM_IMPLDEF_SYS_EVENTS)];

    /* System events defined in POSIX 1003.1 */
    unsigned char       _SystemEventSet [
            _TRACE_FILTER_ARRAY_SIZE(_TRACE_NUM_SYS_EVENTS)];

    unsigned char       _UserEventSet [
            _TRACE_FILTER_ARRAY_SIZE(_TRACE_NUM_USER_EVENTS)];
    } _trace_event_set_data_t;

typedef struct _trace_event_set
    {
    unsigned int                _EventSetMagic;  /* indicates set is initialized */
    _trace_event_set_data_t       _SetData;
    } trace_event_set_t;


int  posix_trace_attr_destroy(trace_attr_t *);
int  posix_trace_attr_getclockres(const trace_attr_t *,
         struct timespec *);
int  posix_trace_attr_getcreatetime(const trace_attr_t *,
         struct timespec *);
int  posix_trace_attr_getgenversion(const trace_attr_t *, char *);
int  posix_trace_attr_getinherited(const trace_attr_t *restrict,
         int *restrict);
int  posix_trace_attr_getlogfullpolicy(const trace_attr_t *restrict,
         int *restrict);
int  posix_trace_attr_getlogsize(const trace_attr_t *restrict,
         size_t *restrict);
int  posix_trace_attr_getmaxdatasize(const trace_attr_t *restrict,
         size_t *restrict);
int  posix_trace_attr_getmaxsystemeventsize(const trace_attr_t *restrict,
         size_t *restrict);
int  posix_trace_attr_getmaxusereventsize(const trace_attr_t *restrict,
         size_t, size_t *restrict);
int  posix_trace_attr_getname(const trace_attr_t *, char *);
int  posix_trace_attr_getstreamfullpolicy(const trace_attr_t *restrict,
         int *restrict);
int  posix_trace_attr_getstreamsize(const trace_attr_t *restrict,
         size_t *restrict);
int  posix_trace_attr_init(trace_attr_t *);
int  posix_trace_attr_setinherited(trace_attr_t *, int);
int  posix_trace_attr_setlogfullpolicy(trace_attr_t *, int);
int  posix_trace_attr_setlogsize(trace_attr_t *, size_t);
int  posix_trace_attr_setmaxdatasize(trace_attr_t *, size_t);
int  posix_trace_attr_setname(trace_attr_t *, const char *);
int  posix_trace_attr_setstreamfullpolicy(trace_attr_t *, int);
int  posix_trace_attr_setstreamsize(trace_attr_t *, size_t);
int  posix_trace_clear(trace_id_t);
int  posix_trace_close(trace_id_t);
int  posix_trace_create(pid_t, const trace_attr_t *restrict,
         trace_id_t *restrict);
int  posix_trace_create_withlog(pid_t, const trace_attr_t *restrict,
         int, trace_id_t *restrict);
void posix_trace_event(trace_event_id_t, const void *restrict, size_t);
int  posix_trace_eventid_equal(trace_id_t, trace_event_id_t,
         trace_event_id_t);
int  posix_trace_eventid_get_name(trace_id_t, trace_event_id_t, char *);
int  posix_trace_eventid_open(const char *restrict,
         trace_event_id_t *restrict);
int  posix_trace_eventset_add(trace_event_id_t, trace_event_set_t *);
int  posix_trace_eventset_del(trace_event_id_t, trace_event_set_t *);
int  posix_trace_eventset_empty(trace_event_set_t *);
int  posix_trace_eventset_fill(trace_event_set_t *, int);
int  posix_trace_eventset_ismember(trace_event_id_t,
         const trace_event_set_t *restrict, int *restrict);
int  posix_trace_eventtypelist_getnext_id(trace_id_t,
         trace_event_id_t *restrict, int *restrict);
int  posix_trace_eventtypelist_rewind(trace_id_t);
int  posix_trace_flush(trace_id_t);
int  posix_trace_get_attr(trace_id_t, trace_attr_t *);
int  posix_trace_get_filter(trace_id_t, trace_event_set_t *);
int  posix_trace_get_status(trace_id_t,
         struct posix_trace_status_info *);
int  posix_trace_getnext_event(trace_id_t,
         struct posix_trace_event_info *restrict, void *restrict,
         size_t, size_t *restrict, int *restrict);
int  posix_trace_open(int, trace_id_t *);
int  posix_trace_rewind(trace_id_t);
int  posix_trace_set_filter(trace_id_t, const trace_event_set_t *, int);
int  posix_trace_shutdown(trace_id_t);
int  posix_trace_start(trace_id_t);
int  posix_trace_stop(trace_id_t);
int  posix_trace_timedgetnext_event(trace_id_t,
         struct posix_trace_event_info *restrict, void *restrict,
         size_t, size_t *restrict, int *restrict,
         const struct timespec *restrict);
int  posix_trace_trid_eventid_open(trace_id_t, const char *restrict,
         trace_event_id_t *restrict);
int  posix_trace_trygetnext_event(trace_id_t,
         struct posix_trace_event_info *restrict, void *restrict, size_t,
         size_t *restrict, int *restrict);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_TRACE_H */

