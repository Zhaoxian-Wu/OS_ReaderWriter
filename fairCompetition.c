#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
  int tid;
  int delay;
  int last;
} Role;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t wrt = PTHREAD_MUTEX_INITIALIZER;
int reader_count = 0;

time_t startTime;

void* reader(void* argPtr) {
  Role role = *(Role*)argPtr;
  sleep(role.delay);
  printf("[%02.0lf秒]读者进程%d等待读取\n", 
    difftime(time(NULL), startTime), role.tid
  );
  pthread_mutex_lock(&mutex);
  ++reader_count;
  if(reader_count == 1) {
    pthread_mutex_lock(&wrt);
  }
  pthread_mutex_unlock(&mutex);
  printf("[%02.0lf秒]读者进程%d开始读取\n", 
    difftime(time(NULL), startTime), role.tid
  );

  // read
  sleep(role.last);

  pthread_mutex_lock(&mutex);
  --reader_count;
  if(reader_count == 0) {
    pthread_mutex_unlock(&wrt);
  }
  pthread_mutex_unlock(&mutex);
  printf("[%02.0lf秒]读者进程%d读取结束\n", 
    difftime(time(NULL), startTime), role.tid
  );
  return NULL;
}

void* writer(void* argPtr) {
  Role role = *(Role*)argPtr;
  sleep(role.delay);
  printf("[%02.0lf秒]写者进程%d等待写入\n", 
    difftime(time(NULL), startTime), role.tid
  );
  pthread_mutex_lock(&wrt);
  printf("[%02.0lf秒]写者进程%d开始写入\n", 
    difftime(time(NULL), startTime), role.tid
  );
  // write
  sleep(role.last);
  pthread_mutex_unlock(&wrt);
  printf("[%02.0lf秒]写者进程%d写入结束\n", 
    difftime(time(NULL), startTime), role.tid
  );
  return NULL;
}

int main() {
  const int MAX_THREAD = 100;
  pthread_t tid[MAX_THREAD];
  Role role[MAX_THREAD];
  int tidEnd = 0;

  startTime = time(NULL);

  int arg_tid;
  int arg_delay;
  int arg_last;
  char arg_type;
  while(scanf("%d %c%d%d", &arg_tid, &arg_type, &arg_delay, &arg_last) == 4) {
    assert(tidEnd < MAX_THREAD);
    if(arg_type == 'R') {
      role[tidEnd].tid = arg_tid;
      role[tidEnd].delay = arg_delay;
      role[tidEnd].last = arg_last;
      pthread_create(tid + tidEnd, NULL, reader, role + tidEnd);
    } else {
      role[tidEnd].tid = arg_tid;
      role[tidEnd].delay = arg_delay;
      role[tidEnd].last = arg_last;
      pthread_create(tid + tidEnd, NULL, writer, role + tidEnd);
    }
    printf("[%02.0lf秒]创建进程%d\n", difftime(time(NULL), startTime), arg_tid);
    ++tidEnd;
  }

  for(int i=0; i!=tidEnd; ++i) {
    pthread_join(tid[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&wrt);
}