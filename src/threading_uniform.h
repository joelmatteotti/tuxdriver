/*
 * FICHIER .H pour la realisation de THREADS PORTABLES LINUX/WINDOWS
 *
 * ce code a été écrit par Smeagol(smeagol-worms4@ifrance.com) et revu par
 * Jerry Kan (pilouface@gmail.com)
 * pour le site
 * http://www.irrlicht.fr/      communauté francaise Irrlicht
 *
 * retrouvez ce tutoriel et beaucoup d'autres sur notre forum,
 */

#ifdef USE_MUTEX
#ifndef _THREADING_UNIFORM_H_
#define _THREADING_UNIFORM_H_

#ifdef WIN32
#   include <windows.h>
#   define callback_t                       unsigned long __stdcall
#   define thread_t                         HANDLE
#   define thread_create(thrd, fct, param)  thrd = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)(fct),(param),0,NULL)
#   define thread_delete(thrd)              CloseHandle(thrd);
#   define thread_wait_close(thrd)          WaitForMultipleObjects(1, &thrd, TRUE, INFINITE)
#   define mutex_t                          CRITICAL_SECTION
#   define mutex_init(mutex)                InitializeCriticalSection(& mutex)
#   define mutex_lock(mutex)                EnterCriticalSection(& mutex)
#   define mutex_unlock(mutex)              LeaveCriticalSection(& mutex)
#   define mutex_delete(mutex)              DeleteCriticalSection(& mutex)
#   define semaphore_t                      HANDLE
#   define semaphore_init(sema, max, place) ((sema) = CreateSemaphore(NULL, (max), (place), NULL))
#   define semaphore_lock(sema)             WaitForSingleObject((sema), INFINITE)
#   define semaphore_unlock(sema)           ReleaseSemaphore((sema), 1, NULL)
#   define semaphore_delete(sema)           CloseHandle(sema)
#else
#   include <pthread.h>
#   define callback_t                       void *
#   define thread_t                         pthread_t
#   define thread_create(thrd, fct, param)  pthread_create(&thrd, NULL, (fct), ((void *)param));
#   define thread_delete(thrd)
#   define thread_wait_close(thrd)          pthread_join(thrd, NULL)
#   include <semaphore.h>
#   define mutex_t                          pthread_mutex_t
#   define mutex_init(mutex)                pthread_mutex_init ((&mutex), NULL)
#   define mutex_lock(mutex)                pthread_mutex_lock((&mutex))
#   define mutex_unlock(mutex)              pthread_mutex_unlock((&mutex))
#   define mutex_delete(mutex)              pthread_mutex_destroy((&mutex))
#   define semaphore_t                      sem_t*
#   define semaphore_init(sema, max, place) (sema) = new sem_t; sem_init ((sema), (max), (place))
#   define semaphore_lock(sema)             sem_wait((sema))
#   define semaphore_unlock(sema)           sem_post((sema))
#   define semaphore_delete(sema)           sem_destroy((sema));  delete ((sema))
#endif

#endif
#endif
